/**
 * @file lv_profiler_builtin.h
 *
 */

#ifndef LV_PROFILER_BUILTIN_H
#define LV_PROFILER_BUILTIN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../lv_conf_internal.h"

#if LV_USE_PROFILER && LV_USE_PROFILER_BUILTIN

#include "lv_types.h"

/*********************
 *      DEFINES
 *********************/

#define LV_PROFILER_BUILTIN_BEGIN_TAG(tag)  lv_profiler_builtin_write((tag), 'B')
#define LV_PROFILER_BUILTIN_END_TAG(tag)    lv_profiler_builtin_write((tag), 'E')
#define LV_PROFILER_BUILTIN_BEGIN           LV_PROFILER_BUILTIN_BEGIN_TAG(__func__)
#define LV_PROFILER_BUILTIN_END             LV_PROFILER_BUILTIN_END_TAG(__func__)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * @brief Инициализировать настройку встроенного профилировщика
 * @param config Указатель на структуру конфигурации встроенного профилировщика
 */
void lv_profiler_builtin_config_init(lv_profiler_builtin_config_t * config);

/**
 * @brief Инициализируйте встроенный профилировщик с заданной конфигурацией.
 * @param config Указатель на структуру конфигурации встроенного профилировщика
 */
void lv_profiler_builtin_init(const lv_profiler_builtin_config_t * config);

/**
 * @brief Деинициализировать встроенный профилировщик
 */
void lv_profiler_builtin_uninit(void);

/**
 * @brief Включить или отключить встроенный профилировщик
 * @param enable true, чтобы включить встроенный профилировщик, false, чтобы отключить
 */
void lv_profiler_builtin_set_enable(bool enable);

/**
 * @brief Сбросить данные профилирования на консоль
 */
void lv_profiler_builtin_flush(void);

/**
 * @brief Запишите данные профилирования для функции с данным тегом.
 * @param func Имя профилируемой функции
 * @param tag Тег для связи с данными профилирования функции.
 */
void lv_profiler_builtin_write(const char * func, char tag);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_PROFILER_BUILTIN*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_PROFILER_BUILTIN_H*/
