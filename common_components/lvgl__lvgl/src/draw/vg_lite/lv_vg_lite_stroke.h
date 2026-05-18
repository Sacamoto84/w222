/**
 * @file lv_vg_lite_stroke.h
 *
 */

#ifndef LV_VG_LITE_STROKE_H
#define LV_VG_LITE_STROKE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_vg_lite_utils.h"

#if LV_USE_DRAW_VG_LITE && LV_USE_VECTOR_GRAPHIC

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_vg_lite_path_t;

struct _lv_draw_vg_lite_unit_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Инициализируйте модуль хода
 * @param unit указатель на единицу
 */
void lv_vg_lite_stroke_init(struct _lv_draw_vg_lite_unit_t * unit, uint32_t cache_cnt);

/**
 * Деинициализировать модуль хода
 * @param unit указатель на единицу
 */
void lv_vg_lite_stroke_deinit(struct _lv_draw_vg_lite_unit_t * unit);

/**
 * Получить запись в кэше штрихов
 * @param unit указатель на единицу
 * @param path указатель пути
 * @param dsc указатель на дескриптор штриха
 * @return указатель на запись кэша штрихов
 */
lv_cache_entry_t * lv_vg_lite_stroke_get(struct _lv_draw_vg_lite_unit_t * unit,
                                         struct _lv_vg_lite_path_t * path,
                                         const lv_vector_stroke_dsc_t * dsc);

/**
 * Получить путь инсульта
 * @param cache_entry указатель на запись кэша штрихов
 * @return указатель пути
 */
struct _lv_vg_lite_path_t * lv_vg_lite_stroke_get_path(lv_cache_entry_t * cache_entry);

/**
 * Удалить запись кэша штрихов
 * @param unit указатель на единицу
 * @param stroke указатель на штрих
 */
void lv_vg_lite_stroke_drop(struct _lv_draw_vg_lite_unit_t * unit, lv_cache_entry_t * cache_entry);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_VG_LITE && LV_USE_VECTOR_GRAPHIC*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_VG_LITE_STROKE_H*/
