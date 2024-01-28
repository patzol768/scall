/*
 * Copyright (c) 2024 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */

#include <uuid/uuid.h>

#include <imp/toolbox/id.h>

using std::string;

namespace imp
{
namespace toolbox
{

std::string create_uuid()
{
    uuid_t uuid;
    uuid_generate_time_safe(uuid);

    char uuid_str[37];
    uuid_unparse_lower(uuid, uuid_str);

    string id(uuid_str);
    return id;
}

} // namespace toolbox
} // namespace imp
