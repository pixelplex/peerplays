#include <boost/test/unit_test.hpp>
#include <wavm.hpp>
#include <iostream>


BOOST_AUTO_TEST_SUITE( wavm_name_tests )

BOOST_AUTO_TEST_CASE( id_to_wavm_name_test )
{
    auto name = vms::wavm::id_to_wavm_name( 1, 0 );
    BOOST_CHECK( eosio::chain::account_name( "a.2" ) == name );
    
    name = vms::wavm::id_to_wavm_name( 1, 1 );
    BOOST_CHECK( eosio::chain::account_name( "c.2" ) == name );

    name = vms::wavm::id_to_wavm_name( 30, 0 );
    BOOST_CHECK( eosio::chain::account_name( "a.z" ) == name );
    
    name = vms::wavm::id_to_wavm_name( 30, 1 );
    BOOST_CHECK( eosio::chain::account_name( "c.z" ) == name );

    name = vms::wavm::id_to_wavm_name( 31, 0 );
    BOOST_CHECK( eosio::chain::account_name( "a.21" ) == name );
    
    name = vms::wavm::id_to_wavm_name( 31, 1 );
    BOOST_CHECK( eosio::chain::account_name( "c.21" ) == name );

    name = vms::wavm::id_to_wavm_name( std::numeric_limits<uint32_t>::max(), 0 );
    BOOST_CHECK( eosio::chain::account_name( "a.5v1oyt4" ) == name );

    name = vms::wavm::id_to_wavm_name( std::numeric_limits<uint32_t>::max(), 1 );
    BOOST_CHECK( eosio::chain::account_name( "c.5v1oyt4" ) == name );
}

BOOST_AUTO_TEST_CASE( wavm_name_to_id_test )
{
    auto type_and_id = vms::wavm::wavm_name_to_id( "a.2" );
    BOOST_CHECK( type_and_id.first == 0 && type_and_id.second == 1 );
    
    type_and_id = vms::wavm::wavm_name_to_id( "c.2" );
    BOOST_CHECK( type_and_id.first == 1 && type_and_id.second == 1 );

    type_and_id = vms::wavm::wavm_name_to_id( "a.z" );
    BOOST_CHECK( type_and_id.first == 0 && type_and_id.second == 30 );
    
    type_and_id = vms::wavm::wavm_name_to_id( "c.z" );
    BOOST_CHECK( type_and_id.first == 1 && type_and_id.second == 30 );

    type_and_id = vms::wavm::wavm_name_to_id( "a.21" );
    BOOST_CHECK( type_and_id.first == 0 && type_and_id.second == 31 );
    
    type_and_id = vms::wavm::wavm_name_to_id( "c.21" );
    BOOST_CHECK( type_and_id.first == 1 && type_and_id.second == 31 );

    type_and_id = vms::wavm::wavm_name_to_id( "a.5v1oyt4" );
    BOOST_CHECK( type_and_id.first == 0 && type_and_id.second == std::numeric_limits<uint32_t>::max() );
    
    type_and_id = vms::wavm::wavm_name_to_id( "c.5v1oyt4" );
    BOOST_CHECK( type_and_id.first == 1 && type_and_id.second == std::numeric_limits<uint32_t>::max() );
}

BOOST_AUTO_TEST_SUITE_END()