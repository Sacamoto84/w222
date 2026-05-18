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
 * @param disp pointer to display
 * @param color_primary the primary color of the theme
 * @param color_secondary the secondary color for the theme
 * @param темный
 * @param font pointer to a font to use.
 * @return a pointer to reference this theme later
 */
lv_theme_t * lv_theme_default_init(lv_display_t * disp, lv_color_t color_primary, lv_color_t color_secondary, bool dark,
                                   const lv_font_t * font);

/**
 * Проверьте, инициализирована ли тема по умолчанию
 * @return true if default theme is initialized, false otherwise
 */
bool lv_theme_default_is_inited(void);

/**
 * Получить тему по умолчанию
 * @return a pointer to default theme, or NULL if this is not initialized
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
