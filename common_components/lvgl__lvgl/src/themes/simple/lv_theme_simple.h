/**
 * @file lv_theme_simple.h
 *
 */

#ifndef LV_THEME_SIMPLE_H
#define LV_THEME_SIMPLE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_theme.h"
#include "../../display/lv_display.h"

#if LV_USE_THEME_SIMPLE

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
 * Инициализировать тему
 * @param disp указатель для отображения
 * @return указатель для ссылки на эту тему позже
 */
lv_theme_t * lv_theme_simple_init(lv_display_t * disp);

/**
* Проверьте, инициализирована ли тема
* @return true, если инициализирована тема по умолчанию, в противном случае — false
*/
bool lv_theme_simple_is_inited(void);

/**
 * Получить простую тему
 * @return указатель на простую тему или NULL, если она не инициализирована
 */
lv_theme_t * lv_theme_simple_get(void);

/**
 * Деинициализировать простую тему
 */
void lv_theme_simple_deinit(void);

/**********************
 *      MACROS
 **********************/

#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_THEME_SIMPLE_H*/
