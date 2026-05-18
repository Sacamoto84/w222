/**
 * @file lv_opengles_window.h
 *
 */

#ifndef LV_OPENGLES_WINDOW_H
#define LV_OPENGLES_WINDOW_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"
#if LV_USE_OPENGLES

#include "../../misc/lv_types.h"

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
 * Удалите окно OpenGL. Если это последний, процесс завершится
 * @param window    OpenGL window to delete
 */
void lv_opengles_window_delete(lv_opengles_window_t * window);

/**
 * Добавьте текстуру в окно OpenGL. Это может быть текстура дисплея LVGL или любая текстура OpenGL.
 * @param window        OpenGL window
 * @param texture_id    OpenGL texture ID
 * @param w             width in pixels of the texture
 * @param h             height in pixels of the texture
 * @return              the new texture handle
 */
lv_opengles_window_texture_t * lv_opengles_window_add_texture(lv_opengles_window_t * window, unsigned int texture_id,
                                                              int32_t w, int32_t h);

lv_display_t * lv_opengles_window_display_create(lv_opengles_window_t * window, int32_t w, int32_t h);

lv_opengles_window_texture_t * lv_opengles_window_display_get_window_texture(lv_display_t * window_display);

/**
 * Удалите текстуру из окна OpenGL и удалите ее.
 * @param texture    handle of an OpenGL window texture
 */
void lv_opengles_window_texture_remove(lv_opengles_window_texture_t * texture);

/**
 * Установите положение x текстуры в ее окне OpenGL.
 * @param texture    handle of an OpenGL window texture
 * @param x          new x position of the texture
 */
void lv_opengles_window_texture_set_x(lv_opengles_window_texture_t * texture, int32_t x);

/**
 * Установите положение текстуры по оси Y в ее окне OpenGL.
 * @param texture    handle of an OpenGL window texture
 * @param y          new y position of the texture
 */
void lv_opengles_window_texture_set_y(lv_opengles_window_texture_t * texture, int32_t y);

/**
 * Установите непрозрачность текстуры в окне OpenGL.
 * @param texture    handle of an OpenGL window texture
 * @param opa        new opacity of the texture
 */
void lv_opengles_window_texture_set_opa(lv_opengles_window_texture_t * texture, lv_opa_t opa);

/**
 * Получите индекс мыши, связанный с текстурой в окне OpenGL, если он существует.
 * @param texture    handle of an OpenGL window texture
 * @return           the indev or `NULL`
 * @note             there will only be an indev if the texture is based on an
 *                   LVGL отображает текстуру и окно создано с помощью
 *                   `use_mouse_indev` как `true`
 */
lv_indev_t * lv_opengles_window_texture_get_mouse_indev(lv_opengles_window_texture_t * texture);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_OPENGLES */

#ifdef __cplusplus
} /* внешний "С" */
#endif

#endif /* LV_OPENGLES_WINDOW_H */
