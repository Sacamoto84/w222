/**
 * @file lv_tileview.h
 *
 */

#ifndef LV_TILEVIEW_H
#define LV_TILEVIEW_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../core/lv_obj.h"

#if LV_USE_TILEVIEW

/*********************
 *      DEFINES
 *********************/

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_tileview_class;
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_tileview_tile_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создать объект тайлового просмотра
 * @param parent      указатель на объект, он будет родителем нового представления плитки
 * @return            указатель на созданный тайлвью
 */
lv_obj_t * lv_tileview_create(lv_obj_t * parent);

/**
 * Добавить плитку в представление плитки
 * @param tv          указатель на объект тайлового просмотра
 * @param col_id      идентификатор столбца плитки
 * @param row_id      идентификатор строки плитки
 * @param dir         направление перехода к следующей плитке
 * @return            указатель на добавленный объект плитки
 */
lv_obj_t * lv_tileview_add_tile(lv_obj_t * tv, uint8_t col_id, uint8_t row_id, lv_dir_t dir);

/**
 * Установите активную плитку в режиме просмотра плиток.
 * @param tv      указатель на объект тайлового просмотра
 * @param tile_obj    указатель на объект плитки, который будет установлен как активный
 * @param anim_en     флаг включения анимации (LV_ANIM_ONилиLV_ANIM_OFF)
 */
void lv_tileview_set_tile(lv_obj_t * tv, lv_obj_t * tile_obj, lv_anim_enable_t anim_en);

/**
 * Установить активную плитку по индексу в представлении плитки
 * @param tv          указатель на объект тайлового просмотра
 * @param col_id      идентификатор столбца плитки, которую нужно сделать активной
 * @param row_id      идентификатор строки плитки, которую нужно сделать активной
 * @param anim_en     флаг включения анимации (LV_ANIM_ONилиLV_ANIM_OFF)
 */
void lv_tileview_set_tile_by_index(lv_obj_t * tv, uint32_t col_id, uint32_t row_id, lv_anim_enable_t anim_en);

/**
 * Получить активную в данный момент плитку в представлении плитки
 * @param obj         указатель на объект тайлового просмотра
 * @return            указатель на текущий активный объект плитки
 */
lv_obj_t * lv_tileview_get_tile_active(lv_obj_t * obj);

/*=====================
 * Другие функции
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_TILEVIEW*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_TILEVIEW_H*/
