#pragma once

#include <graphene/chain/protocol/asset.hpp>
#include <graphene/db/object.hpp>
#include <graphene/db/generic_index.hpp>

namespace graphene { namespace chain {

   // Exeblock: vesting balance class
   class voting_balance_object: public abstract_object<voting_balance_object> 
   {
      public:
         static const uint8_t space_id = protocol_ids;
         static const uint8_t type_id  = voting_balance_object_type;

         voting_balance_id_type get_id() const { return id; }

         // owner of vesting balance
         account_id_type owner;

         share_type mature_balance;

         share_type immature_balance;

         uint16_t voting_coefficient;

         set<vote_id_type> votes_in_period;
         bool confirmed_votes;

         bool is_allowed_to_award() const { return confirmed_votes || votes_in_period.size() > 0; }

         // send amount to vesting
         void deposit( share_type amount ) { immature_balance += amount; }

         void deposit_award( share_type amount ) { mature_balance += amount; }

         void decrease_coefficient( uint16_t amount ) {
            if( voting_coefficient < amount ){
               voting_coefficient = 0;
            }
            else {
               voting_coefficient -= amount;
            }
         }

         // withdraw amount from vesting ( first will be decremented immature_balance, then mature_balance )
         void withdraw( share_type amount ) {
            if( amount > immature_balance ) {
               amount -= immature_balance;
               immature_balance = 0;
               mature_balance -= amount;
            } else {
               immature_balance -= amount;
            }
         }

         // send immature_balance to mature_balance
         void update_mature_balance() {
            mature_balance += immature_balance;
            immature_balance = 0;
         }

         // get all vesting balance
         share_type accumulate_balance() const { return mature_balance + immature_balance; }

         bool is_mature_balance_not_null() const { return mature_balance > 0; }
         bool is_immature_balance_not_null() const { return immature_balance > 0; }
   };

struct by_owner;
struct by_mature_balance;
struct by_immature_balance;

typedef boost::multi_index_container<
   voting_balance_object,
   indexed_by<
      ordered_unique< tag< by_id >, member< object, object_id_type, &object::id > >,
      ordered_unique< tag< by_owner >, member< voting_balance_object, account_id_type, &voting_balance_object::owner > >,
      ordered_non_unique< tag< by_mature_balance >, const_mem_fun< voting_balance_object, bool, &voting_balance_object::is_mature_balance_not_null >, std::greater<bool> >,
      ordered_non_unique< tag< by_immature_balance >, const_mem_fun< voting_balance_object, bool, &voting_balance_object::is_immature_balance_not_null >, std::greater<bool> >
   >
> voting_balace_multi_index_container;

typedef generic_index< voting_balance_object, voting_balace_multi_index_container > voting_balance_index;

} }// graphene::chain

FC_REFLECT_DERIVED( graphene::chain::voting_balance_object,
                    (graphene::chain::object),
                    (owner)
                    (mature_balance)
                    (immature_balance)
                    (voting_coefficient)
                    (votes_in_period)
                    (confirmed_votes)
                  )
