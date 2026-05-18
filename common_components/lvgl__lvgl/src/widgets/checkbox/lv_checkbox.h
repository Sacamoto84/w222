/**
 * @file lv_checkbox.h
 *
 */

#ifndef LV_CHECKBOX_H
#define LV_CHECKBOX_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#include "../../core/lv_obj.h"

#if LV_USE_CHECKBOX != 0

/*********************
 *      DEFINES
 *********************/

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_checkbox_class;

#if LV_USE_OBJ_PROPERTY
enum _lv_property_checkbox_id_t {
    LV_PROPERTY_ID(CHECKBOX, TEXT, LV_PROPERTY_TYPE_TEXT, 0),
    LV_PROPERTY_CHECKBOX_END,
};
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создайте объект флажка
 * @param parent    pointer to an object, it will be the parent of the new button
 * @return          pointer to the created check box
 */
lv_obj_t * lv_checkbox_create(lv_obj_t * parent);

/*=====================
 * Функции установки
 *====================*/

/**
 * Установите текст флажка.  `txt` будет скопирован и может быть освобожден.
 * после того, как эта функция вернется.
 * @param obj   pointer to a check box
 * @param txt   the text of the check box. NULL to refresh with the current text.
 */
void lv_checkbox_set_text(lv_obj_t * obj, const char * txt);

/**
 * Установите текст флажка.  `txt` не должен быть освобожден в течение жизни
 * этого флажка.
 * @param obj   pointer to a check box
 * @param txt   the text of the check box.
 */
void lv_checkbox_set_text_static(lv_obj_t * obj, const char * txt);

/*=====================
 * Геттерные функции
 *====================*/

/**
 * Получить текст флажка
 * @param obj   pointer to check box object
 * @return      pointer to the text of the check box
 */
const char * lv_checkbox_get_text(const lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_CHECKBOX*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_CHECKBOX_H*/
