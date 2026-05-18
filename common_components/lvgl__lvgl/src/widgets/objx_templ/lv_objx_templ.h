/**
 * @file lv_objx_templ.h
 *
 */

/**
 * TODO Удалить эти инструкции
 * Поиск и замена: templ -> короткое имя объекта в нижнем регистре (например, btn, label и т. д.)
 *                    TEMPL -> короткое имя объекта в верхнем регистре (например, BTN , LABEL и т. д.)
 *
 */

#ifndef LV_OBJX_TEMPL_H
#define LV_OBJX_TEMPL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#if LV_USE_TEMPL != 0

#include "../core/lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/*Данные шаблона*/
typedef struct {
    lv_ANCESTOR_t ancestor; /*Виджет-предок, например.  Ползунок lv_slider_t*/
    /*Новые данные для этого типа*/
} lv_templ_t;

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_templ_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создать объект templ
 * @param parent    pointer to an object, it will be the parent of the new templ
 * @return          pointer to the created bar
 */
lv_obj_t * lv_templ_create(lv_obj_t * parent);

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

#endif /*LV_USE_TEMPL*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_OBJX_TEMPL_H*/
