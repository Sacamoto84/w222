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
 * @param disp указатель для отображения
 * @param dark_bg
 * @param font указатель на шрифт, который нужно использовать.
 * @return указатель для ссылки на эту тему позже
 */
lv_theme_t * lv_theme_mono_init(lv_display_t * disp, bool dark_bg, const lv_font_t * font);

/**
* Проверьте, инициализирована ли тема
* @return true, если инициализирована тема по умолчанию, в противном случае — false
*/
bool lv_theme_mono_is_inited(void);

/**
 * Получить моно тему
 * @return указатель на моно-тему или NULL, если она не инициализирована
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
