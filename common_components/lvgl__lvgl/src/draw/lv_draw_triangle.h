/**
 * @file lv_draw_triangle.h
 *
 */

#ifndef LV_DRAW_TRIANGLE_H
#define LV_DRAW_TRIANGLE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_rect.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_draw_dsc_base_t base;

    /**Points of the triangle. If `LV_USE_FLOAT` is enabled floats can be used here*/
    lv_point_precise_t p[3];

    /**Color of the triangle*/
    lv_color_t color;

    /**Непрозрачность дуги в диапазоне 0...255.
     * LV_OPA_TRANSP , LV_OPA_10 , LV_OPA_20 , .. LV_OPA_COVER также можно использовать*/
    lv_opa_t opa;

    /**Describe a gradient. If `grad.dir` is not `LV_GRAD_DIR_NONE` `color` will be ignored*/
    lv_grad_dsc_t grad;

} lv_draw_triangle_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Инициализировать дескриптор рисования треугольника
 * @param draw_dsc  указатель на дескриптор отрисовки
 */
void lv_draw_triangle_dsc_init(lv_draw_triangle_dsc_t * draw_dsc);

/**
 * Попробуйте получить дескриптор рисования треугольника из задачи рисования.
 * @param task      нарисовать задачу
 * @return          дескриптор отрисовки задачи или NULL, если задача не относится к типу LV_DRAW_TASK_TYPE_TRIANGLE
 */
lv_draw_triangle_dsc_t * lv_draw_task_get_triangle_dsc(lv_draw_task_t * task);

/**
 * Создайте задачу рисования треугольника.
 * @param layer     указатель на слой
 * @param draw_dsc  указатель на инициализированный объект `lv_draw_triangle_dsc_t`
 */
void lv_draw_triangle(lv_layer_t * layer, const lv_draw_triangle_dsc_t * draw_dsc);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_TRIANGLE_H*/
