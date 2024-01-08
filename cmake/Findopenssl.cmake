# <XXX>_FOUND           set
# <XXX>_LIBRARIES       set
# <XXX>_LINK_LIBRARIES  set
# <XXX>_LIBRARY_DIRS    set
# <XXX>_LDFLAGS         not set
# <XXX>_LDFLAGS_OTHER   not set
# <XXX>_INCLUDE_DIRS    set
# <XXX>_CFLAGS          not set
# <XXX>_CFLAGS_OTHER    not set

#
# library
#

list( APPEND OPENSSL_LIBRARIES "ssl" )
find_library( OPENSSL_LIB_SHARED "libssl.so" REQUIRED )
if( OPENSSL_LIB_SHARED )
    message( STATUS "Found ssl at: ${OPENSSL_LIB_SHARED}" )
    list( APPEND OPENSSL_LINK_LIBRARIES "${OPENSSL_LIB_SHARED}" )
    get_filename_component( TMP_LIB_DIR "${OPENSSL_LIB_SHARED}" DIRECTORY )
    list( APPEND OPENSSL_LIBRARY_DIRS "${TMP_LIB_DIR}" )
else ( )
    message( FATAL_ERROR "Failed to find libssl.so" )
endif ( )

list( APPEND OPENSSL_LIBRARIES "crypto" )
find_library( CRYPTO_LIB_SHARED "libcrypto.so" REQUIRED )
if( CRYPTO_LIB_SHARED )
    message( STATUS "Found crypto at: ${CRYPTO_LIB_SHARED}" )
    list( APPEND OPENSSL_LINK_LIBRARIES "${CRYPTO_LIB_SHARED}" )
    get_filename_component( TMP_LIB_DIR "${CRYPTO_LIB_SHARED}" DIRECTORY )
    list( APPEND OPENSSL_LIBRARY_DIRS "${TMP_LIB_DIR}" )
else ( )
    message( FATAL_ERROR "Failed to find libcrypto.so" )
endif ( )

#
# include
#

list( APPEND OPENSSL_INCLUDES "openssl/ssl.h" )
find_path( OPENSSL_INC_DIR "openssl/ssl.h" )
if( OPENSSL_INC_DIR )
    message( STATUS "Found openssl/ssl.h in: ${OPENSSL_INC_DIR}" )
    list( APPEND OPENSSL_INCLUDE_DIRS "${OPENSSL_INC_DIR}" )
else ( )
    message( FATAL_ERROR "Failed to find openssl/ssl.h" )
endif ( )

set( OPENSSL_FOUND TRUE )
