#pragma once

#include "lite_app.h"
#include "lvgl.h"

class AppSettings: public LiteApp {
public:
    AppSettings();
    ~AppSettings() override;

    const char *title(void) const override { return "Settings"; }
    const char *icon_text(void) const override { return LV_SYMBOL_SETTINGS; }
    lv_color_t accent_color(void) const override { return lv_color_hex(0x5967F2); }
    bool open(lv_obj_t *parent) override;
    void close(void) override;

private:
    lv_obj_t *memory_label_ = nullptr;
    lv_obj_t *wifi_label_ = nullptr;
    lv_obj_t *ssid_textarea_ = nullptr;
    lv_obj_t *password_textarea_ = nullptr;
    lv_obj_t *status_label_ = nullptr;
    lv_timer_t *refresh_timer_ = nullptr;

    void refresh(void);
    void connect_from_ui(void);

    static void refresh_timer_cb(lv_timer_t *timer);
    static void connect_event_cb(lv_event_t *event);
    static void refresh_event_cb(lv_event_t *event);
};
