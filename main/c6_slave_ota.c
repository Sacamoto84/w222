#include "c6_slave_ota.h"

#include <inttypes.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include "esp_check.h"
#include "esp_hosted.h"
#include "esp_hosted_host_fw_ver.h"
#include "esp_hosted_ota.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "c6_ota";

#define C6_OTA_CHUNK_SIZE 1500

static bool firmware_file_exists(const char *path, off_t *size)
{
    struct stat st = {0};
    if (stat(path, &st) != 0 || !S_ISREG(st.st_mode))
    {
        return false;
    }

    if (size != NULL)
    {
        *size = st.st_size;
    }

    return true;
}

static bool c6_version_is_current(void)
{
    esp_hosted_coprocessor_fwver_t ver = {0};
    esp_err_t ret = esp_hosted_get_coprocessor_fwversion(&ver);
    if (ret != ESP_OK)
    {
        ESP_LOGW(TAG, "Cannot read current C6 firmware version: %s", esp_err_to_name(ret));
        return false;
    }

    ESP_LOGI(TAG, "Current C6 ESP-Hosted FW: %" PRIu32 ".%" PRIu32 ".%" PRIu32,
             ver.major1, ver.minor1, ver.patch1);
    ESP_LOGI(TAG, "Host ESP-Hosted FW: %u.%u.%u",
             ESP_HOSTED_VERSION_MAJOR_1, ESP_HOSTED_VERSION_MINOR_1, ESP_HOSTED_VERSION_PATCH_1);

    return ver.major1 == ESP_HOSTED_VERSION_MAJOR_1 &&
           ver.minor1 == ESP_HOSTED_VERSION_MINOR_1 &&
           ver.patch1 == ESP_HOSTED_VERSION_PATCH_1;
}

static void mark_firmware_as_used(const char *firmware_path)
{
    char done_path[160];
    int written = snprintf(done_path, sizeof(done_path), "%s.done", firmware_path);
    if (written <= 0 || written >= (int)sizeof(done_path))
    {
        ESP_LOGW(TAG, "Firmware path is too long, cannot rename after OTA");
        return;
    }

    unlink(done_path);
    if (rename(firmware_path, done_path) == 0)
    {
        ESP_LOGI(TAG, "Renamed OTA file to %s", done_path);
    }
    else
    {
        ESP_LOGW(TAG, "OTA completed, but failed to rename %s", firmware_path);
    }
}

esp_err_t c6_slave_ota_from_sd(const char *firmware_path)
{
    off_t firmware_size = 0;
    if (!firmware_file_exists(firmware_path, &firmware_size))
    {
        ESP_LOGI(TAG, "No C6 OTA file at %s", firmware_path);
        return ESP_ERR_NOT_FOUND;
    }

    if (c6_version_is_current())
    {
        ESP_LOGI(TAG, "C6 firmware is already current, skipping OTA");
        return ESP_ERR_INVALID_STATE;
    }

    FILE *file = fopen(firmware_path, "rb");
    if (file == NULL)
    {
        ESP_LOGE(TAG, "Failed to open %s", firmware_path);
        return ESP_FAIL;
    }

    uint8_t *chunk = malloc(C6_OTA_CHUNK_SIZE);
    if (chunk == NULL)
    {
        fclose(file);
        ESP_LOGE(TAG, "Failed to allocate OTA chunk buffer");
        return ESP_ERR_NO_MEM;
    }

    ESP_LOGW(TAG, "Starting C6 OTA from %s, size=%" PRIi32 " bytes", firmware_path, (int32_t)firmware_size);

    esp_err_t ret = esp_hosted_slave_ota_begin();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "esp_hosted_slave_ota_begin failed: %s", esp_err_to_name(ret));
        free(chunk);
        fclose(file);
        return ret;
    }

    size_t total_written = 0;
    size_t next_progress = 0;
    while (true)
    {
        size_t bytes_read = fread(chunk, 1, C6_OTA_CHUNK_SIZE, file);
        if (bytes_read == 0)
        {
            if (ferror(file))
            {
                ret = ESP_FAIL;
                ESP_LOGE(TAG, "Failed to read OTA file");
            }
            break;
        }

        ret = esp_hosted_slave_ota_write(chunk, bytes_read);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "esp_hosted_slave_ota_write failed at %u bytes: %s",
                     (unsigned)total_written, esp_err_to_name(ret));
            break;
        }

        total_written += bytes_read;
        if (firmware_size > 0)
        {
            size_t progress = (total_written * 100U) / (size_t)firmware_size;
            if (progress >= next_progress)
            {
                ESP_LOGI(TAG, "C6 OTA progress: %u%%", (unsigned)progress);
                next_progress += 10;
            }
        }
    }

    free(chunk);
    fclose(file);

    if (ret != ESP_OK)
    {
        return ret;
    }

    ret = esp_hosted_slave_ota_end();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "esp_hosted_slave_ota_end failed: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "C6 OTA data transfer completed");

    esp_hosted_coprocessor_fwver_t old_ver = {0};
    bool activate_supported = false;
    if (esp_hosted_get_coprocessor_fwversion(&old_ver) == ESP_OK)
    {
        activate_supported = (old_ver.major1 > 2) || (old_ver.major1 == 2 && old_ver.minor1 > 5);
    }

    if (activate_supported)
    {
        ret = esp_hosted_slave_ota_activate();
        if (ret != ESP_OK)
        {
            ESP_LOGW(TAG, "esp_hosted_slave_ota_activate failed: %s", esp_err_to_name(ret));
        }
        else
        {
            ESP_LOGI(TAG, "C6 OTA firmware activated, C6 will reboot");
        }
    }
    else
    {
        ESP_LOGI(TAG, "C6 activate API is not supported by the old firmware; restart will apply OTA");
    }

    mark_firmware_as_used(firmware_path);

    ESP_LOGW(TAG, "Restarting P4 to resync ESP-Hosted with C6");
    vTaskDelay(pdMS_TO_TICKS(2000));
    esp_restart();

    return ESP_OK;
}
