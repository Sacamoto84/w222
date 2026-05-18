/**
 * @file lv_sysmon.h
 *
 */

#ifndef LV_SYSMON_H
#define LV_SYSMON_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../misc/lv_timer.h"
#include "../../core/lv_observer.h"

#if LV_USE_SYSMON

#if LV_USE_LABEL == 0
#error "lv_sysmon: lv_label is required. Enable it in lv_conf.h (LV_USE_LABEL  1) "
#endif

#if LV_USE_OBSERVER == 0
#error "lv_observer: lv_observer is required. Enable it in lv_conf.h (LV_USE_OBSERVER  1) "
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создайте новую метку системного монитора.
 * @param disp      создать систему. пн. на системном уровне этого дисплея
 * @return          ярлык создания
 */
lv_obj_t * lv_sysmon_create(lv_display_t * disp);

#if LV_USE_PERF_MONITOR

/**
 * Показать монитор производительности системы: использование CPU и количество FPS
 * @param disp      целевой дисплей, NULL: использовать дисплеи по умолчанию.
 */
void lv_sysmon_show_performance(lv_display_t * disp);

/**
 * Скрыть монитор производительности системы
 * @param disp      целевой дисплей, NULL: используйте значение по умолчанию.
 */
void lv_sysmon_hide_performance(lv_display_t * disp);

/**
 * Дамп данных FPS, записанных между последним и текущим вызовом дампа.
 * @param disp      целевой дисплей, NULL: используйте значение по умолчанию.
 */
void lv_sysmon_performance_dump(lv_display_t * disp);

/**
 * Возобновите монитор производительности системы.
 * @param disp      целевой дисплей, NULL: используйте значение по умолчанию.
 */
void lv_sysmon_performance_resume(lv_display_t * disp);

/**
 * Приостановите монитор производительности системы.
 *
 * @param disp      целевой дисплей, NULL: используйте значение по умолчанию.
 * @note Когда системный монитор остановлен, вы можете использовать`lv_sysmon_dump_performance`для
 * получить информацию о производительности. См. `lv_sysmon_dump_performance`для получения дополнительной информации.
 */
void lv_sysmon_performance_pause(lv_display_t * disp);


#endif /*LV_USE_PERF_MONITOR*/

#if LV_USE_MEM_MONITOR

/**
 * Показать монитор системной памяти: используемая память и фрагментация памяти
 * @param disp      целевой дисплей, NULL: использовать дисплеи по умолчанию.
 */
void lv_sysmon_show_memory(lv_display_t * disp);

/**
 * Скрыть монитор системной памяти
 * @param disp      целевой дисплей, NULL: использовать дисплеи по умолчанию.
 */
void lv_sysmon_hide_memory(lv_display_t * disp);

#endif /*LV_USE_MEM_MONITOR*/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SYSMON*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_SYSMON_H*/
