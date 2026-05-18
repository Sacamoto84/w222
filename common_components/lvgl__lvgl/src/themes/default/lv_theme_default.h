/**
 * @file lv_theme_default.h
 *
 */

#ifndef LV_THEME_DEFAULT_H
#define LV_THEME_DEFAULT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_theme.h"

#if LV_USE_THEME_DEFAULT

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
 * @param color_primary основной цвет темы
 * @param color_secondary дополнительный цвет темы
 * @param dark
 * @param font указатель на шрифт, который нужно использовать.
 * @return указатель для ссылки на эту тему позже
 */
lv_theme_t * lv_theme_default_init(lv_display_t * disp, lv_color_t color_primary, lv_color_t color_secondary, bool dark,
                                   const lv_font_t * font);

/**
 * Проверьте, инициализирована ли тема по умолчанию
 * @return true, если инициализирована тема по умолчанию, в противном случае — false
 */
bool lv_theme_default_is_inited(void);

/**
 * Получить тему по умолчанию
 * @return указатель на тему по умолчанию или NULL, если она не инициализирована
 */
lv_theme_t * lv_theme_default_get(void);

/**
 * Деинициализировать тему по умолчанию
 */
void lv_theme_default_deinit(void);

/**********************
 *      MACROS
 **********************/

#endif

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_THEME_DEFAULT_H*/
