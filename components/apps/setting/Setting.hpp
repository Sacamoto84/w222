#pragma once

#include "esp_brookesia.hpp"
#include "lvgl.h"

class AppSettings: public ESP_Brookesia_PhoneApp {
public:
    AppSettings();
    ~AppSettings() override;

    bool init(void) override;
    bool run(void) override;
    bool back(void) override;
    bool close(void) override;

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
    static void back_event_cb(lv_event_t *event);
};
