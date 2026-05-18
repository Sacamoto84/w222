/**
 * @file lv_svg_decoder.h
 *
 */

#ifndef LV_SVG_DECODER_H
#define LV_SVG_DECODER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_SVG

#include "../../draw/lv_image_decoder.h"

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
 * Зарегистрируйте функции декодера SVG в LVGL.
 */
void lv_svg_decoder_init(void);

void lv_svg_decoder_deinit(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SVG*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_SVG_DECODER_H*/
