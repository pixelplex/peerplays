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

#include <fc_pp/io/json.hpp>
#include <fc_pp/smart_ref_impl.hpp>
#include <fc_pp/variant.hpp>
#include <fc_pp/variant_object.hpp>

#include <graphene/chain/protocol/protocol.hpp>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace graphene::chain;

vector< fc_pp::variant_object > g_op_types;

template< typename T >
uint64_t get_wire_size()
{
   T data;
   return fc_pp::raw::pack( data ).size();
}

struct size_check_type_visitor
{
   typedef void result_type;

   int t = 0;
   size_check_type_visitor(int _t ):t(_t){}

   template<typename Type>
   result_type operator()( const Type& op )const
   {
      fc_pp::mutable_variant_object vo;
      vo["name"] = fc_pp::get_typename<Type>::name();
      vo["mem_size"] = sizeof( Type );
      vo["wire_size"] = get_wire_size<Type>();
      g_op_types.push_back( vo );
   }
};

int main( int argc, char** argv )
{
   try
   {
      graphene::chain::operation op;


      vector<uint64_t> witnesses; witnesses.resize(50);
      for( uint32_t i = 0; i < 60*60*24*30; ++i )
      {
         witnesses[ rand() % 50 ]++;
      }

      std::sort( witnesses.begin(), witnesses.end() );
      idump((witnesses.back() - witnesses.front()) );
      idump((60*60*24*30/50));
      idump(("deviation: ")((60*60*24*30/50-witnesses.front())/(60*60*24*30/50.0)));

      idump( (witnesses) );

      for( int32_t i = 0; i < op.count(); ++i )
      {
         op.set_which(i);
         op.visit( size_check_type_visitor(i) );
      }

      // sort them by mem size
      std::stable_sort( g_op_types.begin(), g_op_types.end(),
      [](const variant_object& oa, const variant_object& ob) {
      return oa["mem_size"].as_uint64() > ob["mem_size"].as_uint64();
      });
      std::cout << "[\n";
      for( size_t i=0; i<g_op_types.size(); i++ )
      {
         std::cout << "   " << fc_pp::json::to_string( g_op_types[i] );
         if( i < g_op_types.size()-1 )
            std::cout << ",\n";
         else
            std::cout << "\n";
      }
      std::cout << "]\n";
      std::cerr << "Size of block header: " << sizeof( block_header ) << " " << fc_pp::raw::pack_size( block_header() ) << "\n";
   }
   catch ( const fc_pp::exception& e ){ edump((e.to_detail_string())); }
   idump((sizeof(signed_block)));
   idump((fc_pp::raw::pack_size(signed_block())));
   return 0;
}
