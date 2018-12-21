#pragma once

#include <graphene/chain/database.hpp>
#include <graphene/chain/evaluator.hpp>
#include <graphene/chain/exceptions.hpp>

#include <graphene/chain/voting_balance_object.hpp>

namespace graphene { namespace chain {

   class voting_balance_input_evaluator : public evaluator<voting_balance_input_evaluator>
   {
   public:
      typedef voting_balance_input_operation operation_type;

      void_result do_evaluate(const voting_balance_input_operation& op);

      object_id_type do_apply(const voting_balance_input_operation& op);

      void reserve_issue( const voting_balance_input_operation& op );
   };

   class voting_balance_output_evaluator : public evaluator<voting_balance_output_evaluator>
   {
   public:
      typedef voting_balance_output_operation operation_type;

      void_result do_evaluate(const voting_balance_output_operation& op);

      void_result do_apply(const voting_balance_output_operation& op);

      void drop_issue( const voting_balance_output_operation& op );
   };

} } // graphene::chain
