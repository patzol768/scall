/*
 * Copyright (c) 2024 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */

//
// This rule fills the MDC context of the log4cpus logger.
//

#include <corvusoft/restbed/request.hpp>
#include <corvusoft/restbed/session.hpp>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/mdc.h>

#include "imp/toolbox/id.h"
#include <imp/app/restbed/log_correlation_rule.h>

using imp::toolbox::create_uuid;
using ::restbed::Session;
using ::restbed::String;
using std::function;
using std::shared_ptr;

namespace imp
{
namespace app
{
namespace restbed
{

Log_correlation_rule::Log_correlation_rule()
: Rule()
{
    return;
}

Log_correlation_rule::~Log_correlation_rule() { }

bool Log_correlation_rule::condition(const shared_ptr<Session>)
{
    return true;
}

void Log_correlation_rule::action(const shared_ptr<Session> session, const function<void(const shared_ptr<Session>)>& callback)
{
    const auto request = session->get_request();
    log4cplus::MDC& mdc = log4cplus::getMDC();

    mdc.clear();

    std::string x_request_id;
    bool created = false;

    if (request->has_header("X-Request-ID"))
    {
        x_request_id = request->get_header("X-Request-ID", String::lowercase);
    }
    else
    {
        x_request_id = create_uuid();
        created = true;
    }

    mdc.put(LOG4CPLUS_TEXT("correlationId"), x_request_id);

    if (created)
    {
        log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("main"));

        LOG4CPLUS_WARN(logger, "missing x-request-id");
    }

    callback(session);
}

} // namespace restbed
} // namespace app
} // namespace imp
