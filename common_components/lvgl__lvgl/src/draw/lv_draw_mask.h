/**
 * @file lv_draw_mask.h
 *
 */

#ifndef LV_DRAW_MASK_H
#define LV_DRAW_MASK_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw.h"
#include "../misc/lv_color.h"
#include "../misc/lv_area.h"
#include "../misc/lv_style.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_draw_mask_rect_dsc_t {
    lv_draw_dsc_base_t base;

    /**The area to mask.*/
    lv_area_t area;

    /**The radius of masking*/
    int32_t radius;

    /**0: очистите содержимое `area`.
     * 1: не трогайте область за пределами `area`*/
    uint32_t keep_outside : 1;
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Инициализируйте дескриптор рисования прямоугольной маски.
 * @param dsc       указатель на дескриптор отрисовки
 */
void /* LV_ATTRIBUTE_FAST_MEM */ lv_draw_mask_rect_dsc_init(lv_draw_mask_rect_dsc_t * dsc);

/**
 * Попробуйте получить дескриптор отрисовки прямоугольной маски из задачи рисования.
 * @param task      нарисовать задачу
 * @return          дескриптор отрисовки задачи или NULL, если задача не относится к типу LV_DRAW_TASK_TYPE_MASK_RECTANGLE
 */
lv_draw_mask_rect_dsc_t * lv_draw_task_get_mask_rect_dsc(lv_draw_task_t * task);

/**
 * Создайте задачу рисования, чтобы замаскировать прямоугольник из буфера.
 * @param layer     указатель на слой
 * @param dsc       указатель на дескриптор отрисовки
 */
void lv_draw_mask_rect(lv_layer_t * layer, const lv_draw_mask_rect_dsc_t * dsc);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_MASK_H*/
