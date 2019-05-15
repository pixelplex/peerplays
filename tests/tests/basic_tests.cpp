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

#include <boost/test/unit_test.hpp>

#include <graphene/chain/database.hpp>
#include <graphene/chain/protocol/protocol.hpp>

#include <graphene/chain/account_object.hpp>
#include <graphene/chain/asset_object.hpp>
#include <graphene/chain/witness_scheduler_rng.hpp>
#include <graphene/chain/exceptions.hpp>

#include <graphene/db/simple_index.hpp>

#include <fc_pp/crypto/digest.hpp>
#include <fc_pp/crypto/hex.hpp>
#include <fc_pp/crypto/hash_ctr_rng.hpp>
#include "../common/database_fixture.hpp"

#include <algorithm>
#include <random>

using namespace graphene::chain;
using namespace graphene::db;

BOOST_FIXTURE_TEST_SUITE( basic_tests, database_fixture )

/**
 * Verify that names are RFC-1035 compliant https://tools.ietf.org/html/rfc1035
 * https://github.com/cryptonomex/graphene/issues/15
 */
BOOST_AUTO_TEST_CASE( valid_name_test )
{
   BOOST_CHECK( is_valid_name( "a" ) );
   BOOST_CHECK( !is_valid_name( "A" ) );
   BOOST_CHECK( !is_valid_name( "0" ) );
   BOOST_CHECK( !is_valid_name( "." ) );
   BOOST_CHECK( !is_valid_name( "-" ) );

   BOOST_CHECK( is_valid_name( "aa" ) );
   BOOST_CHECK( !is_valid_name( "aA" ) );
   BOOST_CHECK( is_valid_name( "a0" ) );
   BOOST_CHECK( !is_valid_name( "a." ) );
   BOOST_CHECK( !is_valid_name( "a-" ) );

   BOOST_CHECK( is_valid_name( "aaa" ) );
   BOOST_CHECK( !is_valid_name( "aAa" ) );
   BOOST_CHECK( is_valid_name( "a0a" ) );
   BOOST_CHECK( is_valid_name( "a.a" ) );
   BOOST_CHECK( is_valid_name( "a-a" ) );

   BOOST_CHECK( is_valid_name( "aa0" ) );
   BOOST_CHECK( !is_valid_name( "aA0" ) );
   BOOST_CHECK( is_valid_name( "a00" ) );
   BOOST_CHECK( !is_valid_name( "a.0" ) );
   BOOST_CHECK( is_valid_name( "a-0" ) );

   BOOST_CHECK(  is_valid_name( "aaa-bbb-ccc" ) );
   BOOST_CHECK(  is_valid_name( "aaa-bbb.ccc" ) );

   BOOST_CHECK( !is_valid_name( "aaa,bbb-ccc" ) );
   BOOST_CHECK( !is_valid_name( "aaa_bbb-ccc" ) );
   BOOST_CHECK( !is_valid_name( "aaa-BBB-ccc" ) );

   BOOST_CHECK( !is_valid_name( "1aaa-bbb" ) );
   BOOST_CHECK( !is_valid_name( "-aaa-bbb-ccc" ) );
   BOOST_CHECK( !is_valid_name( ".aaa-bbb-ccc" ) );
   BOOST_CHECK( !is_valid_name( "/aaa-bbb-ccc" ) );

   BOOST_CHECK( !is_valid_name( "aaa-bbb-ccc-" ) );
   BOOST_CHECK( !is_valid_name( "aaa-bbb-ccc." ) );
   BOOST_CHECK( !is_valid_name( "aaa-bbb-ccc.." ) );
   BOOST_CHECK( !is_valid_name( "aaa-bbb-ccc/" ) );

   BOOST_CHECK( !is_valid_name( "aaa..bbb-ccc" ) );
   BOOST_CHECK( is_valid_name( "aaa.bbb-ccc" ) );
   BOOST_CHECK( is_valid_name( "aaa.bbb.ccc" ) );

   BOOST_CHECK(  is_valid_name( "aaa--bbb--ccc" ) );
   BOOST_CHECK(  is_valid_name( "xn--sandmnnchen-p8a.de" ) );
   BOOST_CHECK(  is_valid_name( "xn--sandmnnchen-p8a.dex" ) );
   BOOST_CHECK(  is_valid_name( "xn-sandmnnchen-p8a.de" ) );
   BOOST_CHECK(  is_valid_name( "xn-sandmnnchen-p8a.dex" ) );

   BOOST_CHECK(  is_valid_name( "this-label-has-less-than-64-char.acters-63-to-be-really-precise" ) );
   BOOST_CHECK( !is_valid_name( "this-label-has-more-than-63-char.act.ers-64-to-be-really-precise" ) );
   BOOST_CHECK( !is_valid_name( "none.of.these.labels.has.more.than-63.chars--but.still.not.valid" ) );
}

