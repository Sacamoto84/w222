/**
 * @file lv_spinner.h
 *
 */

#ifndef LV_SPINNER_H
#define LV_SPINNER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_SPINNER

#include "../../misc/lv_types.h"
#include "../../core/lv_obj_property.h"

/*Тестирование зависимостей*/
#if LV_USE_ARC == 0
#error "lv_spinner: lv_arc is required. Enable it in lv_conf.h (LV_USE_ARC  1) "
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_spinner_class;

#if LV_USE_OBJ_PROPERTY
enum _lv_property_spinner_id_t {
    LV_PROPERTY_ID(SPINNER, ANIM_DURATION, LV_PROPERTY_TYPE_INT, 0),
    LV_PROPERTY_ID(SPINNER, ARC_SWEEP,     LV_PROPERTY_TYPE_INT, 1),
    LV_PROPERTY_SPINNER_END,
};
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создайте виджет-спиннер
 * @param parent    указатель на объект, он будет родителем нового счетчика.
 * @return          созданный спиннер
 */
lv_obj_t * lv_spinner_create(lv_obj_t * parent);

/**
 * Установите время анимации и длину дуги спиннера.
 * Анимация подходит для значений угла от 180 до 360.
 * @param obj       указатель на счетчик
 * @param t         время анимации в миллисекундах
 * @param angle     угол дуги в градусах
 */
void lv_spinner_set_anim_params(lv_obj_t * obj, uint32_t t, uint32_t angle);

/**
 * Установите время анимации спиннера
 * @param obj       указатель на счетчик
 * @param t         время анимации в миллисекундах
 */
void lv_spinner_set_anim_duration(lv_obj_t * obj, uint32_t t);

/**
 * Установите длину дуги анимации спиннера.
 * Анимация подходит для значений от 180 до 360.
 * @param obj       указатель на счетчик
 * @param angle     угол дуги в градусах
 */
void lv_spinner_set_arc_sweep(lv_obj_t * obj, uint32_t angle);

/**
 * Получить продолжительность анимации счетчика
 * @param obj       указатель на счетчик
 * @return          время анимации в миллисекундах
 */
uint32_t lv_spinner_get_anim_duration(lv_obj_t * obj);

/**
 * Получить длину дуги анимации спиннера
 * @param obj       указатель на счетчик
 * @return          угол дуги в градусах
 */
uint32_t lv_spinner_get_arc_sweep(lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SPINNER*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_SPINNER_H*/
