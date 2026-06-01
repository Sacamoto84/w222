#include "setting/Setting.hpp"

#include <cstdio>
#include <cstring>

#include "esp_app_desc.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "nvs.h"

namespace {

static const char *TAG = "settings";
static constexpr const char *kWifiNvsNamespace = "wifi_cfg";
static constexpr const char *kWifiNvsSsidKey = "ssid";
static constexpr const char *kWifiNvsPasswordKey = "pass";

static void style_screen(lv_obj_t *obj)
{
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x15171D), 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(obj, lv_color_hex(0xFFFFFF), 0);
}

static lv_obj_t *create_label(lv_obj_t *parent, const char *text, const lv_font_t *font, uint32_t color)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(label, lv_pct(100));
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
    return label;
}

static lv_obj_t *create_card(lv_obj_t *parent, const char *title)
{
    lv_obj_t *card = lv_obj_create(parent);
    lv_obj_set_width(card, lv_pct(100));
    lv_obj_set_height(card, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_color(card, lv_color_hex(0x20242C), 0);
    lv_obj_set_style_border_width(card, 1, 0);
    lv_obj_set_style_border_color(card, lv_color_hex(0x343B47), 0);
    lv_obj_set_style_radius(card, 8, 0);
    lv_obj_set_style_pad_all(card, 14, 0);
    lv_obj_set_style_pad_row(card, 10, 0);
    lv_obj_set_flex_flow(card, LV_FLEX_FLOW_COLUMN);

    create_label(card, title, &lv_font_montserrat_20, 0xFFFFFF);
    return card;
}

static lv_obj_t *create_button(lv_obj_t *parent, const char *text)
{
    lv_obj_t *button = lv_button_create(parent);
    lv_obj_set_height(button, 44);
    lv_obj_set_width(button, lv_pct(100));
    lv_obj_set_style_radius(button, 8, 0);
    lv_obj_set_style_bg_color(button, lv_color_hex(0x2D7DFF), 0);

    lv_obj_t *label = lv_label_create(button);
    lv_label_set_text(label, text);
    lv_obj_center(label);
    return button;
}

static void style_textarea(lv_obj_t *textarea)
{
    lv_obj_set_width(textarea, lv_pct(100));
    lv_obj_set_height(textarea, 48);
    lv_obj_set_style_radius(textarea, 8, 0);
    lv_obj_set_style_bg_color(textarea, lv_color_hex(0x161A22), 0);
    lv_obj_set_style_border_width(textarea, 1, 0);
    lv_obj_set_style_border_color(textarea, lv_color_hex(0x3A4351), 0);
    lv_obj_set_style_text_color(textarea, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_pad_all(textarea, 10, 0);
}

static bool load_saved_wifi_ssid(char *ssid, size_t ssid_size)
{
    if ((ssid == nullptr) || (ssid_size == 0)) {
        return false;
    }

    nvs_handle_t handle = 0;
    if (nvs_open(kWifiNvsNamespace, NVS_READONLY, &handle) != ESP_OK) {
        return false;
    }

    size_t size = ssid_size;
    const esp_err_t err = nvs_get_str(handle, kWifiNvsSsidKey, ssid, &size);
    nvs_close(handle);
    return (err == ESP_OK) && (ssid[0] != '\0');
}

static bool load_saved_wifi_password(char *password, size_t password_size)
{
    if ((password == nullptr) || (password_size == 0)) {
        return false;
    }

    nvs_handle_t handle = 0;
    if (nvs_open(kWifiNvsNamespace, NVS_READONLY, &handle) != ESP_OK) {
        return false;
    }

    size_t size = password_size;
    const esp_err_t err = nvs_get_str(handle, kWifiNvsPasswordKey, password, &size);
    nvs_close(handle);
    return (err == ESP_OK) && (password[0] != '\0');
}

static void get_wifi_status_text(char *buffer, size_t buffer_size)
{
    if ((buffer == nullptr) || (buffer_size == 0)) {
        return;
    }

    wifi_ap_record_t ap_info = {};
    const bool connected = (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK);

    esp_netif_ip_info_t ip_info = {};
    esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    const bool has_ip = (netif != nullptr) && (esp_netif_get_ip_info(netif, &ip_info) == ESP_OK) &&
                        (ip_info.ip.addr != 0);

    if (connected && has_ip) {
        std::snprintf(buffer, buffer_size,
                      "Connected\nSSID: %s\nRSSI: %d dBm\nIP: " IPSTR "\nFTP: ftp://esp32:esp32@" IPSTR "/",
                      reinterpret_cast<const char *>(ap_info.ssid), ap_info.rssi,
                      IP2STR(&ip_info.ip), IP2STR(&ip_info.ip));
    } else if (connected) {
        std::snprintf(buffer, buffer_size,
                      "Connected\nSSID: %s\nRSSI: %d dBm\nIP: waiting DHCP",
                      reinterpret_cast<const char *>(ap_info.ssid), ap_info.rssi);
    } else {
        std::snprintf(buffer, buffer_size, "Disconnected");
    }
}

static void get_memory_status_text(char *buffer, size_t buffer_size)
{
    if ((buffer == nullptr) || (buffer_size == 0)) {
        return;
    }

    const uint32_t internal_free = heap_caps_get_free_size(MALLOC_CAP_INTERNAL) / 1024;
    const uint32_t internal_total = heap_caps_get_total_size(MALLOC_CAP_INTERNAL) / 1024;
    const uint32_t psram_free = heap_caps_get_free_size(MALLOC_CAP_SPIRAM) / 1024;
    const uint32_t psram_total = heap_caps_get_total_size(MALLOC_CAP_SPIRAM) / 1024;
    const uint32_t largest_internal = heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL) / 1024;
    const uint32_t largest_psram = heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM) / 1024;

    std::snprintf(buffer, buffer_size,
                  "Internal: %lu / %lu KB free\nLargest internal block: %lu KB\nPSRAM: %lu / %lu KB free\nLargest PSRAM block: %lu KB",
                  static_cast<unsigned long>(internal_free),
                  static_cast<unsigned long>(internal_total),
                  static_cast<unsigned long>(largest_internal),
                  static_cast<unsigned long>(psram_free),
                  static_cast<unsigned long>(psram_total),
                  static_cast<unsigned long>(largest_psram));
}

