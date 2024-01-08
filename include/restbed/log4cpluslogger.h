/*
 * Copyright (c) 2024 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */

#include <log4cplus/logger.h>
#include <restbed>

typedef void (*FnPtr)(const std::shared_ptr<restbed::Session> session);

namespace restbed
{

class Log4cplusLogger : public restbed::Logger
{
    private:
    log4cplus::Logger logger;

    public:
    Log4cplusLogger();
    void stop(void);
    void start(const std::shared_ptr<const Settings>&);
    void log(const Level, const char* format, ...);
    void log_if(bool expression, const Level level, const char* format, ...);
};

} // namespace restbed
