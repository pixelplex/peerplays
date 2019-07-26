#include <btc-sidechain/network/bitcoin_rpc_client.hpp>

#include <sstream>
#include <utility>

#include <fc/network/ip.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace btc_sidechain {

bitcoin_rpc_client::bitcoin_rpc_client(const bitcoin_rpc_info& btc_rpc, std::function<void(const std::string&, uint64_t)> get_block):
                                        _btc_rpc(btc_rpc), _get_block(get_block)
{
    _authorization.key = "Authorization";
    _authorization.val = "Basic " + fc::base64_encode(btc_rpc.user + ":" + btc_rpc.password);
}

void bitcoin_rpc_client::receive_full_block(const std::string& block_hash)
{
    fc::http::connection conn;
    conn.connect_to(fc::ip::endpoint(fc::ip::address(_btc_rpc.ip), _btc_rpc.rpc_port));

    const auto url = "http://" + _btc_rpc.ip + ":" + std::to_string(_btc_rpc.rpc_port) + "/rest/block/" + block_hash + ".json";

    const auto reply = conn.request("GET", url);
    if (reply.status != 200) {
        elog("Cant't receive Bitcoin block: ${hash}. HTTPS reply status was not 200.", ("hash", block_hash));
        return;
    }

    ilog("Receive Bitcoin block: ${hash}", ("hash", block_hash));
    _get_block(std::string(reply.body.begin(), reply.body.end()), receive_estimated_fee());
}

uint32_t bitcoin_rpc_client::receive_confirmations_tx(const std::string& tx_hash)
{
    const auto body = std::string("{\"jsonrpc\": \"1.0\", \"id\":\"curltest\", \"method\": \"getrawtransaction\", \"params\": [") +
                      std::string("\"") + tx_hash + std::string("\"") + ", " + "true" + std::string("] }");

    const auto reply = send_post_request(body);

    if (reply.status != 200) {
        elog("Cant't receive Bitcoin transaction: ${hash}. HTTP reply status was not 200.", ("hash", tx_hash));
        return 0;
    }

    const auto result = std::string(reply.body.begin(), reply.body.end());
   
    std::stringstream ss(result);
    boost::property_tree::ptree tx;
    boost::property_tree::read_json(ss, tx);

    if (tx.count("result")) {
        if (tx.get_child("result").count("confirmations")) {
            return tx.get_child("result").get_child("confirmations").get_value<int64_t>();
        }
    }
    return 0;
}

bool bitcoin_rpc_client::receive_mempool_entry_tx(const std::string& tx_hash)
{
    const auto body = std::string("{\"jsonrpc\": \"1.0\", \"id\":\"curltest\", \"method\": \"getmempoolentry\", \"params\": [") +
                      std::string("\"") + tx_hash + std::string("\"") + std::string("] }");

    const auto reply = send_post_request(body);

    if (reply.status != 200)
        return false;
   
    return true;
}

uint64_t bitcoin_rpc_client::receive_estimated_fee()
{
    static const auto confirmation_target_blocks = 6;

    const auto body = std::string("{\"jsonrpc\": \"1.0\", \"id\":\"estimated_feerate\", \"method\": \"estimatesmartfee\", \"params\": [") +
                      std::to_string(confirmation_target_blocks) + std::string("] }");

    const auto reply = send_post_request(body);

    if (reply.status != 200) {
        elog("Cant't receive Bitcoin estimated fee. HTTP reply status was not 200. Setting fee in zero.");
        return 0;
    }

    std::stringstream ss(std::string(reply.body.begin(), reply.body.end()));
    boost::property_tree::ptree json;
    boost::property_tree::read_json(ss, json);

    if (json.count("result"))
        if (json.get_child("result").count("feerate")) {
            auto feerate_str = json.get_child("result").get_child("feerate").get_value<std::string>();
            feerate_str.erase(std::remove(feerate_str.begin(), feerate_str.end(), '.'), feerate_str.end());
            return std::stoll(feerate_str);
        }

    elog("Can't receive Bitcoin esitimated fee. Not \"result\" in response! Setting fee in zero.");
    return 0;
}

void bitcoin_rpc_client::send_btc_tx(const std::string& tx_hex)
{
    const auto body = std::string("{\"jsonrpc\": \"1.0\", \"id\":\"send_tx\", \"method\": \"sendrawtransaction\", \"params\": [") +
                     std::string("\"") + tx_hex + std::string("\"") + std::string("] }");

    const auto reply = send_post_request(body);

    if(reply.body.empty())
        return;

    std::string reply_str(reply.body.begin(), reply.body.end());

    std::stringstream ss(reply_str);
    boost::property_tree::ptree json;
    boost::property_tree::read_json(ss, json);

    if(reply.status == 200) {
        idump((tx_hex));
        return;
    }
    else if (json.count("error") && !json.get_child("error").empty()) {
        const auto error_code = json.get_child("error").get_child("code").get_value<int>();
        if(error_code == -27) // transaction already in block chain
            return;

        elog("BTC tx is not sent! Reply: ${msg}", ("msg", reply_str));
    }
}

bool bitcoin_rpc_client::connection_is_not_defined() const
{
    return _btc_rpc.ip.empty() || _btc_rpc.rpc_port == 0 || _btc_rpc.user.empty() || _btc_rpc.password.empty();
}

fc::http::reply bitcoin_rpc_client::send_post_request(std::string body)
{
    fc::http::connection conn;
    conn.connect_to(fc::ip::endpoint(fc::ip::address(_btc_rpc.ip), _btc_rpc.rpc_port));

    const auto url = "http://" + _btc_rpc.ip + ":" + std::to_string(_btc_rpc.rpc_port);

    return conn.request("POST", url, body, fc::http::headers{_authorization});
}

} // namespace btc_sidechain