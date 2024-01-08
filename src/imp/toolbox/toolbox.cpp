/*
 * Copyright (c) 2024 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */

#include <algorithm>
#include <cctype>
#include <chrono>
#include <ctime>
#include <functional>
#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <cxxabi.h>
#include <uuid/uuid.h>

#include "imp/toolbox/toolbox.h"

using std::cin;
using std::cout;
using std::fixed;
using std::gmtime;
using std::hash;
using std::hex;
using std::ostringstream;
using std::pair;
using std::put_time;
using std::setfill;
using std::setprecision;
using std::setw;
using std::string;
using std::transform;
using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::steady_clock;
using std::chrono::system_clock;

namespace imp
{
namespace toolbox
{
/**
 *  Replaces all occurences of the search string inside the subject string with the another string.
 *  THX: https://stackoverflow.com/questions/3418231/replace-part-of-a-string-with-another-string
 *
 *  @param subject The subject string
 *  @param search The string to search for
 *  @param replace The string to insert istead of the search string
 *  @return Nothing
 */
void replace_all(string& subject, const string& search, const string& replace)
{
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != string::npos)
    {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
}

/**
 *  Gets the current time (UTC) in HTTP time string format.
 *
 *  @return Current time formatted as HTTP time string
 */
string http_time()
{
    ostringstream oss;

    std::time_t tt = system_clock::to_time_t(system_clock::now());

    struct std::tm* ptm = gmtime(&tt);
    oss << put_time(ptm, "%a, %d %b %Y %H:%M:%S %Z");

    return oss.str();
}

// See: https://stackoverflow.com/questions/43303828/why-does-case-insensitive-work-with-stdhash-function-for-stdunordered-set

size_t case_insensitive_hasher::operator()(const string& key) const
{
    string keyCopy(key);
    transform(keyCopy.begin(), keyCopy.end(), keyCopy.begin(), ::tolower);
    return hash<string>()(keyCopy);
}

// bool case_insensitive_comparer::operator()(const std::string& arg)
// {
//     case_insensitive_comparer_2 cmp;
//     return cmp(arg, m_baseline);
// }

// bool case_insensitive_comparer::operator()(const std::pair<std::string, std::string>& arg)
// {
//     case_insensitive_comparer_2 cmp;
//     return cmp(arg.first, m_baseline);
// }

bool case_insensitive_comparer_2::operator()(const string& x, const string& y) const
{
    string lx(x);
    transform(lx.begin(), lx.end(), lx.begin(), ::tolower);

    string ly(y);
    transform(ly.begin(), ly.end(), ly.begin(), ::tolower);

    return (lx == ly);
}

// See: https://stackoverflow.com/questions/1413445/reading-a-password-from-stdcin

#ifdef WIN32
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

void set_stdin_echo(bool enable = true)
{
#ifdef WIN32
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hStdin, &mode);

    if (!enable)
        mode &= ~ENABLE_ECHO_INPUT;
    else
        mode |= ENABLE_ECHO_INPUT;

    SetConsoleMode(hStdin, mode);

#else
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    if (!enable)
        tty.c_lflag &= ~ECHO;
    else
        tty.c_lflag |= ECHO;

