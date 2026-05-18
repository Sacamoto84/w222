/**
 * @file lv_draw_sw_blend.h
 *
 */

#ifndef LV_DRAW_SW_BLEND_H
#define LV_DRAW_SW_BLEND_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_draw_sw_mask.h"
#if LV_USE_DRAW_SW

#include "../../../misc/lv_color.h"
#include "../../../misc/lv_area.h"
#include "../../../misc/lv_style.h"

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
 * Пользовательская функция рисования для рендеринга SW.
 * @param dsc             указатель на задачу рисования
 * @param dsc           указатель на инициализированный дескриптор смешивания
 */
typedef void (*lv_draw_sw_blend_handler_t)(lv_draw_task_t * t, const lv_draw_sw_blend_dsc_t * dsc);

typedef struct {
    lv_color_format_t dest_cf;
    lv_draw_sw_blend_handler_t handler;
} lv_draw_sw_custom_blend_handler_t;

/**
 * Вызовите функцию изменения`layer`.
 * @param t     указатель на единицу рисования
 * @param dsc           указатель на инициализированный дескриптор смешивания
 */
void lv_draw_sw_blend(lv_draw_task_t * t, const lv_draw_sw_blend_dsc_t * dsc);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_SW*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_DRAW_SW_BLEND_H*/
