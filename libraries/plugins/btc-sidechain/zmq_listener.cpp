#include <btc-sidechain/network/zmq_listener.hpp>
#include <boost/algorithm/hex.hpp>
#include <thread>

namespace btc_sidechain {

zmq_listener::zmq_listener(std::string ip, uint32_t zmq, std::function<void(const std::string&)> btc_task): 
                       _ip(ip), _zmq_port(zmq), _btc_task(btc_task), _ctx(1), _socket(_ctx, ZMQ_SUB)
{
   std::thread( &zmq_listener::handle_zmq, this ).detach();
}

std::vector<zmq::message_t> zmq_listener::receive_multipart()
{
   std::vector<zmq::message_t> msgs;

   int32_t more;
   size_t more_size = sizeof(more);
   while (true) {
      zmq::message_t msg;
      _socket.recv(&msg, 0);
      _socket.getsockopt(ZMQ_RCVMORE, &more, &more_size);

      if (!more)
         break;
      msgs.push_back( std::move(msg) );
   }

   return msgs;
}

void zmq_listener::handle_zmq()
{
   _socket.setsockopt(ZMQ_SUBSCRIBE, "hashblock", 0);
   _socket.connect("tcp://" + _ip + ":" + std::to_string( _zmq_port ));

   while (true) {
      auto msg = receive_multipart();
      const auto header = std::string(static_cast<char*>(msg[0].data()), msg[0].size());
      const auto hash = boost::algorithm::hex(std::string(static_cast<char*>(msg[1].data()), msg[1].size()));

      _btc_task(hash);
   }
}

} // namespace btc_sidechain