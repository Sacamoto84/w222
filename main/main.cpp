/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_check.h"
#include <inttypes.h>
#include "esp_event.h"
#include "esp_netif.h"
#ifndef CONFIG_SLAVE_IDF_TARGET_ESP32C6
#define CONFIG_SLAVE_IDF_TARGET_ESP32C6 1
#endif
#include "esp_wifi_default_config.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_heap_caps.h"
#include "dirent.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_memory_utils.h"
// #include "esp_dsp.h"
#include "lvgl.h"
#include "bsp/esp-bsp.h"
#include "bsp/display.h"
#include "bsp_board_extra.h"
#include "c6_slave_ota.h"
#include "ftp_server.h"

#include "demos/lv_demos.h"

#include "examples/event/lv_example_event.h"

#include "esp_brookesia.hpp"
#include "../components/apps/calculator/Calculator.hpp"
#include "../components/apps/setting/Setting.hpp"
#include "../components/apps/system_ui_service.h"

#include "esp_hosted.h"

static const char *TAG = "app";

#define WIFI_DEFAULT_SSID "TP-Link_BC0C"
#define WIFI_DEFAULT_PASSWORD "58133514"
#define C6_OTA_FILE BSP_SD_MOUNT_POINT "/network_adapter.bin"

static lv_obj_t *s_ip_label;
static char s_ip_text[96] = "WiFi: connecting...";
static char s_wifi_ssid[33] = WIFI_DEFAULT_SSID;
static char s_wifi_password[65] = WIFI_DEFAULT_PASSWORD;

static constexpr const char *kWifiNvsNamespace = "wifi_cfg";
static constexpr const char *kWifiNvsSsidKey = "ssid";
static constexpr const char *kWifiNvsPasswordKey = "pass";

static void log_c6_fw_info(void)
{
    esp_hosted_coprocessor_fwver_t ver = {0};

    if (esp_hosted_get_coprocessor_fwversion(&ver) == ESP_OK)
    {
        ESP_LOGI("!!!", "C6 ESP-Hosted FW: %lu.%lu.%lu rev=%ld prerelease=%ld build=%ld",
                 ver.major1, ver.minor1, ver.patch1,
                 ver.revision, ver.prerelease, ver.build);
    }
    else
    {
        ESP_LOGW("!!!", "Cannot read C6 ESP-Hosted firmware version");
    }

    uint32_t chip_id = 0;
    char target[16] = {0};

    if (esp_hosted_get_cp_info(&chip_id, target, sizeof(target)) == ESP_OK)
    {
        ESP_LOGI("!!!", "C6 target: %s, chip_id=0x%08lx", target, chip_id);
    }

    esp_hosted_app_desc_t desc = {0};

    if (esp_hosted_get_coprocessor_app_desc(&desc) == ESP_OK &&
        desc.magic_word == ESP_HOSTED_APP_DESC_MAGIC_WORD)
    {
        ESP_LOGI("!!!", "C6 app: %s", desc.project_name);
        ESP_LOGI("!!!", "C6 app version: %s", desc.version);
        ESP_LOGI("!!!", "C6 IDF: %s", desc.idf_ver);
        ESP_LOGI("!!!", "C6 built: %s %s", desc.date, desc.time);
    }
}

static void update_ip_label_text(const char *text)
{
    snprintf(s_ip_text, sizeof(s_ip_text), "%s", text);

    if (s_ip_label == NULL)
    {
        return;
    }

    if (bsp_display_lock(100))
    {
        lv_label_set_text(s_ip_label, s_ip_text);
        bsp_display_unlock();
    }
}

