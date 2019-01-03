#include <graphene/chain/voting_balance_evaluator.hpp>

namespace graphene { namespace chain {
   // voting_balance_input_evaluator
   void_result voting_balance_input_evaluator::do_evaluate( const voting_balance_input_operation& op )
   {
      database& d = db();

      asset acc_balance = db().get_balance( op.payer, asset_id_type() );
      FC_ASSERT( acc_balance.amount.value >= op.amount, " Amount to input to vesting greater then amount on account balance! " );
      const auto& account_idx = d.get_index_type< account_index >().indices().get< by_id >();
      FC_ASSERT( account_idx.find( op.owner ) != account_idx.end(), " Owner of vesting balance not found! " );
      FC_ASSERT( op.amount > 0, " Zero amount! " );

      return void_result();
   }

   object_id_type voting_balance_input_evaluator::do_apply( const voting_balance_input_operation& op )
   {
      database& d = db();

      const auto& voting_balance_idx = d.get_index_type< voting_balance_index >().indices().get< graphene::chain::by_owner >();
      auto iter = voting_balance_idx.find( op.owner );

      auto id = iter->get_id();

      if( iter == voting_balance_idx.end() ) {
         id = d.create< voting_balance_object >( [&](voting_balance_object& obj) {
            obj.immature_balance = op.amount;
            obj.mature_balance = 0;
            obj.voting_coefficient = GRAPHENE_100_PERCENT;
            obj.votes_in_period.clear();
            obj.confirmed_votes = false;
            obj.owner = op.owner;
         }).get_id();
      } else {
         d.modify( *iter, [&]( voting_balance_object& obj ){
            obj.deposit( op.amount );
         });
      }

      reserve_issue( op );

      return id;
   }

   void voting_balance_input_evaluator::reserve_issue( const voting_balance_input_operation& op )
   {
      database& d = db();

      d.adjust_balance( op.payer, -asset( op.amount ) );
   }


   // voting_balance_output_evaluator
   void_result voting_balance_output_evaluator::do_evaluate( const voting_balance_output_operation& op )
   {
      database& d = db();

      const auto& account_idx = d.get_index_type< account_index >().indices().get< by_id >();
      FC_ASSERT( account_idx.find( op.recipient ) != account_idx.end(), " Recipient of asset not found! " );
      const auto& voting_balance_idx = d.get_index_type< voting_balance_index >().indices().get< graphene::chain::by_owner >();
      auto iter = voting_balance_idx.find( op.owner );
      FC_ASSERT( iter != voting_balance_idx.end(), " Owner of vesting balance not found! " );
      FC_ASSERT( iter->accumulate_balance() >= op.amount, " Not enough asset in vesting! " );
      FC_ASSERT( op.amount > 0, " Zero amount! " );

      return void_result();
   }

   void_result voting_balance_output_evaluator::do_apply( const voting_balance_output_operation& op )
   {
      database& d = db();

      const auto& voting_balance_idx = d.get_index_type<voting_balance_index>().indices().get< graphene::chain::by_owner >();
      auto iter = voting_balance_idx.find( op.owner );

      d.modify( *iter, [&]( voting_balance_object& obj ){
         obj.withdraw( op.amount );
      });

      if( iter->accumulate_balance() == 0 ){
         d.remove( *iter );
      }

      auto amount_to_sub = op.amount - iter->immature_balance;

      d.modify( d.get_period_object(), [&]( period_object& obj ) {
          if( amount_to_sub > 0 ) {
              obj.current_supply -= amount_to_sub;
          }
      });

      drop_issue( op );

      return void_result();
   }

   void voting_balance_output_evaluator::drop_issue( const voting_balance_output_operation& op )
   {
      database& d = db();

      d.adjust_balance( op.recipient, asset( op.amount ) );
   }


} } // namespace graphene::chain