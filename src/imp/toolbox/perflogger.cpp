/*
 * Copyright (c) 2024 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */

#include <iomanip>
#include <sstream>

#include <log4cplus/loggingmacros.h>

#include <imp/toolbox/perflogger.h>

namespace imp
{
namespace toolbox
{

Perflogger::Perflogger(std::string const& what)
: m_what(what)
, m_has_result(false)
, m_result()
, m_from(std::chrono::steady_clock::now())
, m_marker(0)
{
}

Perflogger::~Perflogger()
{
    print(true);
}

void Perflogger::set_result(std::string const& result)
{
    m_has_result = true;
    m_result = result;
}

void Perflogger::mark(std::string const& note)
{
    ++m_marker;
    print(false, note);
}

void Perflogger::print(bool final, std::string const& note) const
{
    std::chrono::steady_clock::time_point m_to = std::chrono::steady_clock::now();

    std::chrono::duration<double, std::milli> elapsed = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(m_to - m_from);

    std::ostringstream oss;
    oss << m_what;
    if (!final)
    {
        oss << "." << m_marker;
        if (!note.empty())
        {
            oss << "." << note;
        }
    }
    oss << ";" << (m_has_result ? m_result : "...") << ";";
    oss << std::fixed << std::setprecision(1) << elapsed.count() << " ms";

    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("perflog2"));
    LOG4CPLUS_INFO(logger, oss.str());
}

} // namespace toolbox
} // namespace imp
