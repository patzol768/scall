/*
 * Copyright (c) 2024 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */

#pragma once

#include <memory>
#include <corvusoft/restbed/session.hpp>
#include <string>

namespace imp
{
namespace app
{
namespace restbed
{
/**
 *  Message logging cannot run as a rule, since when rule runs, the request's body is not available.
 */
void log_request_message(const std::shared_ptr<::restbed::Session>& session, bool log_body = true);

/**
 *  Log only the body
 */
void log_request_body(const std::shared_ptr<::restbed::Session>& session, const std::string& body);

} // namespace message
}
}
