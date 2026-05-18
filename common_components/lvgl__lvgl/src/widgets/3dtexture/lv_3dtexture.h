/**
 * @file lv_3dtexture.h
 *
 */

#ifndef LV_3DTEXTURE_H
#define LV_3DTEXTURE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"
#if LV_USE_3DTEXTURE

#include "../../core/lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_3dtexture_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создайте объект 3D-текстуры
 * @param parent    pointer to an object, it will be the parent of the new 3dtexture
 * @return          pointer to the created 3dtexture
 */
lv_obj_t * lv_3dtexture_create(lv_obj_t * parent);

/**
 * Установите исходную текстуру виджета.
 * Размер объекта должен быть установлен вручную.
 * @param obj    the 3dtexture widget
 * @param id     the texture handle from the 3D graphics backend.
 *               То есть текстура `unsigned int` для OpenGL.
 */
void lv_3dtexture_set_src(lv_obj_t * obj, lv_3dtexture_id_t id);

/**
 * Установите поведение виджета при переворачивании.
 * @param obj      the 3dtexture widget
 * @param h_flip   true to flip horizontally.
 * @param v_flip   true to flip vertically.
 */
void lv_3dtexture_set_flip(lv_obj_t * obj, bool h_flip, bool v_flip);

/*======================
 * Добавить/удалить функции
 *=====================*/

/*=====================
 * Функции установки
 *====================*/

/*=====================
 * Геттерные функции
 *====================*/

/*=====================
 * Другие функции
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_3DTEXTURE*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_3DTEXTURE_H*/