static void create_ip_label(void)
{
    lv_obj_t *screen = lv_screen_active();
    s_ip_label = lv_label_create(screen);
    lv_label_set_text(s_ip_label, s_ip_text);
    lv_obj_set_style_text_color(s_ip_label, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_bg_color(s_ip_label, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(s_ip_label, LV_OPA_60, 0);
    lv_obj_set_style_pad_all(s_ip_label, 8, 0);
    lv_obj_set_style_radius(s_ip_label, 4, 0);
    lv_obj_align(s_ip_label, LV_ALIGN_TOP_LEFT, 8, 8);
}

static void load_wifi_credentials_from_nvs(void)
{
    snprintf(s_wifi_ssid, sizeof(s_wifi_ssid), "%s", WIFI_DEFAULT_SSID);
    snprintf(s_wifi_password, sizeof(s_wifi_password), "%s", WIFI_DEFAULT_PASSWORD);

    nvs_handle_t handle = 0;
    if (nvs_open(kWifiNvsNamespace, NVS_READONLY, &handle) != ESP_OK)
    {
        return;
    }

    size_t ssid_size = sizeof(s_wifi_ssid);
    if ((nvs_get_str(handle, kWifiNvsSsidKey, s_wifi_ssid, &ssid_size) != ESP_OK) || (s_wifi_ssid[0] == '\0'))
    {
        snprintf(s_wifi_ssid, sizeof(s_wifi_ssid), "%s", WIFI_DEFAULT_SSID);
    }

    size_t password_size = sizeof(s_wifi_password);
    if (nvs_get_str(handle, kWifiNvsPasswordKey, s_wifi_password, &password_size) != ESP_OK)
    {
        snprintf(s_wifi_password, sizeof(s_wifi_password), "%s", WIFI_DEFAULT_PASSWORD);
    }

    nvs_close(handle);
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        ESP_LOGI(TAG, "WiFi STA start, connecting to %s", s_wifi_ssid);
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        ESP_LOGW(TAG, "WiFi disconnected, reconnecting to %s", s_wifi_ssid);
        system_ui_service::set_wifi_connected(false);
        update_ip_label_text("WiFi: reconnecting...");
        esp_wifi_connect();
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "WiFi connected, IP: " IPSTR, IP2STR(&event->ip_info.ip));

        char text[sizeof(s_ip_text)];
        snprintf(text, sizeof(text), "IP: " IPSTR "\nFTP: esp32/esp32", IP2STR(&event->ip_info.ip));
        update_ip_label_text(text);
        system_ui_service::set_wifi_connected(true);
    }

    log_c6_fw_info();
}

static void nvs_init_once(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

static esp_err_t wifi_start_sta(void)
{
    load_wifi_credentials_from_nvs();

    ESP_RETURN_ON_ERROR(esp_netif_init(), TAG, "esp_netif_init failed");

    esp_err_t ret = esp_event_loop_create_default();
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE)
    {
        ESP_RETURN_ON_ERROR(ret, TAG, "esp_event_loop_create_default failed");
    }

    esp_netif_t *sta_netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    if (sta_netif == NULL)
    {
        sta_netif = esp_netif_create_default_wifi_sta();
        if (sta_netif == NULL)
        {
            return ESP_FAIL;
        }
    }

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_RETURN_ON_ERROR(esp_wifi_init(&cfg), TAG, "esp_wifi_init failed");
    ESP_RETURN_ON_ERROR(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL),
                        TAG, "register WIFI_EVENT failed");
    ESP_RETURN_ON_ERROR(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL),
                        TAG, "register IP_EVENT failed");

    wifi_config_t wifi_config = {};
    snprintf((char *)wifi_config.sta.ssid, sizeof(wifi_config.sta.ssid), "%s", s_wifi_ssid);
    snprintf((char *)wifi_config.sta.password, sizeof(wifi_config.sta.password), "%s", s_wifi_password);

    ESP_RETURN_ON_ERROR(esp_wifi_set_mode(WIFI_MODE_STA), TAG, "esp_wifi_set_mode failed");
    ESP_RETURN_ON_ERROR(esp_wifi_set_config(WIFI_IF_STA, &wifi_config), TAG, "esp_wifi_set_config failed");
    ESP_RETURN_ON_ERROR(esp_wifi_start(), TAG, "esp_wifi_start failed");

    ESP_LOGI(TAG, "WiFi connect started: SSID=%s", s_wifi_ssid);
    return ESP_OK;
}

static void log_sdcard_root(void)
{
    DIR *dir = opendir(BSP_SD_MOUNT_POINT);
    if (dir == NULL)
    {
        ESP_LOGE("!!! SDCARD", "Failed to open %s", BSP_SD_MOUNT_POINT);
        return;
    }

    ESP_LOGI("!!! SDCARD", "Root directory: %s", BSP_SD_MOUNT_POINT);

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        const char *type = (entry->d_type == DT_DIR) ? "DIR " : "FILE";
        ESP_LOGI("SDCARD", "%s %s", type, entry->d_name);
    }

    closedir(dir);
}

static void show_startup_screen(void)
{
    lv_obj_t *screen = lv_screen_active();
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x12343b), 0);

    lv_obj_t *title = lv_label_create(screen);
    lv_label_set_text(title, "ESP32-P4 display OK");
    lv_obj_set_style_text_color(title, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_22, 0);
    lv_obj_align(title, LV_ALIGN_CENTER, 0, -32);

    lv_obj_t *subtitle = lv_label_create(screen);
    lv_label_set_text_fmt(subtitle, "%dx%d  LVGL %d.%d.%d",
                          BSP_LCD_H_RES, BSP_LCD_V_RES,
                          lv_version_major(), lv_version_minor(), lv_version_patch());
    lv_obj_set_style_text_color(subtitle, lv_color_hex(0xb8e6e0), 0);
    lv_obj_align(subtitle, LV_ALIGN_CENTER, 0, 24);
}