BOOST_AUTO_TEST_CASE( valid_symbol_test )
{
   BOOST_CHECK( !is_valid_symbol( "A" ) );
   BOOST_CHECK( !is_valid_symbol( "a" ) );
   BOOST_CHECK( !is_valid_symbol( "0" ) );
   BOOST_CHECK( !is_valid_symbol( "." ) );

   BOOST_CHECK( !is_valid_symbol( "AA" ) );
   BOOST_CHECK( !is_valid_symbol( "Aa" ) );
   BOOST_CHECK( !is_valid_symbol( "A0" ) );
   BOOST_CHECK( !is_valid_symbol( "A." ) );

   BOOST_CHECK( is_valid_symbol( "AAA" ) );
   BOOST_CHECK( !is_valid_symbol( "AaA" ) );
   BOOST_CHECK( is_valid_symbol( "A0A" ) );
   BOOST_CHECK( is_valid_symbol( "A.A" ) );

   BOOST_CHECK( !is_valid_symbol( "A..A" ) );
   BOOST_CHECK( !is_valid_symbol( "A.A." ) );
   BOOST_CHECK( !is_valid_symbol( "A.A.A" ) );

   BOOST_CHECK( is_valid_symbol( "AAAAAAAAAAAAAAAA" ) );
   BOOST_CHECK( !is_valid_symbol( "AAAAAAAAAAAAAAAAA" ) );
   BOOST_CHECK( is_valid_symbol( "A.AAAAAAAAAAAAAA" ) );
   BOOST_CHECK( !is_valid_symbol( "A.AAAAAAAAAAAA.A" ) );

   BOOST_CHECK( is_valid_symbol( "AAA000AAA" ) );
}

BOOST_AUTO_TEST_CASE( price_test )
{
    auto price_max = []( uint32_t a, uint32_t b )
    {   return price::max( asset_id_type(a), asset_id_type(b) );   };
    auto price_min = []( uint32_t a, uint32_t b )
    {   return price::min( asset_id_type(a), asset_id_type(b) );   };

    BOOST_CHECK( price_max(0,1) > price_min(0,1) );
    BOOST_CHECK( price_max(1,0) > price_min(1,0) );
    BOOST_CHECK( price_max(0,1) >= price_min(0,1) );
    BOOST_CHECK( price_max(1,0) >= price_min(1,0) );
    BOOST_CHECK( price_max(0,1) >= price_max(0,1) );
    BOOST_CHECK( price_max(1,0) >= price_max(1,0) );
    BOOST_CHECK( price_min(0,1) < price_max(0,1) );
    BOOST_CHECK( price_min(1,0) < price_max(1,0) );
    BOOST_CHECK( price_min(0,1) <= price_max(0,1) );
    BOOST_CHECK( price_min(1,0) <= price_max(1,0) );
    BOOST_CHECK( price_min(0,1) <= price_min(0,1) );
    BOOST_CHECK( price_min(1,0) <= price_min(1,0) );
    BOOST_CHECK( price_min(1,0) != price_max(1,0) );
    BOOST_CHECK( ~price_max(0,1) != price_min(0,1) );
    BOOST_CHECK( ~price_min(0,1) != price_max(0,1) );
    BOOST_CHECK( ~price_max(0,1) == price_min(1,0) );
    BOOST_CHECK( ~price_min(0,1) == price_max(1,0) );
    BOOST_CHECK( ~price_max(0,1) < ~price_min(0,1) );
    BOOST_CHECK( ~price_max(0,1) <= ~price_min(0,1) );
    price a(asset(1), asset(2,asset_id_type(1)));
    price b(asset(2), asset(2,asset_id_type(1)));
    price c(asset(1), asset(2,asset_id_type(1)));
    BOOST_CHECK(a < b);
    BOOST_CHECK(b > a);
    BOOST_CHECK(a == c);
    BOOST_CHECK(!(b == c));

    price_feed dummy;
    dummy.maintenance_collateral_ratio = 1002;
    dummy.maximum_short_squeeze_ratio = 1234;
    dummy.settlement_price = price(asset(1000), asset(2000, asset_id_type(1)));
    price_feed dummy2 = dummy;
    BOOST_CHECK(dummy == dummy2);
}

