#include <boost/test/unit_test.hpp>
#include "../common/database_fixture.hpp"

#include <graphene/chain/voting_balance_object.hpp>

using namespace graphene::chain;
using namespace graphene::chain::test;

BOOST_FIXTURE_TEST_SUITE( voting_balance_tests, database_fixture )

void clear_preiod_object( database_fixture& df ) {
   auto& period_obj = df.db.get_period_object();
   df.db.modify( period_obj, [&]( period_object& obj ){
      obj.current_supply = 0;
      obj.whole_period_budget = 0;
      obj.witness_pool = 0;
   });
}

share_type cut_voting_fee( share_type amount )
{
   return amount * 10 * GRAPHENE_1_PERCENT / GRAPHENE_100_PERCENT;
}

BOOST_AUTO_TEST_CASE( create_vot_balance_test ) {
   const auto& idx = db.get_index_type<voting_balance_index>().indices().get< graphene::chain::by_owner >();

   BOOST_CHECK( idx.size() == 0 );

   create_voting_balances( 1 );

   BOOST_CHECK( idx.size() == 1 );

   delete_all_vesting();
}

BOOST_AUTO_TEST_CASE( create_many_vot_balances_test ) {
   const auto& idx = db.get_index_type<voting_balance_index>().indices().get< graphene::chain::by_owner >();

   BOOST_CHECK( idx.size() == 0 );

   create_voting_balances( 15 );

   BOOST_CHECK( idx.size() == 15 );

   for( uint32_t ind = 0; ind < 15; ++ind ) {
      BOOST_CHECK( idx.find( account_id_type( ind ) ) != idx.end() );
   }

   delete_all_vesting();
}

BOOST_AUTO_TEST_CASE( index_by_mature_balance_test ) {
   const auto& idx = db.get_index_type<voting_balance_index>().indices().get< graphene::chain::by_mature_balance >();

   BOOST_CHECK( idx.size() == 0 );

   create_voting_balances( 10, true );

   BOOST_CHECK( idx.size() == 10 );

   create_voting_balances( 5, false, true, 10 );

   BOOST_CHECK( idx.size() == 15 );

   auto iter = idx.begin();

   for( uint32_t ind = 0; ind < 5; ++ind ) {
       BOOST_CHECK( iter->mature_balance != 0 );
       ++iter;
   }

   for( uint32_t ind = 5; ind < 15; ++ind ) {
       BOOST_CHECK( iter->mature_balance == 0 );
       ++iter;
   }

   delete_all_vesting();
}

BOOST_AUTO_TEST_CASE( index_by_immature_balance_test ) {
   const auto& idx = db.get_index_type<voting_balance_index>().indices().get< graphene::chain::by_immature_balance >();

   BOOST_CHECK( idx.size() == 0 );

   create_voting_balances( 10, false, true );

   BOOST_CHECK( idx.size() == 10 );

   create_voting_balances( 5, true, false, 10 );

   BOOST_CHECK( idx.size() == 15 );

   auto iter = idx.begin();

   for( uint32_t ind = 0; ind < 5; ++ind ) {
       BOOST_CHECK( iter->immature_balance != 0 );
       ++iter;
   }

   for( uint32_t ind = 5; ind < 15; ++ind ) {
       BOOST_CHECK( iter->immature_balance == 0 );
       ++iter;
   }

   delete_all_vesting();
}