static void brookesia_fail_unlock(const char *message)
{
    ESP_LOGE(TAG, "%s", message);
    bsp_display_unlock();
}

static void install_app_or_delete(ESP_Brookesia_Phone &phone, ESP_Brookesia_PhoneApp *app, const char *name)
{
    if (app == nullptr)
    {
        ESP_LOGE(TAG, "Create Brookesia app failed: %s", name);
        return;
    }

    if (phone.installApp(app) < 0)
    {
        ESP_LOGE(TAG, "Install Brookesia app failed: %s", name);
        delete app;
        return;
    }

    ESP_LOGI(TAG, "Brookesia app installed: %s", name);
}

static void brookesia_fix_layers_after_begin(lv_display_t *display)
{
    lv_obj_t *active_screen = lv_display_get_screen_active(display);
    lv_obj_t *system_layer = lv_display_get_layer_sys(display);

    if (active_screen != nullptr)
    {
        lv_obj_set_style_bg_color(active_screen, lv_color_hex(0x1a1a1a), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(active_screen, LV_OPA_COVER, LV_PART_MAIN);
    }

    if (system_layer != nullptr)
    {
        lv_obj_set_style_bg_opa(system_layer, LV_OPA_TRANSP, LV_PART_MAIN);
        lv_obj_clear_flag(system_layer, LV_OBJ_FLAG_SCROLLABLE);
    }
}

static ESP_Brookesia_PhoneStylesheet_t s_brookesia_480_800_stylesheet =
    ESP_BROOKESIA_PHONE_480_800_DARK_STYLESHEET();

static ESP_Brookesia_PhoneStylesheet_t *brookesia_get_stylesheet(lv_display_t *display)
{
    const int32_t hres = (display != nullptr) ? lv_display_get_horizontal_resolution(display) : BSP_LCD_H_RES;
    const int32_t vres = (display != nullptr) ? lv_display_get_vertical_resolution(display) : BSP_LCD_V_RES;

    ESP_LOGI(TAG, "Brookesia display resolution: %" PRId32 "x%" PRId32, hres, vres);

    // Структура стиля Brookesia довольно большая. Держим ее в статической памяти,
    // а не возвращаем по значению, иначе main_task на ESP32-P4 легко ловит stack protection fault.
    ESP_Brookesia_PhoneStylesheet_t *stylesheet = &s_brookesia_480_800_stylesheet;
    stylesheet->manager.flags.enable_gesture = 1;
    stylesheet->manager.flags.enable_gesture_navigation_back = 1;

    if ((hres != 480) || (vres != 800))
    {
        ESP_LOGW(TAG, "Using 480x800 Brookesia stylesheet on reported resolution %" PRId32 "x%" PRId32, hres, vres);
    }

    return stylesheet;
}

static void brookesia_recents_memory_timer_cb(lv_timer_t *timer)
{
    ESP_Brookesia_Phone *phone = static_cast<ESP_Brookesia_Phone *>(lv_timer_get_user_data(timer));
    if (phone == nullptr)
    {
        return;
    }

    ESP_Brookesia_RecentsScreen *recents_screen = phone->getHome().getRecentsScreen();
    if (recents_screen == nullptr)
    {
        return;
    }

    const int internal_free = heap_caps_get_free_size(MALLOC_CAP_INTERNAL) / 1024;
    const int internal_total = heap_caps_get_total_size(MALLOC_CAP_INTERNAL) / 1024;
    const int psram_free = heap_caps_get_free_size(MALLOC_CAP_SPIRAM) / 1024;
    const int psram_total = heap_caps_get_total_size(MALLOC_CAP_SPIRAM) / 1024;

    recents_screen->setMemoryLabel(internal_free, internal_total, psram_free, psram_total);
}

static ESP_Brookesia_Phone *start_brookesia_phone(lv_display_t *display)
{
    ESP_Brookesia_Phone *phone = new ESP_Brookesia_Phone(display);
    if (phone == nullptr)
    {
        ESP_LOGE(TAG, "Create Brookesia phone failed");
        return nullptr;
    }

    ESP_Brookesia_PhoneStylesheet_t *phone_stylesheet = brookesia_get_stylesheet(display);
    ESP_LOGI(TAG, "Using Brookesia stylesheet: %s", phone_stylesheet->core.name);

    if (!phone->addStylesheet(phone_stylesheet))
    {
        ESP_LOGE(TAG, "Add Brookesia phone stylesheet failed");
        delete phone;
        return nullptr;
    }
    if (!phone->activateStylesheet(phone_stylesheet))
    {
        ESP_LOGE(TAG, "Activate Brookesia phone stylesheet failed");
        delete phone;
        return nullptr;
    }

    lv_indev_t *touch = bsp_display_get_input_dev();
    if (touch != nullptr)
    {
        if (!phone->setTouchDevice(touch))
        {
            ESP_LOGE(TAG, "Set Brookesia touch device failed");
            delete phone;
            return nullptr;
        }
    }
    else
    {
        ESP_LOGW(TAG, "BSP returned no touch device; Brookesia will try the default LVGL pointer device");
    }

    phone->registerLvLockCallback((ESP_Brookesia_GUI_LockCallback_t)bsp_display_lock, 0);
    phone->registerLvUnlockCallback((ESP_Brookesia_GUI_UnlockCallback_t)bsp_display_unlock);

    if (!phone->begin())
    {
        ESP_LOGE(TAG, "Begin Brookesia phone failed");
        delete phone;
        return nullptr;
    }

    brookesia_fix_layers_after_begin(display);
    if (!system_ui_service::initialize(*phone))
    {
        ESP_LOGW(TAG, "System UI service initialization failed");
    }

    install_app_or_delete(*phone, new Calculator(), "calculator");
    install_app_or_delete(*phone, new AppSettings(), "settings");
    lv_timer_create(brookesia_recents_memory_timer_cb, 2000, phone);

    return phone;
}

extern "C" void app_main(void)
{
    nvs_init_once();

    esp_err_t wifi_ret = wifi_start_sta();
    if (wifi_ret != ESP_OK)
    {
        ESP_LOGE(TAG, "WiFi start failed: %s", esp_err_to_name(wifi_ret));
        update_ip_label_text("WiFi: start failed");
    }

    int res = bsp_sdcard_mount();

    ESP_LOGI("!!! SDCARD", "SDCARD %d", res);
    if (res == ESP_OK)
    {
        log_sdcard_root();
        esp_err_t ota_ret = c6_slave_ota_from_sd(C6_OTA_FILE);
        if (ota_ret != ESP_OK && ota_ret != ESP_ERR_NOT_FOUND && ota_ret != ESP_ERR_INVALID_STATE)
        {
            ESP_LOGE(TAG, "C6 OTA failed: %s", esp_err_to_name(ota_ret));
        }

        ftp_server_config_t ftp_config = FTP_SERVER_DEFAULT_CONFIG(BSP_SD_MOUNT_POINT);
        esp_err_t ftp_ret = ftp_server_start(&ftp_config);
        if (ftp_ret != ESP_OK)
        {
            ESP_LOGE(TAG, "FTP server start failed: %s", esp_err_to_name(ftp_ret));
        }
    }

    ESP_ERROR_CHECK(bsp_extra_codec_init());

    bsp_display_cfg_t cfg = {
        .lvgl_port_cfg = ESP_LVGL_PORT_INIT_CONFIG(),
        .buffer_size = BSP_LCD_DRAW_BUFF_SIZE,
        .double_buffer = BSP_LCD_DRAW_BUFF_DOUBLE,
        .flags = {
#if CONFIG_BSP_LCD_COLOR_FORMAT_RGB888
            .buff_dma = false,
#else
            .buff_dma = true,
#endif
            .buff_spiram = false,
            .sw_rotate = true,
        }};
    cfg.lvgl_port_cfg.task_stack = 24 * 1024;
    cfg.lvgl_port_cfg.task_affinity = 1;
    
    lv_display_t *display = bsp_display_start_with_config(&cfg);
    ESP_ERROR_CHECK(display ? ESP_OK : ESP_FAIL);
    ESP_ERROR_CHECK(bsp_display_backlight_on());
    ESP_ERROR_CHECK(bsp_display_brightness_set(40));

    ESP_LOGI(TAG, "Display initialized: %dx%d", BSP_LCD_H_RES, BSP_LCD_V_RES);

    if (!bsp_display_lock(1000))
    {
        ESP_LOGE(TAG, "LVGL lock timeout");
        return;
    }

    // show_startup_screen();

    // ui_init();

    //lv_demo_benchmark();

    // lv_demo_stress();

    //lv_example_event_draw();
    //create_ip_label();

    ESP_Brookesia_Phone *phone = start_brookesia_phone(display);
    if (phone == nullptr)
    {
        show_startup_screen();
        brookesia_fail_unlock("Brookesia startup failed, fallback screen shown");
        return;
    }

    bsp_display_unlock();
}
