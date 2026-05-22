#include "lite_launcher.h"

#include <string.h>

#include "esp_log.h"

namespace {

static const char *TAG = "lite_launcher";

static void make_plain_container(lv_obj_t *obj)
{
    lv_obj_remove_style_all(obj);
    lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_pad_all(obj, 0, 0);
}

static lv_obj_t *create_label(lv_obj_t *parent, const char *text, const lv_font_t *font, uint32_t color)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
    return label;
}

static void make_launcher_child_passthrough(lv_obj_t *obj)
{
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_EVENT_BUBBLE);
}

} // namespace

bool LiteLauncher::add_app(LiteApp *app)
{
    if ((app == nullptr) || (app_count_ >= kMaxApps)) {
        return false;
    }

    apps_[app_count_++] = app;
    return true;
}

bool LiteLauncher::begin(lv_obj_t *screen)
{
    if (screen == nullptr) {
        return false;
    }

    screen_ = screen;
    lv_obj_clean(screen_);
    lv_obj_set_style_bg_color(screen_, lv_color_hex(0x10131A), 0);
    lv_obj_set_style_bg_opa(screen_, LV_OPA_COVER, 0);

    root_ = lv_obj_create(screen_);
    make_plain_container(root_);
    lv_obj_set_size(root_, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(root_, lv_color_hex(0x10131A), 0);
    lv_obj_set_style_bg_opa(root_, LV_OPA_COVER, 0);

    build_home();
    build_app_page();
    show_home();

    return true;
}

void LiteLauncher::set_status_text(const char *text)
{
    if (text == nullptr) {
        text = "";
    }

    if (status_label_ != nullptr) {
        lv_label_set_text(status_label_, text);
    }
}

void LiteLauncher::build_home(void)
{
    home_page_ = lv_obj_create(root_);
    make_plain_container(home_page_);
    lv_obj_set_size(home_page_, lv_pct(100), lv_pct(100));
    lv_obj_set_style_pad_left(home_page_, 22, 0);
    lv_obj_set_style_pad_right(home_page_, 22, 0);
    lv_obj_set_style_pad_top(home_page_, 14, 0);
    lv_obj_set_style_pad_bottom(home_page_, 18, 0);
    lv_obj_set_style_pad_row(home_page_, 18, 0);
    lv_obj_set_flex_flow(home_page_, LV_FLEX_FLOW_COLUMN);

    lv_obj_t *status_row = lv_obj_create(home_page_);
    make_plain_container(status_row);
    lv_obj_set_width(status_row, lv_pct(100));
    lv_obj_set_height(status_row, 36);
    lv_obj_set_flex_flow(status_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(status_row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    create_label(status_row, "W222", &lv_font_montserrat_22, 0xFFFFFF);

    status_label_ = create_label(status_row, "", &lv_font_montserrat_14, 0xAAB6C6);
    lv_label_set_long_mode(status_label_, LV_LABEL_LONG_DOT);
    lv_obj_set_width(status_label_, 270);
    lv_obj_set_style_text_align(status_label_, LV_TEXT_ALIGN_RIGHT, 0);

    lv_obj_t *grid = lv_obj_create(home_page_);
    make_plain_container(grid);
    lv_obj_set_width(grid, lv_pct(100));
    lv_obj_set_flex_grow(grid, 1);
    lv_obj_set_style_pad_row(grid, 20, 0);
    lv_obj_set_style_pad_column(grid, 4, 0);
    lv_obj_set_flex_flow(grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(grid, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

    for (size_t i = 0; i < app_count_; ++i) {
        LiteApp *app = apps_[i];
        icon_events_[i] = {
            .launcher = this,
            .app = app,
        };

        lv_obj_t *tile = lv_button_create(grid);
        lv_obj_remove_style_all(tile);
        lv_obj_set_size(tile, 104, 118);
        lv_obj_set_style_bg_opa(tile, LV_OPA_TRANSP, 0);
        lv_obj_set_style_radius(tile, 8, 0);
        lv_obj_set_style_pad_all(tile, 0, 0);
        lv_obj_set_flex_flow(tile, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(tile, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_row(tile, 8, 0);
        lv_obj_add_flag(tile, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_clear_flag(tile, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_event_cb(tile, icon_event_cb, LV_EVENT_CLICKED, &icon_events_[i]);

        lv_obj_t *icon = lv_obj_create(tile);
        lv_obj_remove_style_all(icon);
        lv_obj_set_size(icon, 70, 70);
        lv_obj_set_style_radius(icon, 18, 0);
        lv_obj_set_style_bg_color(icon, app->accent_color(), 0);
        lv_obj_set_style_bg_opa(icon, LV_OPA_COVER, 0);
        lv_obj_set_style_shadow_width(icon, 12, 0);
        lv_obj_set_style_shadow_opa(icon, LV_OPA_20, 0);
        lv_obj_set_style_shadow_color(icon, app->accent_color(), 0);
        make_launcher_child_passthrough(icon);

        lv_obj_t *icon_text = create_label(icon, app->icon_text(), &lv_font_montserrat_26, 0xFFFFFF);
        lv_obj_center(icon_text);
        make_launcher_child_passthrough(icon_text);

        lv_obj_t *title = create_label(tile, app->title(), &lv_font_montserrat_14, 0xE8ECF3);
        lv_label_set_long_mode(title, LV_LABEL_LONG_DOT);
        lv_obj_set_width(title, 100);
        lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
        make_launcher_child_passthrough(title);
    }
}

void LiteLauncher::build_app_page(void)
{
    app_page_ = lv_obj_create(root_);
    make_plain_container(app_page_);
    lv_obj_set_size(app_page_, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(app_page_, lv_color_hex(0x11151D), 0);
    lv_obj_set_style_bg_opa(app_page_, LV_OPA_COVER, 0);
    lv_obj_set_flex_flow(app_page_, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_event_cb(app_page_, app_gesture_event_cb, LV_EVENT_GESTURE, this);

    lv_obj_t *header = lv_obj_create(app_page_);
    make_plain_container(header);
    lv_obj_set_width(header, lv_pct(100));
    lv_obj_set_height(header, 58);
    lv_obj_set_style_bg_color(header, lv_color_hex(0x171C25), 0);
    lv_obj_set_style_bg_opa(header, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_left(header, 10, 0);
    lv_obj_set_style_pad_right(header, 14, 0);
    lv_obj_set_style_pad_column(header, 12, 0);
    lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *back = lv_button_create(header);
    lv_obj_set_size(back, 42, 42);
    lv_obj_set_style_radius(back, 21, 0);
    lv_obj_set_style_bg_color(back, lv_color_hex(0x252D39), 0);
    lv_obj_set_style_bg_opa(back, LV_OPA_COVER, 0);
    lv_obj_add_event_cb(back, back_button_event_cb, LV_EVENT_CLICKED, this);

    lv_obj_t *back_label = create_label(back, LV_SYMBOL_LEFT, &lv_font_montserrat_22, 0xFFFFFF);
    lv_obj_center(back_label);

    app_title_label_ = create_label(header, "", &lv_font_montserrat_22, 0xFFFFFF);
    lv_obj_set_flex_grow(app_title_label_, 1);

    app_body_ = lv_obj_create(app_page_);
    make_plain_container(app_body_);
    lv_obj_set_width(app_body_, lv_pct(100));
    lv_obj_set_flex_grow(app_body_, 1);
    lv_obj_set_style_bg_color(app_body_, lv_color_hex(0x11151D), 0);
    lv_obj_set_style_bg_opa(app_body_, LV_OPA_COVER, 0);
}

void LiteLauncher::show_home(void)
{
    active_app_ = nullptr;
    if (app_page_ != nullptr) {
        lv_obj_add_flag(app_page_, LV_OBJ_FLAG_HIDDEN);
    }
    if (home_page_ != nullptr) {
        lv_obj_clear_flag(home_page_, LV_OBJ_FLAG_HIDDEN);
    }
}

void LiteLauncher::open_app(LiteApp *app)
{
    if ((app == nullptr) || (app_page_ == nullptr) || (app_body_ == nullptr)) {
        return;
    }

    const int app_index = find_app_index(app);
    if (app_index < 0) {
        ESP_LOGE(TAG, "App is not registered");
        return;
    }

    if (active_app_ != nullptr) {
        close_current_app();
    }

    active_app_ = app;
    active_app_->set_close_request_callback(close_request_cb, this);

    lv_label_set_text(app_title_label_, app->title());
    lv_obj_clean(app_body_);
    lv_obj_add_flag(home_page_, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(app_page_, LV_OBJ_FLAG_HIDDEN);
    lv_obj_update_layout(app_page_);

    if (!app_initialized_[app_index]) {
        if (!active_app_->init()) {
            ESP_LOGE(TAG, "App init failed: %s", app->title());
            close_current_app();
            return;
        }
        app_initialized_[app_index] = true;
    }

    if (!active_app_->open(app_body_)) {
        ESP_LOGE(TAG, "App open failed: %s", app->title());
        close_current_app();
    }
}

int LiteLauncher::find_app_index(LiteApp *app) const
{
    for (size_t i = 0; i < app_count_; ++i) {
        if (apps_[i] == app) {
            return static_cast<int>(i);
        }
    }

    return -1;
}

bool LiteLauncher::dispatch_back(void)
{
    if (active_app_ == nullptr) {
        return false;
    }

    if (!active_app_->back()) {
        close_current_app();
    }

    return true;
}

void LiteLauncher::close_current_app(void)
{
    if (active_app_ != nullptr) {
        active_app_->close();
        active_app_->set_close_request_callback(nullptr, nullptr);
    }

    if (app_body_ != nullptr) {
        lv_obj_clean(app_body_);
    }

    show_home();
}

void LiteLauncher::icon_event_cb(lv_event_t *event)
{
    IconEventData *data = static_cast<IconEventData *>(lv_event_get_user_data(event));
    if ((data != nullptr) && (data->launcher != nullptr)) {
        data->launcher->open_app(data->app);
    }
}

void LiteLauncher::back_button_event_cb(lv_event_t *event)
{
    LiteLauncher *launcher = static_cast<LiteLauncher *>(lv_event_get_user_data(event));
    if (launcher != nullptr) {
        launcher->dispatch_back();
    }
}

void LiteLauncher::app_gesture_event_cb(lv_event_t *event)
{
    LiteLauncher *launcher = static_cast<LiteLauncher *>(lv_event_get_user_data(event));
    lv_indev_t *indev = lv_indev_active();

    if ((launcher == nullptr) || (indev == nullptr)) {
        return;
    }

    if (lv_indev_get_gesture_dir(indev) == LV_DIR_RIGHT) {
        launcher->dispatch_back();
    }
}

void LiteLauncher::close_request_cb(LiteApp *app, void *user_ctx)
{
    LiteLauncher *launcher = static_cast<LiteLauncher *>(user_ctx);
    if ((launcher != nullptr) && (launcher->active_app_ == app)) {
        launcher->close_current_app();
    }
}
