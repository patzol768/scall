/*
 * Copyright (c) 2024 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */

#pragma once

#include <corvusoft/restbed/session.hpp>
#include <corvusoft/restbed/rule.hpp>
#include <functional>
#include <memory>
#include <string>

namespace imp
{
namespace app
{
namespace restbed
{

class Log_correlation_rule : public ::restbed::Rule
{
    public:
    Log_correlation_rule();
    virtual ~Log_correlation_rule();

    bool condition(const std::shared_ptr<::restbed::Session>) final override;
    void action(const std::shared_ptr<::restbed::Session> session, const std::function<void(const std::shared_ptr<::restbed::Session>)>& callback) final override;
};

} // namespace restbed
} // namespace app
} // namespace imp
