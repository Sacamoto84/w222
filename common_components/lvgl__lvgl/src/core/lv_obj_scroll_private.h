/**
 * @file lv_obj_scroll_private.h
 *
 */

#ifndef LV_OBJ_SCROLL_PRIVATE_H
#define LV_OBJ_SCROLL_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_obj_scroll.h"

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
 * Функция низкого уровня для прокрутки по заданным координатам x и y.
 * `LV_EVENT_SCROLL` отправлен.
 * @param obj       указатель на объект для прокрутки
 * @param x         пикселей для горизонтальной прокрутки
 * @param y         пикселей для вертикальной прокрутки
 * @return          `LV_RESULT_INVALID` : объект был удален в`LV_EVENT_SCROLL`;
 *                  `LV_RESULT_OK`: если объект все еще действителен
 */
lv_result_t lv_obj_scroll_by_raw(lv_obj_t * obj, int32_t x, int32_t y);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OBJ_SCROLL_PRIVATE_H*/
