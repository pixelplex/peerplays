#pragma once
#include <graphene/chain/protocol/base.hpp>

namespace graphene { namespace chain {

   // PeerPlays: voting balance input operation
   struct voting_balance_input_operation: public base_operation {
      struct fee_parameters_type{
         uint64_t fee             = 20 * GRAPHENE_BLOCKCHAIN_PRECISION;
         uint32_t price_per_kbyte = 10; /// only required for large memos.
      };

      asset fee;

      // payer for vesting balance
      account_id_type payer;
      // owner of vesting balance
      account_id_type owner;

      share_type amount;

      account_id_type fee_payer()const { return payer; }
      void            validate()const {}
      share_type      calculate_fee(const fee_parameters_type& k )const { return k.fee; }
   };

   // PeerPlays: voting balance output operation
   struct voting_balance_output_operation: public base_operation {
      struct fee_parameters_type{
         uint64_t fee             = 20 * GRAPHENE_BLOCKCHAIN_PRECISION;
         uint32_t price_per_kbyte = 10; /// only required for large memos.
      };

      asset fee;

      // owner of vesting balance
      account_id_type owner;
      // recipient of vesting balance withdraw
      account_id_type recipient;

      share_type amount;

      account_id_type fee_payer()const { return owner; }
      void            validate()const {}
      share_type      calculate_fee(const fee_parameters_type& k )const { return k.fee; }
   };

}} // graphene::chain

FC_REFLECT( graphene::chain::voting_balance_input_operation::fee_parameters_type, (fee)(price_per_kbyte) )
FC_REFLECT( graphene::chain::voting_balance_input_operation, (fee)(payer)(owner)(amount) )

FC_REFLECT( graphene::chain::voting_balance_output_operation::fee_parameters_type, (fee)(price_per_kbyte) )
FC_REFLECT( graphene::chain::voting_balance_output_operation, (fee)(owner)(recipient)(amount) )