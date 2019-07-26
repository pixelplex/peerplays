#pragma once

#include <string>
#include <fc/network/http/connection.hpp>

namespace btc_sidechain {

/** 
 * @struct bitcoin_rpc_info
 * @breif Store all needable for rpc connection
 */
struct bitcoin_rpc_info 
{
    /// ip of btc node
    std::string ip;
    /// rpc port of btc node
    uint32_t rpc_port;
    /// username for rpc
    std::string user;
    /// password for rpc
    std::string password;
};

/** 
 * @class bitcoin_rpc_client
 * @breif This class works with btc node rpc by socket connections
 */
class bitcoin_rpc_client
{

public:
    /**
     * @brief Construct a new bitcoin rpc client object
     * 
     * @param _btc_rpc bitcoin_rpc_info struct with ip, port and etc.
     * @param _get_block Callback for block receive
     */
    bitcoin_rpc_client(const bitcoin_rpc_info& btc_rpc, std::function<void(const std::string&, uint64_t)> get_block);

    /**
     * @brief Connect to btc node and get block. Method work with task callback
     * 
     * @param block_hash hash of block
     */
    void receive_full_block(const std::string& block_hash);

    /**
     * @brief Connect to btc node and get amount of trx conformations
     * 
     * @param tx_hash hash of btc transaction
     * @return uint32_t amount of conformations
     */
    uint32_t receive_confirmations_tx(const std::string& tx_hash);

    /**
     * @brief Connect to btc node and check is trx in mempool
     * 
     * @param tx_hash hash of btc transaction
     * @return true transaction in mempool
     * @return false transaction not in mempool
     */
    bool receive_mempool_entry_tx(const std::string& tx_hash);

    /**
     * @brief Connect to btc node and send transaction
     * 
     * @param tx_hex raw btc transaction
     */
    void send_btc_tx(const std::string& tx_hex);

    /**
     * @brief Check connection settings
     * 
     * @return true enought info for connection
     * @return false not enought info for connection
     */
    bool connection_is_not_defined() const;

private:

    /**
     * @brief Connect to btc node and get estimated fee.
     * @return estimated fee in uint64
     */
    uint64_t receive_estimated_fee();

    fc::http::reply send_post_request(std::string body);

    bitcoin_rpc_info _btc_rpc;

    std::function<void(const std::string&, uint64_t)> _get_block;

    fc::http::header _authorization;
};

} // namespace btc_sidechain