BOOST_AUTO_TEST_CASE( mature_balance_test ) {
   const auto& idx = db.get_index_type<voting_balance_index>().indices().get< graphene::chain::by_id >();

   create_voting_balances(  1, false, true, 10000 );
   create_voting_balances(  1, false, false, 20000 );
   create_voting_balances(  1, false, true, 30000 );

   const voting_balance_object& acc_1 = *(idx.begin());
   const voting_balance_object& acc_2 = *(++idx.begin());
   const voting_balance_object& acc_3 = *(--idx.end());

   set_account_voting( acc_1 );
   set_account_voting( acc_2 );
   set_whole_period_budget( 400 );

   begin_new_period();

   BOOST_CHECK( acc_1.mature_balance == 10166 );
   BOOST_CHECK( acc_2.mature_balance == 40233 );
   BOOST_CHECK( acc_3.mature_balance == 30100 );

   set_account_voting( acc_1 );
   set_whole_period_budget( 1300 );

   create_voting_balances( 1, true, false, 15000 );
   const voting_balance_object& acc_4 = *(--idx.end());

   begin_new_period();

   BOOST_CHECK( acc_1.mature_balance == 10330 );
   BOOST_CHECK( acc_2.mature_balance == 40233 );
   BOOST_CHECK( acc_3.mature_balance == 30100 );
   BOOST_CHECK( acc_4.mature_balance == 15000 );

   delete_all_vesting();
   clear_preiod_object( *this );
}

BOOST_AUTO_TEST_CASE( voting_coefficient_test ) {
   const auto& idx = db.get_index_type<voting_balance_index>().indices().get< graphene::chain::by_id >();

   create_voting_balances( 1, false, true, 10000 );
   create_voting_balances( 1, false, false, 20000 );
   create_voting_balances( 1, false, true, 30000 );

   const voting_balance_object& acc_1 = *(idx.begin());
   const voting_balance_object& acc_2 = *(++idx.begin());
   const voting_balance_object& acc_3 = *(--idx.end());

   set_account_voting( acc_1 );
   set_account_voting( acc_2 );
   set_whole_period_budget( 400 );

   begin_new_period();

   BOOST_CHECK( acc_1.voting_coefficient == GRAPHENE_100_PERCENT );
   BOOST_CHECK( acc_2.voting_coefficient == GRAPHENE_100_PERCENT );
   BOOST_CHECK( acc_3.voting_coefficient == 90*GRAPHENE_1_PERCENT );

   set_account_voting( acc_1 );
   set_whole_period_budget( 1300 );

   create_voting_balances( 1, true, false, 15000 );
   const voting_balance_object& acc_4 = *(--idx.end());

   begin_new_period();

   BOOST_CHECK( acc_1.voting_coefficient == GRAPHENE_100_PERCENT );
   BOOST_CHECK( acc_2.voting_coefficient == 90*GRAPHENE_1_PERCENT );
   BOOST_CHECK( acc_3.voting_coefficient == 80*GRAPHENE_1_PERCENT );
   BOOST_CHECK( acc_4.voting_coefficient == GRAPHENE_100_PERCENT );

   for ( auto i = 0; i < 10; i++ )
      begin_new_period();

   BOOST_CHECK( acc_1.voting_coefficient == 0 );
   BOOST_CHECK( acc_2.voting_coefficient == 0 );
   BOOST_CHECK( acc_3.voting_coefficient == 0 );
   BOOST_CHECK( acc_4.voting_coefficient == 0 );

   delete_all_vesting();
   clear_preiod_object( *this );
}

BOOST_AUTO_TEST_CASE( period_current_supply_test ) {
   const auto& idx = db.get_index_type<voting_balance_index>().indices().get< graphene::chain::by_id >();
   auto& period_obj = db.get_period_object();

   create_voting_balances( 1, true, false, 15000 );

   begin_new_period();

   BOOST_CHECK( period_obj.current_supply == 15000 );

   create_voting_balances( 1, true, false, 8500 );
   set_whole_period_budget( 3000 );

   begin_new_period();

   BOOST_CHECK( period_obj.current_supply == 23500 );

   set_account_voting( *idx.begin() );
   set_account_voting( *(++idx.begin()) );
   set_whole_period_budget( 2000 );

   begin_new_period();

   BOOST_CHECK( period_obj.current_supply == 25371 );

   create_voting_balances( 1, true, false, 8300 );

   begin_new_period();

   BOOST_CHECK( period_obj.current_supply == 33671 );

   delete_all_vesting();
   clear_preiod_object( *this );
}

