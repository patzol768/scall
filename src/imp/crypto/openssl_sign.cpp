/*
 * Copyright (c) 2024 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */

#include <imp/app/error.h>
#include <imp/crypto/base64.h>
#include <imp/crypto/openssl_sign.h>

#include <assert.h>
#include <openssl/err.h>
#include <openssl/pem.h>

using imp::app::application_error;
using imp::crypto::base64_encode;
using std::string;
using std::vector;

namespace imp
{
namespace crypto
{

//--------------------------------------------------------
//-
//- Wrapper class for openssl signature creation
//-
//--------------------------------------------------------

Openssl_digest_sign::Openssl_digest_sign(string const& algorithm, EVP_PKEY* pkey)
{
    if (!pkey)
        throw application_error("ERR_SIGN_HTTP_PRIVATE_KEY_MISSING");

    m_ctx = EVP_MD_CTX_create();

    if (!m_ctx)
        throw application_error("ERR_CRYPTO_OPENSSL_ERROR: " + std::to_string(ERR_get_error()));

    const EVP_MD* md = EVP_get_digestbyname(algorithm.c_str());
    if (!md)
        throw application_error("ERR_SIGN_HTTP_ALGORITHM_INVALID:" + algorithm);

    if (EVP_DigestSignInit(m_ctx, nullptr, md, nullptr, pkey) != 1)
        throw application_error("ERR_CRYPTO_OPENSSL_ERROR" + std::to_string(ERR_get_error()));
}

Openssl_digest_sign::~Openssl_digest_sign()
{
    if (m_signresult)
        OPENSSL_free(m_signresult);
    if (m_ctx)
        EVP_MD_CTX_destroy(m_ctx);
}

void Openssl_digest_sign::update(const void* const data, size_t const size)
{
    assert(!m_finished);

    if (EVP_DigestSignUpdate(m_ctx, data, size) != 1)
        throw application_error("ERR_CRYPTO_OPENSSL_ERROR" + std::to_string(ERR_get_error()));
}

void Openssl_digest_sign::update(vector<uint8_t> const& bytesToHash)
{
    update(bytesToHash.data(), bytesToHash.size());
}

void Openssl_digest_sign::update(string const& str)
{
    update(str.c_str(), str.size());
}

void Openssl_digest_sign::finish()
{
    assert(!m_finished);
    m_finished = true;

    if (EVP_DigestSignFinal(m_ctx, nullptr, &m_result_length) != 1)
        throw application_error("ERR_CRYPTO_OPENSSL_ERROR" + std::to_string(ERR_get_error()));

    assert(m_result_length > 0);

    m_signresult = static_cast<uint8_t*>(OPENSSL_malloc(m_result_length));
    if (!m_signresult)
        throw application_error("ERR_CRYPTO_OPENSSL_ERROR" + std::to_string(ERR_get_error()));

    size_t vlen = m_result_length;

    if (EVP_DigestSignFinal(m_ctx, m_signresult, &m_result_length) != 1)
        throw application_error("ERR_CRYPTO_OPENSSL_ERROR" + std::to_string(ERR_get_error()));

    assert(m_result_length == vlen);
}

vector<uint8_t> Openssl_digest_sign::get_result()
{
    assert(m_finished);

    return vector<uint8_t>(m_signresult, m_signresult + m_result_length);
}

/**
 *  Digitally signs a string's binary representation.
 *
 *  @param string_to_sign The string, containing the original data for signing.
 *  @param pkey A reference to the sign key.
 *  @param asym_algorithm The name of the signing algorithm. *** NOT USED. PROBABLY THE PRIVATE KEY ITSELF DETERMINES THE ALGORITHM. ***
 *  @param hash_algorithm The name of the hash algorithm. (OpenSSL algorithms)
 *  @return The digital signature value in a Base64 encoded string format.
 */
// THX: https://wiki.openssl.org/index.php/EVP_Signing_and_Verifying
string calculate_hmac_base64(string const& string_to_sign, EVP_PKEY* pkey, string const& asym_algorithm, string const& hash_algorithm)
{
    assert(pkey != NULL);

    string hash_algo = hash_algorithm;
    transform(hash_algo.begin(), hash_algo.end(), hash_algo.begin(), ::toupper);

    Openssl_digest_sign signer(hash_algo, pkey);

    signer.update(string_to_sign);
    signer.finish();

    return base64_encode(signer.get_result());
}

/** Loads the private key from a file
 *
*/
std::unique_ptr<EVP_PKEY, EVP_PKEY_delete> pkey_from_file(std::string const& filename)
{
    std::unique_ptr<FILE, FILE_delete> fp(fopen(filename.c_str(), "r"));
    if (!fp)
    {
        throw application_error("ERR_CERT_PRIVATE_KEY_CANNOT_OPEN: " + filename);
    }

    string pass = "";   // TODO: add password handling

    std::unique_ptr<EVP_PKEY, EVP_PKEY_delete> pkey(PEM_read_PrivateKey(fp.get(), nullptr, nullptr, (pass.empty()) ? nullptr : const_cast<char*>(pass.c_str())));
    if (!pkey)
    {
        throw application_error("ERR_CERT_PRIVATE_KEY_CANNOT_READ: " + std::to_string(ERR_get_error()));
    }

    return pkey;
}

} // namespace crypto
} // namespace imp