BOOST_AUTO_TEST_CASE( memo_test )
{ try {
   memo_data m;
   auto sender = generate_private_key("1");
   auto receiver = generate_private_key("2");
   m.from = sender.get_public_key();
   m.to = receiver.get_public_key();
   m.set_message(sender, receiver.get_public_key(), "Hello, world!", 12345);

   decltype(fc_pp::digest(m)) hash("8de72a07d093a589f574460deb19023b4aff354b561eb34590d9f4629f51dbf3");
   if( fc_pp::digest(m) != hash )
   {
      // If this happens, notify the web guys that the memo serialization format changed.
      edump((m)(fc_pp::digest(m)));
      BOOST_FAIL("Memo format has changed. Notify the web guys and update this test.");
   }
   BOOST_CHECK_EQUAL(m.get_message(receiver, sender.get_public_key()), "Hello, world!");
} FC_LOG_AND_RETHROW() }

BOOST_AUTO_TEST_CASE( witness_rng_test_bits )
{
   try
   {
      const uint64_t COUNT = 131072;
      const uint64_t HASH_SIZE = 32;
      string ref_bits = "";
      ref_bits.reserve( COUNT * HASH_SIZE );
      static const char seed_data[] = "\xe3\xb0\xc4\x42\x98\xfc\x1c\x14\x9a\xfb\xf4\xc8\x99\x6f\xb9\x24\x27\xae\x41\xe4\x64\x9b\x93\x4c\xa4\x95\x99\x1b\x78\x52\xb8\x55";

      for( uint64_t i=0; i<COUNT; i++ )
      {
         // grab the bits
         fc_pp::sha256::encoder enc;
         enc.write( seed_data, HASH_SIZE );
         enc.put( char((i        ) & 0xFF) );
         enc.put( char((i >> 0x08) & 0xFF) );
         enc.put( char((i >> 0x10) & 0xFF) );
         enc.put( char((i >> 0x18) & 0xFF) );
         enc.put( char((i >> 0x20) & 0xFF) );
         enc.put( char((i >> 0x28) & 0xFF) );
         enc.put( char((i >> 0x30) & 0xFF) );
         enc.put( char((i >> 0x38) & 0xFF) );

         fc_pp::sha256 result = enc.result();
         auto result_data = result.data();
         std::copy( result_data, result_data+HASH_SIZE, std::back_inserter( ref_bits ) );
      }

      fc_pp::sha256 seed = fc_pp::sha256::hash( string("") );
      // seed = sha256("") = e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
      BOOST_CHECK( memcmp( seed.data(), seed_data, HASH_SIZE ) == 0 );

      fc_pp::hash_ctr_rng<fc_pp::sha256, 32> test_rng(seed.data(), 0);
      // python2 -c 'import hashlib; import struct; h = lambda x : hashlib.sha256(x).digest(); i = lambda x : struct.pack("<Q", x); print( h( h("") + i(0) ) )' | hd
      string ref_bits_hex =
          "5c5d42dcf39f71c0226ca720d8d518db615b5773f038e5e491963f6f47621bbd"   // h( h("") + i(0) )
          "43fd6dae047c400060be262e6d443200eacd1fafcb77828638085c2e2341fd8d"   // h( h("") + i(1) )
          "d666330a7441dc7279b786e65aba32817275989cfc691b3901f000fb0f14cd05"   // h( h("") + i(2) )
          "34bd93f83d7bac4a667d62fee39bd5eb1991fbadc29a5f216ea746772ca31544"   // h( h("") + i(3) )
          "d3b41a093eab01cd25f987a909b2f4812b0f38475e0fe40f6f42a12c6e018aa7"   // ...
          "c8db17b946c5a6bceaa7b903c93e6ccb8cc6c09b0cfd2108d930de1a79c3a68e"
          "cc1945b36c82e356b6d127057d036a150cb03b760e9c9e706c560f32a749e80d"
          "872b28fe97e289d4f6f361f3427d454113e3b513892d129398dac4daf8a0e43e"
          "8d7a5a2f3cbb245fa471e87e30a38d9c775c985c28db6e521e34cf1e88507c26"
          "c662f230eed0f10899c3a74a2d1bfb88d732909b206a2aed3ae0bda728fac8fe"
          "38eface8b1d473e45cbb40603bcef8bf2219e55669c7a2cfb5f8d52610689f14"
          "3b1d1734273b069a7de7cc6dd2e80db09d1feff200c9bdaf033cd553ea40e05d"
          "16653ca7aa7f790a95c6a8d41e5694b0c6bff806c3ce3e0e320253d408fb6f27"
          "b55df71d265de0b86a1cdf45d1d9c53da8ebf0ceec136affa12228d0d372e698"
          "37e9305ce57d386d587039b49b67104fd4d8467e87546237afc9a90cf8c677f9"
          "fc26784c94f754cf7aeacb6189e705e2f1873ea112940560f11dbbebb22a8922"
          ;
      char* ref_bits_chars = new char[ ref_bits_hex.length() / 2 ];
      fc_pp::from_hex( ref_bits_hex, ref_bits_chars, ref_bits_hex.length() / 2 );
      string ref_bits_str( ref_bits_chars, ref_bits_hex.length() / 2 );
      delete[] ref_bits_chars;
      ref_bits_chars = nullptr;

      BOOST_CHECK( ref_bits_str.length() < ref_bits.length() );
      BOOST_CHECK( ref_bits_str == ref_bits.substr( 0, ref_bits_str.length() ) );
      //std::cout << "ref_bits_str: " << fc_pp::to_hex( ref_bits_str.c_str(), std::min( ref_bits_str.length(), size_t(256) ) ) << "\n";
      //std::cout << "ref_bits    : " << fc_pp::to_hex( ref_bits    .c_str(), std::min( ref_bits.length(), size_t(256) ) ) << "\n";

      // when we get to this point, our code to generate the RNG byte output is working.
      // now let's test get_bits() as follows:

      uint64_t ref_get_bits_offset = 0;

      auto ref_get_bits = [&]( uint8_t count ) -> uint64_t
      {
         uint64_t result = 0;
         uint64_t i = ref_get_bits_offset;
         uint64_t mask = 1;
         while( count > 0 )
         {
            if( ref_bits[ i >> 3 ] & (1 << (i & 7)) )
                result |= mask;
            mask += mask;
            i++;
            count--;
         }
         ref_get_bits_offset = i;
         return result;
      };

      // use PRNG to decide between 0-64 bits
      std::minstd_rand rng;
      rng.seed( 9999 );
      std::uniform_int_distribution< uint16_t > bit_dist( 0, 64 );
      for( int i=0; i<10000; i++ )
      {
         uint8_t bit_count = bit_dist( rng );
         uint64_t ref_bits = ref_get_bits( bit_count );
         uint64_t test_bits = test_rng.get_bits( bit_count );
         //std::cout << i << ": get(" << int(bit_count) << ") -> " << test_bits << " (expect " << ref_bits << ")\n";
         if( bit_count < 64 )
         {
            BOOST_CHECK( ref_bits  < (uint64_t( 1 ) << bit_count ) );
            BOOST_CHECK( test_bits < (uint64_t( 1 ) << bit_count ) );
         }
         BOOST_CHECK( ref_bits == test_bits );
         if( ref_bits != test_bits )
            break;
      }

      std::uniform_int_distribution< uint64_t > whole_dist(
         0, std::numeric_limits<uint64_t>::max() );
      for( int i=0; i<10000; i++ )
      {
         uint8_t bit_count = bit_dist( rng );
         uint64_t bound = whole_dist( rng ) & ((uint64_t(1) << bit_count) - 1);
         //std::cout << "bound:" << bound << "\n";
         uint64_t rnum = test_rng( bound );
         //std::cout << "rnum:" << rnum << "\n";
         if( bound > 1 )
         {
            BOOST_CHECK( rnum < bound );
         }
         else
         {
            BOOST_CHECK( rnum == 0 );
         }
      }

   } FC_LOG_AND_RETHROW()
}

