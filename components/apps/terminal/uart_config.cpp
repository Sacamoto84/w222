#include "uart_config.hpp"

#include <atomic>

#include "nvs.h"

#ifndef CONFIG_JC4880_TERMINAL_UART_PORT
#define CONFIG_JC4880_TERMINAL_UART_PORT 1
#endif

#ifndef CONFIG_JC4880_TERMINAL_UART_RX_PIN
#define CONFIG_JC4880_TERMINAL_UART_RX_PIN 35
#endif

#ifndef CONFIG_JC4880_TERMINAL_UART_TX_PIN
#define CONFIG_JC4880_TERMINAL_UART_TX_PIN -1
#endif

#ifndef CONFIG_JC4880_TERMINAL_UART_BAUD_RATE
#define CONFIG_JC4880_TERMINAL_UART_BAUD_RATE 115200
#endif

namespace uartcfg {

namespace {

constexpr const char *kNvsNamespace = "uart_cfg";
constexpr const char *kKeyPort = "port";
constexpr const char *kKeyRx = "rx";
constexpr const char *kKeyTx = "tx";
constexpr const char *kKeyBaud = "baud";
constexpr const char *kKeyParity = "parity";

// Флаг "конфигурация изменилась". Выставляется в save(), сбрасывается в
// consume_dirty() из UART-задачи терминала.
std::atomic<bool> g_dirty{false};

int clamp_int(int v, int lo, int hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

}  // namespace

Config defaults()
{
    Config cfg;
    cfg.port = CONFIG_JC4880_TERMINAL_UART_PORT;
    cfg.rx_pin = CONFIG_JC4880_TERMINAL_UART_RX_PIN;
    cfg.tx_pin = CONFIG_JC4880_TERMINAL_UART_TX_PIN;
    cfg.baud = CONFIG_JC4880_TERMINAL_UART_BAUD_RATE;
    cfg.parity = Parity::None;
    return cfg;
}

Config load()
{
    Config cfg = defaults();

    nvs_handle_t handle = 0;
    if (nvs_open(kNvsNamespace, NVS_READONLY, &handle) != ESP_OK) {
        return cfg;
    }

    int32_t value = 0;
    if (nvs_get_i32(handle, kKeyPort, &value) == ESP_OK) {
        cfg.port = clamp_int(static_cast<int>(value), 0, 2);
    }
    if (nvs_get_i32(handle, kKeyRx, &value) == ESP_OK) {
        cfg.rx_pin = clamp_int(static_cast<int>(value), -1, 56);
    }
    if (nvs_get_i32(handle, kKeyTx, &value) == ESP_OK) {
        cfg.tx_pin = clamp_int(static_cast<int>(value), -1, 56);
    }
    if (nvs_get_i32(handle, kKeyBaud, &value) == ESP_OK) {
        cfg.baud = clamp_int(static_cast<int>(value), 1200, 3000000);
    }
    if (nvs_get_i32(handle, kKeyParity, &value) == ESP_OK) {
        cfg.parity = static_cast<Parity>(clamp_int(static_cast<int>(value), 0, 2));
    }

    nvs_close(handle);
    return cfg;
}

bool save(const Config &cfg)
{
    nvs_handle_t handle = 0;
    if (nvs_open(kNvsNamespace, NVS_READWRITE, &handle) != ESP_OK) {
        return false;
    }

    esp_err_t err = ESP_OK;
    err |= nvs_set_i32(handle, kKeyPort, clamp_int(cfg.port, 0, 2));
    err |= nvs_set_i32(handle, kKeyRx, clamp_int(cfg.rx_pin, -1, 56));
    err |= nvs_set_i32(handle, kKeyTx, clamp_int(cfg.tx_pin, -1, 56));
    err |= nvs_set_i32(handle, kKeyBaud, clamp_int(cfg.baud, 1200, 3000000));
    err |= nvs_set_i32(handle, kKeyParity, clamp_int(static_cast<int>(cfg.parity), 0, 2));

    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }
    nvs_close(handle);

    if (err == ESP_OK) {
        g_dirty.store(true, std::memory_order_release);
        return true;
    }
    return false;
}

bool consume_dirty()
{
    return g_dirty.exchange(false, std::memory_order_acq_rel);
}

}  // namespace uartcfg
