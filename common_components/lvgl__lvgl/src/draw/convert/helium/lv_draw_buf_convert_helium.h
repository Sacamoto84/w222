/**
 * @file lv_draw_buf_convert_helium.h
 *
 */

#ifndef LV_DRAW_BUF_CONVERT_HELIUM_H
#define LV_DRAW_BUF_CONVERT_HELIUM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../../misc/lv_color.h"
#include "../../lv_draw_buf.h"

#if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_HELIUM

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

#ifndef LV_DRAW_CONVERT_PREMULTIPLY_INDEXED
#define LV_DRAW_CONVERT_PREMULTIPLY_INDEXED(buf) \
    _lv_draw_buf_convert_premultiply_indexed_helium(buf)
#endif

#ifndef LV_DRAW_CONVERT_PREMULTIPLY_ARGB8888
#define LV_DRAW_CONVERT_PREMULTIPLY_ARGB8888(buf) \
    _lv_draw_buf_convert_premultiply_argb8888_helium(buf)
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Преобразование индексированногоdraw_bufв формате с предварительным умножением с оптимизацией, специфичной для гелия.
 * @param buf     указатель на буфер рисования
 */
lv_result_t _lv_draw_buf_convert_premultiply_indexed_helium(lv_draw_buf_t * buf);

/**
 * Преобразование argb8888draw_bufв форме передового умножения с оптимизацией, специфичной для гелия.
 * @param buf     указатель на буфер рисования
 */
lv_result_t _lv_draw_buf_convert_premultiply_argb8888_helium(lv_draw_buf_t * buf);

#endif /*LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_HELIUM*/
#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_DRAW_BUF_CONVERT_HELIUM_H */
