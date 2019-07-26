#pragma once

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>

#include <fc/crypto/sha256.hpp>

#include <graphene/chain/protocol/types.hpp>

using boost::multi_index_container;
using namespace boost::multi_index;

namespace btc_sidechain {

struct bitcoin_transaction_confirmations
{
   bitcoin_transaction_confirmations() = default;

   bitcoin_transaction_confirmations( const fc::sha256& trx_id, const std::set<fc::sha256>& vins ) :
      id( count_id_tx_conf++ ), transaction_id( trx_id ), valid_vins( vins ) {}

   static uint64_t count_id_tx_conf;
   uint64_t id;

   fc::sha256 transaction_id;
   std::set<fc::sha256> valid_vins;

   uint64_t count_block = 0;
};

struct by_hash;

using btc_tx_conf_index = boost::multi_index_container<bitcoin_transaction_confirmations,
   indexed_by<
      ordered_unique<tag<by_hash>, member<bitcoin_transaction_confirmations, fc::sha256, &bitcoin_transaction_confirmations::transaction_id>>
   >
>;

} // namespace btc_sidechain