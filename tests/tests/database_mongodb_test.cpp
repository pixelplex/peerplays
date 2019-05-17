/*
 * Copyright (c) 2017 Cryptonomex, Inc., and contributors.
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
#include <boost/filesystem.hpp>

#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/make_unique.hpp>
// #include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/exception/exception.hpp>

#include <fc_pp/reflect/reflect.hpp>
#include <fc_pp/reflect/variant.hpp>
#include <fc_pp/io/json.hpp>

#include <iostream>

// namespace bfs = boost::filesystem;
// using namespace bsoncxx::builder::basic;


///////////////////////////////////////////////////////////////////////  OBJECT to BSON begin
using namespace bsoncxx::builder::stream;

template<typename T>
document to_bson( const T& o );

uint8_t to_bson( const uint8_t& value )
{
   return value;
}

int8_t to_bson( const int8_t& value )
{
   return value;
}

uint16_t to_bson( const uint16_t& value )
{
   return value;
}

int16_t to_bson( const int16_t& value )
{
   return value;
}

int64_t to_bson( const uint32_t& value )
{
   return static_cast<int64_t>( value );
}

int32_t to_bson( const int32_t& value )
{
   return value;
}

// uint64_t to_bson( uint64_t value ) // TODO
// {
//    return value;
// }

int64_t to_bson( const int64_t value )
{
   return value;
}

bool to_bson( const bool value )
{
   return value;
}

double to_bson( const double value )
{
   return value;
}

float to_bson( const float value )
{
   return value;
}

char to_bson( const char value )
{
   return value;
}

std::string to_bson( const std::string value )
{
   return value;
}

template<typename A, typename B>
document to_bson( const std::pair<A,B>& t ){
   document doc;
   doc << "first" << to_bson( t.first ) << "second" << to_bson( t.second );
   return doc;
}

template<typename T, std::size_t S>
array to_bson( const std::array<T,S>& t )
{
   array array_builder;
   for( size_t i = 0; i < S; ++i )
      array_builder << to_bson( t[i] );
   return array_builder;
}

template<typename T>
array to_bson( const std::vector<T>& t )
{
   array array_builder;
   for( size_t i = 0; i < t.size(); ++i )
      array_builder << to_bson( t[i] );
   return array_builder;
}

template<typename T>
array to_bson( const std::deque<T>& t )
{
   array array_builder;
   for( size_t i = 0; i < t.size(); ++i )
      array_builder << to_bson( t[i] );
   return array_builder;
}

template<typename T>
array to_bson( const std::set<T>& var )
{
   array array_builder;
   for( auto itr = var.begin(); itr != var.end(); ++itr )
      array_builder << to_bson( *itr );
   return array_builder;
}

template<typename T>
array to_bson( const std::unordered_set<T>& var )
{
   array array_builder;
   for( auto itr = var.begin(); itr != var.end(); ++itr )
      array_builder << to_bson( *itr );
   return array_builder;
}

template<typename K, typename T>
array to_bson( const std::map<K, T>& var )
{
   array array_builder;
   for( auto itr = var.begin(); itr != var.end(); ++itr )
      array_builder << to_bson( *itr );
   return array_builder;
}

template<typename K, typename T>
array to_bson( const std::unordered_map<K, T>& var )
{
   array array_builder;
   for( auto itr = var.begin(); itr != var.end(); ++itr )
      array_builder << to_bson( *itr );
   return array_builder;
}

template<typename K, typename T>
array to_bson( const std::multimap<K, T>& var )
{
   array array_builder;
   for( auto itr = var.begin(); itr != var.end(); ++itr )
      array_builder << to_bson( *itr );
   return array_builder;
}

template<typename T>
class to_bson_visitor
{
   public:

      to_bson_visitor( document& _doc, const T& _val ) : doc( _doc ), val( _val ) {}

      template<typename Member, class Class, Member (Class::*member)>
      void operator()( const char* name )const
      {
         this->add( doc, name, ( val.*member ) );
      }

   private:

      template<typename M>
      void add( document& doc, const char* name, const M& v )const
      {
         doc << std::string( name ) << to_bson( v );
      }

      document& doc;
      const T& val;
};

template<typename IsReflected=fc_pp::false_type>
struct if_enum 
{
   template<typename T>
   static inline void to_bson( const T& v, document& vo ) 
   {
      fc_pp::reflector<T>::visit( to_bson_visitor<T>( vo, v ) );
   }
};

template<typename T>
document to_bson( const T& o )
{
   document doc;
   if_enum<typename fc_pp::reflector<T>::is_enum>::to_bson( o, doc );
   return doc;
}
///////////////////////////////////////////////////////////////////////  OBJECT to BSON end



































// class object_identifiers
// {

//    public:

//       template<class ObjectType>
//       uint64_t get_next_id() { return object_ids[std::string( BOOST_PP_STRINGIZE( ObjectType ) )]; }

//       template<class ObjectType>
//       void identifier_increment() { object_ids[std::string( BOOST_PP_STRINGIZE( ObjectType ) )]++; }

//       uint8_t id = 1;
//       // <collection, next_id>
//       std::map<std::string, uint64_t> object_ids;
// };
// FC_REFLECT( object_identifiers, (id)(object_ids) )

class database
{

   public:

      enum open_flags {
         read_only     = 0,
         read_write    = 1
      };

      database( const mongocxx::uri& uri, open_flags write = read_only );
      database( database&& ) = default;
      database& operator=( database&& ) = default;
      ~database();

      template<typename ObjectType, typename Modifier>
      void modify( const ObjectType& obj, Modifier&& m );

      template<typename ObjectType>
      void remove( const ObjectType& obj );

      template<typename ObjectType, typename Constructor>
      const ObjectType create( Constructor&& con );

   private:

      // template<typename ObjectType>
      // bsoncxx::document::value object_to_bson( const ObjectType& obj );

      std::unique_ptr<mongocxx::instance> _instance = nullptr;
      mongocxx::client _client;

      // object_identifiers object_ids;
};

database::database( const mongocxx::uri& uri, open_flags write )
{
   // TODO: It is possible to enable database logging
   _instance = bsoncxx::stdx::make_unique<mongocxx::instance>();
   _client = mongocxx::client( uri );

   try {
      auto value = _client["documentation_examples"].run_command( bsoncxx::builder::basic::make_document( bsoncxx::builder::basic::kvp( "ismaster", 1 ) ) );
      bool status = value.view()["ismaster"].get_bool();

      if( status ) {
         // TODO: Make output to logs stream
         std::cout << "Database is open (MongoDB)" << std::endl;
      } else {
         BOOST_THROW_EXCEPTION( std::runtime_error( "Cannot connect to the database (MongoDB)" ) );
      }
   } catch( ... ) {
      BOOST_THROW_EXCEPTION( std::runtime_error( "Cannot connect to the database (MongoDB)" ) );
   }
}

database::~database()
{
   // auto bson = object_to_bson( object_ids );

   // auto collection = _client["testdb3"][std::string( BOOST_PP_STRINGIZE( object_identifiers ) )];

   // auto cursor = collection.find({});
   // if( std::distance(cursor.begin(), cursor.end()) == 0 ) {
   //    collection.insert_one( std::move( bson ) );
   // } else {
   //    // bsoncxx::document doc{};
   //    collection.delete_one( bsoncxx::builder::stream::document{} << "id" << 1 << bsoncxx::builder::stream::finalize );
   //    collection.insert_one( std::move( bson ) );
   // }

   std::cout << "Database is close (MongoDB)" << std::endl;
}

template<typename ObjectType, typename Modifier>
void database::modify( const ObjectType& obj, Modifier&& m )
{
   ObjectType new_object = obj;
   m( new_object );

   auto bson = to_bson( new_object );

   std::string name_collection( BOOST_PP_STRINGIZE( ObjectType ) );
   auto collection = _client["testdb"][name_collection];

   collection.update_one(document{} << "id" << obj.id << finalize, document{} << "$set" << bson << finalize);

   std::cout << "*********************************   modify   *********************************" << std::endl;
   auto cursor = collection.find({});
   for (auto&& doc : cursor) {
      std::cout << bsoncxx::to_json(doc) << std::endl;
   }
}

template<typename ObjectType>
void database::remove( const ObjectType& obj )
{
   std::string name_collection( BOOST_PP_STRINGIZE( ObjectType ) );
   auto collection = _client["testdb"][name_collection];

   collection.delete_one( document{} << "id" << obj.id << finalize );

   std::cout << "*********************************   remove   *********************************" << std::endl;
   auto cursor = collection.find({});
   for (auto&& doc : cursor) {
      std::cout << bsoncxx::to_json(doc) << std::endl;
   }
}

template<typename ObjectType, typename Constructor>
const ObjectType database::create( Constructor&& con )
{
   ObjectType new_object;
   con( new_object );

   auto bson = to_bson( new_object );

   std::string name_collection( BOOST_PP_STRINGIZE( ObjectType ) );
   auto collection = _client["testdb"][name_collection];
   collection.insert_one( bson.view() );

   std::cout << "*********************************   create   *********************************" << std::endl;
   auto cursor = collection.find({});
   for (auto&& doc : cursor) {
      std::cout << bsoncxx::to_json(doc) << std::endl;
   }

   return new_object;
}

// template<typename ObjectType>
// bsoncxx::document::value database::object_to_bson( const ObjectType& obj )
// {
//    fc_pp::variant vresult;
//    fc_pp::to_variant( obj, vresult );
//    auto var = fc_pp::json::to_string( vresult );
//    return bsoncxx::from_json(var);
// }





class abcd2 {

public:

   int64_t a;
   std::string b;
   std::vector<int64_t> c;

};
FC_REFLECT( abcd2, (a)(b)(c) )

class abcd3 {

public:

   int64_t id;

   int64_t a;
   std::string b;
   uint8_t c;
   int8_t d;
   uint16_t q;
   int16_t w;
   uint32_t e;
   int32_t r;
   // uint64_t // TODO
   int64_t t;
   float y;
   double u;
   std::vector<char> data;
   abcd2 dsa;
   std::pair<int64_t, abcd2> p;
};
FC_REFLECT( abcd3, (id)(a)(b)(c)(d)(q)(w)(e)(r)(t)(y)(u)(data)(dsa)(p) )


// class account_object : public chainbase::object<account_object_type, account_object> {
//       OBJECT_CTOR(account_object,(code)(abi))

//       id_type              id;
//       account_name         name;
//       uint8_t              vm_type      = 0;
//       uint8_t              vm_version   = 0;
//       bool                 privileged   = false;

//       time_point           last_code_update;
//       digest_type          code_version;
//       block_timestamp_type creation_date;

//       shared_blob    code;
//       shared_blob    abi;

//       void set_abi( const eosio::chain::abi_def& a ) {
//          abi.resize( fc_pp::raw::pack_size( a ) );
//          fc_pp::datastream<char*> ds( abi.data(), abi.size() );
//          fc_pp::raw::pack( ds, a );
//       }

//       eosio::chain::abi_def get_abi()const {
//          eosio::chain::abi_def a;
//          EOS_ASSERT( abi.size() != 0, abi_not_found_exception, "No ABI set on account ${n}", ("n",name) );

//          fc_pp::datastream<const char*> ds( abi.data(), abi.size() );
//          fc_pp::raw::unpack( ds, a );
//          return a;
//       }
//    };










class elementary_types {
public:

   bool operator<( const elementary_types& value ) const
   {
      if(uint8t < value.uint8t && int8t < value.int8t &&
         uint16t < value.uint16t && int16t < value.int16t &&
         uint32t < value.uint32t && int32t < value.int32t &&
         int64t < value.int64t )
      {
         return true;
      }
      return false;
   }

   uint8_t uint8t;
   int8_t int8t;
   uint16_t uint16t;
   int16_t int16t;
   uint32_t uint32t;
   int32_t int32t;
   // uint64_t uint64t; // TODO
   int64_t int64t;
};

class elementary_types2 {
public:

   bool boolt;
   double doublet;
   float floatt;
};

class custom_type {
public:
   elementary_types elem_types;
};

class pair_type {
public:
   std::pair<int64_t,elementary_types> pair;
};

class array_type {
public:
   std::array<elementary_types, 2> array1;
   std::array<int64_t, 5> array2;
};

class vector_type {
public:
   std::vector<elementary_types> vector1;
   std::vector<int64_t> vector2;
};

class deque_type {
public:
   std::deque<elementary_types> deque1;
   std::deque<int64_t> deque2;
};

class set_type {
public:
   std::set<elementary_types> set1;
   std::set<int64_t> set2;
};

class map_type {
public:
   std::map<int64_t, elementary_types> map1;
   std::map<int64_t, int64_t> map2;
   std::unordered_map<int64_t, elementary_types> map3;
   std::unordered_map<int64_t, int64_t> map4;
   std::multimap<int64_t, elementary_types> map5;
   std::multimap<int64_t, int64_t> map6;
};

FC_REFLECT( elementary_types, (uint8t)(int8t)(uint16t)(int16t)(uint32t)(int32t)/*(uint64t)*/(int64t) )
FC_REFLECT( elementary_types2, (boolt)(doublet)(floatt) )
FC_REFLECT( custom_type, (elem_types) )
FC_REFLECT( pair_type, (pair) )
FC_REFLECT( array_type, (array1)(array2) )
FC_REFLECT( vector_type, (vector1)(vector2) )
FC_REFLECT( deque_type, (deque1)(deque2) )
FC_REFLECT( set_type, (set1)(set2) )
FC_REFLECT( map_type, (map1)(map2)(map3)(map4)(map5)(map6) )

