#pragma once

#include "lite_app.h"
#include "lvgl.h"

// Приложение сетевых настроек терминала.
//
// Терминал работает полностью от сети: получает поток (строки, префикс @N,
// виджеты `ui ...`) по TCP от внешнего сервера (например, ESP32-C3, который
// читает UART и отдаёт поток по TCP). Здесь настраиваются адрес/порт сервера и
// опциональный UDP-листенер; значения хранятся через модуль netcfg, терминал
// переподключается по version().
class UartSettingApp : public LiteApp {
public:
    UartSettingApp() = default;
    ~UartSettingApp() override = default;

    const char *title(void) const override { return "Network"; }
    const char *icon_text(void) const override { return LV_SYMBOL_WIFI; }
    lv_color_t accent_color(void) const override { return lv_color_hex(0x2D7DFF); }

    bool open(lv_obj_t *parent) override;
    void close(void) override;

private:
    lv_obj_t *content_ = nullptr;   // прокручиваемый контейнер (для запаса под клавиатуру)

    // Network
    lv_obj_t *tcp_switch_ = nullptr;
    lv_obj_t *host_textarea_ = nullptr;
    lv_obj_t *tcp_port_textarea_ = nullptr;
    lv_obj_t *udp_switch_ = nullptr;
    lv_obj_t *udp_port_textarea_ = nullptr;
    lv_obj_t *net_status_label_ = nullptr;
    lv_timer_t *net_status_timer_ = nullptr;

    // Экранная клавиатура: всплывает по тапу на поле ввода, прячется по OK/✕.
    lv_obj_t *keyboard_ = nullptr;

    void load_into_ui(void);
    void save_net_from_ui(void);
    void refresh_net_status(void);
    void attach_keyboard(lv_obj_t *textarea, int mode);

    static void save_net_event_cb(lv_event_t *event);
    static void net_status_timer_cb(lv_timer_t *timer);
    static void textarea_event_cb(lv_event_t *event);
    static void keyboard_event_cb(lv_event_t *event);
};
