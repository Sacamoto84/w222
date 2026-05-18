/**
 * @file lv_draw_sw_grad.h
 *
 */

#ifndef LV_DRAW_SW_GRAD_H
#define LV_DRAW_SW_GRAD_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../misc/lv_color.h"
#include "../../misc/lv_style.h"

#if LV_USE_DRAW_SW

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_color_t   *  color_map;
    lv_opa_t   *  opa_map;
    uint32_t size;
} lv_draw_sw_grad_calc_t;


/**********************
 *      PROTOTYPES
 **********************/

/** Вычислить цвет в заданном градиенте и дроби
 *  Градиент указан в виртуальном диапазоне [0–255], поэтому эта функция масштабирует виртуальный диапазон до заданного диапазона.
 * @param dsc       The gradient descriptor to use
 * @param range     The range to use in computation.
 * @param frac      The current part used in the range. frac is in [0; range]
 * @param color_out Calculated gradient color
 * @param opa_out   Calculated opacity
 */

void /* LV_ATTRIBUTE_FAST_MEM */ lv_draw_sw_grad_color_calculate(const lv_grad_dsc_t * dsc, int32_t range,
                                                                 int32_t frac, lv_color_t * color_out, lv_opa_t * opa_out);

/** Получить градиентный кеш по заданным параметрам */
lv_draw_sw_grad_calc_t * lv_draw_sw_grad_get(const lv_grad_dsc_t * gradient, int32_t w, int32_t h);

/**
 * Очистите элемент градиента после того, как он был получен с помощью `lv_grad_get_from_cache`.
 * @param grad      pointer to a gradient
 */
void lv_draw_sw_grad_cleanup(lv_draw_sw_grad_calc_t * grad);

#if LV_USE_DRAW_SW_COMPLEX_GRADIENTS


/**
 * Вычислить константы по заданным параметрам, которые используются во время рендеринга.
 * @param dsc      gradient descriptor
 * @param coords    the area where to draw the gradient
 */
void lv_draw_sw_grad_linear_setup(lv_grad_dsc_t * dsc, const lv_area_t * coords);

/**
 * Освободите выделенную память для расчета градиента.
 * @param dsc      gradient descriptor
 */
void lv_draw_sw_grad_linear_cleanup(lv_grad_dsc_t * dsc);

/**
 * Вычислить отрезок линейного градиента
 * @param dsc       gradient descriptor
 * @param xp        starting point x coordinate in gradient space
 * @param yp        starting point y coordinate in gradient space
 * @param width     width of the line segment in pixels
 * @param result    color buffer for the resulting line segment
 */
void /* LV_ATTRIBUTE_FAST_MEM */ lv_draw_sw_grad_linear_get_line(lv_grad_dsc_t * dsc, int32_t xp, int32_t yp,
                                                                 int32_t width,
                                                                 lv_draw_sw_grad_calc_t * result);

/**
 * Вычислить константы по заданным параметрам, которые используются во время рендеринга.
 * @param dsc       gradient descriptor
 * @param coords    the area where to draw the gradient
 */
void lv_draw_sw_grad_radial_setup(lv_grad_dsc_t * dsc, const lv_area_t * coords);

/**
 * Освободите выделенную память для расчета градиента.
 * @param dsc      gradient descriptor
 */
void lv_draw_sw_grad_radial_cleanup(lv_grad_dsc_t * dsc);

/**
 * Вычислить отрезок радиального градиента
 * @param dsc       gradient descriptor
 * @param xp        starting point x coordinate in gradient space
 * @param yp        starting point y coordinate in gradient space
 * @param width     width of the line segment in pixels
 * @param result    color buffer for the resulting line segment
 */
void /* LV_ATTRIBUTE_FAST_MEM */ lv_draw_sw_grad_radial_get_line(lv_grad_dsc_t * dsc, int32_t xp, int32_t yp,
                                                                 int32_t width,
                                                                 lv_draw_sw_grad_calc_t * result);

/**
 * Вычислить константы по заданным параметрам, которые используются во время рендеринга.
 * @param dsc      gradient descriptor
 * @param coords    the area where to draw the gradient
 */
void lv_draw_sw_grad_conical_setup(lv_grad_dsc_t * dsc, const lv_area_t * coords);

/**
 * Освободите выделенную память для расчета градиента.
 * @param dsc      gradient descriptor
 */
void lv_draw_sw_grad_conical_cleanup(lv_grad_dsc_t * dsc);

/**
 * Вычислить отрезок линии конического градиента
 * @param dsc       gradient descriptor
 * @param xp        starting point x coordinate in gradient space
 * @param yp        starting point y coordinate in gradient space
 * @param width     width of the line segment in pixels
 * @param result    color buffer for the resulting line segment
 */
void /* LV_ATTRIBUTE_FAST_MEM */ lv_draw_sw_grad_conical_get_line(lv_grad_dsc_t * dsc, int32_t xp, int32_t yp,
                                                                  int32_t width,
                                                                  lv_draw_sw_grad_calc_t * result);

#endif /*LV_USE_DRAW_SW_COMPLEX_GRADIENTS*/

#endif /*LV_USE_DRAW_SW*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_DRAW_SW_GRAD_H*/
