message( "You use OS X, so Boost::log will be in ExternalProject!")

cmake_minimum_required( VERSION 3.5.1 )

# For some external project macros
include(ExternalProject)

set( Boost_url "https://datapacket.dl.sourceforge.net/project/boost/boost/1.67.0/boost_1_67_0.tar.gz")
set( Boost_md5 4850fceb3f2222ee011d4f3ea304d2cb )
set( Boost_Bootstrap_Command ./bootstrap.sh )
set( Boost_b2_Command ./b2 )

set( Boost_dir ${CMAKE_BINARY_DIR}/deps/boostdir )

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(Boost_address_model 64)
else()
  set(Boost_address_model 32)
endif()

ExternalProject_Add(
  Boost_log
  BUILD_IN_SOURCE 1
  URL ${Boost_url}
  URL_MD5 ${Boost_md5}
  UPDATE_COMMAND ""
  CONFIGURE_COMMAND ${Boost_Bootstrap_Command} --prefix=${Boost_dir}/lib
  BUILD_COMMAND ${Boost_b2_Command} install  --prefix=${Boost_dir} --with-log cflags="-fvisibility=default" address-model=${Boost_address_model} link=static,shared --visibility=global
  INSTALL_COMMAND ""
)
set( Boost_INCLUDE_DIRS_ep ${Boost_dir}/include )
set( Boost_LIBRARIES_ep ${Boost_dir}/lib/libboost_log.a) 

# Configure lib

add_library( Boost::log STATIC IMPORTED )
file( MAKE_DIRECTORY "${Boost_INCLUDE_DIRS_ep}" )  # Must exist.
set_property( TARGET Boost::log PROPERTY IMPORTED_CONFIGURATIONS Release )
set_property( TARGET Boost::log PROPERTY IMPORTED_LOCATION_RELEASE "${Boost_LIBRARIES_ep}" )
set_property( TARGET Boost::log PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${Boost_INCLUDE_DIRS_ep}" )
add_dependencies( Boost::log Boost_log )