    (void)tcsetattr(STDIN_FILENO, TCSANOW, &tty);
#endif
}

string read_passwd_stdin(const string& prompt)
{

    cout << prompt;
    set_stdin_echo(false);

    string password;
    try
    {
        cin >> password;
    }
    catch (...)
    {
    };

    set_stdin_echo(true);

    return password;
}

std::vector<std::string> split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

std::vector<std::string> split_quoted(const std::string& str, const char delim, const char quote, const char escape)
{
    std::vector<std::string> result;

    bool insideQuoted = false;
    bool escaped = false;

    ostringstream oss;

    for (auto c : str)
    {
        if (escaped)
        {
            if (c == '"')
            {
                oss << '"';
            }
            else if (c == escape)
            {
                oss << escape;
            }
            else
            {
                if (insideQuoted)
                {
                    oss << escape << c;
                }
                else
                {
                    oss << c;
                }
            }

            escaped = false;
        }
        else if (c == escape)
        {
            escaped = true;
        }
        else if (c == delim)
        {
            if (insideQuoted)
            {
                oss << c;
            }
            else
            {
                result.push_back(oss.str());
                oss.clear();
                oss.str(string());
            }
        }
        else if (c == quote)
        {
            insideQuoted = !insideQuoted;
        }
        else
        {
            oss << c;
        }
    }

    if (escaped)
    {
        oss << escape;
    }

    string lastEntry = oss.str();

    if (lastEntry.size() > 0)
    {
        result.push_back(lastEntry);
    }

    return result;
}

std::pair<std::string, std::string> cut(const std::string& str, const char delim)
{
    if (str.size() == 0)
    {
        return pair<string, string>(string(), string());
    }

    size_t pos = str.find_first_of(delim);

    if (pos == string::npos)
    {
        return pair<string, string>(str, string());
    }

    return pair<string, string>(str.substr(0, pos), str.substr(pos + 1));
}

std::string unquote(std::string const& str, const char delim, const char escape)
{
    size_t size = str.size();

    if (size < 2 || str[0] != delim || str[size - 1] != delim)
    {
        throw tool_exception("no delimiters");
    }

    size -= 2;

    std::string result;
    result.reserve(size);

    std::string::const_iterator in = str.begin()++;
    std::string::const_iterator end = str.end()--;

    while (in != end)
    {
        char c = *in++;
        if (c == escape && in != end)
        {
            c = *in++;
        }
        result += c;
    }

    return result;
}

std::map<std::string, std::string> to_map(std::vector<std::string> const& v, char const delim, bool const key_quoted, bool const value_quoted, char const quote, char const escape)
{
    std::map<std::string, std::string> result;

    // note quoted keys are not allowed to contain the delimiter

    for (auto e : v)
    {
        auto p = cut(e, delim);
        auto key = (key_quoted) ? unquote(p.first, quote, escape) : p.first;
        auto value = (value_quoted) ? unquote(p.second, quote, escape) : p.second;

        // results in error if duplicated
        // TODO: a parameter which on duplication instructs
        //  - to report an error
        //  - to leave the first one untouched
        //  - to update the previous one with the next one (signHttp 2.2 Ambiguous Parameters)
        result.insert({key, value});
    }

    return result;
}


std::pair<std::string, std::string> splitStrQuoted(const std::string& str, const char delim, const bool quoted)
{
    if (str.size() == 0)
    {
        return pair<string, string>(string(), string());
    }

    size_t pos = str.find_first_of(delim);

    if (pos == string::npos)
    {
        return pair<string, string>(str, string());
    }

    if (quoted)
    {
        return pair<string, string>(str.substr(0, pos), str.substr(pos + 2, str.size() - pos - 3));
    }
    else
    {
        return pair<string, string>(str.substr(0, pos), str.substr(pos + 1));
    }
}

std::string trim(const std::string& s)
{
    auto wsfront = std::find_if_not(s.begin(), s.end(), [](int c)
                                    { return std::isspace(c); });
    auto wsback = std::find_if_not(s.rbegin(), s.rend(), [](int c)
                                   { return std::isspace(c); })
                  .base();
    return (wsback <= wsfront ? std::string() : std::string(wsfront, wsback));
}


std::string emplace(std::string const& base, std::map<std::string, std::string> const& params)
{
    std::string text(base);

    for (auto e : params)
    {
        // TODO: allow inserting '{' and '}' using escape
        text = std::regex_replace(text, std::regex("\\{" + e.first + "\\}"), e.second);
    }

    return text;
}

/** I18n string to real string.
 * 
 *  lang language code
 *  i18n_data[0] identifier of the text, {n} marks the place of the n-th  parameter
 *  i18n_data[1+] texts to emplace
 * 
 *  if text not found on the given language, the searched parameter name is being returned
 */
std::string i18n_to_string(std::string const& lang, std::vector<std::string> const& i18n_data)
{
    // TODO: gettext or project fluent or similar would be much better

    if (i18n_data.size() == 0)
    {
        return std::string();
    }

    // TODO: read the appropriate string from language file
    std::string text_param_value = i18n_data[0];

    for (size_t i = 1; i < i18n_data.size(); ++i)
    {
        text_param_value = std::regex_replace(text_param_value, std::regex("\\{" + std::to_string(i) + "\\}"), i18n_data[i]);
    }

    return text_param_value;
}

/** I18n string to real string.
 * 
 *  lang language code
 *  i18n_data["id"] identifier of the text, {name} marks the place of the parameter with the given name
 *  i18n_data[name] texts to emplace
 * 
 *  if text not found on the given language, the searched parameter name is being returned
 */
std::string i18n_to_string(std::string const& lang, std::map<std::string, std::string> const& i18n_data)
{
    // TODO: gettext or project fluent or similar would be much better

    if (i18n_data.size() == 0 || i18n_data.find("id") == i18n_data.end())
    {
        return std::string();
    }

    // TODO: read the appropriate string from language file
    std::string text_param_value = i18n_data.at("id");

    for (auto const& elem : i18n_data)
    {
        if (elem.first != "id")
        {
            text_param_value = std::regex_replace(text_param_value, std::regex("\\{" + elem.first + "\\}"), elem.second);
        }
    }

    return text_param_value;
}


std::string elapsed_time(const std::chrono::steady_clock::time_point& begin)
{
    steady_clock::time_point end = steady_clock::now();
    duration<double, std::milli> elapsed = duration_cast<duration<double, std::milli>>(end - begin);
    ostringstream oss;
    oss << fixed << setprecision(1) << elapsed.count() << " ms";
    return oss.str();
}

time_t now()
{
    return system_clock::to_time_t(system_clock::now());
}

uint64_t now_millis()
{
    return duration_cast<std::chrono::milliseconds>(system_clock::now().time_since_epoch()).count();
}

const int seconds_per_minute = 60;
const int seconds_per_hour = 3600;
const int seconds_per_day = 86400;
const int days_of_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

time_t begin_of_day(time_t ts)
{
    return (ts / seconds_per_day) * seconds_per_day;
}

time_t end_of_day(time_t ts)
{
    return begin_of_day(ts) + seconds_per_day - 1;
}

bool is_today(time_t ts)
{
    return ts >= begin_of_day(ts) && ts <= end_of_day(ts);
}

bool is_leap_year(short year)
{
    if (year % 4 != 0)
        return false;
    if (year % 100 != 0)
        return true;
    return (year % 400) == 0;
}

/** UTC time from struct tm
 * 
 *  source: https://stackoverflow.com/questions/12353011/how-to-convert-a-utc-date-time-to-a-time-t-in-c
 */
time_t mkgmtime(const struct tm* ptm)
{
    time_t secs = 0;
    // tm_year is years since 1900
    int year = ptm->tm_year + 1900;
    for (int y = 1970; y < year; ++y)
    {
        secs += (is_leap_year(y) ? 366 : 365) * seconds_per_day;
    }
    // tm_mon is month from 0..11
    for (int m = 0; m < ptm->tm_mon; ++m)
    {
        secs += days_of_month[m] * seconds_per_day;
        if (m == 1 && is_leap_year(year))
            secs += seconds_per_day;
    }
    secs += (ptm->tm_mday - 1) * seconds_per_day;
    secs += ptm->tm_hour * seconds_per_hour;
    secs += ptm->tm_min * seconds_per_minute;
    secs += ptm->tm_sec;
    return secs;
}

/** From string to time_t
 *
 *  Format: yyyy-mm-ddThh:mm:ss
 *  If eod is true and no time part provided, than end of day would be set
 */
time_t string_to_time_t(const std::string& s, bool eod)
{
    std::time_t time = 0;

    // TODO: jelenleg az OpenLDAP-ban levo GMT idpontot rosszul irjuk ki.

    if (s.size() >= 10)
    {
        struct std::tm tm = {}; // If not initialized, valgrind complains

        tm.tm_year = atoi(s.substr(0, 4).c_str()) - 1900;
        tm.tm_mon = atoi(s.substr(5, 2).c_str()) - 1;
        tm.tm_mday = atoi(s.substr(8, 2).c_str());

        if (s.size() >= 19)
        {

            tm.tm_hour = atoi(s.substr(11, 2).c_str());
            tm.tm_min = atoi(s.substr(14, 2).c_str());
            tm.tm_sec = atoi(s.substr(17, 2).c_str());

            if (s.size() > 19)
            {
                // TODO: Fraction of second
                // TODO: Timezone
            }
        }
        else
        {
            if (eod)
            {
                tm.tm_hour = 23;
                tm.tm_min = 59;
                tm.tm_sec = 59;
            }
        }

        time = mkgmtime(&tm);
    }

    return time;
}

/** From date string to time_t
 *
 *  Format: yyyy-mm-dd
 *  If eod is true, than end of day would be set
 */
time_t date_string_to_time_t(const std::string& s, bool eod)
{
    std::time_t time = 0;

    // TODO: jelenleg az OpenLDAP-ban levo GMT idpontot rosszul irjuk ki.

    if (s.size() >= 10)
    {
        struct std::tm tm = {}; // If not initialized, valgrind complains

        tm.tm_year = atoi(s.substr(0, 4).c_str()) - 1900;
        tm.tm_mon = atoi(s.substr(5, 2).c_str()) - 1;
        tm.tm_mday = atoi(s.substr(8, 2).c_str());

        if (eod)
        {
            tm.tm_hour = 23;
            tm.tm_min = 59;
            tm.tm_sec = 59;
        }
        else
        {
            tm.tm_hour = 0;
            tm.tm_min = 0;
            tm.tm_sec = 0;
        }


        time = mktime(&tm);
    }

    return time;
}

/** From string to time_t
 *
 *  Format: yyyymmddhhmmss
 */
time_t ldaptime_to_time_t(const std::string& s)
{
    std::time_t time = 0;

    // TODO: jelenleg az OpenLDAP-ban levo GMT idpontot rosszul irjuk ki.

    if (s.size() >= 14)
    {
        struct std::tm tm = {}; // If not initialized, valgrind complains

        tm.tm_year = atoi(s.substr(0, 4).c_str()) - 1900;
        tm.tm_mon = atoi(s.substr(4, 2).c_str()) - 1;
        tm.tm_mday = atoi(s.substr(6, 2).c_str());
        tm.tm_hour = atoi(s.substr(8, 2).c_str());
        tm.tm_min = atoi(s.substr(10, 2).c_str());
        tm.tm_sec = atoi(s.substr(12, 2).c_str());

        if (s.size() > 14)
        {
            // TODO: Timezone
        }

        time = mktime(&tm);
    }

    return time;
}

std::string gmttime_to_string(const std::time_t& t)
{
    if (t > 0)
    {
        std::tm* ptm = std::gmtime(&t);
        char buffer[32];
        std::strftime(buffer, 32, "%Y-%m-%dT%H:%M:%SZ", ptm);

        return string(buffer);
    }
    else
    {
        return string();
    }
}

std::string gmttime_to_date_string(const std::time_t& t)
{
    if (t > 0)
    {
        std::tm* ptm = std::gmtime(&t);
        char buffer[32];
        std::strftime(buffer, 32, "%Y-%m-%d", ptm);

        return string(buffer);
    }
    else
    {
        return string();
    }
}

std::string gmttime_to_ldaptime(const std::time_t& t)
{
    if (t > 0)
    {
        std::tm* ptm = std::gmtime(&t);
        char buffer[32];
        std::strftime(buffer, 32, "%Y%m%d%H%M%SZ", ptm);

        return string(buffer);
    }
    else
    {
        return string();
    }
}

std::string demangle_typeid(std::type_info const& type_id)
{
    int status;
    char* realname = abi::__cxa_demangle(type_id.name(), 0, 0, &status);

    if (!realname)
    {
        throw tool_exception("demangle failed");
    }

    std::string result(realname);
    free(realname);

    return result;
}

string buf_to_hex_string(const unsigned char* ptr, const size_t len)
{
    ostringstream oss;

    for (size_t i = 0; i < len; i++)
        oss << hex << setw(2) << setfill('0') << (int)ptr[i];

    return oss.str();
}

std::string amount_to_string(int64_t amount_in_minor_unit, uint d)
{
    std::string amount = std::to_string(amount_in_minor_unit);

    if (d == 0)
    {
        return amount;
    }

    if (amount.size() < d)
    {
        amount.insert(0, d - amount.size() + 1, '0');
    }

    return amount.insert(amount.size() - d, ".");
}

int64_t amount_from_string(std::string const& amount_in_string, uint d)
{
    // major unit of the amount
    auto p_pos = amount_in_string.find('.');
    auto amount = (p_pos != std::string::npos) ? amount_in_string.substr(0, p_pos) : amount_in_string;

    // minor unit part
    auto minor = (p_pos != std::string::npos) ? amount_in_string.substr(p_pos + 1) : "";

    // 3. right pad if needed
    while (minor.size() < d)
    {
        minor.append("0");
    }

    // concatenate minor to major
    amount.append(minor.substr(0, d));

    // if there are still digits, all has to be zero
    if (minor.size() > d)
    {
        if (minor.substr(d).find_first_not_of('0') != std::string::npos)
        {
            throw tool_exception("fractions of minor currency unit not allowed");
        }
    }

    return std::stoll(amount);
}

} // namespace toolbox
} // namespace imp
