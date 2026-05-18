/**
 * @file lv_bar.h
 *
 */

#ifndef LV_BAR_H
#define LV_BAR_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_BAR != 0

#include "../../core/lv_obj.h"
#include "../../misc/lv_anim.h"
#include "../label/lv_label.h"
#include "../../core/lv_observer.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    LV_BAR_MODE_NORMAL,
    LV_BAR_MODE_SYMMETRICAL,
    LV_BAR_MODE_RANGE
} lv_bar_mode_t;

typedef enum {
    LV_BAR_ORIENTATION_AUTO,
    LV_BAR_ORIENTATION_HORIZONTAL,
    LV_BAR_ORIENTATION_VERTICAL
} lv_bar_orientation_t;

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_bar_class;

#if LV_USE_OBJ_PROPERTY
enum _lv_property_bar_id_t {
    LV_PROPERTY_ID(BAR, VALUE,          LV_PROPERTY_TYPE_INT,   0),
    LV_PROPERTY_ID(BAR, START_VALUE,    LV_PROPERTY_TYPE_INT,   1),
    LV_PROPERTY_ID(BAR, MIN_VALUE,      LV_PROPERTY_TYPE_INT,   2),
    LV_PROPERTY_ID(BAR, MAX_VALUE,      LV_PROPERTY_TYPE_INT,   3),
    LV_PROPERTY_ID(BAR, MODE,           LV_PROPERTY_TYPE_INT,   4),
    LV_PROPERTY_ID(BAR, ORIENTATION,    LV_PROPERTY_TYPE_INT,   5),
    LV_PROPERTY_BAR_END,
};
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создать объект-бар
 * @param parent        указатель на объект, он будет родительским элементом новой панели
 * @return              указатель на созданный бар
 */
lv_obj_t * lv_bar_create(lv_obj_t * parent);

/*=====================
 * Функции установки
 *====================*/

/**
 * Установите новое значение на панели
 * @param obj           указатель на объект панели
 * @param value         новое значение
 * @param anim          LV_ANIM_ON: установите значение с анимацией;  LV_ANIM_OFF: немедленно изменить значение.
 */
void lv_bar_set_value(lv_obj_t * obj, int32_t value, lv_anim_enable_t anim);

/**
 * Установите новое начальное значение на панели
 * @param obj             указатель на объект панели
 * @param start_value     новое начальное значение
 * @param anim            LV_ANIM_ON: установите значение с анимацией;  LV_ANIM_OFF: немедленно изменить значение.
 */
void lv_bar_set_start_value(lv_obj_t * obj, int32_t start_value, lv_anim_enable_t anim);

/**
 * Установите минимальное и максимальное значения бара
 * @param obj       указатель на объект панели
 * @param min       минимальное значение
 * @param max       максимальное значение
 * @note Если min больше max, направление рисования становится противоположным.
 */
void lv_bar_set_range(lv_obj_t * obj, int32_t min, int32_t max);

/**
 * Установить минимальное значение бара
 * @param obj       указатель на объект панели
 * @param min       минимальное значение
 */
void lv_bar_set_min_value(lv_obj_t * obj, int32_t min);

/**
 * Установить максимальное значение бара
 * @param obj       указатель на объект панели
 * @param max       максимальное значение
 */
void lv_bar_set_max_value(lv_obj_t * obj, int32_t max);

/**
 * Установите тип панели.
 * @param obj       указатель на объект панели
 * @param mode      тип стержня от `lv_bar_mode_t`
 */
void lv_bar_set_mode(lv_obj_t * obj, lv_bar_mode_t mode);

/**
 * Установите ориентацию панели.
 * @param obj           указатель на объект панели
 * @param orientation   ориентация стержня от `lv_bar_orientation_t`
 */
void lv_bar_set_orientation(lv_obj_t * obj, lv_bar_orientation_t orientation);

/*=====================
 * Геттерные функции
 *====================*/

/**
 * Получить стоимость бара
 * @param obj       указатель на объект панели
 * @return          стоимость бара
 */
int32_t lv_bar_get_value(const lv_obj_t * obj);

/**
 * Получить начальное значение бара
 * @param obj       указатель на объект панели
 * @return          начальное значение бара
 */
int32_t lv_bar_get_start_value(const lv_obj_t * obj);

/**
 * Получить минимальное значение бара
 * @param obj       указатель на объект панели
 * @return          минимальное значение бара
 */
int32_t lv_bar_get_min_value(const lv_obj_t * obj);

/**
 * Получить максимальное значение бара
 * @param obj       указатель на объект панели
 * @return          максимальное значение бара
 */
int32_t lv_bar_get_max_value(const lv_obj_t * obj);

/**
 * Получите тип бара.
 * @param obj       указатель на объект панели
 * @return          тип стержня от `lv_bar_mode_t`
 */
lv_bar_mode_t lv_bar_get_mode(lv_obj_t * obj);

/**
 * Получите ориентацию бара.
 * @param obj       указатель на объект панели
 * @return          ориентация стержня от `lv_bar_orientation_t`
 */
lv_bar_orientation_t lv_bar_get_orientation(lv_obj_t * obj);

/**
 * Подскажите бар в симметричном режиме или нет
 * @param obj       указатель на объект панели
 * @return          true: в симметричном режиме false: нет
*/
bool lv_bar_is_symmetrical(lv_obj_t * obj);

#if LV_USE_OBSERVER
/**
 * Привязка целого числа или числа с плавающей точкой к значению бара.
 * @param obj       указатель на Бар
 * @param subject   указатель на тему
 * @return          указатель на вновь созданный наблюдатель
 */
lv_observer_t * lv_bar_bind_value(lv_obj_t * obj, lv_subject_t * subject);
#endif

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_BAR*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_BAR_H*/
