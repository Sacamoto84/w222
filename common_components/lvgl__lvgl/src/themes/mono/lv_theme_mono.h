/**
 * @file lv_theme_mono.h
 *
 */

#ifndef LV_THEME_MONO_H
#define LV_THEME_MONO_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_theme.h"

#if LV_USE_THEME_MONO

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
 * @param dark_bg
 * @param font pointer to a font to use.
 * @return a pointer to reference this theme later
 */
lv_theme_t * lv_theme_mono_init(lv_display_t * disp, bool dark_bg, const lv_font_t * font);

/**
* Проверьте, инициализирована ли тема
* @return true if default theme is initialized, false otherwise
*/
bool lv_theme_mono_is_inited(void);

/**
 * Получить моно тему
 * @return a pointer to mono theme, or NULL if this is not initialized
 */
lv_theme_t * lv_theme_mono_get(void);

/**
 * Деинициализировать моно-тему
 */
void lv_theme_mono_deinit(void);

/**********************
 *      MACROS
 **********************/

#endif

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /* LV_THEME_MONO_H */
