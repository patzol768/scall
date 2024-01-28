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
#include <memory>
#include <optional>
#include <set>
#include <string>

#include <nlohmann/json.hpp>
#include <restbed>

namespace imp
{
namespace app
{

typedef std::function<void(const std::shared_ptr<::restbed::Session>)> restbed_handler_fn;
typedef std::function<void(const int, const std::exception&, const std::shared_ptr<::restbed::Session>)> restbed_error_handler_fn;
typedef std::function<void(const std::shared_ptr<::restbed::Session>, const restbed_handler_fn&)> restbed_authentication_handler_fn;

// singleton
class App_config
{
    public:
    App_config();
    ~App_config() { }

    static App_config* get_instance();

    // getters
    bool get_http_enabled() const;
    bool get_https_enabled() const;
    bool get_sslv2_enabled() const;
    bool get_sslv3_enabled() const;
    bool get_tlsv1_enabled() const;
    bool get_tlsv11_enabled() const;
    bool get_tlsv12_enabled() const;
    // bool get_tlsv13_enabled() const;
    bool get_compression_enabled() const;
    bool get_default_workarounds_enabled() const;
    bool get_single_diffie_hellman_use_enabled() const;
    bool get_hs_enabled() const;
    bool get_mtls_enabled() const;
    bool get_target_verify_peer() const;
    bool get_target_verify_host() const;

    uint16_t get_port() const;
    uint16_t get_ssl_port() const;

    uint get_worker_limit() const;
    uint get_connection_limit() const;
    uint get_path_max_depth() const;

    std::chrono::milliseconds get_connection_timeout() const;

    const std::string& get_cert_location() const;
    const std::string& get_bind_address() const;
    const std::string& get_ssl_bind_address() const;
    const std::string& get_passphrase() const;
    const std::string& get_private_rsa_key() const;
    const std::string& get_certificate_chain() const;
    const std::string& get_target_base_url() const;
    const std::string& get_target_ca() const;
    const std::string& get_hs_version() const;
    const std::string& get_mtls_key_id() const;
    const std::string& get_keys_dir() const;

    const std::optional<::restbed::Uri>& get_private_key() const;
    const std::optional<::restbed::Uri>& get_certificate() const;
    const std::optional<::restbed::Uri>& get_temporary_diffie_hellman() const;
    const std::optional<::restbed::Uri>& get_certificate_authority_pool() const;

    const restbed_handler_fn& get_not_found_handler() const;
    const restbed_handler_fn& get_method_not_allowed_handler() const;
    const restbed_handler_fn& get_method_not_implemented_handler() const;
    const restbed_error_handler_fn& get_error_handler() const;
    const restbed_authentication_handler_fn& get_authentication_handler() const;

    std::shared_ptr<::restbed::Settings> get_restbed_settings() const;
    std::shared_ptr<::restbed::SSLSettings> get_restbed_ssl_settings() const;

    bool has_pool_config(std::string const& name) const;
    const std::map<std::string, std::string>& get_hs_params() const;
    const std::map<std::string, std::string>& get_passwords() const;
    const std::optional<std::string> get_password(std::string const& key) const;

    const std::set<std::string>& get_verbs() const;

    // setters
    void set_config(nlohmann::json const& j);

    void set_connection_timeout(nlohmann::json const& j);

    void set_private_key(nlohmann::json const& j);
    void set_certificate(nlohmann::json const& j);
    void set_temporary_diffie_hellman(nlohmann::json const& j);
    void set_certificate_authority_pool(nlohmann::json const& j);

    void set_not_found_handler(restbed_handler_fn const& fn);
    void set_method_not_allowed_handler(restbed_handler_fn const& fn);
    void set_method_not_implemented_handler(restbed_handler_fn const& fn);
    void set_error_handler(restbed_error_handler_fn const& fn);
    void set_authentication_handler(restbed_authentication_handler_fn const& fn);

    void set_pool_config(nlohmann::json const& j);

    private:
    App_config(const App_config&) = delete;                  // copy constructor
    App_config& operator=(const App_config& other) = delete; // assignment operator
    App_config(App_config&& other) = delete;                 // move constructor
    App_config& operator=(App_config&& other) = delete;      // move assignment operator

    bool m_http_enabled;
    bool m_https_enabled;
    bool m_sslv2_enabled;
    bool m_sslv3_enabled;
    bool m_tlsv1_enabled;
    bool m_tlsv11_enabled;
    bool m_tlsv12_enabled;
    // bool m_tlsv13_enabled;
    bool m_compression_enabled;
    bool m_default_workarounds_enabled;
    bool m_single_diffie_hellman_use_enabled;
    bool m_hs_enabled;
    bool m_mtls_enabled;
    bool m_target_verify_peer;
    bool m_target_verify_host;

    uint16_t m_port;
    uint16_t m_ssl_port;

    uint m_worker_limit;
    uint m_connection_limit;
    uint m_path_max_depth;

    std::chrono::milliseconds m_connection_timeout;

    std::string m_cert_location;
    std::string m_bind_address;
    std::string m_ssl_bind_address;
    std::string m_passphrase;
    std::string m_private_rsa_key;
    std::string m_certificate_chain;
    std::string m_target_base_url;
    std::string m_target_ca;
    std::string m_hs_version;
    std::string m_mtls_key_id;
    std::string m_keys_dir;

    std::optional<::restbed::Uri> m_private_key;
    std::optional<::restbed::Uri> m_certificate;
    std::optional<::restbed::Uri> m_temporary_diffie_hellman;
    std::optional<::restbed::Uri> m_certificate_authority_pool;

    restbed_handler_fn m_not_found_handler;
    restbed_handler_fn m_method_not_allowed_handler;
    restbed_handler_fn m_method_not_implemented_handler;
    restbed_error_handler_fn m_error_handler;
    restbed_authentication_handler_fn m_authentication_handler;

    std::map<std::string, std::string> m_hs_params;
    std::map<std::string, std::string> m_passwords;

    std::set<std::string> m_verbs;
};

} // namespace app
} // namespace imp
