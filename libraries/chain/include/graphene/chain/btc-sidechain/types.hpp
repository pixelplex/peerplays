#pragma once

#include <map>
#include <vector>
#include <string>
#include <fc/reflect/reflect.hpp>

#include <graphene/chain/protocol/types.hpp>

namespace btc_sidechain {

class bitcoin_transaction;

using bytes = std::vector<char>;
using accounts_keys = std::map< graphene::chain::account_id_type, graphene::chain::public_key_type >;
using info_for_vout = graphene::chain::info_for_vout_object;

/// Describes payment types of Bitcoin address
enum class payment_type
{
   NULLDATA,
   P2PK,
   P2PKH,
   P2SH,
   P2WPKH,
   P2WSH,
   P2SH_WPKH,
   P2SH_WSH
};

/**
 * @class prev_out
 * @brief Describes object in prev out chain 
 */
struct prev_out
{
   bool operator!=( const prev_out& obj ) const
   {
      if( this->hash_tx != obj.hash_tx ||
          this->n_vout != obj.n_vout ||
          this->amount != obj.amount )
      {
         return true;
      }
      return false;
   }

   std::string hash_tx;
   uint32_t n_vout;
   uint64_t amount;
};

} // namespace btc_sidechain

FC_REFLECT_ENUM( btc_sidechain::payment_type, (NULLDATA)(P2PK)(P2PKH)(P2SH)(P2WPKH)(P2WSH)(P2SH_WPKH)(P2SH_WSH) );
FC_REFLECT( btc_sidechain::prev_out, (hash_tx)(n_vout)(amount) );
