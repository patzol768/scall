cmake_minimum_required( VERSION 3.14.0 FATAL_ERROR )

project( "scall" VERSION 0.1.0 LANGUAGES CXX C )
message( STATUS "See README.md" )

set( APP_NAME "scall" )

#
# Tools
#
include(FindPkgConfig)
include(FetchContent)

#
# Build options
#
option( ASIO_STANDALONE "Standalone ASIO, no boost"  ON )
option( OPENSSL_NO_ENGINE "Disable use of openssl engine"  ON )

#
# Build types
#
set( CMAKE_CONFIGURATION_TYPES Debug Release)

message("Generated with config types: ${CMAKE_CONFIGURATION_TYPES}")
message( STATUS "Configured for: ${CMAKE_BUILD_TYPE}" )

#
# Configuration
#
set( CMAKE_CXX_STANDARD 20 )
set( INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include" )
set( SOURCE_DIR  "${PROJECT_SOURCE_DIR}/src" )
set( TEST_SOURCE_DIR "${PROJECT_SOURCE_DIR}/test" )

if ( CMAKE_PREFIX_INITIALIZED_TO_DEFAULT )
    set( CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/distribution" CACHE PATH "Install path prefix" FORCE )
endif ( )

if( NOT WIN32 )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finput-charset=utf-8 -DBUILD_SSL" )

    if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
        message( STATUS "Debug build" )
        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb -Wall -Wno-deprecated-declarations -O0" )
    endif ( )

    if( CMAKE_BUILD_TYPE STREQUAL "Release" )
        message( STATUS "Release build" )
        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2" )
    endif ( )
endif ( )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake" )

if ( ASIO_STANDALONE )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DASIO_STANDALONE=YES" )
    message( STATUS "ASIO_STANDALONE: defined" )
else ( )
    message( STATUS "ASIO_STANDALONE: not defined" )
endif ( )
unset( ASIO_STANDALONE CACHE )

if ( OPENSSL_NO_ENGINE )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOPENSSL_NO_ENGINE" )
    message( STATUS "OPENSSL_NO_ENGINE: defined" )
else ( )
    message( STATUS "OPENSSL_NO_ENGINE: not defined" )
endif ( )
unset( OPENSSL_NO_ENGINE CACHE )

#
# Environment
#

#
# Libraries
#
find_package( openssl REQUIRED )

pkg_check_modules(LIBCURL libcurl REQUIRED) # required by restclient-cpp
pkg_check_modules(LOG4CPLUS log4cplus REQUIRED)

FetchContent_Declare( restbed GIT_REPOSITORY https://github.com/Corvusoft/restbed.git GIT_TAG 4.8 SOURCE_SUBDIR not_exist )
FetchContent_Declare( nlohmann_json GIT_REPOSITORY https://github.com/nlohmann/json.git GIT_TAG v3.11.2 SOURCE_SUBDIR not_exist )
FetchContent_Declare( Catch2 GIT_REPOSITORY https://github.com/catchorg/Catch2.git GIT_TAG v3.4.0 )

# note: only build restbed-shared from restbed
message( STATUS "Getting restbed")
FetchContent_GetProperties( restbed )
if( NOT restbed_POPULATED )
  FetchContent_Populate( restbed )
  set( BUILD_TESTS OFF CACHE BOOL "" FORCE )
  add_subdirectory( ${restbed_SOURCE_DIR} ${restbed_BINARY_DIR} EXCLUDE_FROM_ALL )
endif()

set( RESTBED_INCLUDE_DIRS ${restbed_SOURCE_DIR}/source )
message( STATUS "restbed include dir: ${RESTBED_INCLUDE_DIRS}" )

set( RESTBED_LIBRARY_DIRS ${restbed_BINARY_DIR} )
message( STATUS "restbed lib dir: ${RESTBED_LIBRARY_DIRS}" )

set( RESTBED_LIBRARIES "restbed" )

# note: header only, no build needed
message( STATUS "Getting nlohmann json")
FetchContent_MakeAvailable( nlohmann_json )

set( JSON_INCLUDE_DIRS ${nlohmann_json_SOURCE_DIR}/include )
message( STATUS "nlohmann::json include dir: ${JSON_INCLUDE_DIRS}" )

# note: has some changes in the source, hence not refreshed from github
message( STATUS "Set restclient_cpp")
set( RESTCLIENT_CPP_SOURCE "deps/restclient-cpp" )

add_subdirectory( ${RESTCLIENT_CPP_SOURCE} EXCLUDE_FROM_ALL )
set( RESTCLIENT_CPP_INCLUDE_DIRS "${RESTCLIENT_CPP_SOURCE}/include" )
set( RESTCLIENT_CPP_LIBRARY_DIRS "${RESTCLIENT_CPP_SOURCE}/.libs" )
set( RESTCLIENT_CPP_LIBRARIES "restclient-cpp" )

#
# Sources
#
##
## all *.cpp in 'source', except the 'generated'
##
file ( GLOB_RECURSE SOURCE_FILES "${SOURCE_DIR}/*.cpp" )

message( STATUS "Sources in: ${SOURCE_DIR}")

#
# RPATH setting
#
set( CMAKE_SKIP_BUILD_RPATH FALSE )
set( CMAKE_BUILD_WITH_INSTALL_RPATH FALSE ) 
set( CMAKE_INSTALL_RPATH "\${CMAKE_INSTALL_PREFIX}/lib" )
set( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE )

#
# Compile
#
add_executable( ${APP_NAME} ${SOURCE_FILES} )

target_include_directories( ${APP_NAME} PUBLIC ${INCLUDE_DIR} SYSTEM ${JSON_INCLUDE_DIRS} ${RESTBED_INCLUDE_DIRS} ${LIBCURL_INCLUDE_DIRS} ${RESTCLIENT_CPP_INCLUDE_DIRS} )
target_link_directories( ${APP_NAME} PUBLIC ${OPENSSL_LIBRARY_DIRS} ${RESTBED_LIBRARY_DIRS} ${LIBCURL_LIBRARY_DIRS} ${RESTCLIENT_CPP_LIBRARY_DIRS} ${LOG4CPLUS_LIBRARY_DIRS} )
target_link_libraries( ${APP_NAME} ${OPENSSL_LIBRARIES} ${RESTBED_LIBRARIES} ${LIBCURL_LIBRARIES} ${RESTCLIENT_CPP_LIBRARIES} ${LOG4CPLUS_LIBRARIES} )

set_target_properties( ${APP_NAME} PROPERTIES VERSION ${PROJECT_VERSION} )

add_dependencies( ${APP_NAME} restbed-shared )

#
# Install
#
install( TARGETS ${APP_NAME} "restbed-shared" "restclient-cpp"
         RUNTIME DESTINATION bin
         LIBRARY DESTINATION lib )

#
# Tests
#

if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
    set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)

    include(CTest)
    enable_testing()

    FetchContent_MakeAvailable( Catch2 )

    set( CATCH2_INCLUDE ${Catch2_SOURCE_DIR}/src )
    message( STATUS "Catch2 include: ${CATCH2_INCLUDE}")

    # each cpp file is a separate test file
    file ( GLOB_RECURSE TEST_SOURCE_FILES "${TEST_SOURCE_DIR}/*.cpp" )

    foreach ( TMP_PATH ${TEST_SOURCE_FILES} )
        get_filename_component ( TMP_APP_NAME ${TMP_PATH} NAME_WLE )

        add_executable ( ${TMP_APP_NAME} ${TMP_PATH} )

        target_include_directories( ${TMP_APP_NAME} PUBLIC ${INCLUDE_DIR} ${TEST_SOURCE_DIR} ${LOG4CPLUS_INCLUDE_DIRS} )
        target_link_directories( ${TMP_APP_NAME} PUBLIC ${LOG4CPLUS_LIBRARY_DIRS} )
        target_link_libraries( ${TMP_APP_NAME} Catch2::Catch2 ${LOG4CPLUS_LIBRARIES} )

        target_compile_options( ${TMP_APP_NAME} PUBLIC "-DUNIT_TEST" )

        add_test ( NAME ${TMP_APP_NAME} COMMAND ${TMP_APP_NAME} )

        set_property(TEST ${TMP_APP_NAME} PROPERTY ENVIRONMENT "LOG4CPLUS_CONFIG=../log.ini;IBAN_BANK_REGISTRY=../registry/bank_registry/;IBAN_REGISTRY=../registry/iban_registry/")
    endforeach ( TMP_PATH )
endif( )
