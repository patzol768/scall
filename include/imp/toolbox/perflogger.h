/*
 * Copyright (c) 2024 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */

#pragma once

#include <chrono>
#include <string>

namespace imp
{
namespace toolbox
{

class Perflogger
{
    public:
    Perflogger() = delete;
    Perflogger(std::string const& what);
    ~Perflogger();

    void set_result(std::string const& result);
    void mark(std::string const& note = "");

    private:
    void print(bool final, std::string const& note = "") const;

    std::string m_what;
    bool m_has_result;
    std::string m_result;
    std::chrono::steady_clock::time_point m_from;
    int m_marker;
};

} // namespace toolbox
} // namespace imp