BOOST_AUTO_TEST_SUITE(database_mongodb_tests)

///////////////////////////////////////////////////////////////////////  OBJECT to BSON test begin
BOOST_AUTO_TEST_CASE( elementary_types_test )
{
   std::string standard_for_elementary_types{ "{ \"uint8t\" : 1, \"int8t\" : 2, \"uint16t\" : 3, \"int16t\" : 4, \"uint32t\" : 5, \"int32t\" : 6, \"int64t\" : 7 }" };
   auto doc = to_bson( elementary_types{1,2,3,4,5,6,7} );
   // std::cout << bsoncxx::to_json(doc) << std::endl;
   BOOST_CHECK( standard_for_elementary_types == bsoncxx::to_json(doc) );
}

BOOST_AUTO_TEST_CASE( elementary_types2_test )
{
   std::string standard_for_elementary_types2{ "{ \"boolt\" : true, \"doublet\" : 2.2000000000000001776, \"floatt\" : 3.2999999523162841797 }" };
   auto doc = to_bson( elementary_types2{true,2.2,3.3} );
   // std::cout << bsoncxx::to_json(doc) << std::endl;
   BOOST_CHECK( standard_for_elementary_types2 == bsoncxx::to_json(doc) );
}

BOOST_AUTO_TEST_CASE( custom_type_test )
{
   std::string standard_for_custom_type{ "{ \"elem_types\" : { \"uint8t\" : 1, \"int8t\" : 2, \"uint16t\" : 3, \"int16t\" : 4, \"uint32t\" : 5, \"int32t\" : 6, \"int64t\" : 7 } }" };
   auto doc = to_bson( custom_type{ {1,2,3,4,5,6,7} } );
   // std::cout << bsoncxx::to_json(doc) << std::endl;
   BOOST_CHECK( standard_for_custom_type == bsoncxx::to_json(doc) );
}

