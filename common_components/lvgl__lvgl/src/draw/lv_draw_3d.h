/**
 * @file lv_draw_3d.h
 *
 */

#ifndef LV_DRAW_3D_H
#define LV_DRAW_3D_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../lv_conf_internal.h"
#if LV_USE_3DTEXTURE

#include "lv_draw.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_draw_dsc_base_t base;
    lv_3dtexture_id_t tex_id;
    bool h_flip;
    bool v_flip;
    lv_opa_t opa;
} lv_draw_3d_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Инициализировать дескриптор трехмерной отрисовки
 * @param dsc       указатель на дескриптор отрисовки
 */
void lv_draw_3d_dsc_init(lv_draw_3d_dsc_t * dsc);

/**
 * Попробуйте получить дескриптор трехмерной отрисовки из задачи рисования.
 * @param task      нарисовать задачу
 * @return          дескриптор отрисовки задачи или NULL, если задача не относится к типу LV_DRAW_TASK_TYPE_3D
 */
lv_draw_3d_dsc_t * lv_draw_task_get_3d_dsc(lv_draw_task_t * task);

/**
 * Создайте задачу 3D-рисования.
 * @param layer     указатель на слой
 * @param dsc       указатель на инициализированную переменную `lv_draw_3d_dsc_t`
 */
void lv_draw_3d(lv_layer_t * layer, const lv_draw_3d_dsc_t * dsc, const lv_area_t * coords);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_3DTEXTURE*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_DRAW_3D_H*/
