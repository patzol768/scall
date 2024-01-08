/*
 * Copyright (c) 2024 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */

#pragma once

#include <string>
#include <vector>

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
 *  @return The hash value in a binary format.
 */
std::vector<uint8_t> digest(const void* const data, size_t const size, std::string const& algorithm);

/**
 *  Creates a hash for the string's binary representation.
 *
 *  @param stringToHash The string, containing the data for hashing.
 *  @param algorithm The name of the hash algorithm. (OpenSSL algorithms)
 *  @return The hash value in a binary format.
 */
std::vector<uint8_t> digest(std::string const& stringToHash, std::string const& algorithm);

std::vector<uint8_t> digest(std::vector<uint8_t> const& bytesToHash, std::string const& algorithm);

} // namespace crypto
} // namespace imp
