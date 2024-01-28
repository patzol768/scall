/*
 * Copyright (c) 2024 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */

#pragma once

#include <log4cplus/logger.h>
#include <restclient-cpp/connection.h>

namespace RestClient
{

class Log4cplus_logger : public RestClient::Logger
{
    public:
    Log4cplus_logger(std::string name);

    void log(curl_infotype type, char* data, size_t size);

    private:
    log4cplus::Logger m_logger;
    static const std::map<curl_infotype, std::string> m_info_map;
};

} // namespace RestClient
