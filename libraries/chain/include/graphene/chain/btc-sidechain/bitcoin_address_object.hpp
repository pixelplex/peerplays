#pragma once

#include <graphene/db/generic_index.hpp>
#include <graphene/chain/committee_member_object.hpp>

#include <graphene/chain/btc-sidechain/bitcoin_address.hpp>

namespace graphene { namespace chain {

/**
 *  @brief acts as an address deposit.
 *  @ingroup object
 *
 *  It needs to validate transactions between ECHO and BTC.
 *  For this purpose commettees's public keys are used to generate multisig address
 *  for ECHO account. Then them private keys are used to sign transactions of ECHO
 *  account.
 */
class bitcoin_address_object : public abstract_object<bitcoin_address_object>
{
   public:
      static const uint8_t space_id = protocol_ids;
      static const uint8_t type_id  = bitcoin_address_object_type;

      bitcoin_address_id_type get_id()const { return id; }

      /// @return true if count of invalid public keys after changer commettee memebers less then min value
      bool valid() { return count_invalid_pub_key < 5 /*BTC_SIDECHAIN_NUMBER_INVALID_KEYS*/; }

      /// Get a deposit address
      std::string get_address() const { return address.address; }

      /// Update count of invalid public keys after changing commettee members
      void update_count_invalid_pub_key( const btc_sidechain::accounts_keys& incoming_committe_keys ) {
         count_invalid_pub_key = incoming_committe_keys.size() - address.count_intersection( incoming_committe_keys );
      }

      /// owner of deposit address 
      account_id_type      owner;
      btc_sidechain::btc_multisig_segwit_address address; ///< represents a deposit address
      uint8_t count_invalid_pub_key; ///< count of committees member whose public keys were used in address generation
};

struct by_address;
struct by_owner;

/**
 * @ingroup object_index
 */
typedef boost::multi_index_container<
   bitcoin_address_object,
   indexed_by<
      ordered_unique< tag<by_id>, member< object, object_id_type, &object::id > >,
      ordered_unique< tag<by_address>, const_mem_fun< bitcoin_address_object, std::string, &bitcoin_address_object::get_address > >,
      ordered_non_unique< tag<by_owner>, member< bitcoin_address_object, account_id_type, &bitcoin_address_object::owner > >
   >
> bitcoin_address_multi_index_container;

/**
 * @ingroup object_index
 */
typedef generic_index<bitcoin_address_object, bitcoin_address_multi_index_container> bitcoin_address_index;

} } // namespace graphene::chain

FC_REFLECT_DERIVED( graphene::chain::bitcoin_address_object, (graphene::chain::object), (owner)(address)(count_invalid_pub_key) )

