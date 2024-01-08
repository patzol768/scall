/*
 * Copyright (c) 2024 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */

#pragma once

#include <algorithm>
#include <map>
#include <memory>
#include <openssl/evp.h>
#include <string>
#include <vector>

#include <imp/crypto/openssl.h>

using std::transform;

namespace imp
{
namespace crypto
{

class Openssl_digest_sign
{
    public:
    Openssl_digest_sign(std::string const& algorithm, EVP_PKEY* pkey);
    ~Openssl_digest_sign();
    void update(std::string const& str);
    void update(const void* const data, size_t const size);
    void update(std::vector<uint8_t> const& bytesToHash);
    void finish();
    std::vector<uint8_t> get_result();

    private:
    EVP_MD_CTX* m_ctx = nullptr;
    uint8_t* m_signresult = nullptr;
    size_t m_result_length = 0;
    bool m_finished = false;
};

std::string calculate_hmac_base64(std::string const& string_to_sign, EVP_PKEY* pkey, std::string const& asym_algorithm, std::string const& hash_algorithm);

std::unique_ptr<EVP_PKEY, EVP_PKEY_delete> pkey_from_file(std::string const& filename);

} // namespace crypto
} // namespace imp
