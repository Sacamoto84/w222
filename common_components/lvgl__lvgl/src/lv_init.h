/**
 * @file lv_init.h
 *
 */

#ifndef LV_INIT_H
#define LV_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf_internal.h"
#include "misc/lv_types.h"

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
 * Инициализируйте библиотеку LVGL.
 * Должен вызываться перед любой другой функцией, связанной с LVGL.
 */
void lv_init(void);

/**
 * Деинитизировать библиотеку «lv»
 */
void lv_deinit(void);

/**
 * Возвращает, инициализирована ли библиотека «lv» в данный момент.
 */
bool lv_is_initialized(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_INIT_H*/
