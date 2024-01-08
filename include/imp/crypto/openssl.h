/*
 * Copyright (c) 2024 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */

#pragma once

#include <openssl/x509.h>

#if OPENSSL_VERSION_NUMBER < 0x30000000L
#include <openssl/engine.h>
#endif

namespace imp
{
namespace crypto
{

struct EVP_PKEY_delete
{
    void operator()(EVP_PKEY* p)
    {
        if (p)
            EVP_PKEY_free(p);
    }
};

struct X509_delete
{
    void operator()(X509* p)
    {
        if (p)
            X509_free(p);
    }
};

struct FILE_delete
{
    void operator()(FILE* p)
    {
        if (p)
            fclose(p);
    }
};

#if OPENSSL_VERSION_NUMBER < 0x30000000L
// openssl engine deprecated in openssl 3

struct ENGINE_delete
{
    void operator()(ENGINE* p)
    {
        if (p)
            ENGINE_free(p);
    }
};
#endif

struct BIO_delete
{
    void operator()(BIO* p)
    {
        if (p)
            BIO_free_all(p);
    }
};

} // namespace crypto
} // namespace imp
