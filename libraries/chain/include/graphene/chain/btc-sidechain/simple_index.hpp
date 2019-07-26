#pragma once

#include <fc/optional.hpp>

namespace btc_sidechain {

template<class T1>
class simple_index
{

public:

   using iterator = typename T1::iterator;

   std::pair<iterator,bool> insert( const typename T1::value_type& value ) {
      return data.insert( value );
   }

   template<class T2, typename Key>
   bool modify( const Key _key, const std::function<void( typename T1::value_type& e)>& func ) {
      const auto option = find_iterator<T2>( _key );
      if( !option )
         return false;
      data.modify( data.iterator_to( **option ), [&func]( typename T1::value_type& obj ) { func( obj ); } );
      return true;
   }

   template<class T2, typename Key>
   bool remove( const Key _key ) {
      const auto option = find_iterator<T2>( _key );
      if( !option )
         return false;
      data.erase( data.iterator_to( **option ) );
      return true;
   }

   size_t size() {
      return data.size();
   }

   template<class T2, typename Key>
   fc::optional<typename T1::value_type> find( const Key _key ) {
      auto& index = data.template get<T2>();
      auto it = index.find( _key );
      if( it != index.end() ) {
         return fc::optional<typename T1::value_type>(*it);
      }
      return fc::optional<typename T1::value_type>();
   }

   template<class T2>
   void safe_for(std::function<void(typename T1::template index<T2>::type::iterator itr1, 
                                    typename T1::template index<T2>::type::iterator itr2)> func) {
       auto& index = data.template get<T2>();
       func(index.begin(), index.end());
   }

private:

   template<class T2, typename Key>
   fc::optional<typename T1::template index<T2>::type::iterator> find_iterator( const Key _key ) {
      auto& index = data.template get<T2>();
      auto it = index.find( _key );
      if( it == index.end() )
         return fc::optional<typename T1::template index<T2>::type::iterator>();
      return it;
   }
   
   T1 data;

};

} // namespace btc_sidechain
