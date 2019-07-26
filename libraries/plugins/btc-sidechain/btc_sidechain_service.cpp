#include <btc-sidechain/btc_sidechain_service.hpp>

namespace btc_sidechain {

btc_sidechain_service::btc_sidechain_service( gc::database& db, gn::node& node, const std::vector<son_creds>& sons, std::function<bool(void)> is_ready ):
                                              _db( db ), _node( node ), _sons( sons ), _is_ready( is_ready )
{
    _signals.send_issue_op.connect([&](const std::vector<fc::sha256> trxs){
        idump(("!!!btc_issue_operation!!!"));
    });
    _signals.send_revert_op.connect([&](const std::vector< std::pair<fc::sha256, std::set<fc::sha256>> > trxs_and_valid_visn){
        idump(("!!!btc_revert_operation!!!"));
    });
    _signals.send_vins_op.connect([&](const graphene::chain::infos_for_vins ifvs, const uint64_t fee) {
        idump(("!!!send_vins_op!!!"));
    });
}

} // namespace btc_sidechain