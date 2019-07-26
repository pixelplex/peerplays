#pragma once
#ifndef BTC_SIDECHAIN_GUARD
#define BTC_SIDECHAIN_GUARD


// TODO: TEMP
namespace graphene { namespace chain {
using infos_for_vins = std::vector<btc_sidechain::info_for_vin>;
// class info_for_vin{};
}}

namespace btc_sidechain {

/**
 * @struct btc_sidechain_signals
 * @brief Describes Bitcoin sidechain signals called in network manager
 */
struct btc_sidechain_signals {
    /// Send issue op
    fc::signal<void(const std::vector<fc::sha256>)> send_issue_op;
    
    /// Send revert op
    fc::signal<void(const std::vector< std::pair<fc::sha256, std::set<fc::sha256>> >)> send_revert_op;

    /// Send vins info to echo
    fc::signal<void(const graphene::chain::infos_for_vins, const uint64_t)> send_vins_op;
};

} // namespace btc_sidechain

#endif
