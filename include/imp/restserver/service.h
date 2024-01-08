/*
 * Copyright (c) 2024 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */

#pragma once

// forward declare
namespace restbed
{
class Service;
}

namespace imp
{
namespace restserver
{

void service_ready_handler(restbed::Service& service);

} // namespace restserver
} // namespace imp
