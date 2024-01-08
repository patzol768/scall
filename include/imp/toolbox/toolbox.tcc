/*
 * Copyright (c) 2024 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */

#include <algorithm>
#include <optional>
#include <string>

namespace imp
{
namespace toolbox
{

template <typename T>
T tolower(T const& key)
{
    T result;
    result.resize(key.size());
    transform(key.begin(), key.end(), result.begin(), ::tolower);
    return result;
}

template <class Container, class T>
bool has(Container const& container, T const& val)
{
    return std::find(container.begin(), container.end(), val) != container.end();
}

template <class Container, class T>
typename Container::iterator find(Container const& container, T const& val)
{
    return std::find(container.begin(), container.end(), val);
}

template <class Container>
bool has_case_insensitive(Container const& container, std::string const& val)
{
    return (find_case_insensitive(container, val) != container.end());
}

template <class Container>
typename Container::const_iterator find_case_insensitive(Container const& container, std::string const& val)
{
    std::string lx(val);
    std::transform(lx.begin(), lx.end(), lx.begin(), ::tolower);
    auto it = container.begin();

    for (; it != container.end(); ++it)
    {
        std::string ly(it->first);
        std::transform(ly.begin(), ly.end(), ly.begin(), ::tolower);
        
        if (lx == ly)
        {
            return it;
        }
    }

    return it;

    // return std::find_if(container.begin(), container.end(), case_insensitive_comparer(val));
}

template <class Key_type, class Value_type>
std::optional<Value_type> map_get_optional(std::map<Key_type, Value_type> const& container, Key_type const& key)
{
    if (container.find(key) != container.end())
    {
        return container.at(key);
    }

    return {};
}

template <class C>
std::string stl_to_string(C elements, std::string delim)
{
    std::string result;

    for (auto const& e : elements)
    {
        result = result + (std::string)e + delim;
    }

    result.resize(result.size() - delim.size());

    return result;
}

template <class C>
std::string stl_to_string(C elements)
{
    std::string result = "[" + stl_to_string(elements, ", ") + "]";

    return result;
}

} // namespace toolbox
} // namespace imp