BOOST_AUTO_TEST_CASE( exceptions )
{
   GRAPHENE_CHECK_THROW(FC_THROW_EXCEPTION(balance_claim_invalid_claim_amount, "Etc"), balance_claim_invalid_claim_amount);
}

BOOST_AUTO_TEST_CASE( scaled_precision )
{
   const int64_t _k = 1000;
   const int64_t _m = _k*_k;
   const int64_t _g = _m*_k;
   const int64_t _t = _g*_k;
   const int64_t _p = _t*_k;
   const int64_t _e = _p*_k;

   BOOST_CHECK( asset::scaled_precision( 0) == share_type(   1   ) );
   BOOST_CHECK( asset::scaled_precision( 1) == share_type(  10   ) );
   BOOST_CHECK( asset::scaled_precision( 2) == share_type( 100   ) );
   BOOST_CHECK( asset::scaled_precision( 3) == share_type(   1*_k) );
   BOOST_CHECK( asset::scaled_precision( 4) == share_type(  10*_k) );
   BOOST_CHECK( asset::scaled_precision( 5) == share_type( 100*_k) );
   BOOST_CHECK( asset::scaled_precision( 6) == share_type(   1*_m) );
   BOOST_CHECK( asset::scaled_precision( 7) == share_type(  10*_m) );
   BOOST_CHECK( asset::scaled_precision( 8) == share_type( 100*_m) );
   BOOST_CHECK( asset::scaled_precision( 9) == share_type(   1*_g) );
   BOOST_CHECK( asset::scaled_precision(10) == share_type(  10*_g) );
   BOOST_CHECK( asset::scaled_precision(11) == share_type( 100*_g) );
   BOOST_CHECK( asset::scaled_precision(12) == share_type(   1*_t) );
   BOOST_CHECK( asset::scaled_precision(13) == share_type(  10*_t) );
   BOOST_CHECK( asset::scaled_precision(14) == share_type( 100*_t) );
   BOOST_CHECK( asset::scaled_precision(15) == share_type(   1*_p) );
   BOOST_CHECK( asset::scaled_precision(16) == share_type(  10*_p) );
   BOOST_CHECK( asset::scaled_precision(17) == share_type( 100*_p) );
   BOOST_CHECK( asset::scaled_precision(18) == share_type(   1*_e) );
   GRAPHENE_CHECK_THROW( asset::scaled_precision(19), fc_pp::exception );
}

