/**
 * @file lv_line.h
 *
 */

#ifndef LV_LINE_H
#define LV_LINE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../core/lv_obj.h"
#if LV_USE_LINE != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_line_class;

#if LV_USE_OBJ_PROPERTY
enum _lv_property_line_id_t {
    LV_PROPERTY_ID(LINE, Y_INVERT, LV_PROPERTY_TYPE_BOOL, 0),
    LV_PROPERTY_LINE_END,
};
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создать объект линии
 * @param parent pointer to an object, it will be the parent of the new line
 * @return pointer to the created line
 */
lv_obj_t * lv_line_create(lv_obj_t * parent);

/*=====================
 * Функции установки
 *====================*/

/**
 * Задайте массив точек. Линейный объект соединит эти точки.
 * @param obj           pointer to a line object
 * @param points        an array of points. Only the address is saved, so the array needs to be alive while the line exists
 * @param point_num     number of points in 'point_a'
 */
void lv_line_set_points(lv_obj_t * obj, const lv_point_precise_t points[], uint32_t point_num);

/**
 * Установите неконстантный массив точек. Идентичен `lv_line_set_points`, за исключением того, что массив можно получить с помощью `lv_line_get_points_mutable`.
 * @param obj           pointer to a line object
 * @param points        a non-const array of points. Only the address is saved, so the array needs to be alive while the line exists.
 * @param point_num     number of points in 'point_a'
 */
void lv_line_set_points_mutable(lv_obj_t * obj, lv_point_precise_t points[], uint32_t point_num);

/**
 * Включите (или отключите) инверсию координаты y.
 * Если этот параметр включен, то y будет вычитаться из высоты объекта.
 * поэтому координата y = 0 будет внизу.
 * @param obj       pointer to a line object
 * @param en        true: enable the y inversion, false:disable the y inversion
 */
void lv_line_set_y_invert(lv_obj_t * obj, bool en);

/*=====================
 * Геттерные функции
 *====================*/

/**
 * Получите указатель на массив точек.
 * @param obj           pointer to a line object
 * @return              const pointer to the array of points
 */
const lv_point_precise_t * lv_line_get_points(lv_obj_t * obj);

/**
 * Получите количество точек в массиве точек.
 * @param obj           pointer to a line object
 * @return              number of points in array of points
 */
uint32_t lv_line_get_point_count(lv_obj_t * obj);

/**
 * Проверьте изменчивость указателя сохраненного массива точек.
 * @param obj           pointer to a line object
 * @return              true: the point array pointer is mutable, false: constant
 */
bool lv_line_is_point_array_mutable(lv_obj_t * obj);

/**
 * Получите указатель на изменяемый массив точек или NULL, если он не является изменяемым.
 * @param obj           pointer to a line object
 * @return              pointer to the array of points. NULL if not mutable.
 */
lv_point_precise_t * lv_line_get_points_mutable(lv_obj_t * obj);

/**
 * Получить атрибут инверсии y
 * @param obj       pointer to a line object
 * @return          true: y inversion is enabled, false: disabled
 */
bool lv_line_get_y_invert(const lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_LINE*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_LINE_H*/
