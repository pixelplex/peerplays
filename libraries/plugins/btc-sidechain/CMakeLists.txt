file( GLOB BTC_SIDECHAIN_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/btc-sidechain/*.hpp" )
file( GLOB BTC_SIDECHAIN_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" )

add_library( btc_sidechain_plugin ${BTC_SIDECHAIN_SOURCES} ${BTC_SIDECHAIN_HEADERS} )

target_link_libraries( btc_sidechain_plugin graphene_net fc graphene_app zmq )
target_include_directories( btc_sidechain_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )

install( TARGETS
   btc_sidechain_plugin

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)