BOOST_AUTO_TEST_CASE( pair_type_test )
{
   std::string standard_for_pair_type{ "{ \"pair\" : { \"first\" : 13, \"second\" : { \"uint8t\" : 1, \"int8t\" : 2, \"uint16t\" : 3, \"int16t\" : 4, \"uint32t\" : 5, \"int32t\" : 6, \"int64t\" : 7 } } }" };
   auto doc = to_bson( pair_type{ std::make_pair( 13, elementary_types{1,2,3,4,5,6,7} ) } );
   // std::cout << bsoncxx::to_json(doc) << std::endl;
   BOOST_CHECK( standard_for_pair_type == bsoncxx::to_json(doc) );
}

BOOST_AUTO_TEST_CASE( array_type_test )
{
   std::string standard_for_array_type{ "{ \"array1\" : [ { \"uint8t\" : 1, \"int8t\" : 2, \"uint16t\" : 3, \"int16t\" : 4, \"uint32t\" : 5, \"int32t\" : 6, \"int64t\" : 7 }, { \"uint8t\" : 8, \"int8t\" : 9, \"uint16t\" : 10, \"int16t\" : 11, \"uint32t\" : 12, \"int32t\" : 13, \"int64t\" : 14 } ], \"array2\" : [ 1, 2, 3, 4, 5 ] }" };

   array_type obj;
   obj.array1 = { elementary_types{1,2,3,4,5,6,7}, elementary_types{8,9,10,11,12,13,14} };
   obj.array2 = { 1,2,3,4,5 };

   auto doc = to_bson( obj );
   // std::cout << bsoncxx::to_json(doc) << std::endl;
   BOOST_CHECK( standard_for_array_type == bsoncxx::to_json(doc) );
}

