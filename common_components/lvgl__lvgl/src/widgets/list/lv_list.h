/**
 * @file lv_list.h
 *
 */

#ifndef LV_LIST_H
#define LV_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../core/lv_obj.h"

#if LV_USE_LIST

#if LV_USE_FLEX == 0
#error "lv_list: lv_flex is required. Enable it in lv_conf.h (LV_USE_FLEX 1)"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_list_class;
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_list_text_class;
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_list_button_class;
/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создать объект списка
 * @param parent    указатель на объект, он будет родителем нового списка
 * @return          указатель на созданный список
 */
lv_obj_t * lv_list_create(lv_obj_t * parent);

/**
 * Добавить текст в список
 * @param list      указатель на список, он будет родительским элементом новой метки
 * @param txt       текст новой этикетки
 * @return          указатель на созданную метку
 */
lv_obj_t * lv_list_add_text(lv_obj_t * list, const char * txt);

/**
 * Добавить кнопку в список
 * @param list      указатель на список, он будет родителем новой кнопки
 * @param icon      значок для кнопки, когдаNULLу него не будет значка
 * @param txt       текст новой кнопки, приNULLтекст не будет добавлен
 * @return          указатель на созданную кнопку
 */
lv_obj_t * lv_list_add_button(lv_obj_t * list, const void * icon, const char * txt);

/**
 * Получить текст данной кнопки списка
 * @param list      указатель на список
 * @param btn       указатель на кнопку
 * @return          текст кнопки, если у кнопки нет текста, будет возвращен текст ""
 */
const char * lv_list_get_button_text(lv_obj_t * list, lv_obj_t * btn);

/**
 * Установить текст данной кнопки списка
 * @param list      указатель на список
 * @param btn       указатель на кнопку
 * @param txt       указатель на текст
 */
void lv_list_set_button_text(lv_obj_t * list, lv_obj_t * btn, const char * txt);

#if LV_USE_TRANSLATION

/**
 * Добавить текст тега перевода в список
 * @param list      указатель на список, он будет родительским элементом новой метки
 * @param tag       тег перевода новой метки
 * @return          указатель на созданную метку
 */
lv_obj_t * lv_list_add_translation_tag(lv_obj_t * list, const char * tag);

/**
 * Добавить кнопку тега перевода в список
 * @param list      указатель на список, он будет родителем новой кнопки
 * @param icon      значок для кнопки, когдаNULLу него не будет значка
 * @param tag       тег перевода новой кнопки, приNULLтег перевода не будет добавлен
 * @return          указатель на созданную кнопку
 */
lv_obj_t * lv_list_add_button_translation_tag(lv_obj_t * list, const void * icon, const char * tag);

/**
 * Установить текст тега перевода для данной кнопки списка
 * @param list      указатель на список
 * @param btn       указатель на кнопку
 * @param tag       указатель на тег перевода
 */
void lv_list_set_button_translation_tag(lv_obj_t * list, lv_obj_t * btn, const char * tag);

#endif

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_LIST*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_LIST_H*/
