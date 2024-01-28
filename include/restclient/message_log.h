/*
 * Copyright (c) 2024 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */

#pragma once

#include <restclient-cpp/restclient.h>

namespace RestClient
{

std::string const& get_http_status_text(int const status);
std::string get_http_status_line(int const status);
void log_response(RestClient::Response const& rp);

}