BOOST_AUTO_TEST_CASE( vector_type_test )
{
   std::string standard_for_vector_type{ "{ \"vector1\" : [ { \"uint8t\" : 1, \"int8t\" : 2, \"uint16t\" : 3, \"int16t\" : 4, \"uint32t\" : 5, \"int32t\" : 6, \"int64t\" : 7 }, { \"uint8t\" : 8, \"int8t\" : 9, \"uint16t\" : 10, \"int16t\" : 11, \"uint32t\" : 12, \"int32t\" : 13, \"int64t\" : 14 } ], \"vector2\" : [ 1, 2, 3, 4, 5 ] }" };

   vector_type obj;
   obj.vector1 = { elementary_types{1,2,3,4,5,6,7}, elementary_types{8,9,10,11,12,13,14} };
   obj.vector2 = { 1,2,3,4,5 };

   auto doc = to_bson( obj );
   // std::cout << bsoncxx::to_json(doc) << std::endl;
   BOOST_CHECK( standard_for_vector_type == bsoncxx::to_json(doc) );
}

BOOST_AUTO_TEST_CASE( deque_type_test )
{
   std::string standard_for_deque_type{ "{ \"deque1\" : [ { \"uint8t\" : 1, \"int8t\" : 2, \"uint16t\" : 3, \"int16t\" : 4, \"uint32t\" : 5, \"int32t\" : 6, \"int64t\" : 7 }, { \"uint8t\" : 8, \"int8t\" : 9, \"uint16t\" : 10, \"int16t\" : 11, \"uint32t\" : 12, \"int32t\" : 13, \"int64t\" : 14 } ], \"deque2\" : [ 1, 2, 3, 4, 5 ] }" };

   deque_type obj;
   obj.deque1 = { elementary_types{1,2,3,4,5,6,7}, elementary_types{8,9,10,11,12,13,14} };
   obj.deque2 = { 1,2,3,4,5 };

   auto doc = to_bson( obj );
   // std::cout << bsoncxx::to_json(doc) << std::endl;
   BOOST_CHECK( standard_for_deque_type == bsoncxx::to_json(doc) );
}

