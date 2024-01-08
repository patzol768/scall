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
#include <map>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <unordered_set>
#include <utility>
#include <vector>

namespace imp
{
namespace toolbox
{
typedef std::logic_error tool_exception;

void replace_all(std::string& subject, const std::string& search, const std::string& replace);

std::string http_time();
void set_stdin_echo(bool enable);
std::string read_passwd_stdin(const std::string& prompt);

std::vector<std::string> split(const std::string& s, char delimiter);
std::vector<std::string> split_quoted(const std::string& str, const char delim = ',', const char quote = '"', const char escape = '\\');
std::pair<std::string, std::string> cut(const std::string& str, const char delim);
std::map<std::string, std::string> to_map(std::vector<std::string> const& v, char const delim = '"', bool const key_quoted = false, bool const value_quoted = false, char const quote = '"', char const escape = '\\');
std::string unquote(std::string const& str, const char delim, const char escape);

std::string emplace(std::string const& base, std::map<std::string, std::string> const& params);

std::pair<std::string, std::string> splitStrQuoted(const std::string& str, const char delim = '=', const bool quoted = true);
std::string trim(const std::string& s);
std::string i18n_to_string(std::string const& lang, std::vector<std::string> const& i18n_data);

std::string elapsed_time(const std::chrono::steady_clock::time_point& begin);
time_t now();
uint64_t now_millis();
time_t begin_of_day(time_t ts);
time_t end_of_day(time_t ts);
bool is_today(time_t ts);
bool is_leap_year(short year);
time_t mkgmtime(const struct tm *ptm);
time_t string_to_time_t(const std::string& s, bool eod = false);
time_t date_string_to_time_t(const std::string& s, bool eod = false);
time_t ldaptime_to_time_t(const std::string& s);
std::string gmttime_to_string(const std::time_t& t);
std::string gmttime_to_date_string(const std::time_t& t);
std::string gmttime_to_ldaptime(const std::time_t& t);

std::string demangle_typeid(std::type_info const& type_id);

struct case_insensitive_hasher
{
    size_t operator()(const std::string& key) const;
};

struct case_insensitive_comparer_2
{
    bool operator()(const std::string& x, const std::string& y) const;
};

// Deprecated
/*
struct case_insensitive_comparer : public std::unary_function<std::string, bool>
{
    std::string m_baseline;

    explicit case_insensitive_comparer(const std::string& baseline)
    : m_baseline(baseline)
    {
    }

    explicit case_insensitive_comparer(const std::pair<std::string, std::string>& baseline)
    : m_baseline(baseline.first)
    {
    }

    bool operator()(const std::string& arg);

    bool operator()(const std::pair<std::string, std::string>& arg);
};
*/

constexpr size_t const_strlen(const char* str)
{
    return (*str == 0) ? 0 : const_strlen(str + 1) + 1;
}

std::string buf_to_hex_string(const unsigned char* ptr, const size_t len);

std::string amount_to_string(int64_t amount_in_minor_unit, uint d);
int64_t amount_from_string(std::string const& amount_in_string, uint d);

} // namespace toolbox
} // namespace imp

#include "imp/toolbox/toolbox.tcc"
