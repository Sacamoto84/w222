#pragma once

#include <stddef.h>

#include "lite_app.h"
#include "lvgl.h"

class LiteLauncher {
public:
    bool add_app(LiteApp *app);
    bool begin(lv_obj_t *screen);

    void set_status_text(const char *text);
    void set_battery_text(const char *text);
    void open_app(LiteApp *app);
    void close_current_app(void);

private:
    struct IconEventData {
        LiteLauncher *launcher;
        LiteApp *app;
    };

    static constexpr size_t kMaxApps = 16;

    LiteApp *apps_[kMaxApps] = {};
    IconEventData icon_events_[kMaxApps] = {};
    bool app_initialized_[kMaxApps] = {};
    size_t app_count_ = 0;

    lv_obj_t *screen_ = nullptr;
    lv_obj_t *root_ = nullptr;
    lv_obj_t *home_page_ = nullptr;
    lv_obj_t *app_page_ = nullptr;
    lv_obj_t *app_header_ = nullptr;
    lv_obj_t *app_title_label_ = nullptr;
    lv_obj_t *app_body_ = nullptr;
    lv_obj_t *status_label_ = nullptr;
    lv_obj_t *battery_label_ = nullptr;
    LiteApp *active_app_ = nullptr;

    void build_home(void);
    void build_app_page(void);
    void show_home(void);
    int find_app_index(LiteApp *app) const;
    bool dispatch_back(void);

    static void icon_event_cb(lv_event_t *event);
    static void back_button_event_cb(lv_event_t *event);
    static void app_gesture_event_cb(lv_event_t *event);
    static void close_request_cb(LiteApp *app, void *user_ctx);
};
