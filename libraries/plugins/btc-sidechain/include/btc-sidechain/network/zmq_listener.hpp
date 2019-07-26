#pragma once

#include <string>
#include <vector>

#include <fc/signals.hpp>

#include <zmq.hpp>


namespace btc_sidechain {

/** 
 * @class zmq_listener
 * @breif This class listens for bitcoin node zmq port
 */
class zmq_listener
{
public:

    /**
     * @brief Construct a new zmq listener object
     * 
     * @param _ip ip of node
     * @param _zmq zmq port
     * @param _btc_task callback task
     */
    zmq_listener(std::string _ip, uint32_t _zmq, std::function<void(const std::string&)> _btc_task);

    /// check connection information
    bool connection_is_not_defined() const { return _zmq_port == 0 && _ip.empty(); }

private:
    /// listen for event
    void handle_zmq();

    std::vector<zmq::message_t> receive_multipart();

    std::string _ip;

    uint32_t _zmq_port;

    std::function<void(const std::string&)> _btc_task;

    zmq::context_t _ctx;
   
    zmq::socket_t _socket;
};

} // namespace btc_sidechain
