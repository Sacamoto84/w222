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
 * Удалите окно OpenGL. Если это последнее, процесс завершится
 * @param window    Окно OpenGL, которое нужно удалить
 */
void lv_opengles_window_delete(lv_opengles_window_t * window);

/**
 * Добавьте текстуру в окно OpenGL. Это может быть текстура дисплеяLVGLили любая текстура OpenGL.
 * @param window        окно OpenGL
 * @param texture_id    Текстура OpenGL ID
 * @param w             ширина текстуры в пикселях
 * @param h             высота текстуры в пикселях
 * @return              новый дескриптор текстуры
 */
lv_opengles_window_texture_t * lv_opengles_window_add_texture(lv_opengles_window_t * window, unsigned int texture_id,
                                                              int32_t w, int32_t h);

lv_display_t * lv_opengles_window_display_create(lv_opengles_window_t * window, int32_t w, int32_t h);

lv_opengles_window_texture_t * lv_opengles_window_display_get_window_texture(lv_display_t * window_display);

/**
 * Удалите текстуру окна OpenGL и удалите ее.
 * @param texture    дескриптор текстуры окна OpenGL
 */
void lv_opengles_window_texture_remove(lv_opengles_window_texture_t * texture);

/**
 * Установите положение x текстуры в ее окне OpenGL.
 * @param texture    дескриптор текстуры окна OpenGL
 * @param x          новое положение текстуры по оси X
 */
void lv_opengles_window_texture_set_x(lv_opengles_window_texture_t * texture, int32_t x);

/**
 * Установите положение текстуры по оси Y в окне OpenGL.
 * @param texture    дескриптор текстуры окна OpenGL
 * @param y          новое положение текстуры по оси Y
 */
void lv_opengles_window_texture_set_y(lv_opengles_window_texture_t * texture, int32_t y);

/**
 * Настройте непрозрачность текстур в окне OpenGL.
 * @param texture    дескриптор текстуры окна OpenGL
 * @param opa        новая непрозрачность текстуры
 */
void lv_opengles_window_texture_set_opa(lv_opengles_window_texture_t * texture, lv_opa_t opa);

/**
 * Получите индекс мыши, связанный с текстурой в окне OpenGL, если она существует.
 * @param texture    дескриптор текстуры окна OpenGL
 * @return           индев или `NULL`
 * @note             indev будет только в том случае, если текстура основана на
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
