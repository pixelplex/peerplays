#include <btc-sidechain/btc_sidechain_plugin.hpp>

#include <memory>

namespace btc_sidechain {

namespace bpo = boost::program_options;

btc_sidechain_plugin::btc_sidechain_plugin() {}

btc_sidechain_plugin::~btc_sidechain_plugin() {}

void btc_sidechain_plugin::plugin_set_program_options(options_descr& command_line_options, options_descr & config_file_options)
{
    command_line_options.add_options()
        (options_btc_enable, bpo::bool_switch(), "Run Bitcoin Sidechain service")
        (options_btc_node_ip, bpo::value<std::string>()->implicit_value("127.0.0.1"), "IP address of Bitcoin node")
        (options_btc_node_zmq_port, bpo::value<uint32_t>()->implicit_value(28332), "ZMQ port of Bitcoin node")
        (options_btc_node_rpc_port, bpo::value<uint32_t>()->implicit_value(18332), "RPC port of Bitcoin node")
        (options_btc_node_rpc_user, bpo::value<std::string>(), "Bitcoin RPC user")
        (options_btc_node_rpc_password, bpo::value<std::string>(), "Bitcoin RPC password")
        (options_btc_son, bpo::value<std::vector<std::string>>()->composing()->multitoken(), "Tuple of [account_id, WIF private key] (may specify multiple times)")
        ;
    config_file_options.add(command_line_options);
}

std::string btc_sidechain_plugin::plugin_name() const { return "btc_sidechain"; }

void btc_sidechain_plugin::plugin_initialize(const boost::program_options::variables_map& options)
{
    try {
        if( !options[options_btc_enable].as<bool>() )
            return;

        ilog( "Bitcoion Sidechain plugin: initialize" );

        for( auto opt: {options_btc_node_ip, options_btc_node_zmq_port, options_btc_node_rpc_port, 
                        options_btc_node_rpc_user, options_btc_node_rpc_password, options_btc_son} )
        {
            FC_ASSERT( options.count(opt), "Cannot start sidechain service without ${opt} option.", ("opt", opt) );
        }

        const auto& acc_index = graphene::app::plugin::database().get_index_type<graphene::chain::account_index>().indices().get<graphene::chain::by_id>();
        const auto btc_sons_plugin = options[options_btc_son].as<std::vector<std::string>>();
        for( const auto& son_string: btc_sons_plugin ) {
            son_creds son;
            const auto son_pair = graphene::app::dejsonify<std::pair<graphene::chain::account_id_type, std::string>>(son_string);;
            son.privkey = graphene::chain::private_key_type::regenerate( fc::sha256( son_pair.second ) );
            son.pubkey = son.privkey.get_public_key();
            son.account = son_pair.first;

            auto account_iter = acc_index.find(son.account);
            FC_ASSERT( account_iter != acc_index.end(), "Such account not found! Wrong ID. Account id: ${id}", ("id", son.account) );

            auto owner_pubkeys = account_iter->owner.get_keys();
            auto active_pubkeys = account_iter->active.get_keys();
            FC_ASSERT( std::find( owner_pubkeys.begin(), owner_pubkeys.end(), son.pubkey ) != owner_pubkeys.end(), "Such account not found! Account id: ${id}", ("id", son.account) );
            FC_ASSERT( std::find( active_pubkeys.begin(), active_pubkeys.end(), son.pubkey ) != active_pubkeys.end(), "Such account not found! Account id: ${id}", ("id", son.account) );

            _sons.emplace_back(son);
        }

        _btc_node_info.ip = options.at(options_btc_node_ip).as<std::string>();
        _btc_node_info.zmq = options.at(options_btc_node_zmq_port).as<uint32_t>();
        _btc_node_info.rpc = options.at(options_btc_node_rpc_port).as<uint32_t>();
        _btc_node_info.user = options.at(options_btc_node_rpc_user).as<std::string>();
        _btc_node_info.password = options.at(options_btc_node_rpc_password).as<std::string>();

        // _btc_net_manager = std::make_unique<btc_sidechain_net_manager>(_db, btc_node_info, btc_signals);

        _service = std::unique_ptr<btc_sidechain_service>( new btc_sidechain_service( graphene::app::plugin::database(), p2p_node(), _sons, [this](){
            return !app().is_finished_syncing() && app().p2p_node();
        } ) );

    } FC_LOG_AND_RETHROW()
}

void btc_sidechain_plugin::plugin_startup()
{
    //
}

void btc_sidechain_plugin::plugin_shutdown()
{
    //
}

} // namespace btc_sidechain