/**
 * @file lv_draw_eve.h
 *
 */

/*  Создано: 3 декабря 2023 г.
 *      Author: juanj
 *
 *  Изменено LVGL
 */

#ifndef LV_DRAW_EVE_H
#define LV_DRAW_EVE_H


#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"
#if LV_USE_DRAW_EVE

#include "lv_draw_eve_target.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_eve_init(void);

void lv_draw_eve_set_display_data(lv_display_t * disp, const lv_draw_eve_parameters_t * params,
                                  lv_draw_eve_operation_cb_t op_cb);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_EVE*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif


#endif /* LV_DRAW_EVE_H */