BOOST_AUTO_TEST_CASE( set_type_test )
{
   std::string standard_for_set_type{ "{ \"set1\" : [ { \"uint8t\" : 1, \"int8t\" : 2, \"uint16t\" : 3, \"int16t\" : 4, \"uint32t\" : 5, \"int32t\" : 6, \"int64t\" : 7 }, { \"uint8t\" : 8, \"int8t\" : 9, \"uint16t\" : 10, \"int16t\" : 11, \"uint32t\" : 12, \"int32t\" : 13, \"int64t\" : 14 } ], \"set2\" : [ 1, 2, 3, 4, 5 ] }" };

   set_type obj;
   obj.set1 = { elementary_types{1,2,3,4,5,6,7}, elementary_types{8,9,10,11,12,13,14} };
   obj.set2 = { 1,2,3,4,5 };

   auto doc = to_bson( obj );
   // std::cout << bsoncxx::to_json(doc) << std::endl;
   BOOST_CHECK( standard_for_set_type == bsoncxx::to_json(doc) );
}

BOOST_AUTO_TEST_CASE( map_type_test )
{
   std::string standard_for_map_type{ "{ \"map1\" : [ { \"first\" : 1, \"second\" : { \"uint8t\" : 1, \"int8t\" : 2, \"uint16t\" : 3, \"int16t\" : 4, \"uint32t\" : 5, \"int32t\" : 6, \"int64t\" : 7 } }, { \"first\" : 2, \"second\" : { \"uint8t\" : 8, \"int8t\" : 9, \"uint16t\" : 10, \"int16t\" : 11, \"uint32t\" : 12, \"int32t\" : 13, \"int64t\" : 14 } } ], \"map2\" : [ { \"first\" : 1, \"second\" : 1 }, { \"first\" : 2, \"second\" : 2 }, { \"first\" : 3, \"second\" : 3 }, { \"first\" : 4, \"second\" : 4 }, { \"first\" : 5, \"second\" : 5 } ], \"map3\" : [ { \"first\" : 2, \"second\" : { \"uint8t\" : 8, \"int8t\" : 9, \"uint16t\" : 10, \"int16t\" : 11, \"uint32t\" : 12, \"int32t\" : 13, \"int64t\" : 14 } }, { \"first\" : 1, \"second\" : { \"uint8t\" : 1, \"int8t\" : 2, \"uint16t\" : 3, \"int16t\" : 4, \"uint32t\" : 5, \"int32t\" : 6, \"int64t\" : 7 } } ], \"map4\" : [ { \"first\" : 5, \"second\" : 5 }, { \"first\" : 4, \"second\" : 4 }, { \"first\" : 3, \"second\" : 3 }, { \"first\" : 2, \"second\" : 2 }, { \"first\" : 1, \"second\" : 1 } ], \"map5\" : [ { \"first\" : 1, \"second\" : { \"uint8t\" : 1, \"int8t\" : 2, \"uint16t\" : 3, \"int16t\" : 4, \"uint32t\" : 5, \"int32t\" : 6, \"int64t\" : 7 } }, { \"first\" : 2, \"second\" : { \"uint8t\" : 8, \"int8t\" : 9, \"uint16t\" : 10, \"int16t\" : 11, \"uint32t\" : 12, \"int32t\" : 13, \"int64t\" : 14 } } ], \"map6\" : [ { \"first\" : 1, \"second\" : 1 }, { \"first\" : 2, \"second\" : 2 }, { \"first\" : 3, \"second\" : 3 }, { \"first\" : 4, \"second\" : 4 }, { \"first\" : 5, \"second\" : 5 } ] }" };

   map_type obj;
   obj.map1 = { { 1, elementary_types{1,2,3,4,5,6,7} }, { 2, elementary_types{8,9,10,11,12,13,14} } };
   obj.map2 = { { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 }, { 5, 5 } };
   obj.map3 = { { 1, elementary_types{1,2,3,4,5,6,7} }, { 2, elementary_types{8,9,10,11,12,13,14} } };
   obj.map4 = { { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 }, { 5, 5 } };
   obj.map5 = { { 1, elementary_types{1,2,3,4,5,6,7} }, { 2, elementary_types{8,9,10,11,12,13,14} } };
   obj.map6 = { { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 }, { 5, 5 } };

   auto doc = to_bson( obj );
   // std::cout << bsoncxx::to_json(doc) << std::endl;
   BOOST_CHECK( standard_for_map_type == bsoncxx::to_json(doc) );
}
///////////////////////////////////////////////////////////////////////  OBJECT to BSON test end





