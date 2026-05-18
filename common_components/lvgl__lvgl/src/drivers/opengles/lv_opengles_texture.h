/**
 * @file lv_opengles_texture.h
 *
 */

#ifndef LV_OPENGLES_TEXTURE_H
#define LV_OPENGLES_TEXTURE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"
#if LV_USE_OPENGLES

#include "../../display/lv_display.h"

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
 * Создайте дисплей, который сливается с текстурой OpenGL.
 * Если у вас уже есть текстура и вы хотите привязать ее к дисплею,
 *    см. `lv_opengles_texture_create_from_texture_id`
 * @param w    width in pixels of the texture
 * @param h    height in pixels of the texture
 * @return     the new display or NULL on failure
 */
lv_display_t * lv_opengles_texture_create(int32_t w, int32_t h);

/**
 * Создайте отображение, которое будет соответствовать предоставленной текстуре OpenGL.
 * Если у вас нет текстуры для привязки ее к дисплею,
 *    см. `lv_opengles_texture_create`
 * @param w         width in pixels of the texture
 * @param h         height in pixels of the texture
 * @param texture_id    the texture LVGL will render to
 * @return     the new display or NULL on failure
 */
lv_display_t * lv_opengles_texture_create_from_texture_id(int32_t w, int32_t h, unsigned int texture_id);

/**
 * Получите текстуру OpenGL ID дисплея.
 * @param disp    display
 * @return        texture ID
 */
unsigned int lv_opengles_texture_get_texture_id(lv_display_t * disp);

/**
 * Получить отображение текстуры OpenGL, если она связана с ней.
 * @param texture_id   OpenGL texture ID
 * @return             display or `NULL` if there no display with that texture ID
 */
lv_display_t * lv_opengles_texture_get_from_texture_id(unsigned int texture_id);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_OPENGLES */

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_OPENGLES_TEXTURE_H*/
