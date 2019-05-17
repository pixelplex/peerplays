file(GLOB COMMON_SOURCES "common/*.cpp")

find_package( Gperftools QUIET )
if( GPERFTOOLS_FOUND )
    message( STATUS "Found gperftools; compiling tests with TCMalloc")
    list( APPEND PLATFORM_SPECIFIC_LIBS tcmalloc )
endif()

find_package(libmongoc-1.0 1.8)
if (libmongoc-1.0_FOUND)
   find_package(libbsoncxx-static REQUIRED)
   message(STATUS "Found bsoncxx headers: ${LIBBSONCXX_STATIC_INCLUDE_DIRS}")

   # mongo-cxx-driver 3.2 release altered LIBBSONCXX_LIBRARIES semantics. Instead of library names,
   #  it now hold library paths.
   if((LIBBSONCXX_STATIC_VERSION_MAJOR LESS 3) OR ((LIBBSONCXX_STATIC_VERSION_MAJOR EQUAL 3) AND (LIBBSONCXX_STATIC_VERSION_MINOR LESS 2)))
      find_library(LIBBSONCXX ${LIBBSONCXX_STATIC_LIBRARIES} PATHS ${LIBBSONCXX_STATIC_LIBRARY_DIRS} NO_DEFAULT_PATH)
   else()
      set(LIBBSONCXX ${LIBBSONCXX_STATIC_LIBRARIES})
   endif()

      message(STATUS "Found bsoncxx library: ${LIBBSONCXX}")

      find_package(libmongocxx-static REQUIRED)
      message(STATUS "Found mongocxx headers: ${LIBMONGOCXX_STATIC_INCLUDE_DIRS}")

      # mongo-cxx-driver 3.2 release altered LIBBSONCXX_LIBRARIES semantics. Instead of library names,
      #  it now hold library paths.
      if((LIBMONGOCXX_STATIC_VERSION_MAJOR LESS 3) OR ((LIBMONGOCXX_STATIC_VERSION_MAJOR EQUAL 3) AND (LIBMONGOCXX_STATIC_VERSION_MINOR LESS 2)))
         find_library(LIBMONGOCXX ${LIBMONGOCXX_STATIC_LIBRARIES} PATHS ${LIBMONGOCXX_STATIC_LIBRARY_DIRS} NO_DEFAULT_PATH)
      else()
         set(LIBMONGOCXX ${LIBMONGOCXX_STATIC_LIBRARIES})
      endif()

      message(STATUS "Found mongocxx library: ${LIBMONGOCXX}")
   else()
      message("Could NOT find MongoDB. mongo_db_plugin with MongoDB support will not be included.")
      return()
endif()

message(STATUS "1111111111111111111111: ${LIBMONGOCXX_STATIC_INCLUDE_DIRS}")
message(STATUS "2222222222222222222222: ${LIBBSONCXX_STATIC_INCLUDE_DIRS}")
message(STATUS "3333333333333333333333: ${LIBMONGOCXX}")
message(STATUS "4444444444444444444444: ${LIBBSONCXX}")

file(GLOB UNIT_TESTS "tests/*.cpp")
add_executable( chain_test ${UNIT_TESTS} ${COMMON_SOURCES} )
#target_link_libraries( chain_test graphene_chain graphene_app graphene_account_history graphene_bookie graphene_egenesis_none fc_pp graphene_wallet ${PLATFORM_SPECIFIC_LIBS} )
target_include_directories (chain_test PRIVATE ${LIBMONGOCXX_STATIC_INCLUDE_DIRS} ${LIBBSONCXX_STATIC_INCLUDE_DIRS} )
target_link_libraries( chain_test graphene_chain graphene_app graphene_account_history graphene_bookie graphene_egenesis_none fc_pp graphene_wallet ${PLATFORM_SPECIFIC_LIBS} ${LIBMONGOCXX} ${LIBBSONCXX} )
if(MSVC)
  set_source_files_properties( tests/serialization_tests.cpp PROPERTIES COMPILE_FLAGS "/bigobj" )
endif(MSVC)

file(GLOB PERFORMANCE_TESTS "performance/*.cpp")
add_executable( performance_test ${PERFORMANCE_TESTS} ${COMMON_SOURCES} )
target_link_libraries( performance_test graphene_chain graphene_app graphene_account_history graphene_bookie graphene_egenesis_none fc_pp ${PLATFORM_SPECIFIC_LIBS} )

file(GLOB BENCH_MARKS "benchmarks/*.cpp")
add_executable( chain_bench ${BENCH_MARKS} ${COMMON_SOURCES} )
target_link_libraries( chain_bench graphene_chain graphene_app graphene_account_history graphene_bookie graphene_egenesis_none fc_pp ${PLATFORM_SPECIFIC_LIBS} )

file(GLOB APP_SOURCES "app/*.cpp")
add_executable( app_test ${APP_SOURCES} )
target_link_libraries( app_test graphene_app graphene_account_history graphene_bookie graphene_net graphene_chain graphene_egenesis_none fc_pp ${PLATFORM_SPECIFIC_LIBS} )

file(GLOB INTENSE_SOURCES "intense/*.cpp")
add_executable( intense_test ${INTENSE_SOURCES} ${COMMON_SOURCES} )
target_link_libraries( intense_test graphene_chain graphene_app graphene_account_history graphene_bookie graphene_egenesis_none fc_pp ${PLATFORM_SPECIFIC_LIBS} )

file(GLOB BETTING_TESTS "betting/*.cpp")
add_executable( betting_test ${BETTING_TESTS} ${COMMON_SOURCES} )
target_link_libraries( betting_test graphene_chain graphene_app graphene_account_history graphene_bookie graphene_egenesis_none fc_pp graphene_wallet ${PLATFORM_SPECIFIC_LIBS} )

file(GLOB TOURNAMENT_TESTS "tournament/*.cpp")
add_executable( tournament_test ${TOURNAMENT_TESTS} ${COMMON_SOURCES} )
target_link_libraries( tournament_test graphene_chain graphene_app graphene_account_history graphene_egenesis_none fc_pp ${PLATFORM_SPECIFIC_LIBS} )

file(GLOB RANDOM_SOURCES "random/*.cpp")
add_executable( random_test ${RANDOM_SOURCES} ${COMMON_SOURCES} )
target_link_libraries( random_test graphene_chain graphene_app graphene_egenesis_none fc_pp ${PLATFORM_SPECIFIC_LIBS} )

add_subdirectory( generate_empty_blocks )
