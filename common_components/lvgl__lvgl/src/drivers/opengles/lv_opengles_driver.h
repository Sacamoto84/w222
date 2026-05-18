/**
 * @file lv_opengles_driver.h
 *
 */

#ifndef LV_OPENGLES_DRIVER_H
#define LV_OPENGLES_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"
#if LV_USE_OPENGLES

#include "../../misc/lv_area.h"
#include "../../misc/lv_color.h"

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
 * Инициализировать OpenGL
 * @note    it is not necessary to call this if you use `lv_opengles_glfw_window_create`
 */
void lv_opengles_init(void);

/**
 * Деинициализировать OpenGL
 * @note    it is not necessary to call this if you use `lv_opengles_glfw_window_create`
 */
void lv_opengles_deinit(void);

/**
 * Рендеринг текстуры с использованием альтернативного режима наложения для более плавных полупрозрачных материалов и правильного сглаживания элементов glTF при использовании прозрачного фона.
 * @param texture        OpenGL texture ID
 * @param texture_area   the area in the window to render the texture in
 * @param opa            opacity to blend the texture with existing contents
 * @param disp_w         width of the window/framebuffer being rendered to
 * @param disp_h         height of the window/framebuffer being rendered to
 * @param h_flip         horizontal flip
 * @param v_flip         vertical flip
 */
void lv_opengles_render_texture(unsigned int texture, const lv_area_t * texture_area, lv_opa_t opa, int32_t disp_w,
                                int32_t disp_h, const lv_area_t * texture_clip_area, bool h_flip, bool v_flip);

/**
 * Рендеринг текстуры дисплея. Поддержка вращения. Переключение красного и синего каналов.
 * @param display           LVGL Texture display. Created with the `lv_opengles_texture` module
 * @param h_flip            horizontal flip
 * @param v_flip            vertical flip
 */
void lv_opengles_render_display_texture(lv_display_t * display, bool h_flip, bool v_flip);

/**
 * Рендеринг заливки
 * @param color          the color of the fill
 * @param area           the area in the window to render the fill
 * @param opa            opacity to blend the fill with existing contents
 * @param disp_w         width of the window/framebuffer being rendered to
 * @param disp_h         height of the window/framebuffer being rendered to
 */
void lv_opengles_render_fill(lv_color_t color, const lv_area_t * area, lv_opa_t opa, int32_t disp_w, int32_t disp_h);

/**
 * Очистить окно/дисплей
 */
void lv_opengles_render_clear(void);

/**
 * Установите область просмотра OpenGL
 * @param x        x position of the viewport
 * @param y        y position of the viewport
 * @param w        width of the viewport
 * @param h        height of the viewport
 */
void lv_opengles_viewport(int32_t x, int32_t y, int32_t w, int32_t h);

/**
 * Повторно инициализировать состояние OpenGL после внешних операций GL (например, NanoVG)
 * Это перепривязывает VAO , VBO , IBO и сбрасывает атрибуты вершин.
 */
void lv_opengles_reinit_state(void);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_OPENGLES */

#ifdef __cplusplus
} /* внешний "С" */
#endif

#endif /* LV_OPENGLES_DRIVER_H */
