/*
 * Copyright (c) 2024 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */

#include <corvusoft/restbed/logger.hpp>
#include <fstream>
#include <iostream>
#include <log4cplus/configurator.h>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <memory>
#include <nlohmann/json.hpp>
#include <openssl/crypto.h>
#include <restbed/log4cpluslogger.h>
#include <string>

#include <curl/curl.h>

#include <imp/app/app_config.h>
#include <imp/app/error.h>
#include <imp/app/log.h>
#include <imp/restserver/service.h>
#include <imp/scall/cavage12.h>
#include <imp/scall/http_sign_factory.h>
#include <imp/scall/wrapper_service.h>
#include <imp/toolbox/toolbox.h>

// forward declare
namespace restbed
{
class Settings;
}

using imp::app::App_config;
using imp::app::application_error;
using imp::app::init_logger;
using imp::restserver::service_ready_handler;
using imp::scall::cavage12_sign;
using imp::scall::Http_sign_factory;
using imp::toolbox::demangle_typeid;
using log4cplus::Logger;
using nlohmann::json;
using restbed::Settings;
using std::ifstream;
using std::logic_error;
using std::make_shared;
using std::setlocale;
using std::shared_ptr;
using std::string;
using std::vector;

// ------------------------------------------------------------------------------------

json read_config()
{
    string config_filename = "config.json";
    char* config_filename_ptr = getenv("SCALL_CONFIG");

    if (config_filename_ptr)
    {
        config_filename = config_filename_ptr;
    }

    auto logger = Logger::getInstance(LOG4CPLUS_TEXT("main"));
    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Loading config from: " << config_filename));

    ifstream ifs(config_filename);

    if (!ifs.is_open() || !ifs.good())
    {
        throw logic_error("cannot read: " + config_filename);
    }

    json j;
    try
    {
        j = json::parse(ifs);
    }
    catch (json::parse_error& ex)
    {
        std::cerr << config_filename << " parse error at byte " << ex.byte << std::endl;
        throw;
    }

    return j;
}

void check_environment_variables()
{
    auto logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("main"));

    vector<string> env_vars = {"OPENSSL_CONF"};
    string missing;

    for (auto const& env_var : env_vars)
    {
        auto str = getenv(env_var.c_str());
        if (str == NULL || str[0] == '\0')
        {
            missing.push_back(' ');
            missing.append(env_var);
        }
        else
        {
            LOG4CPLUS_DEBUG(logger, env_var << "=" << str);
        }
    }

    if (!missing.empty())
    {
        LOG4CPLUS_WARN(logger, "Environment variable setting might be missing:" << missing);
    }
}

#if !defined(UNIT_TEST)

int main(const int parc, const char** pars)
{
    (void)parc;
    (void)pars;

    std::setlocale(LC_NUMERIC, "C");

    // initialize Log4cplus
    init_logger();
    auto logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("main"));

    // throw a warning if some of the "desired" environment variables are missing
    check_environment_variables();

    // make sure that openssl libcrypto is loading the config for its default context
    OPENSSL_init_crypto(OPENSSL_INIT_LOAD_CONFIG, NULL);

    std::shared_ptr<restbed::Settings const> settings = nullptr;

    int exit_code = EXIT_SUCCESS;

    try
    {
        // load configuration
        auto jc = read_config();
        LOG4CPLUS_TRACE(logger, "Configuration:" << std::endl
                                                 << jc);
        App_config::get_instance()->set_config(jc);
        //         App_config::get_instance()->set_authentication_handler(nullptr);
        //         App_config::get_instance()->set_error_handler(nullptr);
        //         App_config::get_instance()->set_ssl_context_finish(nullptr);
        //         App_config::get_instance()->set_verify_callback(nullptr);
        // #if (OPENSSL_VERSION_NUMBER >= 0x00906000L) && !defined(OPENSSL_NO_ENGINE)
        //         App_config::get_instance()->set_load_ssl_client_cert_function(nullptr);
        // #endif
        // #if (OPENSSL_VERSION_NUMBER >= 0x30000000L)
        //         App_config::get_instance()->set_ui_method(nullptr);
        // #endif

        // Setup used libraries
        //  - libcurl: global init should run before multi threaded part
        curl_global_init(CURL_GLOBAL_DEFAULT);

        // Setup factories
        //  - Register connection handlers
        // Connection_factory::get_instance()->register_type("https", connection_https_creator);

        // restbed settings
        // auto ui = Console_UI();
        // App_config::get_instance()->set_ui_method(ui.get_ui_method());
        settings = App_config::get_instance()->get_restbed_settings();

        // setup factories
        Http_sign_factory::get_instance().register_named("cavage12", cavage12_sign);
    }
    catch (const std::exception& exc)
    {
        LOG4CPLUS_FATAL(logger, "Initialization failed: " << exc.what());
        exit_code = EXIT_FAILURE;
    }

    // Do restbed initialization and enter main loop
    if (exit_code == EXIT_SUCCESS)
    {
        try
        {
            restbed::Service service;

            // logging for restbed
            std::shared_ptr<restbed::Logger> restbed_logger(new restbed::Log4cplusLogger);
            service.set_logger(restbed_logger);

            // restbed ready handler (called when all services are up and running)
            service.set_ready_handler(service_ready_handler);

            // add services...
            imp::scall::WrapperService wrapper_api(service);

            service.start(settings);
        }
        catch (std::system_error const& exc)
        {
            LOG4CPLUS_FATAL(logger, "sytem_error caught: " << exc.what());
            exit_code = EXIT_FAILURE;
        }
        catch (application_error const& exc)
        {
            // TODO: better error logging (exc.what() not outputs the parameters)
            LOG4CPLUS_FATAL(logger, "unhandled exception caught [" << demangle_typeid(typeid(exc)) << "]: " << exc.what());
            exit_code = EXIT_FAILURE;
        }
        catch (std::exception const& exc)
        {
            LOG4CPLUS_FATAL(logger, "unhandled exception caught [" << demangle_typeid(typeid(exc)) << "]: " << exc.what());
            exit_code = EXIT_FAILURE;
        }
    }

    // cleanup
    OPENSSL_cleanup();

    return exit_code;
}

#endif
