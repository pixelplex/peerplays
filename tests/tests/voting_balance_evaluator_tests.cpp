#include <boost/test/unit_test.hpp>
#include "../common/database_fixture.hpp"

#include <graphene/chain/voting_balance_object.hpp>

using namespace graphene::chain;

BOOST_FIXTURE_TEST_SUITE( voting_balance_evaluator_tests, database_fixture )

BOOST_AUTO_TEST_CASE( create_vot_balance_by_evaluator_test ) {
   transaction_evaluation_state context(&db);
   const auto& idx = db.get_index_type<voting_balance_index>().indices().get< graphene::chain::by_owner >();

   BOOST_CHECK( idx.size() == 0 );

   voting_balance_input_operation voting_input;
   voting_input.payer = account_id_type();
   voting_input.owner = account_id_type();
   voting_input.amount = 1000;

   asset acc_balance = db.get_balance( voting_input.payer, asset_id_type() );

   db.apply_operation( context, voting_input );

   BOOST_REQUIRE( idx.size() == 1 );
   const auto& vot_obj = *idx.begin();
   BOOST_CHECK( vot_obj.owner == account_id_type() );
   BOOST_CHECK( vot_obj.mature_balance == 0 );
   BOOST_CHECK( vot_obj.immature_balance == 1000 );
   BOOST_CHECK( vot_obj.voting_coefficient == GRAPHENE_100_PERCENT );

   asset diff_balance = acc_balance - db.get_balance( voting_input.payer, asset_id_type() );
   BOOST_CHECK( diff_balance.amount == 1000 );
}

BOOST_AUTO_TEST_CASE( create_many_vot_balances_test ) {
   auto accounts = create_accounts_with_balances( 20 );
   create_voting_balance( accounts );
}

BOOST_AUTO_TEST_CASE( withdraw_from_vot_balance_test ) {
   INVOKE( create_vot_balance_by_evaluator_test );
   withdraw_all();
}

BOOST_AUTO_TEST_CASE( withdraw_from_many_vot_balance_test ) {
   INVOKE( create_many_vot_balances_test );
   withdraw_all();
}

BOOST_AUTO_TEST_CASE( withdraw_from_vot_mature_balance_test ) {
   INVOKE( create_vot_balance_by_evaluator_test );
   begin_new_period();
   withdraw_all( true );

   auto period = db.get_period_object();
   BOOST_REQUIRE( period.current_supply == 0 );
}

BOOST_AUTO_TEST_CASE( withdraw_from_many_vot_mature_balance_test ) {
   INVOKE( create_many_vot_balances_test );
   begin_new_period();
   withdraw_all( true );

   auto period = db.get_period_object();
   BOOST_REQUIRE( period.current_supply == 0 );
}

BOOST_AUTO_TEST_SUITE_END()
