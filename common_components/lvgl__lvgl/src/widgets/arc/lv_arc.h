/**
 * @file lv_arc.h
 *
 */

#ifndef LV_ARC_H
#define LV_ARC_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_ARC != 0

#include "../../core/lv_obj.h"
#include "../../core/lv_observer.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * В каком направлении должен расти показатель.
 */
typedef enum {
    LV_ARC_MODE_NORMAL,      /**< Clock-wise */
    LV_ARC_MODE_SYMMETRICAL, /**< Left/right from the midpoint */
    LV_ARC_MODE_REVERSE      /**< Counterclock-wise */
} lv_arc_mode_t;

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_arc_class;

#if LV_USE_OBJ_PROPERTY
enum _lv_property_arc_id_t {
    LV_PROPERTY_ID(ARC, START_ANGLE,        LV_PROPERTY_TYPE_PRECISE,   0),
    LV_PROPERTY_ID(ARC, END_ANGLE,          LV_PROPERTY_TYPE_PRECISE,   1),
    LV_PROPERTY_ID(ARC, BG_START_ANGLE,     LV_PROPERTY_TYPE_PRECISE,   2),
    LV_PROPERTY_ID(ARC, BG_END_ANGLE,       LV_PROPERTY_TYPE_PRECISE,   3),
    LV_PROPERTY_ID(ARC, ROTATION,           LV_PROPERTY_TYPE_INT,       4),
    LV_PROPERTY_ID(ARC, MODE,               LV_PROPERTY_TYPE_INT,       5),
    LV_PROPERTY_ID(ARC, VALUE,              LV_PROPERTY_TYPE_INT,       6),
    LV_PROPERTY_ID(ARC, MIN_VALUE,          LV_PROPERTY_TYPE_INT,       7),
    LV_PROPERTY_ID(ARC, MAX_VALUE,          LV_PROPERTY_TYPE_INT,       8),
    LV_PROPERTY_ID(ARC, CHANGE_RATE,        LV_PROPERTY_TYPE_INT,       9),
    LV_PROPERTY_ID(ARC, KNOB_OFFSET,        LV_PROPERTY_TYPE_INT,       10),
    LV_PROPERTY_ARC_END,
};
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создайте объект дуги
 * @param parent    указатель на объект, он будет родителем новой дуги
 * @return          указатель на созданную дугу
 */
lv_obj_t * lv_arc_create(lv_obj_t * parent);

/*======================
 * Добавить/удалить функции
 *=====================*/

/*=====================
 * Функции установки
 *====================*/

/**
 * Установите начальный угол дуги. 0 градусов: вправо, 90 вниз и т. д.
 * @param obj       указатель на объект дуги
 * @param start     начальный угол. (если`LV_USE_FLOAT`включен, он также может быть дробным.)
 */
void lv_arc_set_start_angle(lv_obj_t * obj, lv_value_precise_t start);

/**
 * Установите конечный угол дуги. 0 градусов: вправо, 90 вниз и т. д.
 * @param obj       указатель на объект дуги
 * @param end       конечный угол (если`LV_USE_FLOAT`включен, он также может быть дробным.)
 */
void lv_arc_set_end_angle(lv_obj_t * obj, lv_value_precise_t end);

/**
 * Установите начальный и конечный углы
 * @param obj       указатель на объект дуги
 * @param start     начальный угол (если`LV_USE_FLOAT`включен, он также может быть дробным.)
 * @param end       конечный угол (если`LV_USE_FLOAT`включен, он также может быть дробным.)
 */
void lv_arc_set_angles(lv_obj_t * obj, lv_value_precise_t start, lv_value_precise_t end);

/**
 * Установите начальный угол фона дуги. 0 градусов: вправо, 90 вниз и т.д.
 * @param obj       указатель на объект дуги
 * @param start     начальный угол (если`LV_USE_FLOAT`включен, он также может быть дробным.)
 */
void lv_arc_set_bg_start_angle(lv_obj_t * obj, lv_value_precise_t start);

/**
 * Установите начальный угол фона дуги. 0 градусов: вправо, 90 вниз и т. д.
 * @param obj       указатель на объект дуги
 * @param end       конечный угол (если`LV_USE_FLOAT`включен, он также может быть дробным.)
 */
void lv_arc_set_bg_end_angle(lv_obj_t * obj, lv_value_precise_t end);

/**
 * Установите начальный и конечный углы фона дуги.
 * @param obj       указатель на объект дуги
 * @param start     начальный угол (если`LV_USE_FLOAT`включен, он также может быть дробным.)
 * @param end       конечный угол (если`LV_USE_FLOAT`включен, он также может быть дробным.)
 */
void lv_arc_set_bg_angles(lv_obj_t * obj, lv_value_precise_t start, lv_value_precise_t end);

/**
 * Установите вращение для всей дуги
 * @param obj           указатель на объект дуги
 * @param rotation      угол поворота
 */
void lv_arc_set_rotation(lv_obj_t * obj, int32_t rotation);

/**
 * Установите, в каком направлении должен расти индикатор.
 * @param obj       указатель на объект дуги
 * @param type      режим дуги
 */
void lv_arc_set_mode(lv_obj_t * obj, lv_arc_mode_t type);

/**
 * Установите новое значение дуги
 * @param obj       указатель на объект дуги
 * @param value     новое значение
 */
