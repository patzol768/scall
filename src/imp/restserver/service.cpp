/*
 * Copyright (c) 2024 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

#include <imp/restserver/service.h>

using restbed::Service;

namespace imp
{
namespace restserver
{

void service_ready_handler(Service& service)
{
    (void)service;

    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("main"));

    LOG4CPLUS_INFO(logger, "Hey! The services are up and running.");
}


} // namespace restserver
} // namespace imp
