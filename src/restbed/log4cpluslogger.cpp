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
#include <log4cplus/configurator.h>

#include "restbed/log4cpluslogger.h"

using namespace std;
using namespace restbed;

namespace restbed
{

static const map<int, log4cplus::LogLevel> logLevelMap = {{restbed::Logger::INFO, log4cplus::INFO_LOG_LEVEL},
                                          {restbed::Logger::DEBUG, log4cplus::DEBUG_LOG_LEVEL},
                                          {restbed::Logger::FATAL, log4cplus::FATAL_LOG_LEVEL},
                                          {restbed::Logger::ERROR, log4cplus::ERROR_LOG_LEVEL},
                                          {restbed::Logger::WARNING, log4cplus::WARN_LOG_LEVEL},
                                          {restbed::Logger::SECURITY, log4cplus::ERROR_LOG_LEVEL}

};

Log4cplusLogger::Log4cplusLogger(void) : logger(log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("restbed")))
{
    return;
}

void Log4cplusLogger::stop(void)
{
    return;
}

void Log4cplusLogger::start(const shared_ptr<const Settings> &)
{
    return;
}

void Log4cplusLogger::log(const Level logLevel, const char *format, ...)
{
    va_list arguments;
    va_start(arguments, format);

    va_list temporary;
    va_copy(temporary, arguments);
    const auto length = vsnprintf(nullptr, 0, format, temporary) + 1;
    va_end(temporary);

    string buffer;
    buffer.resize(length);

    if (vsnprintf(&buffer[0], length, format, arguments) < 0)
    {
        throw runtime_error("");
    }

    buffer.pop_back();

    //logger.log(restbed::logLevelMap.at(logLevel), buffer, __FILE__, __LINE__, LOG4CPLUS_MACRO_FUNCTION());
    logger.log(restbed::logLevelMap.at(logLevel), buffer, __FILE__, __LINE__);

    va_end(arguments);
}

void Log4cplusLogger::log_if(bool expression, const Logger::Level level, const char *format, ...)
{
    if (expression)
    {
        va_list arguments;
        va_start(arguments, format);
        log(level, format, arguments);
        va_end(arguments);
    }
}

}; // namespace restbed
