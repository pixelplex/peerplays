#pragma once

#include <graphene/app/plugin.hpp>
#include <graphene/chain/account_object.hpp>

#include <btc-sidechain/btc_sidechain_service.hpp>
#include <btc-sidechain/btc_network_manager.hpp>

namespace btc_sidechain {

static const auto options_btc_enable            = "btc-sidechain-enabled";
static const auto options_btc_son               = "btc-committee";
static const auto options_btc_sidechain         = "btc-sidechain-enabled";
static const auto options_btc_node_ip           = "btc-node-ip";
static const auto options_btc_node_zmq_port     = "btc-node-zmq-port";
static const auto options_btc_node_rpc_port     = "btc-node-rpc-port";
static const auto options_btc_node_rpc_user     = "btc-node-rpc-user";
static const auto options_btc_node_rpc_password = "btc-node-rpc-password";

using options_descr = boost::program_options::options_description;

class btc_sidechain_plugin: public graphene::app::plugin
{
public:
    btc_sidechain_plugin();
    ~btc_sidechain_plugin();

    std::string plugin_name() const override;
    void plugin_set_program_options( options_descr& command_line_options, options_descr& config_file_options ) override;
    void plugin_initialize( const boost::program_options::variables_map& options ) override;
    void plugin_startup() override;
    void plugin_shutdown() override;

private:
    std::vector<son_creds>                       _sons;
    std::unique_ptr<btc_sidechain_service>       _service;
    std::unique_ptr<btc_sidechain_net_manager>   _btc_net_manager;
    btc_sidechain::btc_node                      _btc_node_info;
};

} // namespace btc_sidechain