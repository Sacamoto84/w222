#pragma once

#include "lite_app.h"
#include "lvgl.h"

// Приложение настройки ввода/вывода терминала (I/O Settings).
//
// Карточка UART: порт, скорость, чётность, пины RX/TX — сохраняются в NVS через
// модуль uartcfg и применяются терминалом к живому драйверу.
// Карточка Network: TCP-клиент (host/port) и опциональный UDP-листенер —
// сохраняются через модуль netcfg; терминал переподключается по version().
class UartSettingApp : public LiteApp {
public:
    UartSettingApp() = default;
    ~UartSettingApp() override = default;

    const char *title(void) const override { return "I/O Settings"; }
    const char *icon_text(void) const override { return LV_SYMBOL_USB; }
    lv_color_t accent_color(void) const override { return lv_color_hex(0xC77F0A); }

    bool open(lv_obj_t *parent) override;
    void close(void) override;

private:
    // UART
    lv_obj_t *port_dropdown_ = nullptr;
    lv_obj_t *baud_dropdown_ = nullptr;
    lv_obj_t *parity_dropdown_ = nullptr;
    lv_obj_t *rx_textarea_ = nullptr;
    lv_obj_t *tx_textarea_ = nullptr;
    lv_obj_t *status_label_ = nullptr;

    // Network
    lv_obj_t *tcp_switch_ = nullptr;
    lv_obj_t *host_textarea_ = nullptr;
    lv_obj_t *tcp_port_textarea_ = nullptr;
    lv_obj_t *udp_switch_ = nullptr;
    lv_obj_t *udp_port_textarea_ = nullptr;
    lv_obj_t *net_status_label_ = nullptr;
    lv_timer_t *net_status_timer_ = nullptr;

    void load_into_ui(void);
    void save_uart_from_ui(void);
    void save_net_from_ui(void);
    void refresh_net_status(void);

    static void save_event_cb(lv_event_t *event);
    static void save_net_event_cb(lv_event_t *event);
    static void net_status_timer_cb(lv_timer_t *timer);
};
