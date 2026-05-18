/**
 * @file lv_imagebutton.h
 *
 */

#ifndef LV_IMAGEBUTTON_H
#define LV_IMAGEBUTTON_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../core/lv_obj.h"

#if LV_USE_IMAGEBUTTON != 0

/*********************
 *      DEFINES
 *********************/
typedef enum {
    LV_IMAGEBUTTON_STATE_RELEASED,
    LV_IMAGEBUTTON_STATE_PRESSED,
    LV_IMAGEBUTTON_STATE_DISABLED,
    LV_IMAGEBUTTON_STATE_CHECKED_RELEASED,
    LV_IMAGEBUTTON_STATE_CHECKED_PRESSED,
    LV_IMAGEBUTTON_STATE_CHECKED_DISABLED,
    LV_IMAGEBUTTON_STATE_NUM,
} lv_imagebutton_state_t;

/**********************
 *      TYPEDEFS
 **********************/
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_imagebutton_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создайте объект кнопки изображения
 * @param parent указатель на объект, он будет родителем кнопки нового изображения
 * @return указатель на кнопку созданного изображения
 */
lv_obj_t * lv_imagebutton_create(lv_obj_t * parent);

/*======================
 * Добавить/удалить функции
 *=====================*/

/*=====================
 * Функции установки
 *====================*/

/**
 * Установка изображений для состояния кнопки изображения
 * @param obj           указатель на объект кнопки изображения
 * @param state         для какого штата установлено новое изображение
 * @param src_left      указатель на источник изображения для левой стороны кнопки (массив C или путь к
 * файл)
 * @param src_mid       указатель на источник изображения для середины кнопки (в идеале шириной 1 пиксель) (C
 * массив или путь к файлу)
 * @param src_right     указатель на источник изображения для правой стороны кнопки (массив C или путь
 * в файл)
 */
void lv_imagebutton_set_src(lv_obj_t * obj, lv_imagebutton_state_t state, const void * src_left,
                            const void * src_mid,
                            const void * src_right);

/**
 * Установите левое изображение для состояния кнопки изображения.
 * @param obj           указатель на объект кнопки изображения
 * @param state         для какого штата установлено новое изображение
 * @param src_left      указатель на источник изображения для левой стороны кнопки
 *                      (массив C или путь к файлу)
 */
void lv_imagebutton_set_src_left(lv_obj_t * obj, lv_imagebutton_state_t state, const void * src_left);

/**
 * Установите правильное изображение для состояния кнопки изображения.
 * @param obj           указатель на объект кнопки изображения
 * @param state         для какого штата установлено новое изображение
 * @param src_right      указатель на источник изображения для правой стороны кнопки
 *                      (массив C или путь к файлу)
 */
void lv_imagebutton_set_src_right(lv_obj_t * obj, lv_imagebutton_state_t state, const void * src_right);

/**
 * Установите среднее изображение для состояния кнопки изображения.
 * @param obj           указатель на объект кнопки изображения
 * @param state         для какого штата установлено новое изображение
 * @param src_mid       указатель на источник изображения для середины кнопки
 *                      (массив C или путь к файлу)
 */
void lv_imagebutton_set_src_mid(lv_obj_t * obj, lv_imagebutton_state_t state, const void * src_mid);

/**
 * Используйте эту функцию вместо `lv_obj_add/remove_state`, чтобы установить состояние вручную.
 * @param obj           указатель на объект кнопки изображения
 * @param state         новое государство
 */
void lv_imagebutton_set_state(lv_obj_t * obj, lv_imagebutton_state_t state);

/*=====================
 * Геттерные функции
 *====================*/

/**
 * Получить левое изображение в заданном состоянии
 * @param obj           указатель на объект кнопки изображения
 * @param state         состояние, где получить изображение (из `lv_button_state_t`) `
 * @return              указатель на левый источник изображения (массив C или путь к файлу)
 */
const void * lv_imagebutton_get_src_left(lv_obj_t * obj, lv_imagebutton_state_t state);

/**
 * Получить среднее изображение в заданном состоянии
 * @param obj           указатель на объект кнопки изображения
 * @param state         состояние, где получить изображение (из `lv_button_state_t`) `
 * @return              указатель на средний источник изображения (массив C или путь к файлу)
 */
const void * lv_imagebutton_get_src_middle(lv_obj_t * obj, lv_imagebutton_state_t state);

/**
 * Получить правильное изображение в заданном состоянии
 * @param obj           указатель на объект кнопки изображения
 * @param state         состояние, где получить изображение (из `lv_button_state_t`) `
 * @return              указатель на левый источник изображения (массив C или путь к файлу)
 */
const void * lv_imagebutton_get_src_right(lv_obj_t * obj, lv_imagebutton_state_t state);

/*=====================
 * Другие функции
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_IMAGEBUTTON*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_IMAGEBUTTON_H*/
