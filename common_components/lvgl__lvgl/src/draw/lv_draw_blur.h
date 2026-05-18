/**
 * @file lv_draw_blur.h
 *
 */

#ifndef LV_DRAW_BLUR_H
#define LV_DRAW_BLUR_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"
#include "../misc/lv_color.h"
#include "../misc/lv_area.h"
#include "../misc/lv_style.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_draw_dsc_base_t base;

    /**
     * Интенсивность размытия.
     */
    int32_t blur_radius;

    /**
     * Угловой радиус размытой области
     */
    int32_t corner_radius;

    /**
     * Устанавливает, предпочитать ли скорость или точность
     */
    lv_blur_quality_t quality;

} lv_draw_blur_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Инициализировать дескриптор отрисовки размытия
 * @param dsc       указатель на дескриптор отрисовки
 */
void lv_draw_blur_dsc_init(lv_draw_blur_dsc_t * dsc);

/**
 * Попробуйте получить дескриптор отрисовки размытия из задачи рисования.
 * @param task      нарисовать задачу
 * @return          дескриптор отрисовки задачи или NULL, если задача не относится к типу LV_DRAW_TASK_TYPE_BLUR
 */
lv_draw_blur_dsc_t * lv_draw_task_get_blur_dsc(lv_draw_task_t * task);

/**
 * Создание задачи рисования размытия
 * @param layer     указатель на слой
 * @param dsc       указатель на инициализированную переменную `lv_draw_blur_dsc_t`
 * @param coords    координаты персонажа
 */
void lv_draw_blur(lv_layer_t * layer, const lv_draw_blur_dsc_t * dsc, const lv_area_t * coords);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_DRAW_BLUR_H*/
