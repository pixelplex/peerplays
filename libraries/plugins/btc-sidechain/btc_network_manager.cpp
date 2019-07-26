#include <btc-sidechain/btc_network_manager.hpp>

#include <fc/thread/task.hpp>
#include <fc/thread/thread.hpp>

#include <fc/network/http/connection.hpp>
#include <fc/network/ip.hpp>

#include <thread>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <graphene/chain/btc-sidechain/bitcoin_address_object.hpp>

/** Scheme of tasks chain:
 *
 *        EXTERN THREAD            *         MAIN THREAD               *         RPC CLIENT THREAD        *                ZMQ CLIENT THREAD              *
 *                                 *                                   *                                  *                                               *
 *                                 *     `update_tx_approvals` <----------------------------------------------------------- Gets block hash.              *
 *                                 *                 /                 *                                  *          Sets tasks to main thread scheldure. *
 * `update_transactions_status`   <---------------- /                  *                                  *                       |                       *
 *   -`receive_confirmations_tx`   *                                   *                                  *                       |                       *
 *    -`receive_mempool_entry_tx`  *                                   *                                  *                       |                       *
 *    -`get_valid_vins`            *                                   *                                  *                       |                       *
 *     -`receive_confirmations_tx` *                                   *                                  *                       |                       *
 *              \                  *                                   *                                  *                       |                       *
 *               \-------------------->`task_update_conformations`     *                                  *                       |                       *
 *                \-------------------->`task_update_flag_conf`        *                                  *                       |                       *
 *                 \-------------------->`task_update_missing`         *                                  *                       |                       *
 *                                 *                                   *      `receive_full_block` <------------------------------/                       *
 *                                 *                                   *        -`receive_estimated_fee`` *                                               *
 *                                 *                                   *                /                 *                                               *
 *                                 *        `handle_block` <---------------------------/                  *                                               * 
 *                                 *                                   *                                  *                                               *
 */

