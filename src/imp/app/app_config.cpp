/*
 * Copyright (c) 2024 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */

#include <imp/app/app_config.h>

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

#include <imp/toolbox/toolbox.h>

using imp::toolbox::read_passwd_stdin;
using nlohmann::json;
using ::restbed::Settings;
using ::restbed::SSLSettings;
using ::restbed::Uri;
using std::make_shared;
using std::map;
using std::optional;
using std::string;

using json_pointer = nlohmann::json::json_pointer;

namespace imp
{
namespace app
{

App_config::App_config()
: m_http_enabled(false)
, m_https_enabled(true)
, m_sslv2_enabled(false)
, m_sslv3_enabled(false)
, m_tlsv1_enabled(false)
, m_tlsv11_enabled(false)
, m_tlsv12_enabled(true)
// , m_tlsv13_enabled(true)
, m_compression_enabled(true)
, m_default_workarounds_enabled(true)
, m_single_diffie_hellman_use_enabled(true)
, m_hs_enabled(false)
, m_mtls_enabled(false)
, m_target_verify_peer(true)
, m_port(80)
, m_ssl_port(443)
, m_worker_limit(1)
, m_connection_limit(128)
, m_path_max_depth(5)
, m_connection_timeout(std::chrono::milliseconds(5000))
, m_cert_location("")
, m_bind_address("0.0.0.0")
, m_ssl_bind_address("0.0.0.0")
, m_passphrase("")
, m_private_rsa_key("")
, m_certificate_chain("")
, m_target_base_url("")
, m_target_ca("")
, m_hs_version("")
, m_mtls_key_id("")
, m_keys_dir("./")
, m_not_found_handler(nullptr)
, m_method_not_allowed_handler(nullptr)
, m_method_not_implemented_handler(nullptr)
, m_error_handler(nullptr)
, m_authentication_handler(nullptr)
{
}

App_config* App_config::get_instance()
{
    static std::unique_ptr<App_config> m_instance(new App_config);
    return m_instance.get();
}

bool App_config::get_http_enabled() const
{
    return m_http_enabled;
}

bool App_config::get_https_enabled() const
{
    return m_https_enabled;
}

bool App_config::get_sslv2_enabled() const
{
    return m_sslv2_enabled;
}

bool App_config::get_sslv3_enabled() const
{
    return m_sslv3_enabled;
}

bool App_config::get_tlsv1_enabled() const
{
    return m_tlsv1_enabled;
}

bool App_config::get_tlsv11_enabled() const
{
    return m_tlsv11_enabled;
}

bool App_config::get_tlsv12_enabled() const
{
    return m_tlsv12_enabled;
}

bool App_config::get_compression_enabled() const
{
    return m_compression_enabled;
}

bool App_config::get_default_workarounds_enabled() const
{
    return m_default_workarounds_enabled;
}

bool App_config::get_single_diffie_hellman_use_enabled() const
{
    return m_single_diffie_hellman_use_enabled;
}

bool App_config::get_hs_enabled() const
{
    return m_hs_enabled;
}

bool App_config::get_mtls_enabled() const
{
    return m_mtls_enabled;
}

bool App_config::get_target_verify_peer() const
{
    return m_target_verify_peer;
}

uint16_t App_config::get_port() const
{
    return m_port;
}

uint16_t App_config::get_ssl_port() const
{
    return m_ssl_port;
}

uint App_config::get_worker_limit() const
{
    return m_worker_limit;
}

uint App_config::get_connection_limit() const
{
    return m_connection_limit;
}

uint App_config::get_path_max_depth() const
{
    return m_path_max_depth;
}

std::chrono::milliseconds App_config::get_connection_timeout() const
{
    return m_connection_timeout;
}

const std::string& App_config::get_cert_location() const
{
    return m_cert_location;
}

const std::string& App_config::get_bind_address() const
{
    return m_bind_address;
}

const std::string& App_config::get_ssl_bind_address() const
{
    return m_ssl_bind_address;
}

const std::string& App_config::get_passphrase() const
{
    return m_passphrase;
}

const std::string& App_config::get_private_rsa_key() const
{
    return m_private_rsa_key;
}

const std::string& App_config::get_certificate_chain() const
{
    return m_certificate_chain;
}

const std::string& App_config::get_target_base_url() const
{
    return m_target_base_url;
}

const std::string& App_config::get_target_ca() const
{
    return m_target_ca;
}

const std::string& App_config::get_hs_version() const
{
    return m_hs_version;
}

const std::string& App_config::get_mtls_key_id() const
{
    return m_mtls_key_id;
}

const std::string& App_config::get_keys_dir() const
{
    return m_keys_dir;
}

const std::optional<::restbed::Uri>& App_config::get_private_key() const
{
    return m_private_key;
}

const std::optional<::restbed::Uri>& App_config::get_certificate() const
{
    return m_certificate;
}

const std::optional<::restbed::Uri>& App_config::get_temporary_diffie_hellman() const
{
    return m_temporary_diffie_hellman;
}

const std::optional<::restbed::Uri>& App_config::get_certificate_authority_pool() const
{
    return m_certificate_authority_pool;
}

const restbed_handler_fn& App_config::get_not_found_handler() const
{
    return m_not_found_handler;
}

const restbed_handler_fn& App_config::get_method_not_allowed_handler() const
{
    return m_method_not_allowed_handler;
}

const restbed_handler_fn& App_config::get_method_not_implemented_handler() const
{
    return m_method_not_implemented_handler;
}

const restbed_error_handler_fn& App_config::get_error_handler() const
{
    return m_error_handler;
}

const restbed_authentication_handler_fn& App_config::get_authentication_handler() const
{
    return m_authentication_handler;
}

std::shared_ptr<restbed::Settings> App_config::get_restbed_settings() const
{
    auto settings = make_shared<Settings>();

    settings->set_bind_address(m_bind_address);
    settings->set_port(m_port);
    settings->set_worker_limit(m_worker_limit);
    if (m_https_enabled)
    {
        settings->set_ssl_settings(get_restbed_ssl_settings());
    }

    return settings;
}

std::shared_ptr<restbed::SSLSettings> App_config::get_restbed_ssl_settings() const
{
    // restbed https
    auto ssl_settings = make_shared<SSLSettings>();

    ssl_settings->set_http_disabled(!m_http_enabled);
    if (m_private_key.has_value())
    {
        ssl_settings->set_private_key(m_private_key.value());
    }
    if (m_certificate.has_value())
    {
        ssl_settings->set_certificate(m_certificate.value());
    }
    if (m_temporary_diffie_hellman.has_value())
    {
        ssl_settings->set_temporary_diffie_hellman(m_temporary_diffie_hellman.value());
    }
    ssl_settings->set_bind_address(m_ssl_bind_address);
    ssl_settings->set_port(m_ssl_port);
    ssl_settings->set_sslv2_enabled(m_sslv2_enabled);
    ssl_settings->set_sslv3_enabled(m_sslv3_enabled);
    ssl_settings->set_tlsv1_enabled(m_tlsv1_enabled);
    ssl_settings->set_tlsv11_enabled(m_tlsv11_enabled);
    ssl_settings->set_tlsv12_enabled(m_tlsv12_enabled);
    // ssl_settings->set_tlsv13_enabled(m_tlsv13_enabled);

    if (m_certificate_authority_pool.has_value())
    {
        ssl_settings->set_certificate_authority_pool(m_certificate_authority_pool.value());
    }

    return ssl_settings;
}

const std::map<std::string, std::string>& App_config::get_hs_params() const
{
    return m_hs_params;
}

const std::map<std::string, std::string>& App_config::get_passwords() const
{
    return m_passwords;
}

const std::optional<std::string> App_config::get_password(std::string const& key) const
{
    auto const it = m_passwords.find(key);
    if (it != m_passwords.end())
    {
        return it->second;
    }
    return {};
}

const std::set<std::string>& App_config::get_verbs() const
{
    return m_verbs;
}

#define FILL_IF_EXISTS(jsn, path, variable) \
    if (jsn.contains(json_pointer(path)))   \
        variable = jsn[json_pointer(path)];

#define CALL_IF_EXISTS(jsn, path, fn)     \
    if (jsn.contains(json_pointer(path))) \
        fn(jsn[json_pointer(path)]);

void App_config::set_connection_timeout(json const& j)
{
    uint64_t value = j;
    m_connection_timeout = std::chrono::milliseconds(value);
}

void App_config::set_private_key(json const& j)
{
    std::string value = j;
    m_private_key = Uri(value);
}

void App_config::set_certificate(json const& j)
{
    string value = j;
    m_certificate = Uri(value);
}

void App_config::set_temporary_diffie_hellman(json const& j)
{
    string value = j;
    m_temporary_diffie_hellman = Uri(value);
}

void App_config::set_certificate_authority_pool(json const& j)
{
    string value = j;
    m_certificate_authority_pool = Uri(value);
}

void App_config::set_not_found_handler(restbed_handler_fn const& fn)
{
    m_not_found_handler = fn;
}

void App_config::set_method_not_allowed_handler(restbed_handler_fn const& fn)
{
    m_method_not_allowed_handler = fn;
}

void App_config::set_method_not_implemented_handler(restbed_handler_fn const& fn)
{
    m_method_not_implemented_handler = fn;
}

void App_config::set_error_handler(restbed_error_handler_fn const& fn)
{
    m_error_handler = fn;
}

void App_config::set_authentication_handler(restbed_authentication_handler_fn const& fn)
{
    m_authentication_handler = fn;
}

void App_config::set_config(nlohmann::json const& j)
{
    FILL_IF_EXISTS(j, "/worker_limit", m_worker_limit);
    FILL_IF_EXISTS(j, "/connection_limit", m_connection_limit);
    CALL_IF_EXISTS(j, "/connection_timeout", set_connection_timeout);

    FILL_IF_EXISTS(j, "/http/enabled", m_http_enabled);
    FILL_IF_EXISTS(j, "/http/address", m_bind_address);
    FILL_IF_EXISTS(j, "/http/port", m_port);

    FILL_IF_EXISTS(j, "/https/enabled", m_https_enabled);
    FILL_IF_EXISTS(j, "/https/address", m_ssl_bind_address);
    FILL_IF_EXISTS(j, "/https/port", m_ssl_port);

    FILL_IF_EXISTS(j, "/https/sslv2Enabled", m_sslv2_enabled);
    FILL_IF_EXISTS(j, "/https/sslv3Enabled", m_sslv3_enabled);
    FILL_IF_EXISTS(j, "/https/tlsv1Enabled", m_tlsv1_enabled);
    FILL_IF_EXISTS(j, "/https/tlsv11Enabled", m_tlsv11_enabled);
    FILL_IF_EXISTS(j, "/https/tlsv12Enabled", m_tlsv12_enabled);
    // FILL_IF_EXISTS(j, "/https/tlsv13Enabled", m_tlsv13_enabled);
    FILL_IF_EXISTS(j, "/https/compressionEnabled", m_compression_enabled);
    FILL_IF_EXISTS(j, "/https/default_workarounds_enabled", m_default_workarounds_enabled);
    FILL_IF_EXISTS(j, "/https/single_diffie_hellman_use_enabled", m_single_diffie_hellman_use_enabled);

    FILL_IF_EXISTS(j, "/keys/passwords", m_passwords);
    FILL_IF_EXISTS(j, "/keys/dir", m_keys_dir);

    if (m_keys_dir.back() != '/')
    {
        m_keys_dir.append("/");
    }

    CALL_IF_EXISTS(j, "/https/key", set_private_key);
    CALL_IF_EXISTS(j, "/https/certificate", set_certificate);

    FILL_IF_EXISTS(j, "/target/base_url", m_target_base_url);
    FILL_IF_EXISTS(j, "/target/ca", m_target_ca);
    FILL_IF_EXISTS(j, "/target/target_verify_peer", m_target_verify_peer);

    FILL_IF_EXISTS(j, "/http_signature/enabled", m_hs_enabled);
    FILL_IF_EXISTS(j, "/http_signature/version", m_hs_version);
    FILL_IF_EXISTS(j, "/http_signature/" + m_hs_version + "_params", m_hs_params);

    FILL_IF_EXISTS(j, "/mtls/enabled", m_mtls_enabled);
    FILL_IF_EXISTS(j, "/mtls/key_id", m_mtls_key_id);

    FILL_IF_EXISTS(j, "/verbs", m_verbs);
    FILL_IF_EXISTS(j, "/path_max_depth", m_path_max_depth);
}

} // namespace app
} // namespace imp
