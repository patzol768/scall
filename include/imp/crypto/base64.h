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

std::string base64_encode(const std::vector<unsigned char>& binary);
std::string base64_encode(const unsigned char* ptr, const size_t len);

std::vector<unsigned char> base64_decode(const char* encoded);

}
}