BOOST_AUTO_TEST_CASE(asd2)
{

   database db(mongocxx::uri{});

   abcd3 obj;

   for(size_t i = 0; i < 3; i++) {
      obj = db.create<abcd3>( [&]( abcd3& gtx ) {
         gtx.id = i;
         gtx.a = 1;
         gtx.b = "asd";
         gtx.c = 2;
         gtx.d = 3;
         gtx.q = 0;
         gtx.w = 4;
         gtx.e = 5;
         gtx.r = 6;
         gtx.t = 7;
         gtx.y = 8.8;
         gtx.u = 9.9;
         gtx.data = {0x01,0x02,0x03};
         gtx.dsa = {13,"13",{1,2,3}};
         gtx.p = std::make_pair(65,gtx.dsa);
      });
   }

   db.modify( obj, [&]( abcd3& gtx ) {
      gtx.a = 11;
      gtx.b = "asd1";
      gtx.c = 21;
      gtx.d = 31;
      gtx.q = 01;
      gtx.w = 41;
      gtx.e = 51;
      gtx.r = 61;
      gtx.t = 71;
      gtx.y = 8.81;
      gtx.u = 9.91;
      gtx.data = {0x11,0x12,0x13};
      gtx.dsa = {131,"131",{11,21,31}};
      gtx.p = std::make_pair(651,gtx.dsa);
   });

   db.remove( obj );


   // auto array_builder = bsoncxx::builder::stream::array{};
   // array_builder << 1 << 2 << 3;

   // bsoncxx::builder::stream::document doc;
   // doc << "array" << array_builder;

   // doc << "array2" << array_builder;

   // std::cout << bsoncxx::to_json(doc) << std::endl;

   // const auto new_object = db.create<abcd3>( [&]( abcd3& gtx ) {
   // account_object

   // const auto new_object2 = db.create<graphene::chain::balance_object>( [&]( graphene::chain::balance_object& gtx ) {
   //    fc_pp::ecc::public_key_data key;
   //    for( size_t i = 0; i < 33; i++) {
   //       key.at(i) = 0x13;
   //    }
   //    gtx.owner = graphene::chain::address( key );
   //    gtx.balance = graphene::chain::asset( 13, graphene::chain::asset_id_type(13) );
   //    gtx.last_claim_date = fc_pp::time_point_sec(15151515);
   // });


   // mongocxx::instance inst{};
   // mongocxx::client conn{mongocxx::uri{}};

   // auto collection = conn["testdb2"]["testcollection"];

   // abcd3 obj;
   // obj.a = 1;
   // obj.b = "asd";
   // obj.c = 2;
   // obj.d = 3;
   // obj.q = 0;
   // obj.w = 4;
   // obj.e = 5;
   // obj.r = 6;
   // obj.t = 7;
   // obj.y = 8.8;
   // obj.u = 9.9;
   // obj.data = {0x01,0x02,0x03};
   // obj.dsa = {13,"13",{1,2,3}};
   // obj.p = std::make_pair(65,obj.dsa);

   // std::string name2( BOOST_PP_STRINGIZE(abcd3) );
   // std::cout << name2 << std::endl;

   // fc_pp::variant vresult;
   // fc_pp::to_variant( obj, vresult );
   // auto var = fc_pp::json::to_string( vresult );

   // auto asd = bsoncxx::from_json(var);

   // collection.insert_one(std::move(asd));
   // auto cursor = collection.find({});

   // for (auto&& doc : cursor) {
   //    std::cout << bsoncxx::to_json(doc) << std::endl;
   // }
}

BOOST_AUTO_TEST_SUITE_END()
