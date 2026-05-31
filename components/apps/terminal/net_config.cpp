#include "net_config.hpp"

#include <atomic>
#include <cstdio>
#include <cstring>

#include "nvs.h"

namespace netcfg {

namespace {

constexpr const char *kNvsNamespace = "net_cfg";
constexpr const char *kKeyTcpEn = "tcp_en";
constexpr const char *kKeyHost = "host";
constexpr const char *kKeyTcpPort = "tcp_port";
constexpr const char *kKeyUdpEn = "udp_en";
constexpr const char *kKeyUdpPort = "udp_port";

std::atomic<unsigned> g_version{0};
std::atomic<int> g_tcp_state{static_cast<int>(TcpState::Disabled)};

int clamp_port(int v)
{
    if (v < 1) return 1;
    if (v > 65535) return 65535;
    return v;
}

}  // namespace

Config defaults()
{
    return Config{};
}

Config load()
{
    Config cfg = defaults();

    nvs_handle_t handle = 0;
    if (nvs_open(kNvsNamespace, NVS_READONLY, &handle) != ESP_OK) {
        return cfg;
    }

    int32_t value = 0;
    if (nvs_get_i32(handle, kKeyTcpEn, &value) == ESP_OK) {
        cfg.tcp_enabled = (value != 0);
    }
    if (nvs_get_i32(handle, kKeyTcpPort, &value) == ESP_OK) {
        cfg.tcp_port = clamp_port(static_cast<int>(value));
    }
    if (nvs_get_i32(handle, kKeyUdpEn, &value) == ESP_OK) {
        cfg.udp_enabled = (value != 0);
    }
    if (nvs_get_i32(handle, kKeyUdpPort, &value) == ESP_OK) {
        cfg.udp_port = clamp_port(static_cast<int>(value));
    }

    size_t host_len = sizeof(cfg.host);
    if (nvs_get_str(handle, kKeyHost, cfg.host, &host_len) != ESP_OK) {
        cfg.host[0] = '\0';
    }
    cfg.host[sizeof(cfg.host) - 1] = '\0';

    nvs_close(handle);
    return cfg;
}

bool save(const Config &cfg)
{
    nvs_handle_t handle = 0;
    if (nvs_open(kNvsNamespace, NVS_READWRITE, &handle) != ESP_OK) {
        return false;
    }

    char host[sizeof(cfg.host)];
    std::snprintf(host, sizeof(host), "%s", cfg.host);

    esp_err_t err = ESP_OK;
    err |= nvs_set_i32(handle, kKeyTcpEn, cfg.tcp_enabled ? 1 : 0);
    err |= nvs_set_str(handle, kKeyHost, host);
    err |= nvs_set_i32(handle, kKeyTcpPort, clamp_port(cfg.tcp_port));
    err |= nvs_set_i32(handle, kKeyUdpEn, cfg.udp_enabled ? 1 : 0);
    err |= nvs_set_i32(handle, kKeyUdpPort, clamp_port(cfg.udp_port));

    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }
    nvs_close(handle);

    if (err == ESP_OK) {
        g_version.fetch_add(1, std::memory_order_release);
        return true;
    }
    return false;
}

unsigned version()
{
    return g_version.load(std::memory_order_acquire);
}

void set_tcp_state(TcpState state)
{
    g_tcp_state.store(static_cast<int>(state), std::memory_order_relaxed);
}

TcpState tcp_state()
{
    return static_cast<TcpState>(g_tcp_state.load(std::memory_order_relaxed));
}

const char *tcp_state_name(TcpState state)
{
    switch (state) {
    case TcpState::Disabled:   return "Disabled";
    case TcpState::Idle:       return "Idle";
    case TcpState::Connecting: return "Connecting";
    case TcpState::Connected:  return "Connected";
    case TcpState::Error:      return "Error";
    default:                   return "?";
    }
}

}  // namespace netcfg
