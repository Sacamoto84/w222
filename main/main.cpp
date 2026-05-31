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
#include "esp_event.h"
#include "esp_netif.h"
#ifndef CONFIG_SLAVE_IDF_TARGET_ESP32C6
#define CONFIG_SLAVE_IDF_TARGET_ESP32C6 1
#endif
#include "esp_wifi_default_config.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "dirent.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_memory_utils.h"
// #include "esp_dsp.h"
#include "lvgl.h"
#include "bsp/esp-bsp.h"
#include "bsp/display.h"
#include "c6_slave_ota.h"
#include "ftp_server.h"

#include "demos/lv_demos.h"

#include "examples/event/lv_example_event.h"

#include "lite_launcher.h"
#include "Setting.hpp"
#if CONFIG_JC4880_APP_UART_TERMINAL
#include "TerminalApp.hpp"
#endif
#if CONFIG_JC4880_APP_UART_SETTINGS
#include "UartSetting.hpp"
#endif

#include "esp_hosted.h"

static const char *TAG = "app";

#define WIFI_DEFAULT_SSID "TP-Link_BC0C"
#define WIFI_DEFAULT_PASSWORD "58133514"
#define C6_OTA_FILE BSP_SD_MOUNT_POINT "/network_adapter.bin"

static char s_ip_text[96] = "WiFi: connecting...";
static char s_wifi_ssid[33] = WIFI_DEFAULT_SSID;
static char s_wifi_password[65] = WIFI_DEFAULT_PASSWORD;
static LiteLauncher *s_launcher = nullptr;

static constexpr const char *kWifiNvsNamespace = "wifi_cfg";
static constexpr const char *kWifiNvsSsidKey = "ssid";
static constexpr const char *kWifiNvsPasswordKey = "pass";

static void log_c6_fw_info(void)
{
    esp_hosted_coprocessor_fwver_t ver = {};

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

    esp_hosted_app_desc_t desc = {};

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

    if (s_launcher == nullptr)
    {
        return;
    }

    if (bsp_display_lock(100))
    {
        s_launcher->set_status_text(s_ip_text);
        bsp_display_unlock();
    }
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
        update_ip_label_text("WiFi: reconnecting...");
        esp_wifi_connect();
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "WiFi connected, IP: " IPSTR, IP2STR(&event->ip_info.ip));

        char text[sizeof(s_ip_text)];
        snprintf(text, sizeof(text), "IP: " IPSTR "  FTP: esp32/esp32", IP2STR(&event->ip_info.ip));
        update_ip_label_text(text);
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

static bool start_lite_launcher(void)
{
    static LiteLauncher launcher;
#if CONFIG_JC4880_APP_UART_TERMINAL
    static UartTerminalApp terminal;
#endif
#if CONFIG_JC4880_APP_UART_SETTINGS
    static UartSettingApp uart_settings;
#endif
    static AppSettings settings;

#if CONFIG_JC4880_APP_UART_TERMINAL
    if (!launcher.add_app(&terminal))
    {
        ESP_LOGE(TAG, "Register UART Terminal failed");
        return false;
    }
#endif

#if CONFIG_JC4880_APP_UART_SETTINGS
    if (!launcher.add_app(&uart_settings))
    {
        ESP_LOGE(TAG, "Register UART Settings failed");
        return false;
    }
#endif

    if (!launcher.add_app(&settings))
    {
        ESP_LOGE(TAG, "Register Settings failed");
        return false;
    }

    if (!launcher.begin(lv_screen_active()))
    {
        ESP_LOGE(TAG, "Lite launcher startup failed");
        return false;
    }

    s_launcher = &launcher;
    launcher.set_status_text(s_ip_text);
#if CONFIG_JC4880_APP_UART_TERMINAL
    launcher.open_app(&terminal);
#endif
    ESP_LOGI(TAG, "Lite launcher started");
    return true;
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

    // Применяем сохранённую ориентацию экрана ДО старта дисплея: BSP так
    // корректно настраивает и панель, и тачскрин (рантайм-поворот тач не
    // разворачивает). Смена ориентации в настройках применяется перезагрузкой.
    {
        int32_t rot = 0;
        nvs_handle_t rot_nvs = 0;
        if (nvs_open("display_cfg", NVS_READONLY, &rot_nvs) == ESP_OK)
        {
            nvs_get_i32(rot_nvs, "rotation", &rot);
            nvs_close(rot_nvs);
        }
        if (rot < 0 || rot > 3)
        {
            rot = 0;
        }
        bsp_display_set_startup_rotation(static_cast<lv_disp_rotation_t>(rot));
        ESP_LOGI(TAG, "Display startup rotation: %d (0=portrait,1=landscape,2=portrait180,3=landscape270)", (int)rot);
    }

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
    if (!start_lite_launcher())
    {
        show_startup_screen();
        ESP_LOGE(TAG, "Lite launcher startup failed, fallback screen shown");
        bsp_display_unlock();
        return;
    }

    bsp_display_unlock();
}
