/*
 * Copyright (c) 2024 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */

#include <restclient/message_log.h>

#include <sstream>

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

using log4cplus::Logger;
using std::endl;
using std::ostringstream;
using std::to_string;

namespace RestClient
{

static const std::map<int, std::string> status_messages {
{100, "Continue"},
{101, "Switching Protocols"},
{102, "Processing"},
{200, "OK"},
{201, "Created"},
{202, "Accepted"},
{203, "Non-Authoritative Information"},
{204, "No Content"},
{205, "Reset Content"},
{206, "Partial Content"},
{207, "Multi-Status"},
{208, "Already Reported"},
{226, "IM Used"},
{300, "Multiple Choices"},
{301, "Moved Permanently"},
{302, "Found"},
{303, "See Other"},
{304, "Not Modified"},
{305, "Use Proxy"},
{306, "Reserved"},
{307, "Temporary Redirect"},
{308, "Permanent Redirect"},
{400, "Bad Request"},
{401, "Unauthorized"},
{402, "Payment Required"},
{403, "Forbidden"},
{404, "Not Found"},
{405, "Method Not Allowed"},
{406, "Not Acceptable"},
{407, "Proxy Authentication Required"},
{408, "Request Timeout"},
{409, "Conflict"},
{410, "Gone"},
{411, "Length Required"},
{412, "Precondition Failed"},
{413, "Request Entity Too Large"},
{414, "Request URI Too Long"},
{415, "Unsupported Media Type"},
{416, "Requested Range Not Satisfiable"},
{417, "Expectation Failed"},
{422, "Unprocessable Entity"},
{423, "Locked"},
{424, "Failed Dependency"},
{426, "Upgrade Required"},
{428, "Precondition Required"},
{429, "Too Many Requests"},
{431, "Request Header Fields Too Large"},
{500, "Internal Server Error"},
{501, "Not Implemented"},
{502, "Bad Gateway"},
{503, "Service Unavailable"},
{504, "Gateway Timeout"},
{505, "HTTP Version Not Supported"},
{506, "Variant Also Negotiates"},
{507, "Insufficient Storage"},
{508, "Loop Detected"},
{510, "Not Extended"},
{511, "Network Authentication Required"}};

static const std::string empty = "";

std::string const& get_http_status_text(int const status)
{
    auto it = status_messages.find(status);

    return (it != status_messages.end()) ? it->second : empty;
}

std::string get_http_status_line(int const status)
{
    return to_string(status) + " " + get_http_status_text(status);
}

void log_response(RestClient::Response const& rp)
{
    Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("message"));

    ostringstream oss;

    if (rp.code > 99)
    {
        oss << get_http_status_line(rp.code) << endl;

        for (auto const& header : rp.headers)
        {
            oss << header.first << ": " << header.second << endl;
        }

        if (!rp.body.empty())
        {
            oss << endl
                << rp.body;
        }
    }
    else
    {
        oss << "curl error (" + to_string(rp.code) + ") " + rp.body;
    }

    LOG4CPLUS_INFO(logger, oss.str());
}

} // namespace RestClient