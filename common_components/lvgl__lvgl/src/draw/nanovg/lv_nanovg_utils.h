/**
 * @file lv_nanovg_utils.h
 *
 */

#ifndef LV_NANOVG_UTILS_H
#define LV_NANOVG_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_DRAW_NANOVG

#include "../../misc/lv_assert.h"
#include "../../misc/lv_matrix.h"
#include "../../misc/lv_color.h"
#include "../../libs/nanovg/nanovg.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_draw_nanovg_unit_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Инициализация утилит NanoVG
 * @param u pointer to the nanovg unit
 */
void lv_nanovg_utils_init(struct _lv_draw_nanovg_unit_t * u);

/**
 * Деинициализация утилит NanoVG
 * @param u pointer to the nanovg unit
 */
void lv_nanovg_utils_deinit(struct _lv_draw_nanovg_unit_t * u);

/**
 * Преобразование матрицы LVGL в преобразование NanoVG (матрица 3x2)
 * @param xform the NanoVG transform array (6 floats)
 * @param matrix the LVGL matrix
 */
static inline void lv_nanovg_matrix_convert(float * xform, const lv_matrix_t * matrix)
{
    LV_ASSERT_NULL(xform);
    LV_ASSERT_NULL(matrix);
    xform[0] = matrix->m[0][0];
    xform[1] = matrix->m[1][0];
    xform[2] = matrix->m[0][1];
    xform[3] = matrix->m[1][1];
    xform[4] = matrix->m[0][2];
    xform[5] = matrix->m[1][2];
}

/**
 * Преобразование цвета LVGL в цвет NanoVG.
 * @param color the LVGL color
 * @param opa the opacity
 * @return the NanoVG color
 */
static inline NVGcolor lv_nanovg_color_convert(lv_color_t color, lv_opa_t opa)
{
    return nvgRGBA(color.red, color.green, color.blue, opa);
}

/**
 * Примените матрицу преобразования к контексту NanoVG
 * @param ctx the NanoVG context
 * @param matrix the transform matrix
 */
void lv_nanovg_transform(NVGcontext * ctx, const lv_matrix_t * matrix);

/**
 * Установите область отсечения
 * @param ctx the NanoVG context
 * @param area the clipping area
 */
void lv_nanovg_set_clip_area(NVGcontext * ctx, const lv_area_t * area);

/**
 * Добавить прямоугольник к пути
 * @param ctx the NanoVG context
 * @param x the x coordinate of the rectangle
 * @param y the y coordinate of the rectangle
 * @param w the width of the rectangle
 * @param h the height of the rectangle
 * @param r the radius of the rectangle (0 for no rounding)
 */
void lv_nanovg_path_append_rect(NVGcontext * ctx, float x, float y, float w, float h, float r);

/**
 * Добавить область к пути
 * @param ctx the NanoVG context
 * @param area the area
 */
void lv_nanovg_path_append_area(NVGcontext * ctx, const lv_area_t * area);

/**
 * Добавить дугу под прямым углом к пути
 * @param ctx the NanoVG context
 * @param start_x the starting x coordinate
 * @param start_y the starting y coordinate
 * @param center_x the center x coordinate
 * @param center_y the center y coordinate
 * @param end_x the ending x coordinate
 * @param end_y the ending y coordinate
 */
void lv_nanovg_path_append_arc_right_angle(NVGcontext * ctx,
                                           float start_x, float start_y,
                                           float center_x, float center_y,
                                           float end_x, float end_y);

/**
 * Добавить дугу к пути
 * @param ctx the NanoVG context
 * @param cx the center x coordinate
 * @param cy the center y coordinate
 * @param radius the radius
 * @param start_angle the starting angle in radians
 * @param sweep the sweep angle in radians
 * @param pie whether to draw a pie slice (connected to center)
 */
void lv_nanovg_path_append_arc(NVGcontext * ctx,
                               float cx, float cy,
                               float radius,
                               float start_angle,
                               float sweep,
                               bool pie);

/**
 * Заполните текущий путь
 * @param ctx the NanoVG context
 * @param winding the winding rule
 * @param composite_operation the blend mode
 * @param color the fill color
 */
void lv_nanovg_fill(NVGcontext * ctx, enum NVGwinding winding, enum NVGcompositeOperation composite_operation,
                    NVGcolor color);

/**
 * Завершить текущий кадр
 * @param u pointer to the nanovg unit
 */
void lv_nanovg_end_frame(struct _lv_draw_nanovg_unit_t * u);

/**
 * Очистите блок NanoVG (например, в конце задания).
 * @param u pointer to the nanovg unit
 */
void lv_nanovg_clean_up(struct _lv_draw_nanovg_unit_t * u);

/**
 * Измените форму глобального буфера изображения
 * @param u pointer to the nanovg unit
 * @param cf the color format
 * @param w the new width
 * @param h the new height
 * @return pointer to the resized draw buffer
 */
lv_draw_buf_t * lv_nanovg_reshape_global_image(struct _lv_draw_nanovg_unit_t * u,
                                               lv_color_format_t cf,
                                               uint32_t w,
                                               uint32_t h);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_DRAW_NANOVG */

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_NANOVG_UTILS_H*/
