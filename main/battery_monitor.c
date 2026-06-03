#include "battery_monitor.h"

#include <stdbool.h>

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_log.h"

static const char *TAG = "battery";

/*
 * Делитель напряжения на GPIO53:
 *
 *   +АКБ ── R_TOP(68k) ──┬── R_BOTTOM(100k) ── GND
 *                        │
 *                        └── GPIO53 (ADC2_CH4)
 *
 * Напряжение на средней точке: Vadc = Vbat * R_BOTTOM / (R_TOP + R_BOTTOM)
 * Обратный пересчёт:           Vbat = Vadc * (R_TOP + R_BOTTOM) / R_BOTTOM
 *
 * Для одной банки Li-ion (до 4.2 В) на пине получается ~2.5 В, что укладывается
 * в диапазон ADC с затуханием 12 dB. Если поменяете резисторы местами или
 * номиналы — правьте только эти две константы.
 */
#define BATTERY_DIVIDER_R_TOP_OHMS     68000.0f
#define BATTERY_DIVIDER_R_BOTTOM_OHMS  100000.0f

#define BATTERY_ADC_UNIT      ADC_UNIT_2
#define BATTERY_ADC_CHANNEL   ADC_CHANNEL_4   /* GPIO53 -> ADC2 channel 4 */
#define BATTERY_ADC_ATTEN     ADC_ATTEN_DB_12
#define BATTERY_ADC_BITWIDTH  ADC_BITWIDTH_DEFAULT
#define BATTERY_SAMPLE_COUNT  16

static adc_oneshot_unit_handle_t s_adc_handle = NULL;
static adc_cali_handle_t s_cali_handle = NULL;
static bool s_calibrated = false;

esp_err_t battery_monitor_init(void)
{
    if (s_adc_handle != NULL) {
        return ESP_OK;
    }

    adc_oneshot_unit_init_cfg_t unit_cfg = {
        .unit_id = BATTERY_ADC_UNIT,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    esp_err_t ret = adc_oneshot_new_unit(&unit_cfg, &s_adc_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "adc_oneshot_new_unit failed: %s", esp_err_to_name(ret));
        s_adc_handle = NULL;
        return ret;
    }

    adc_oneshot_chan_cfg_t chan_cfg = {
        .atten = BATTERY_ADC_ATTEN,
        .bitwidth = BATTERY_ADC_BITWIDTH,
    };
    ret = adc_oneshot_config_channel(s_adc_handle, BATTERY_ADC_CHANNEL, &chan_cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "adc_oneshot_config_channel failed: %s", esp_err_to_name(ret));
        adc_oneshot_del_unit(s_adc_handle);
        s_adc_handle = NULL;
        return ret;
    }

    adc_cali_curve_fitting_config_t cali_cfg = {
        .unit_id = BATTERY_ADC_UNIT,
        .chan = BATTERY_ADC_CHANNEL,
        .atten = BATTERY_ADC_ATTEN,
        .bitwidth = BATTERY_ADC_BITWIDTH,
    };
    ret = adc_cali_create_scheme_curve_fitting(&cali_cfg, &s_cali_handle);
    if (ret == ESP_OK) {
        s_calibrated = true;
    } else {
        ESP_LOGW(TAG, "ADC calibration unavailable (%s), using raw scaling", esp_err_to_name(ret));
        s_cali_handle = NULL;
        s_calibrated = false;
    }

    ESP_LOGI(TAG, "Battery monitor ready: GPIO53 (ADC2 ch4), divider %d/%d ohm, calib=%d",
             (int)BATTERY_DIVIDER_R_TOP_OHMS, (int)BATTERY_DIVIDER_R_BOTTOM_OHMS, (int)s_calibrated);
    return ESP_OK;
}

esp_err_t battery_monitor_read_mv(int *out_mv)
{
    if (out_mv == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (s_adc_handle == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    int32_t acc_mv = 0;
    int valid = 0;
    for (int i = 0; i < BATTERY_SAMPLE_COUNT; ++i) {
        int raw = 0;
        if (adc_oneshot_read(s_adc_handle, BATTERY_ADC_CHANNEL, &raw) != ESP_OK) {
            continue;
        }

        int mv = 0;
        if (s_calibrated) {
            if (adc_cali_raw_to_voltage(s_cali_handle, raw, &mv) != ESP_OK) {
                continue;
            }
        } else {
            /* Грубый пересчёт без калибровки: 12 бит, диапазон ~3100 мВ при 12 dB. */
            mv = (raw * 3100) / 4095;
        }

        acc_mv += mv;
        valid++;
    }

    if (valid == 0) {
        return ESP_FAIL;
    }

    const float pin_mv = (float)acc_mv / (float)valid;
    const float ratio = (BATTERY_DIVIDER_R_TOP_OHMS + BATTERY_DIVIDER_R_BOTTOM_OHMS) /
                        BATTERY_DIVIDER_R_BOTTOM_OHMS;
    *out_mv = (int)(pin_mv * ratio + 0.5f);
    return ESP_OK;
}
