#pragma once

#include <btc-sidechain/network/zmq_listener.hpp>
#include <btc-sidechain/network/bitcoin_rpc_client.hpp>
#include <graphene/chain/database.hpp>
#include <graphene/chain/btc-sidechain/simple_index.hpp>
#include <btc-sidechain/bitcoin_transaction_confirmations.hpp>

#include <btc-sidechain/sidechain_types.ipp>

namespace btc_sidechain {

/** 
 * @struct btc_node
 * @breif Store all needable for connection to btc node
 */
struct btc_node 
{
    /// ip of btc node
    std::string ip;
    /// zmq port of btc node
    uint32_t zmq;
    /// rpc port of btc node
    uint32_t rpc;
    /// username of rpc
    std::string user;
    /// password of rpc
    std::string password;

    /// get info for rpc in one structure
    bitcoin_rpc_info get_rpc_info() const
    {
        return bitcoin_rpc_info({ ip, rpc, user, password });
    }
};

/// put task in schedule of given thread
template<typename Functor, typename... Args>
void schedule_task(fc::thread& main_thread, Functor functor, Args... args)
{
    auto bind_functor = std::bind(std::forward<Functor>(functor), std::forward<Args>(args)...);
    main_thread.schedule(bind_functor, fc::time_point::now() + fc::seconds(1), "btc_task_manager", fc::priority::max());
}

/** 
 * @class btc_sidechain_net_manager
 * @breif This class manages operations with btc node
 */
class btc_sidechain_net_manager 
{
public:

    /**
     * @brief Construct a new btc sidechain net manager object
     * 
     * @param _db pointer to graphene database object
     * @param btc info of btc node (ip, zmq port and etc.)
     */
    btc_sidechain_net_manager(graphene::chain::database& db, const btc_node& btc, const btc_sidechain_signals& signals);

 private:
    /// Search for trx, which need to be updated and run processing in thread
    void update_tx_approvals();

    /// Send bitcoin transaction
    void send_btc_tx( /*const sidechain::bitcoin_transaction& trx*/ );

    /// Check connection information
    bool connection_is_not_defined() const;

    /// Parse given block
    void handle_block(const std::string& block, uint64_t fee);

    /// Update transactions conformations and etc.
    void update_transactions_status(const std::vector<fc::sha256>& trx_for_check);

    /// Check vins for given tx
    std::set<fc::sha256> get_valid_vins(const fc::sha256& tx_hash);

    /// Extract data from given Bitcoin block
    std::vector<info_for_vin> extract_info_from_block(const std::string& block);

    std::unique_ptr<zmq_listener> _listener;
    std::unique_ptr<bitcoin_rpc_client> _bitcoin_client;
    graphene::chain::database& _db;
    simple_index<btc_tx_conf_index> _btc_trx_conformations;
    fc::thread& _main_tread;
    const btc_sidechain_signals& _signals;
};

}