BOOST_AUTO_TEST_CASE( merkle_root )
{
   signed_block block;
   vector<processed_transaction> tx;
   vector<digest_type> t;
   const uint32_t num_tx = 10;

   for( uint32_t i=0; i<num_tx; i++ )
   {
      tx.emplace_back();
      tx.back().ref_block_prefix = i;
      t.push_back( tx.back().merkle_digest() );
   }

   auto c = []( const digest_type& digest ) -> checksum_type
   {   return checksum_type::hash( digest );   };
   
   auto d = []( const digest_type& left, const digest_type& right ) -> digest_type
   {   return digest_type::hash( std::make_pair( left, right ) );   };

   BOOST_CHECK( block.calculate_merkle_root() == checksum_type() );

   block.transactions.push_back( tx[0] );
   BOOST_CHECK( block.calculate_merkle_root() ==
      c(t[0])
      );

   digest_type dA, dB, dC, dD, dE, dI, dJ, dK, dM, dN, dO;

   /*
      A=d(0,1)
         / \ 
        0   1
   */

   dA = d(t[0], t[1]);

   block.transactions.push_back( tx[1] );
   BOOST_CHECK( block.calculate_merkle_root() == c(dA) );

   /*
            I=d(A,B)
           /        \
      A=d(0,1)      B=2
         / \        /
        0   1      2
   */

   dB = t[2];
   dI = d(dA, dB);

   block.transactions.push_back( tx[2] );
   BOOST_CHECK( block.calculate_merkle_root() == c(dI) );

   /*
          I=d(A,B)
           /    \
      A=d(0,1)   B=d(2,3)
         / \    /   \
        0   1  2     3
   */

   dB = d(t[2], t[3]);
   dI = d(dA, dB);

   block.transactions.push_back( tx[3] );
   BOOST_CHECK( block.calculate_merkle_root() == c(dI) );

   /*
                     __M=d(I,J)__
                    /            \
            I=d(A,B)              J=C
           /        \            /
      A=d(0,1)   B=d(2,3)      C=4
         / \        / \        /
        0   1      2   3      4
   */

   dC = t[4];
   dJ = dC;
   dM = d(dI, dJ);

   block.transactions.push_back( tx[4] );
   BOOST_CHECK( block.calculate_merkle_root() == c(dM) );

   /*
                     __M=d(I,J)__
                    /            \
            I=d(A,B)              J=C
           /        \            /
      A=d(0,1)   B=d(2,3)   C=d(4,5)
         / \        / \        / \
        0   1      2   3      4   5
   */

   dC = d(t[4], t[5]);
   dJ = dC;
   dM = d(dI, dJ);

   block.transactions.push_back( tx[5] );
   BOOST_CHECK( block.calculate_merkle_root() == c(dM) );

   /*
                     __M=d(I,J)__
                    /            \
            I=d(A,B)              J=d(C,D)
           /        \            /        \
      A=d(0,1)   B=d(2,3)   C=d(4,5)      D=6
         / \        / \        / \        /
        0   1      2   3      4   5      6
   */

   dD = t[6];
   dJ = d(dC, dD);
   dM = d(dI, dJ);

   block.transactions.push_back( tx[6] );
   BOOST_CHECK( block.calculate_merkle_root() == c(dM) );

   /*
                     __M=d(I,J)__
                    /            \
            I=d(A,B)              J=d(C,D)
           /        \            /        \
      A=d(0,1)   B=d(2,3)   C=d(4,5)   D=d(6,7)
         / \        / \        / \        / \
        0   1      2   3      4   5      6   7
   */

   dD = d(t[6], t[7]);
   dJ = d(dC, dD);
   dM = d(dI, dJ);

   block.transactions.push_back( tx[7] );
   BOOST_CHECK( block.calculate_merkle_root() == c(dM) );

   /*
                                _____________O=d(M,N)______________
                               /                                   \   
                     __M=d(I,J)__                                  N=K
                    /            \                              /
            I=d(A,B)              J=d(C,D)                 K=E
           /        \            /        \            /
      A=d(0,1)   B=d(2,3)   C=d(4,5)   D=d(6,7)      E=8
         / \        / \        / \        / \        /
        0   1      2   3      4   5      6   7      8
   */

   dE = t[8];
   dK = dE;
   dN = dK;
   dO = d(dM, dN);

   block.transactions.push_back( tx[8] );
   BOOST_CHECK( block.calculate_merkle_root() == c(dO) );

   /*
                                _____________O=d(M,N)______________
                               /                                   \   
                     __M=d(I,J)__                                  N=K
                    /            \                              /
            I=d(A,B)              J=d(C,D)                 K=E
           /        \            /        \            /
      A=d(0,1)   B=d(2,3)   C=d(4,5)   D=d(6,7)   E=d(8,9)
         / \        / \        / \        / \        / \
        0   1      2   3      4   5      6   7      8   9
   */

   dE = d(t[8], t[9]);
   dK = dE;
   dN = dK;
   dO = d(dM, dN);

   block.transactions.push_back( tx[9] );
   BOOST_CHECK( block.calculate_merkle_root() == c(dO) );
}

BOOST_AUTO_TEST_SUITE_END()
