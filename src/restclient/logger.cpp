/*
 * Copyright (c) 2024 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */

#include <sstream>

#include <restclient/logger.h>

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

using std::endl;
using std::ostringstream;

namespace RestClient
{

Log4cplus_logger::Log4cplus_logger(std::string name)
: m_logger(log4cplus::Logger::getInstance(LOG4CPLUS_TEXT(name)))
{
}

// const std::map<curl_infotype, std::string> Log4cplus_logger::m_info_map = {
// {CURLINFO_TEXT, ""},
// {CURLINFO_HEADER_IN, "header_in"},
// {CURLINFO_HEADER_OUT, "header_out"},
// {CURLINFO_DATA_IN, "data_in"},
// {CURLINFO_DATA_OUT, "data_out"},
// {CURLINFO_SSL_DATA_IN, "ssl_data_in"},
// {CURLINFO_SSL_DATA_OUT, "ssl_data_out"},
// {CURLINFO_END, ""}};

void Log4cplus_logger::log(curl_infotype type, char* data, size_t size)
{
    ostringstream oss;
    bool print = false;

    switch (type)
    {
        case CURLINFO_TEXT:
        case CURLINFO_HEADER_IN:
        case CURLINFO_HEADER_OUT:
        case CURLINFO_DATA_IN:
        case CURLINFO_DATA_OUT:
        {
            // auto it = m_info_map.find(type);
            // if (it != m_info_map.end())
            // {
            //     oss << "(curl) " << it->second << ": ";
            // }

            while (data[size - 1] == '\n' && size > 0)
            {
                --size;
            }

            oss.write(data, size);
            print = true;
        }
        break;

        default:
            break;
    }

    if (print)
    {
        LOG4CPLUS_DEBUG(m_logger, oss.str());
    }
}

} // namespace RestClient