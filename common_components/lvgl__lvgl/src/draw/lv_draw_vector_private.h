/**
 * @file lv_draw_vector_private.h
 *
 */

#ifndef LV_DRAW_VECTOR_PRIVATE_H
#define LV_DRAW_VECTOR_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_vector.h"

#if LV_USE_VECTOR_GRAPHIC

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Сохраняет форму пути в виде массивов операций и точек.
 * Например, перейдите к 10;20, затем проведите линию к 30;40 и нарисуйте
 * дуга радиусом 30 и стреловидностью 70°.
 *
 * `lv_vector_path_ctx_t` также необходим для описания того, как заполнять и обводить контур.
 */
struct _lv_vector_path_t {
    lv_vector_path_quality_t quality;
    lv_array_t ops;
    lv_array_t points;
};

struct _lv_vector_gradient_t {
    lv_vector_gradient_style_t style;
    lv_grad_stop_t   stops[LV_GRADIENT_MAX_STOPS];  /**< A gradient stop array */
    uint16_t         stops_count;                   /**< The number of used stops in the array */
    float x1;
    float y1;
    float x2;
    float y2;
    float cx;
    float cy;
    float cr;
    lv_vector_gradient_spread_t spread;
};

struct _lv_vector_fill_dsc_t {
    lv_vector_draw_style_t style;
    lv_color32_t color;
    lv_opa_t opa;
    lv_vector_fill_t fill_rule;
    lv_vector_fill_units_t fill_units;
    lv_draw_image_dsc_t img_dsc;
    lv_vector_gradient_t gradient;
    lv_matrix_t matrix;
};

struct _lv_vector_stroke_dsc_t {
    lv_vector_draw_style_t style;
    lv_color32_t color;
    lv_opa_t opa;
    float width;
    lv_array_t dash_pattern;
    lv_vector_stroke_cap_t cap;
    lv_vector_stroke_join_t join;
    uint16_t miter_limit;
    lv_vector_gradient_t gradient;
    lv_matrix_t matrix;
};

/**
 * Сохраняет, как заполнять, обводить, трансформировать и т. д. заданный путь.
 */
struct _lv_vector_path_ctx_t {
    lv_vector_fill_dsc_t fill_dsc;
    lv_vector_stroke_dsc_t stroke_dsc;
    lv_matrix_t matrix;
    lv_vector_blend_t blend_mode;
    lv_area_t scissor_area;
};

struct _lv_draw_vector_dsc_t {
    lv_draw_dsc_base_t base;

    /** Текущие цвета, непрозрачность, матрица и т. д. для следующей задачи, которая будет добавлена.
     * by */
    lv_vector_path_ctx_t * ctx;

    /**
     * Сохраняйте формы путей и их атрибуты.
     * в списке как`lv_draw_vector_subtask_t`. */
    lv_ll_t * task_list;
};


/**
 * Содержит форму пути и ее атрибуты вместе.
 * Эта задача будет передана в механизм векторного рендеринга.
 * Он используется в`task_list``lv_draw_vector_dsc_t`.
 */
typedef struct {
    lv_vector_path_t * path;
    lv_vector_path_ctx_t ctx;
} lv_draw_vector_subtask_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Это основная функция для отрисовки накопленных векторных задач путем их передачи
 * к обратному вызову векторного рендеринга.
 * При возврате обратного вызова обработанная векторная задача будет уничтожена.
 * @param task_list     указатель на связанный список в `lv_draw_vector_dsc_t`, в котором хранятся
 *                      формы пути и их атрибуты.
 * @param cb            обратный вызов, используемый для итерации задачи
 * @param used_data     пользовательский указатель, который будет передан в обратный вызов
 */
void lv_vector_for_each_destroy_tasks(lv_ll_t * task_list, vector_draw_task_cb cb, void * used_data);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_VECTOR_GRAPHIC */

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_DRAW_VECTOR_PRIVATE_H*/
