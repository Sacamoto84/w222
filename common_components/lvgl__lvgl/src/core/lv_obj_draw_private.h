/**
 * @file lv_obj_draw_private.h
 *
 */

#ifndef LV_OBJ_DRAW_PRIVATE_H
#define LV_OBJ_DRAW_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_obj_draw.h"

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
 * Получите расширенную область прорисовки объекта.
 * @param obj       указатель на объект
 * @return          размер расширенной области рисования вокруг реальных координат
 */
int32_t lv_obj_get_ext_draw_size(const lv_obj_t * obj);

lv_layer_type_t lv_obj_get_layer_type(const lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OBJ_DRAW_PRIVATE_H*/
