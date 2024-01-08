/*
 * Copyright (c) 2024 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */

#include <corvusoft/restbed/request.hpp>
#include <corvusoft/restbed/session.hpp>
#include <log4cplus/loggingmacros.h>
#include <sstream>

#include "imp/app/restbed/message_log.h"

using log4cplus::Logger;
using restbed::Bytes;
using restbed::Response;
using restbed::Session;
using std::endl;
using std::ostringstream;
using std::shared_ptr;
using std::string;

namespace imp
{
namespace app
{
namespace restbed
{

/**
 *  Message logging cannot run as a rule, since when rule runs, the request's body is not available.
 */
void log_request_message(const shared_ptr<Session>& session, bool log_body)
{
    auto logger = Logger::getInstance(LOG4CPLUS_TEXT("message"));
    auto request = session->get_request();
    ostringstream oss;

    // Network data
    oss << session->get_origin() << " --> " << session->get_destination() << " ";

    // Session data
    oss << "[" << session->get_id() << "] ";

    // URI
    oss << request->get_method() << " " << request->get_path();

    // Path params
    const auto pathparam = request->get_path_parameters();
    for (auto pp : pathparam)
    {
        oss << " {" << pp.first << "}=" << pp.second;
    }

    // Query params
    const auto queryparam = request->get_query_parameters();
    for (auto qp : queryparam)
    {
        oss << " &" << qp.first << "=" << qp.second;
    }

    // Header params
    const auto headerparam = request->get_headers();
    for (auto hp : headerparam)
    {
        oss << endl
            << hp.first << ": " << hp.second;
    }

    // Body
    if (log_body && (request->get_method() == "GET" || request->get_method() == "POST"))
    {
        oss << endl
            << endl;

        size_t content_length = request->get_header("Content-Length", 0);

        if (content_length > 0)
        {
            auto body = request->get_body();

            if (body.size())
            {
                string body_string(body.begin(), body.end());
                oss << body_string;
            }
            else
            {
                oss << "*** Not available yet ***";
            }
        }
    }

    LOG4CPLUS_INFO(logger, oss.str());
}

/**
 *  Log only the body
 */
void log_request_body(const shared_ptr<Session>& session, const string& body)
{
    (void)session;

    auto logger = Logger::getInstance(LOG4CPLUS_TEXT("message"));

    if (!body.empty())
    {
        LOG4CPLUS_TRACE(logger, body);
    }
    else
    {
        LOG4CPLUS_TRACE(logger, "-- no request body --");
    }
}

} // namespace restbed
} // namespace app
} // namespace imp
