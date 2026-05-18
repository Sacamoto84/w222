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
 * Инициализация OpenGL
 * @note    нет необходимости вызывать это, если вы используете `lv_opengles_glfw_window_create`
 */
void lv_opengles_init(void);

/**
 * Деинициализировать OpenGL
 * @note    нет необходимости вызывать это, если вы используете `lv_opengles_glfw_window_create`
 */
void lv_opengles_deinit(void);

/**
 * Рендеринг текстур с использованием альтернативного режима наложения для более плавных полупрозрачных материалов и с плавным плавлением элементов glTF при использовании прозрачного фонаря.
 * @param texture        Текстура OpenGL ID
 * @param texture_area   область в окне для рендеринга текстуры
 * @param opa            непрозрачность, чтобы смешать текстуру с существующим содержимым
 * @param disp_w         ширина окна/кадрового буфера, в который отображается
 * @param disp_h         высота отображаемого окна/фреймбуфера
 * @param h_flip         горизонтальный флип
 * @param v_flip         вертикальный флип
 */
void lv_opengles_render_texture(unsigned int texture, const lv_area_t * texture_area, lv_opa_t opa, int32_t disp_w,
                                int32_t disp_h, const lv_area_t * texture_clip_area, bool h_flip, bool v_flip);

/**
 * Рендеринг текстуры дисплея. Поддержка вращения. Переключение красного и синего каналов.
 * @param display           LVGL Отображение текстур. Создано с помощью модуля `lv_opengles_texture`.
 * @param h_flip            горизонтальный флип
 * @param v_flip            вертикальный флип
 */
void lv_opengles_render_display_texture(lv_display_t * display, bool h_flip, bool v_flip);

/**
 * Рендеринг заливки
 * @param color          цвет заливки
 * @param area           область в окне для визуализации заливки
 * @param opa            непрозрачность, чтобы смешать заливку с существующим содержимым
 * @param disp_w         ширина окна/кадрового буфера, в который отображается
 * @param disp_h         высота отображаемого окна/фреймбуфера
 */
void lv_opengles_render_fill(lv_color_t color, const lv_area_t * area, lv_opa_t opa, int32_t disp_w, int32_t disp_h);

/**
 * Очистить окно/дисплей
 */
void lv_opengles_render_clear(void);

/**
 * Выбрать область просмотра OpenGL
 * @param x        положение x области просмотра
 * @param y        положение окна просмотра по оси y
 * @param w        ширина области просмотра
 * @param h        высота видового экрана
 */
void lv_opengles_viewport(int32_t x, int32_t y, int32_t w, int32_t h);

/**
 * Повторно создавать состояние OpenGL после смены операцийGL(например, NanoVG)
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