void lv_arc_set_value(lv_obj_t * obj, int32_t value);

/**
 * Установите минимальное и максимальное значения дуги
 * @param obj       указатель на объект дуги
 * @param min       минимальное значение
 * @param max       максимальное значение
 */
void lv_arc_set_range(lv_obj_t * obj, int32_t min, int32_t max);

/**
 * Установите минимальные значения дуги
 * @param obj       указатель на объект дуги
 * @param min       минимальное значение
 */
void lv_arc_set_min_value(lv_obj_t * obj, int32_t min);

/**
 * Установите максимальные значения дуги
 * @param obj       указатель на объект дуги
 * @param max       максимальное значение
 */
void lv_arc_set_max_value(lv_obj_t * obj, int32_t max);

/**
 * Установите скорость изменения, чтобы ограничить скорость, с которой дуга должна достигать нажатой точки.
 * @param obj       указатель на объект дуги
 * @param rate      скорость изменения
 */
void lv_arc_set_change_rate(lv_obj_t * obj, uint32_t rate);

/**
 * Установите угол смещения ручки
 * @param obj       указатель на объект дуги
 * @param offset    смещение ручки от основной дуги в градусах
 */
void lv_arc_set_knob_offset(lv_obj_t * obj, int32_t offset);

/*=====================
 * Геттерные функции
 *====================*/

/**
 * Получите начальный угол дуги.
 * @param obj       указатель на объект дуги
 * @return          начальный угол [0..360] (если`LV_USE_FLOAT`включен, он также может быть дробным.)
 */
lv_value_precise_t lv_arc_get_angle_start(lv_obj_t * obj);

/**
 * Получите конечный угол дуги.
 * @param obj       указатель на объект дуги
 * @return          конечный угол [0..360] (если`LV_USE_FLOAT`включен, он также может быть дробным.)
 */
lv_value_precise_t lv_arc_get_angle_end(lv_obj_t * obj);

/**
 * Получите начальный угол фона дуги.
 * @param obj       указатель на объект дуги
 * @return          начальный угол [0..360] (если`LV_USE_FLOAT`включен, он также может быть дробным.)
 */
lv_value_precise_t lv_arc_get_bg_angle_start(lv_obj_t * obj);

/**
 * Получите конечный угол фона дуги.
 * @param obj       указатель на объект дуги
 * @return          конечный угол [0..360] (если`LV_USE_FLOAT`включен, он также может быть дробным.)
 */
lv_value_precise_t lv_arc_get_bg_angle_end(lv_obj_t * obj);

/**
 * Получить значение дуги
 * @param obj       указатель на объект дуги
 * @return          значение дуги
 */
int32_t lv_arc_get_value(const lv_obj_t * obj);

/**
 * Получить минимальное значение дуги
 * @param obj       указатель на объект дуги
 * @return          минимальное значение дуги
 */
int32_t lv_arc_get_min_value(const lv_obj_t * obj);

/**
 * Получить максимальное значение дуги
 * @param obj       указатель на объект дуги
 * @return          максимальное значение дуги
 */
int32_t lv_arc_get_max_value(const lv_obj_t * obj);

/**
 * Узнайте, является ли дуга типом или нет.
 * @param obj       указатель на объект дуги
 * @return          режим дуги
 */
lv_arc_mode_t lv_arc_get_mode(const lv_obj_t * obj);

/**
 * Получить вращение для всей дуги
 * @param obj       указатель на объект дуги
 * @return          текущее вращение дуги
 */
int32_t lv_arc_get_rotation(const lv_obj_t * obj);

/**
 * Получить текущее смещение угла ручки
 * @param obj       указатель на объект дуги
 * @return          смещение ручки тока дуги
 */
int32_t lv_arc_get_knob_offset(const lv_obj_t * obj);

/**
 * Получить скорость изменения дуги
 * @param obj       указатель на объект дуги
 * @return          скорость изменения
 */
uint32_t lv_arc_get_change_rate(lv_obj_t * obj);

/*=====================
 * Другие функции
 *====================*/

#if LV_USE_OBSERVER
/**
 * Привяжите значения к последствиям Arc.
 * @param obj       указатель на дугу
 * @param subject   указатель на тему
 * @return          указатель на вновь созданный наблюдатель
 */
lv_observer_t * lv_arc_bind_value(lv_obj_t * obj, lv_subject_t * subject);
#endif


/**
 * Выровняйте объект по текущему положению дуги (ручки)
 * @param obj           указатель на объект дуги
 * @param obj_to_align  указатель на объект для выравнивания
 * @param r_offset      считайте радиус большим с этим значением (< 0: для меньшего радиуса)
 */
void lv_arc_align_obj_to_angle(const lv_obj_t * obj, lv_obj_t * obj_to_align, int32_t r_offset);

/**
 * Поворот объекта в текущее положение дуги (ручка)
 * @param obj            указатель на объект дуги
 * @param obj_to_rotate  указатель на объект, который нужно повернуть
 * @param r_offset       считайте радиус большим с этим значением (< 0: для меньшего радиуса)
 */
void lv_arc_rotate_obj_to_angle(const lv_obj_t * obj, lv_obj_t * obj_to_rotate, int32_t r_offset);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_ARC*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_ARC_H*/