static void save_wifi_credentials(const char *ssid, const char *password)
{
    nvs_handle_t handle = 0;
    if (nvs_open(kWifiNvsNamespace, NVS_READWRITE, &handle) != ESP_OK) {
        return;
    }

    nvs_set_str(handle, kWifiNvsSsidKey, ssid);
    nvs_set_str(handle, kWifiNvsPasswordKey, password);
    nvs_commit(handle);
    nvs_close(handle);
}

} // namespace

AppSettings::AppSettings()
{
}

AppSettings::~AppSettings()
{
}

bool AppSettings::open(lv_obj_t *parent)
{
    if (parent == nullptr) {
        return false;
    }

    style_screen(parent);

    lv_obj_t *root = lv_obj_create(parent);
    lv_obj_remove_style_all(root);
    lv_obj_set_size(root, lv_pct(100), lv_pct(100));
    lv_obj_set_style_pad_all(root, 12, 0);
    lv_obj_set_style_pad_row(root, 12, 0);
    lv_obj_set_flex_flow(root, LV_FLEX_FLOW_COLUMN);

    lv_obj_t *content = lv_obj_create(root);
    lv_obj_set_width(content, lv_pct(100));
    lv_obj_set_flex_grow(content, 1);
    lv_obj_set_style_bg_opa(content, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(content, 0, 0);
    lv_obj_set_style_pad_all(content, 0, 0);
    lv_obj_set_style_pad_row(content, 12, 0);
    lv_obj_set_flex_flow(content, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scrollbar_mode(content, LV_SCROLLBAR_MODE_AUTO);
    content_ = content;

    lv_obj_t *memory_card = create_card(content, "Memory");
    memory_label_ = create_label(memory_card, "Loading...", &lv_font_montserrat_16, 0xC8D1DC);

    lv_obj_t *wifi_card = create_card(content, "WiFi");
    wifi_label_ = create_label(wifi_card, "Loading...", &lv_font_montserrat_16, 0xC8D1DC);

    create_label(wifi_card, "SSID", &lv_font_montserrat_14, 0x96A2B3);
    ssid_textarea_ = lv_textarea_create(wifi_card);
    style_textarea(ssid_textarea_);
    lv_textarea_set_one_line(ssid_textarea_, true);

    char saved_ssid[33] = {};
    if (load_saved_wifi_ssid(saved_ssid, sizeof(saved_ssid))) {
        lv_textarea_set_text(ssid_textarea_, saved_ssid);
    } else {
        wifi_ap_record_t ap_info = {};
        if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
            lv_textarea_set_text(ssid_textarea_, reinterpret_cast<const char *>(ap_info.ssid));
        }
    }

    create_label(wifi_card, "Password", &lv_font_montserrat_14, 0x96A2B3);
    password_textarea_ = lv_textarea_create(wifi_card);
    style_textarea(password_textarea_);
    lv_textarea_set_one_line(password_textarea_, true);
    lv_textarea_set_password_mode(password_textarea_, true);

    char saved_password[65] = {};
    if (load_saved_wifi_password(saved_password, sizeof(saved_password))) {
        lv_textarea_set_text(password_textarea_, saved_password);
    }

    lv_obj_t *connect_button = create_button(wifi_card, "Connect and save");
    lv_obj_add_event_cb(connect_button, connect_event_cb, LV_EVENT_CLICKED, this);

    lv_obj_t *refresh_button = create_button(wifi_card, "Refresh status");
    lv_obj_set_style_bg_color(refresh_button, lv_color_hex(0x3A4351), 0);
    lv_obj_add_event_cb(refresh_button, refresh_event_cb, LV_EVENT_CLICKED, this);

    status_label_ = create_label(wifi_card, "", &lv_font_montserrat_14, 0x8FD3FF);

    // --- Display orientation ---
    lv_obj_t *display_card = create_card(content, "Display");
    create_label(display_card, "Orientation (applies after reboot)", &lv_font_montserrat_14, 0x96A2B3);

    rotation_dropdown_ = lv_dropdown_create(display_card);
    lv_dropdown_set_options(rotation_dropdown_,
                            "Portrait\nLandscape\nPortrait 180\nLandscape 270");
    lv_obj_set_width(rotation_dropdown_, lv_pct(100));
    lv_obj_set_style_bg_color(rotation_dropdown_, lv_color_hex(0x161A22), 0);
    lv_obj_set_style_border_width(rotation_dropdown_, 1, 0);
    lv_obj_set_style_border_color(rotation_dropdown_, lv_color_hex(0x3A4351), 0);
    lv_obj_set_style_text_color(rotation_dropdown_, lv_color_hex(0xFFFFFF), 0);

    {
        int32_t rot = 0;
        nvs_handle_t rot_nvs = 0;
        if (nvs_open("display_cfg", NVS_READONLY, &rot_nvs) == ESP_OK) {
            nvs_get_i32(rot_nvs, "rotation", &rot);
            nvs_close(rot_nvs);
        }
        if (rot < 0 || rot > 3) rot = 0;
        lv_dropdown_set_selected(rotation_dropdown_, static_cast<uint16_t>(rot));
    }

    lv_obj_t *rotate_button = create_button(display_card, "Apply and reboot");
    lv_obj_add_event_cb(rotate_button, rotate_apply_event_cb, LV_EVENT_CLICKED, this);

    lv_obj_t *about_card = create_card(content, "System");
    const esp_app_desc_t *desc = esp_app_get_description();
    lv_display_t *disp = lv_display_get_default();
    const int hor = disp ? (int)lv_display_get_horizontal_resolution(disp) : 0;
    const int ver = disp ? (int)lv_display_get_vertical_resolution(disp) : 0;
    char about[208] = {};
    std::snprintf(about, sizeof(about),
                  "Project: %s\nVersion: %s\nIDF: %s\nChip: ESP32-P4\nScreen: %dx%d",
                  desc != nullptr ? desc->project_name : "unknown",
                  desc != nullptr ? desc->version : "unknown",
                  desc != nullptr ? desc->idf_ver : "unknown",
                  hor, ver);
    create_label(about_card, about, &lv_font_montserrat_16, 0xC8D1DC);

    // Экранная клавиатура поверх контента (скрыта; всплывает по тапу на поле).
    // FLOATING + явная геометрия — чтобы оверлеить низ независимо от flex-потока
    // родителя.
    keyboard_ = lv_keyboard_create(parent);
    lv_obj_add_flag(keyboard_, LV_OBJ_FLAG_FLOATING);
    lv_obj_set_size(keyboard_, lv_pct(100), lv_pct(45));
    lv_obj_align(keyboard_, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_flag(keyboard_, LV_OBJ_FLAG_HIDDEN);
    lv_keyboard_set_textarea(keyboard_, nullptr);
    lv_obj_add_event_cb(keyboard_, keyboard_event_cb, LV_EVENT_READY, this);
    lv_obj_add_event_cb(keyboard_, keyboard_event_cb, LV_EVENT_CANCEL, this);

    // Тап по полю ввода поднимает клавиатуру.
    lv_obj_add_event_cb(ssid_textarea_, textarea_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_add_event_cb(password_textarea_, textarea_event_cb, LV_EVENT_CLICKED, this);

    refresh_timer_ = lv_timer_create(refresh_timer_cb, 1000, this);
    refresh();

    return true;
}

void AppSettings::close(void)
{
    if (refresh_timer_ != nullptr) {
        lv_timer_delete(refresh_timer_);
    }

    memory_label_ = nullptr;
    wifi_label_ = nullptr;
    ssid_textarea_ = nullptr;
    password_textarea_ = nullptr;
    status_label_ = nullptr;
    rotation_dropdown_ = nullptr;
    content_ = nullptr;
    keyboard_ = nullptr;
    refresh_timer_ = nullptr;
}

void AppSettings::refresh(void)
{
    char buffer[256] = {};

    if (memory_label_ != nullptr) {
        get_memory_status_text(buffer, sizeof(buffer));
        lv_label_set_text(memory_label_, buffer);
    }

    if (wifi_label_ != nullptr) {
        get_wifi_status_text(buffer, sizeof(buffer));
        lv_label_set_text(wifi_label_, buffer);
    }
}

void AppSettings::connect_from_ui(void)
{
    const char *ssid = ssid_textarea_ != nullptr ? lv_textarea_get_text(ssid_textarea_) : "";
    const char *password = password_textarea_ != nullptr ? lv_textarea_get_text(password_textarea_) : "";

    if ((ssid == nullptr) || (ssid[0] == '\0')) {
        if (status_label_ != nullptr) {
            lv_label_set_text(status_label_, "SSID is empty");
        }
        return;
    }

    wifi_config_t wifi_config = {};
    std::snprintf(reinterpret_cast<char *>(wifi_config.sta.ssid), sizeof(wifi_config.sta.ssid), "%s", ssid);
    std::snprintf(reinterpret_cast<char *>(wifi_config.sta.password), sizeof(wifi_config.sta.password), "%s", password);

    save_wifi_credentials(ssid, password);

    esp_err_t err = esp_wifi_disconnect();
    if ((err != ESP_OK) && (err != ESP_ERR_WIFI_NOT_STARTED) && (err != ESP_ERR_WIFI_NOT_CONNECT)) {
        ESP_LOGW(TAG, "esp_wifi_disconnect failed: %s", esp_err_to_name(err));
    }

    err = esp_wifi_set_mode(WIFI_MODE_STA);
    if (err == ESP_OK) {
        err = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    }
    if (err == ESP_OK) {
        err = esp_wifi_connect();
    }

    if (status_label_ != nullptr) {
        lv_label_set_text_fmt(status_label_, "%s: %s",
                              err == ESP_OK ? "Connecting" : "WiFi error",
                              err == ESP_OK ? ssid : esp_err_to_name(err));
    }

    refresh();
}

void AppSettings::refresh_timer_cb(lv_timer_t *timer)
{
    AppSettings *app = static_cast<AppSettings *>(lv_timer_get_user_data(timer));
    if (app != nullptr) {
        app->refresh();
    }
}

void AppSettings::connect_event_cb(lv_event_t *event)
{
    AppSettings *app = static_cast<AppSettings *>(lv_event_get_user_data(event));
    if (app != nullptr) {
        app->connect_from_ui();
    }
}

void AppSettings::refresh_event_cb(lv_event_t *event)
{
    AppSettings *app = static_cast<AppSettings *>(lv_event_get_user_data(event));
    if (app != nullptr) {
        app->refresh();
    }
}

// Показывает экранную клавиатуру при тапе/фокусе на текстовом поле и поднимает
// само поле над клавиатурой (через отступ снизу + прокрутку).
void AppSettings::textarea_event_cb(lv_event_t *event)
{
    AppSettings *app = static_cast<AppSettings *>(lv_event_get_user_data(event));
    if ((app == nullptr) || (app->keyboard_ == nullptr)) {
        return;
    }

    lv_obj_t *textarea = static_cast<lv_obj_t *>(lv_event_get_target(event));

    lv_keyboard_set_textarea(app->keyboard_, textarea);
    lv_obj_remove_flag(app->keyboard_, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(app->keyboard_);

    // Запас прокрутки снизу = высота клавиатуры, чтобы поле можно было поднять
    // над ней, а не оставить перекрытым.
    if (app->content_ != nullptr) {
        lv_obj_update_layout(app->keyboard_);
        const int32_t kb_h = lv_obj_get_height(app->keyboard_);
        lv_obj_set_style_pad_bottom(app->content_, kb_h, 0);
        lv_obj_update_layout(app->content_);
    }
    lv_obj_scroll_to_view(textarea, LV_ANIM_ON);
}

// Скрывает клавиатуру по нажатию Ok/Esc на ней и возвращает обычный отступ.
void AppSettings::keyboard_event_cb(lv_event_t *event)
{
    AppSettings *app = static_cast<AppSettings *>(lv_event_get_user_data(event));
    if ((app == nullptr) || (app->keyboard_ == nullptr)) {
        return;
    }

    lv_obj_add_flag(app->keyboard_, LV_OBJ_FLAG_HIDDEN);
    lv_keyboard_set_textarea(app->keyboard_, nullptr);
    if (app->content_ != nullptr) {
        lv_obj_set_style_pad_bottom(app->content_, 0, 0);
    }
}

// Сохраняет выбранную ориентацию в NVS и перезагружает устройство — ориентация
// (и панель, и тач) применяется на старте через bsp_display_set_startup_rotation.
void AppSettings::rotate_apply_event_cb(lv_event_t *event)
{
    AppSettings *app = static_cast<AppSettings *>(lv_event_get_user_data(event));
    if ((app == nullptr) || (app->rotation_dropdown_ == nullptr)) {
        return;
    }

    const int32_t rot = static_cast<int32_t>(lv_dropdown_get_selected(app->rotation_dropdown_));

    nvs_handle_t rot_nvs = 0;
    if (nvs_open("display_cfg", NVS_READWRITE, &rot_nvs) == ESP_OK) {
        nvs_set_i32(rot_nvs, "rotation", rot);
        nvs_commit(rot_nvs);
        nvs_close(rot_nvs);
    }

    esp_restart();
}
