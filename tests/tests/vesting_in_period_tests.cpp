#include <boost/test/unit_test.hpp>
#include "../common/database_fixture.hpp"

#include <graphene/chain/voting_balance_object.hpp>
#include <graphene/chain/committee_member_object.hpp>

using namespace graphene::chain;

BOOST_FIXTURE_TEST_SUITE( vesting_in_period_tests, database_fixture )

/* PREAMBLE  - creates nathan accounts and clear `nathan.options.votes`
 *           - creates `voting_balance_index`
 *           - creates `fc::ecc::private_key nathan_new_key` and `public_key_type key_id`
 *           - creates `active_committee_members`
*/

#define PREAMBLE \
   const account_object& nathan = create_account("nathan", init_account_pub_key); \
   const fc::ecc::private_key nathan_new_key = fc::ecc::private_key::generate(); \
   const public_key_type key_id = nathan_new_key.get_public_key(); \
   const auto& active_committee_members = db.get_global_properties().active_committee_members; \
   const auto& idx = db.get_index_type< voting_balance_index >().indices().get< graphene::chain::by_owner >(); \
   create_voting_balance_without_new_block( nathan.get_id(), 5000, true, true ); \
   db.modify( nathan, []( account_object& obj ) { \
      obj.options.votes.clear(); \
   }); 

void vote_for( database_fixture &df, const flat_set<vote_id_type> &votes, const account_object &nathan, const public_key_type& key_id ) {
   df.trx.operations.clear();
   account_update_operation op;
   op.account = nathan.id;
   op.owner = authority(2, key_id, 1, df.init_account_pub_key, 1);
   op.active = authority(2, key_id, 1, df.init_account_pub_key, 1);
   op.new_options = nathan.options;
   op.new_options->votes = votes;
   op.new_options->num_committee = 2;
   df.trx.operations.push_back(op);

   PUSH_TX( df.db, df.trx, ~0 );
}

BOOST_AUTO_TEST_CASE( one_vote_test ) {
   try {
      PREAMBLE;
      
      flat_set<vote_id_type> votes({active_committee_members[0](db).vote_id, active_committee_members[5](db).vote_id});
      vote_for( *this, votes, nathan, key_id );

      BOOST_REQUIRE( idx.size() == 1 );
      const auto& vot_obj = *idx.begin();

      for( auto vote: votes ) {
         BOOST_CHECK( vot_obj.votes_in_period.find( vote ) != vot_obj.votes_in_period.end() );
      }
   } catch (fc::exception& e) {
      edump((e.to_detail_string()));
      throw;
   }
}

BOOST_AUTO_TEST_CASE( bring_back_vote_test ) {
   try {
      PREAMBLE;
      
      flat_set<vote_id_type> votes( { active_committee_members[0](db).vote_id, active_committee_members[5](db).vote_id, active_committee_members[6](db).vote_id } );
      vote_for( *this, votes, nathan, key_id );

      BOOST_REQUIRE( idx.size() == 1 );
      const auto& vot_obj = *idx.begin();

      for( auto vote: votes ) {
         BOOST_CHECK( vot_obj.votes_in_period.find( vote ) != vot_obj.votes_in_period.end() );
      }

      votes.clear();
      votes.insert( active_committee_members[0](db).vote_id );
      votes.insert( active_committee_members[1](db).vote_id );
      votes.insert( active_committee_members[2](db).vote_id );

      vote_for( *this, votes, nathan, key_id );

      for( auto vote: votes ) {
         BOOST_CHECK( vot_obj.votes_in_period.find( vote ) != vot_obj.votes_in_period.end() );
      }
   } catch (fc::exception& e) {
      edump((e.to_detail_string()));
      throw;
   }
}

BOOST_AUTO_TEST_CASE( vote_test ) {
   try {
      PREAMBLE;

      flat_set<vote_id_type> votes;
      for( uint8_t i = 0; i < 5; ++i ) {
         votes.insert( active_committee_members[i](db).vote_id );
      }
      vote_for( *this, votes, nathan, key_id );

      BOOST_REQUIRE( idx.size() == 1 );
      const auto& vot_obj = *idx.begin();

      for( auto vote: votes ) {
         BOOST_CHECK( vot_obj.votes_in_period.find( vote ) != vot_obj.votes_in_period.end() );
      }

      votes.clear();
      for( uint8_t i = 5; i < 10; ++i ) {
         votes.insert( active_committee_members[i](db).vote_id );
      }
      vote_for( *this, votes, nathan, key_id );

      for( auto vote: votes ) {
         BOOST_CHECK( vot_obj.votes_in_period.find( vote ) != vot_obj.votes_in_period.end() );
      }
   } catch (fc::exception& e) {
      edump((e.to_detail_string()));
      throw;
   }
}

BOOST_AUTO_TEST_SUITE_END()