/*
 * Copyright (c) 2024 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */

#include <cstring>
#include <memory>
#include <stdexcept>

#include <openssl/bio.h>
#include <openssl/evp.h>

#include <imp/crypto/base64.h>

using std::string;
using std::unique_ptr;
using std::vector;

namespace
{

struct BIOFreeAll
{
    void operator()(BIO* p) { BIO_free_all(p); }
};

} // namespace


namespace imp
{
namespace crypto
{

/**
 *  Creates a Base64 encoded string representation from a byte object.
 *
 *  @param binary The original content to encode.
 *  @return The Base64 encoded string value.
 */
string base64_encode(const vector<unsigned char>& binary)
{
    unique_ptr<BIO, BIOFreeAll> b64(BIO_new(BIO_f_base64()));
    if (!b64.get())
        throw std::bad_alloc();
    BIO_set_flags(b64.get(), BIO_FLAGS_BASE64_NO_NL);

    BIO* sink = BIO_new(BIO_s_mem());
    if (!sink)
        throw std::bad_alloc();

    BIO_push(b64.get(), sink);
    BIO_write(b64.get(), binary.data(), binary.size());
    (void)BIO_flush(b64.get());

    const char* encoded;
    const long len = BIO_get_mem_data(sink, &encoded);
    return string(encoded, len);
}

/**
 *  Creates a Base64 encoded string representation from a binary char object.
 *
 *  @param ptr A pointer to the object to encode.
 *  @param len The size of the object in bytes.
 *  @return The Base64 encoded string value.
 */
string base64_encode(const unsigned char* ptr, const size_t len)
{
    const vector<unsigned char> vec(ptr, ptr + len);

    return base64_encode(vec);
}

/**
 *  Decodes a string, containing Base64 encoded data into its byte representation.
 *  Assumes no newlines or extra characters in encoded string
 *
 *  @param encoded The Base64 encoded value.
 *  @return A vector containing the decoded byte value
 */
vector<unsigned char> base64_decode(const char* encoded)
{
    std::unique_ptr<BIO, BIOFreeAll> b64(BIO_new(BIO_f_base64()));
    if (!b64.get())
        throw std::bad_alloc();

    BIO_set_flags(b64.get(), BIO_FLAGS_BASE64_NO_NL);

    // no unique_ptr, since b64 uses BIO_free_all
    BIO* source = BIO_new_mem_buf(encoded, -1); // read-only source
    if (!source)
        throw std::bad_alloc();

    BIO_push(b64.get(), source);

    const int maxlen = strlen(encoded) / 4 * 3 + 1;

    vector<unsigned char> decoded(maxlen);

    int len = BIO_read(b64.get(), decoded.data(), maxlen);
    if (len < 0)
    {
        // Ignore error, just pass an empty result back
        len = 0;
    }

    decoded.resize(len);

    return decoded;
}

} // namespace crypto
} // namespace imp
