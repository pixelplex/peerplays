/*
 * Copyright (c) 2015 Cryptonomex, Inc., and contributors.
 *
 * The MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#pragma once

#include <graphene/app/application.hpp>
#include <graphene/chain/database.hpp>
#include <fc_pp/io/json.hpp>
#include <fc_pp/smart_ref_impl.hpp>

#include <graphene/chain/operation_history_object.hpp>

#include <boost/parameter.hpp>

#include <iostream>

using namespace graphene::db;

extern uint32_t GRAPHENE_TESTING_GENESIS_TIMESTAMP;

#define PUSH_TX \
   graphene::chain::test::_push_transaction

#define PUSH_BLOCK \
   graphene::chain::test::_push_block

// See below
#define REQUIRE_OP_VALIDATION_SUCCESS( op, field, value ) \
{ \
   const auto temp = op.field; \
   op.field = value; \
   op.validate(); \
   op.field = temp; \
}
#define REQUIRE_OP_EVALUATION_SUCCESS( op, field, value ) \
{ \
   const auto temp = op.field; \
   op.field = value; \
   trx.operations.back() = op; \
   op.field = temp; \
   db.push_transaction( trx, ~0 ); \
}

#define GRAPHENE_REQUIRE_THROW( expr, exc_type )          \
{                                                         \
   std::string req_throw_info = fc_pp::json::to_string(      \
      fc_pp::mutable_variant_object()                        \
      ("source_file", __FILE__)                           \
      ("source_lineno", __LINE__)                         \
      ("expr", #expr)                                     \
      ("exc_type", #exc_type)                             \
      );                                                  \
   if( fc_pp::enable_record_assert_trip )                    \
      std::cout << "GRAPHENE_REQUIRE_THROW begin "        \
         << req_throw_info << std::endl;                  \
   BOOST_REQUIRE_THROW( expr, exc_type );                 \
   if( fc_pp::enable_record_assert_trip )                    \
      std::cout << "GRAPHENE_REQUIRE_THROW end "          \
         << req_throw_info << std::endl;                  \
}

#define GRAPHENE_CHECK_THROW( expr, exc_type )            \
{                                                         \
   std::string req_throw_info = fc_pp::json::to_string(      \
      fc_pp::mutable_variant_object()                        \
      ("source_file", __FILE__)                           \
      ("source_lineno", __LINE__)                         \
      ("expr", #expr)                                     \
      ("exc_type", #exc_type)                             \
      );                                                  \
   if( fc_pp::enable_record_assert_trip )                    \
      std::cout << "GRAPHENE_CHECK_THROW begin "          \
         << req_throw_info << std::endl;                  \
   BOOST_CHECK_THROW( expr, exc_type );                   \
   if( fc_pp::enable_record_assert_trip )                    \
      std::cout << "GRAPHENE_CHECK_THROW end "            \
         << req_throw_info << std::endl;                  \
}

#define REQUIRE_OP_VALIDATION_FAILURE_2( op, field, value, exc_type ) \
{ \
   const auto temp = op.field; \
   op.field = value; \
   GRAPHENE_REQUIRE_THROW( op.validate(), exc_type ); \
   op.field = temp; \
}
#define REQUIRE_OP_VALIDATION_FAILURE( op, field, value ) \
   REQUIRE_OP_VALIDATION_FAILURE_2( op, field, value, fc_pp::exception )

#define REQUIRE_THROW_WITH_VALUE_2(op, field, value, exc_type) \
{ \
   auto bak = op.field; \
   op.field = value; \
   trx.operations.back() = op; \
   op.field = bak; \
   GRAPHENE_REQUIRE_THROW(db.push_transaction(trx, ~0), exc_type); \
}

#define REQUIRE_THROW_WITH_VALUE( op, field, value ) \
   REQUIRE_THROW_WITH_VALUE_2( op, field, value, fc_pp::exception )

///This simply resets v back to its default-constructed value. Requires v to have a working assingment operator and
/// default constructor.
#define RESET(v) v = decltype(v)()
///This allows me to build consecutive test cases. It's pretty ugly, but it works well enough for unit tests.
/// i.e. This allows a test on update_account to begin with the database at the end state of create_account.
#define INVOKE(test) ((struct test*)this)->test_method(); trx.clear()

#define PREP_ACTOR(name) \
   fc_pp::ecc::private_key name ## _private_key = generate_private_key(BOOST_PP_STRINGIZE(name));   \
   public_key_type name ## _public_key = name ## _private_key.get_public_key();

#define ACTOR(name) \
   PREP_ACTOR(name) \
   const auto& name = create_account(BOOST_PP_STRINGIZE(name), name ## _public_key); \
   account_id_type name ## _id = name.id; (void)name ## _id;

#define GET_ACTOR(name) \
   fc_pp::ecc::private_key name ## _private_key = generate_private_key(BOOST_PP_STRINGIZE(name)); \
   const account_object& name = get_account(BOOST_PP_STRINGIZE(name)); \
   account_id_type name ## _id = name.id; \
   (void)name ##_id

#define ACTORS_IMPL(r, data, elem) ACTOR(elem)
#define ACTORS(names) BOOST_PP_SEQ_FOR_EACH(ACTORS_IMPL, ~, names)

namespace graphene { namespace chain {

   namespace keywords {
      BOOST_PARAMETER_NAME(event_id)
      BOOST_PARAMETER_NAME(event_group_id)
      BOOST_PARAMETER_NAME(name)
      BOOST_PARAMETER_NAME(season)
      BOOST_PARAMETER_NAME(status)
      BOOST_PARAMETER_NAME(force)
      BOOST_PARAMETER_NAME(betting_market_group_id)
      BOOST_PARAMETER_NAME(description)
      BOOST_PARAMETER_NAME(rules_id)
   }

struct database_fixture {
   // the reason we use an app is to exercise the indexes of built-in
   //   plugins
   graphene::app::application app;
   genesis_state_type genesis_state;
   chain::database &db;
   signed_transaction trx;
   public_key_type committee_key;
   account_id_type committee_account;
   fc_pp::ecc::private_key private_key = fc_pp::ecc::private_key::generate();
   fc_pp::ecc::private_key init_account_priv_key = fc_pp::ecc::private_key::regenerate(fc_pp::sha256::hash(string("null_key")) );
   public_key_type init_account_pub_key;

   optional<fc_pp::temp_directory> data_dir;
   bool skip_key_index_test = false;
   uint32_t anon_acct_count;

   database_fixture();
   ~database_fixture();

   static fc_pp::ecc::private_key generate_private_key(string seed);
   string generate_anon_acct_name();
   static void verify_asset_supplies( const database& db );
   void verify_account_history_plugin_index( )const;
   void open_database();
   signed_block generate_block(uint32_t skip = ~0,
                               const fc_pp::ecc::private_key& key = generate_private_key("null_key"),
                               int miss_blocks = 0);

   /**
    * @brief Generates block_count blocks
    * @param block_count number of blocks to generate
    */
   void generate_blocks(uint32_t block_count);

   /**
    * @brief Generates blocks until the head block time matches or exceeds timestamp
    * @param timestamp target time to generate blocks until
    */
   void generate_blocks(fc_pp::time_point_sec timestamp, bool miss_intermediate_blocks = true, uint32_t skip = ~0);

   account_create_operation make_account(
      const std::string& name = "nathan",
      public_key_type = public_key_type()
      );

   account_create_operation make_account(
      const std::string& name,
      const account_object& registrar,
      const account_object& referrer,
      uint8_t referrer_percent = 100,
      public_key_type key = public_key_type()
      );

   void force_global_settle(const asset_object& what, const price& p);
   operation_result force_settle(account_id_type who, asset what)
   { return force_settle(who(db), what); }
   operation_result force_settle(const account_object& who, asset what);
   void update_feed_producers(asset_id_type mia, flat_set<account_id_type> producers)
   { update_feed_producers(mia(db), producers); }
   void update_feed_producers(const asset_object& mia, flat_set<account_id_type> producers);
   void publish_feed(asset_id_type mia, account_id_type by, const price_feed& f)
   { publish_feed(mia(db), by(db), f); }
   void publish_feed(const asset_object& mia, const account_object& by, const price_feed& f);
   const call_order_object* borrow(account_id_type who, asset what, asset collateral)
   { return borrow(who(db), what, collateral); }
   const call_order_object* borrow(const account_object& who, asset what, asset collateral);
   void cover(account_id_type who, asset what, asset collateral_freed)
   { cover(who(db), what, collateral_freed); }
   void cover(const account_object& who, asset what, asset collateral_freed);

   const asset_object& get_asset( const string& symbol )const;
   const account_object& get_account( const string& name )const;
   const asset_object& create_bitasset(const string& name,
                                       account_id_type issuer = GRAPHENE_WITNESS_ACCOUNT,
                                       uint16_t market_fee_percent = 100 /*1%*/,
                                       uint16_t flags = charge_market_fee);
   const asset_object& create_prediction_market(const string& name,
                                       account_id_type issuer = GRAPHENE_WITNESS_ACCOUNT,
                                       uint16_t market_fee_percent = 100 /*1%*/,
                                       uint16_t flags = charge_market_fee);
   const asset_object& create_user_issued_asset( const string& name );
   const asset_object& create_user_issued_asset( const string& name,
                                                 const account_object& issuer,
                                                 uint16_t flags );
   void issue_uia( const account_object& recipient, asset amount );
   void issue_uia( account_id_type recipient_id, asset amount );

   const account_object& create_account(
      const string& name,
      const public_key_type& key = public_key_type()
      );

   const account_object& create_account(
      const string& name,
      const account_object& registrar,
      const account_object& referrer,
      uint8_t referrer_percent = 100,
      const public_key_type& key = public_key_type()
      );

   const account_object& create_account(
      const string& name,
      const private_key_type& key,
      const account_id_type& registrar_id = account_id_type(),
      const account_id_type& referrer_id = account_id_type(),
      uint8_t referrer_percent = 100
      );

   const committee_member_object& create_committee_member( const account_object& owner );
   const witness_object& create_witness(account_id_type owner,
                                        const fc_pp::ecc::private_key& signing_private_key = generate_private_key("null_key"));
   const witness_object& create_witness(const account_object& owner,
                                        const fc_pp::ecc::private_key& signing_private_key = generate_private_key("null_key"));
   uint64_t fund( const account_object& account, const asset& amount = asset(500000) );
   digest_type digest( const transaction& tx );
   void sign( signed_transaction& trx, const fc_pp::ecc::private_key& key );
   const limit_order_object* create_sell_order( account_id_type user, const asset& amount, const asset& recv );
   const limit_order_object* create_sell_order( const account_object& user, const asset& amount, const asset& recv );
   asset cancel_limit_order( const limit_order_object& order );
   void transfer( account_id_type from, account_id_type to, const asset& amount, const asset& fee = asset() );
   void transfer( const account_object& from, const account_object& to, const asset& amount, const asset& fee = asset() );
   void fund_fee_pool( const account_object& from, const asset_object& asset_to_fund, const share_type amount );
   void enable_fees();
   void change_fees( const flat_set< fee_parameters >& new_params, uint32_t new_scale = 0 );
   void upgrade_to_lifetime_member( account_id_type account );
   void upgrade_to_lifetime_member( const account_object& account );
   void upgrade_to_annual_member( account_id_type account );
   void upgrade_to_annual_member( const account_object& account );
   void print_market( const string& syma, const string& symb )const;
   string pretty( const asset& a )const;
   void print_limit_order( const limit_order_object& cur )const;
   void print_call_orders( )const;
   void print_joint_market( const string& syma, const string& symb )const;
   int64_t get_balance( account_id_type account, asset_id_type a )const;
   int64_t get_balance( const account_object& account, const asset_object& a )const;
   int64_t get_dividend_pending_payout_balance(asset_id_type dividend_holder_asset_type,
                                               account_id_type dividend_holder_account_id, 
                                               asset_id_type dividend_payout_asset_type) const;
   vector< operation_history_object > get_operation_history( account_id_type account_id )const;
   void  process_operation_by_witnesses(operation op);
   void  process_operation_by_committee(operation op);
   void  force_operation_by_witnesses(operation op);
   void  set_is_proposed_trx(operation op);
   const sport_object& create_sport(internationalized_string_type name);
   void  update_sport(sport_id_type sport_id, internationalized_string_type name);
   void  delete_sport(sport_id_type sport_id);
   const event_group_object& create_event_group(internationalized_string_type name, sport_id_type sport_id);
   void  update_event_group(event_group_id_type event_group_id,
                            fc_pp::optional<object_id_type> sport_id,
                            fc_pp::optional<internationalized_string_type> name);
   void  delete_event_group(event_group_id_type event_group_id);
   void  try_update_event_group(event_group_id_type event_group_id,
                                fc_pp::optional<object_id_type> sport_id,
                                fc_pp::optional<internationalized_string_type> name,
                                bool dont_set_is_proposed_trx = false);
   const event_object& create_event(internationalized_string_type name, internationalized_string_type season, event_group_id_type event_group_id);
   void update_event_impl(event_id_type event_id,
                          fc_pp::optional<object_id_type> event_group_id,
                          fc_pp::optional<internationalized_string_type> name,
                          fc_pp::optional<internationalized_string_type> season,
                          fc_pp::optional<event_status> status,
                          bool force);
   BOOST_PARAMETER_MEMBER_FUNCTION((void), update_event, keywords::tag, 
                                   (required (event_id, (event_id_type)))
                                   (optional (event_group_id, (fc_pp::optional<object_id_type>), fc_pp::optional<object_id_type>())
                                             (name, (fc_pp::optional<internationalized_string_type>), fc_pp::optional<internationalized_string_type>())
                                             (season, (fc_pp::optional<internationalized_string_type>), fc_pp::optional<internationalized_string_type>())
                                             (status, (fc_pp::optional<event_status>), fc_pp::optional<event_status>())
                                             (force, (bool), false)))
   {
      update_event_impl(event_id, event_group_id, name, season, status, force);
   }

   const betting_market_rules_object& create_betting_market_rules(internationalized_string_type name, internationalized_string_type description);
   void update_betting_market_rules(betting_market_rules_id_type rules_id,
                                     fc_pp::optional<internationalized_string_type> name,
                                     fc_pp::optional<internationalized_string_type> description);
   const betting_market_group_object& create_betting_market_group(internationalized_string_type description, 
                                                                  event_id_type event_id, 
                                                                  betting_market_rules_id_type rules_id, 
                                                                  asset_id_type asset_id,
                                                                  bool never_in_play,
                                                                  uint32_t delay_before_settling);
   void update_betting_market_group_impl(betting_market_group_id_type betting_market_group_id,
                                         fc_pp::optional<internationalized_string_type> description,
                                         fc_pp::optional<object_id_type> rules_id,
                                         fc_pp::optional<betting_market_group_status> status,
                                         bool force);
   BOOST_PARAMETER_MEMBER_FUNCTION((void), update_betting_market_group, keywords::tag, 
                                   (required (betting_market_group_id, (betting_market_group_id_type)))
                                   (optional (description, (fc_pp::optional<internationalized_string_type>), fc_pp::optional<internationalized_string_type>())
                                             (rules_id, (fc_pp::optional<object_id_type>), fc_pp::optional<object_id_type>())
                                             (status, (fc_pp::optional<betting_market_group_status>), fc_pp::optional<betting_market_group_status>())
                                             (force, (bool), false)))
   {
      update_betting_market_group_impl(betting_market_group_id, description, rules_id, status, force);
   }

   const betting_market_object& create_betting_market(betting_market_group_id_type group_id, internationalized_string_type payout_condition);
   void update_betting_market(betting_market_id_type betting_market_id,
                                                fc_pp::optional<object_id_type> group_id,
                                                /*fc_pp::optional<internationalized_string_type> description,*/
                                                fc_pp::optional<internationalized_string_type> payout_condition);

   bet_id_type place_bet(account_id_type bettor_id, betting_market_id_type betting_market_id, bet_type back_or_lay, asset amount_to_bet, bet_multiplier_type backer_multiplier);
   void resolve_betting_market_group(betting_market_group_id_type betting_market_group_id, std::map<betting_market_id_type, betting_market_resolution_type> resolutions);
   void cancel_unmatched_bets(betting_market_group_id_type betting_market_group_id);

   proposal_id_type propose_operation(operation op);
   void process_proposal_by_witnesses(const std::vector<witness_id_type>& witnesses, proposal_id_type proposal_id, bool remove = false);
};

namespace test {
/// set a reasonable expiration time for the transaction
void set_expiration( const database& db, transaction& tx );

bool _push_block( database& db, const signed_block& b, uint32_t skip_flags = 0 );
processed_transaction _push_transaction( database& db, const signed_transaction& tx, uint32_t skip_flags = 0 );
}

} }