BOOST_AUTO_TEST_CASE( period_witness_pool_test ) {
   const auto& idx = db.get_index_type<voting_balance_index>().indices().get< graphene::chain::by_id >();
   auto& period_obj = db.get_period_object();

   create_voting_balances( 1, true, false, 15000 );

   begin_new_period();

   BOOST_CHECK( period_obj.witness_pool == 0 );

   create_voting_balances( 1, true, false, 8500 );
   set_whole_period_budget( 1800 );

   begin_new_period();

   BOOST_CHECK( period_obj.witness_pool == 1800 );

   set_account_voting( *idx.begin() );
   set_whole_period_budget( 2000 );

   begin_new_period();

   BOOST_CHECK( period_obj.witness_pool == 852 );

   set_account_voting( *idx.begin() );
   set_account_voting( *(++idx.begin()) );
   set_whole_period_budget( 4000 );

   begin_new_period();

   BOOST_CHECK( period_obj.witness_pool == 139 );

   delete_all_vesting();
   clear_preiod_object( *this );
}

BOOST_AUTO_TEST_CASE( whole_period_budget_test ) {
   try
   {
      ACTORS((alice)(bob));

      auto& schedule = db.get_global_properties().parameters.current_fees;
      auto& period_obj = db.get_period_object();
      share_type fee;
      share_type budget = 0;

      transfer( committee_account, alice_id, asset( 1000000 * asset::scaled_precision( asset_id_type()(db).precision ) ) );

      enable_fees();

      {
         auto fees_to_pay = schedule->get<asset_create_operation>();
         signed_transaction tx;
         asset_create_operation op;
         op.issuer = alice_id;
         op.symbol = "ALICE";
         op.common_options.core_exchange_rate = asset( 1 ) / asset( 1, asset_id_type( 1 ) );
         op.fee = schedule->calculate_fee(op);
         tx.operations.push_back( op );
         set_expiration( db, tx );
         sign( tx, alice_private_key );
         PUSH_TX( db, tx );
         fee = op.fee.amount;
      }
      budget += cut_voting_fee(fee) / 2;

      BOOST_CHECK_EQUAL( period_obj.whole_period_budget.value, budget.value );
      verify_asset_supplies( db );

      {
         signed_transaction tx;
         account_upgrade_operation op;
         op.account_to_upgrade = alice_id;
         op.upgrade_to_lifetime_member = true;
         op.fee = schedule->calculate_fee(op);
         tx.operations.push_back( op );
         set_expiration( db, tx );
         sign( tx, alice_private_key );
         PUSH_TX( db, tx );
         fee = op.fee.amount;
      }
      budget += cut_voting_fee(fee);

      BOOST_CHECK_EQUAL( period_obj.whole_period_budget.value, budget.value );
      verify_asset_supplies( db );

      {      
         signed_transaction tx;
         transfer_operation op;
         op.from = alice_id;
         op.to = bob_id;
         op.amount = asset( 10000 );
         op.fee = schedule->calculate_fee(op);
         tx.operations.push_back(op);
         set_expiration( db, tx );
         sign( tx, alice_private_key );
         PUSH_TX( db, tx );
         fee = op.fee.amount;
      }
      budget += cut_voting_fee(fee);

      BOOST_CHECK_EQUAL( period_obj.whole_period_budget.value, budget.value );
      verify_asset_supplies( db );

      {
         signed_transaction tx;
         worker_create_operation op;
         op.owner = alice_id;
         op.daily_pay = 1000;
         op.initializer = vesting_balance_worker_initializer(1);
         op.work_begin_date = db.head_block_time() + 10;
         op.work_end_date = op.work_begin_date + fc::days(2);
         op.fee = schedule->calculate_fee(op);
         tx.operations.push_back(op);
         set_expiration( db, tx );
         sign( tx, alice_private_key );
         PUSH_TX( db, tx );
         fee = op.fee.amount;
      }
      budget += cut_voting_fee(fee);

      BOOST_CHECK_EQUAL( period_obj.whole_period_budget.value, budget.value );
      verify_asset_supplies( db );

   }
   catch( const fc::exception& e )
   {
      edump((e.to_detail_string()));
      throw;
   }
}

BOOST_AUTO_TEST_SUITE_END()
