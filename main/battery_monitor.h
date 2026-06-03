#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Измерение напряжения аккумулятора через ADC на GPIO53 (ESP32-P4: ADC2, канал 4).
 * На входе стоит резистивный делитель, см. battery_monitor.c.
 */
esp_err_t battery_monitor_init(void);

/*
 * Возвращает напряжение аккумулятора в милливольтах (уже с учётом делителя).
 * В случае успеха пишет результат в *out_mv и возвращает ESP_OK.
 */
esp_err_t battery_monitor_read_mv(int *out_mv);

#ifdef __cplusplus
}
#endif
