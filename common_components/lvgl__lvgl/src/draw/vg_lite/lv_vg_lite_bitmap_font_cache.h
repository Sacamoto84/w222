/**
 * @file lv_vg_lite_bitmap_font_cache.h
 *
 */

#ifndef LV_VG_LITE_BITMAP_FONT_CACHE_H
#define LV_VG_LITE_BITMAP_FONT_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_DRAW_VG_LITE

#include "../../font/lv_font.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_draw_vg_lite_unit_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * @brief Инициализируйте кэш растровых шрифтов для блока рисованияVG-Lite.
 * @param unit указатель на блок рисованияVG-Lite
 * @param cache_cnt количество записей кэша для выделения
 */
void lv_vg_lite_bitmap_font_cache_init(struct _lv_draw_vg_lite_unit_t * unit, uint32_t cache_cnt);

/**
 * @brief Деинициализируйте кэш растровых шрифтов для модуля рисованияVG-Lite.
 * @param unit указатель на блок рисованияVG-Lite
 */
void lv_vg_lite_bitmap_font_cache_deinit(struct _lv_draw_vg_lite_unit_t * unit);

/**
 * @brief Получить запись кэша растровых шрифтов для данного шрифта и буквы.
 * @param unit указатель на блок рисованияVG-Lite
 * @param g_dsc указатель на дескриптор глифа
 * @return указатель на буфер прорисовки, содержащий глиф кэшированного растрового шрифта, или NULL, если глифIDравен 0 или если создание кэша не удалось
 */
lv_draw_buf_t * lv_vg_lite_bitmap_font_cache_get(struct _lv_draw_vg_lite_unit_t * unit,
                                                 const lv_font_glyph_dsc_t * g_dsc);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_VG_LITE*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_VG_LITE_BITMAP_FONT_CACHE_H*/
