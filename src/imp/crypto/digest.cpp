/*
 * Copyright (c) 2024 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */

#include <algorithm>
#include <openssl/err.h>
#include <openssl/evp.h>

#include "imp/app/error.h"
#include "imp/crypto/base64.h"
#include "imp/crypto/digest.h"

using imp::app::application_error;
using std::string;
using std::transform;

namespace imp
{
namespace crypto
{

/**
 *  Creates a hash for the binary array.
 *
 *  @param data Pointer to the beginning of the array
 *  @param size Number of bytes in the array
 *  @param algorithm The name of the hash algorithm. (OpenSSL algorithms)
 *  @return The hash value in a Base64 encoded string format.
 */
std::vector<uint8_t> digest(const void* const data, size_t const size, string const& algorithm)
{
    EVP_MD_CTX* ctx = EVP_MD_CTX_create();
    if (!ctx)
        throw application_error("ERR_CRYPTO_OPENSSL_ERROR" + std::to_string(ERR_get_error()));

    string hashAlgo = algorithm;
    // transform(hashAlgo.begin(), hashAlgo.end(), hashAlgo.begin(), ::toupper);

    const EVP_MD* md = EVP_get_digestbyname(algorithm.c_str());
    if (!md)
        throw application_error("ERR_ALGORITHM_ALGORITHM_INVALID: " + algorithm);

    int rc = EVP_DigestInit_ex(ctx, md, NULL);
    if (rc != 1)
        throw application_error("ERR_CRYPTO_OPENSSL_ERROR" + std::to_string(ERR_get_error()));

    rc = EVP_DigestUpdate(ctx, data, size);
    if (rc != 1)
        throw application_error("ERR_CRYPTO_OPENSSL_ERROR" + std::to_string(ERR_get_error()));

    unsigned char mdArr[EVP_MAX_MD_SIZE];
    unsigned int mdlen;
    rc = EVP_DigestFinal_ex(ctx, mdArr, &mdlen);
    if (rc != 1)
        throw application_error("ERR_CRYPTO_OPENSSL_ERROR" + std::to_string(ERR_get_error()));

    EVP_MD_CTX_destroy(ctx);

    std::vector<uint8_t> vec(mdArr, mdArr + mdlen);
    return vec;
}

/**
 *  Creates a hash for the string's binary representation.
 *
 *  @param stringToHash The string, containing the data for hashing.
 *  @param algorithm The name of the hash algorithm. (OpenSSL algorithms)
 *  @return The hash value in a Base64 encoded string format.
 */
std::vector<uint8_t> digest(string const& stringToHash, string const& algorithm)
{
    return digest(stringToHash.c_str(), stringToHash.size(), algorithm);
}

std::vector<uint8_t> digest(std::vector<uint8_t> const& bytesToHash, string const& algorithm)
{
    return digest(bytesToHash.data(), bytesToHash.size(), algorithm);
}

} // namespace crypto
} // namespace imp