namespace btc_sidechain {

btc_sidechain_net_manager::btc_sidechain_net_manager(graphene::chain::database& db, const btc_node& btc, const btc_sidechain_signals& signals): 
                                                           _db( db ), _main_tread(fc::thread::current()), _signals(signals)
{
    fc::http::connection conn;
    try {
        conn.connect_to(fc::ip::endpoint(fc::ip::address( btc.ip ), btc.rpc ));
    }
    catch (fc::exception e) {
        elog("No BTC node running at ${ip} or wrong rpc port: ${port}", ("ip", btc.ip) ("port", btc.rpc));
        FC_ASSERT(false);
    }

    // Initialization ZMQ listener with functor, that put task in scheldure
    // This task runs block parser, updates btc tx conformations and gets estimate fee
    _listener = std::unique_ptr<zmq_listener>(new zmq_listener(btc.ip, btc.zmq, [&](const std::string& hash) {
        auto task = [&](const std::string& hash)
        {
            idump(("!!!get_block!!!"));
            std::thread(&bitcoin_rpc_client::receive_full_block, *_bitcoin_client, hash).detach();
            this->update_tx_approvals();
        };
        schedule_task(_main_tread, task, hash); 
    } ) );

    // Task for block parsing
    auto task_get_block = [&](const std::string& block, uint64_t fee) {
        auto task = [&](const std::string& block, uint64_t fee)
        {
            this->handle_block(block, fee);
        };
        schedule_task(_main_tread, task, block, fee);
    };

    _bitcoin_client = std::unique_ptr<bitcoin_rpc_client>(new bitcoin_rpc_client(btc.get_rpc_info(), task_get_block));
}

void btc_sidechain_net_manager::update_tx_approvals()
{
    std::vector<fc::sha256> trx_for_check;
    const auto& confirmations_num = _db.get_sidechain_params().confirmations_num;

    _btc_trx_conformations.safe_for<by_hash>([&]( btc_tx_conf_index::iterator itr_b, btc_tx_conf_index::iterator itr_e ){
        for (auto iter = itr_b; iter != itr_e; iter++) {
            _btc_trx_conformations.modify<by_hash>( iter->transaction_id, [&]( bitcoin_transaction_confirmations& obj ) {
                obj.count_block++;
            });

            if (iter->count_block == confirmations_num) {
               trx_for_check.push_back( iter->transaction_id );
            }
        }
    });
    if(!trx_for_check.empty())
        std::thread(&btc_sidechain_net_manager::update_transactions_status, this, trx_for_check ).detach();
}

void btc_sidechain_net_manager::send_btc_tx( /*const sidechain::bitcoin_transaction& trx*/ )
{
    // TODO: after bitcoin_transaction class add
    FC_ASSERT(!_bitcoin_client->connection_is_not_defined());
    const auto tx_hex = std::string();

    std::thread(&bitcoin_rpc_client::send_btc_tx, *_bitcoin_client, tx_hex).detach();
}

bool btc_sidechain_net_manager::connection_is_not_defined() const 
{
    return _listener->connection_is_not_defined() && _bitcoin_client->connection_is_not_defined();
}

std::vector<info_for_vin> btc_sidechain_net_manager::extract_info_from_block( const std::string& block )
{
    auto parse_amount = [](std::string raw){
        raw.erase(std::remove(raw.begin(), raw.end(), '.'), raw.end());
        return std::stoll(raw);
    };
    std::stringstream ss( block );
    boost::property_tree::ptree _block;
    boost::property_tree::read_json( ss, _block );

    std::vector<info_for_vin> result;

   const auto& addr_idx = _db.template get_index_type<graphene::chain::bitcoin_address_index>().indices().template get<graphene::chain::by_address>();

    for (const auto& tx_child : _block.get_child("tx")) {
        const auto& tx = tx_child.second;

        for ( const auto& o : tx.get_child("vout") ) {
            const auto script = o.second.get_child("scriptPubKey");

            if( !script.count("addresses") ) continue;

            for (const auto& addr : script.get_child("addresses")) { // in which cases there can be more addresses?
                const auto address_base58 = addr.second.get_value<std::string>();

                if( !addr_idx.count( address_base58 ) ) continue;

                info_for_vin vin;
                vin.out.hash_tx = tx.get_child("txid").get_value<std::string>();
                vin.out.amount = parse_amount( o.second.get_child( "value" ).get_value<std::string>() );
                vin.out.n_vout = o.second.get_child( "n" ).get_value<uint32_t>();
                vin.address = address_base58;
                result.push_back( vin );
            }
        }
    }

    return result;
}

void btc_sidechain_net_manager::handle_block(const std::string& block, uint64_t fee)
{
    if(block != "") {
        graphene::chain::infos_for_vins vins_info;
        const auto& vins = extract_info_from_block(block);
        const auto& addr_idx = _db.template get_index_type<graphene::chain::bitcoin_address_index>().indices().template get<graphene::chain::by_address>();
        for(const auto& v : vins) {
            const auto& addr_itr = addr_idx.find(v.address);
            FC_ASSERT(addr_itr != addr_idx.end());
            info_for_vin ifv(prev_out{v.out.hash_tx, v.out.n_vout, v.out.amount}, v.address, addr_itr->address.committee_script);
            vins_info.push_back(ifv);
        }
        _signals.send_vins_op(vins_info, fee);
    } else {
        elog("Received empty string, not Bitcoin block!");
    }
}

std::set<fc::sha256> btc_sidechain_net_manager::get_valid_vins(const fc::sha256& tx_hash)
{
    const auto& confirmations_obj = _btc_trx_conformations.find<btc_sidechain::by_hash>(tx_hash);
    FC_ASSERT(confirmations_obj.valid());

    std::set<fc::sha256> valid_vins;
    for(const auto& v : confirmations_obj->valid_vins) {
        auto confirmations = _bitcoin_client->receive_confirmations_tx(v.str());
        if(confirmations == 0) {
            continue;
        }
        valid_vins.insert( v );
    }
    return valid_vins;
}

void btc_sidechain_net_manager::update_transactions_status(const std::vector<fc::sha256>& trx_for_check )
{
    using trx_hash_and_valid_vins = std::vector< std::pair<fc::sha256, std::set<fc::sha256>> >;
    const auto& confirmations_num = _db.get_sidechain_params().confirmations_num;

    const auto task_update_conformations = [this](fc::sha256 trx, uint64_t confirmations) {
        this->_btc_trx_conformations.modify<by_hash>(trx, [&](bitcoin_transaction_confirmations& obj) {
            obj.count_block = confirmations;
        });
    };
    const auto task_call_issue = [this](const std::vector<fc::sha256> trxs) {
        _signals.send_issue_op(trxs);
    };
    const auto task_call_revert = [this](const trx_hash_and_valid_vins trx_and_valid_vins) {
        _signals.send_revert_op(trx_and_valid_vins);
    };

    trx_hash_and_valid_vins trx_and_valid_vins_to_revert;
    std::vector<fc::sha256> trxs_to_issue;

    for(const auto& trx : trx_for_check) {
        auto confirmations = _bitcoin_client->receive_confirmations_tx(trx.str());
        schedule_task(_main_tread, task_update_conformations, trx, confirmations);

        if(confirmations >= confirmations_num) {
            trxs_to_issue.push_back( trx );
        } else if(confirmations == 0) {
            auto is_in_mempool =  _bitcoin_client->receive_mempool_entry_tx(trx.str());
            std::set<fc::sha256> valid_vins;
            if(!is_in_mempool) {
                valid_vins = get_valid_vins(trx);
                trx_and_valid_vins_to_revert.push_back(std::make_pair(trx, valid_vins));
            }
        }
    }
    if(!trxs_to_issue.empty())
        schedule_task(_main_tread, task_call_issue, trxs_to_issue);
    if(!trx_and_valid_vins_to_revert.empty())
        schedule_task(_main_tread, task_call_revert, trx_and_valid_vins_to_revert);
}

}// namespace btc_sidechain
