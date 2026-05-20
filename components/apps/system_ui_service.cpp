#include "system_ui_service.h"

#include <algorithm>
#include <atomic>
#include <ctime>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "bsp/esp-bsp.h"
#include "esp_brookesia.hpp"
#include "esp_log.h"
#include "esp_wifi.h"

namespace {

static const char *TAG = "system_ui";
static constexpr uint32_t kStatusRefreshTaskStack = 4096;
static constexpr TickType_t kStatusRefreshPeriod = pdMS_TO_TICKS(2000);

static ESP_Brookesia_StatusBar *s_status_bar = nullptr;
static std::atomic<bool> s_initialized{false};
static std::atomic<bool> s_wifi_connected{false};
static std::atomic<int> s_wifi_signal_level{0};

static int wifi_signal_strength_from_rssi(int rssi)
{
    if (rssi > -50) {
        return 3;
    }
    if (rssi > -60) {
        return 2;
    }
    if (rssi > -75) {
        return 1;
    }
    return 0;
}

static int get_wifi_level_from_driver(bool *connected)
{
    wifi_ap_record_t ap_info = {};
    if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
        const int level = std::max(1, wifi_signal_strength_from_rssi(ap_info.rssi));
        if (connected != nullptr) {
            *connected = true;
        }
        s_wifi_connected.store(true);
        s_wifi_signal_level.store(level);
        return level;
    }

    if (connected != nullptr) {
        *connected = false;
    }
    s_wifi_connected.store(false);
    s_wifi_signal_level.store(0);
    return 0;
}

static void update_status_bar(void)
{
    if (s_status_bar == nullptr) {
        return;
    }

    time_t now = 0;
    struct tm timeinfo = {};
    time(&now);
    localtime_r(&now, &timeinfo);

    bool driver_connected = false;
    int wifi_level = get_wifi_level_from_driver(&driver_connected);
    if (!driver_connected && s_wifi_connected.load()) {
        wifi_level = std::max(1, s_wifi_signal_level.load());
    }

    if (bsp_display_lock(0)) {
        s_status_bar->setClock(timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_hour >= 12);
        s_status_bar->setWifiIconState(wifi_level);
        s_status_bar->setBatteryPercent(false, 100);
        bsp_display_unlock();
    }
}

static void status_refresh_task(void *arg)
{
    (void)arg;

    while (true) {
        update_status_bar();
        vTaskDelay(kStatusRefreshPeriod);
    }
}

} // namespace

namespace system_ui_service {

bool initialize(ESP_Brookesia_Phone &phone)
{
    if (s_initialized.load()) {
        return true;
    }

    s_status_bar = phone.getHome().getStatusBar();
    if (s_status_bar == nullptr) {
        ESP_LOGW(TAG, "Status bar is unavailable");
        return false;
    }

    if (xTaskCreatePinnedToCore(status_refresh_task,
                                "status_refresh",
                                kStatusRefreshTaskStack,
                                nullptr,
                                1,
                                nullptr,
                                1) != pdPASS) {
        ESP_LOGE(TAG, "Failed to start status refresh task");
        return false;
    }

    s_initialized.store(true);
    update_status_bar();
    return true;
}

void set_wifi_connected(bool connected)
{
    s_wifi_connected.store(connected);
    if (!connected) {
        s_wifi_signal_level.store(0);
    }
    update_status_bar();
}

void refresh_wifi_from_driver(void)
{
    get_wifi_level_from_driver(nullptr);
    update_status_bar();
}

} // namespace system_ui_service
