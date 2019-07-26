#pragma once
#include <graphene/chain/database.hpp>
#include <graphene/net/node.hpp>


#include <btc-sidechain/sidechain_types.ipp>

namespace btc_sidechain {

namespace gc = graphene::chain;
namespace gn = graphene::net;

struct son_creds
{
    graphene::chain::account_id_type account;
    graphene::chain::private_key_type privkey;
    graphene::chain::public_key_type pubkey;
};

class btc_sidechain_service
{
public:
    btc_sidechain_service( gc::database& db, gn::node& node, const std::vector<son_creds>& sons, std::function<bool(void)> is_ready );


private:
    gc::database&             _db;
    gn::node&                 _node;
    std::vector<son_creds>  _sons;
    btc_sidechain_signals     _signals;
    std::function<bool(void)> _is_ready;
};

} // namespace btc_sidechain