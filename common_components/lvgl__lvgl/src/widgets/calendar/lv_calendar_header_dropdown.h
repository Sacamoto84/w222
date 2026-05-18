/**
 * @file lv_calendar_header_dropdown.h
 *
 */

#ifndef LV_CALENDAR_HEADER_DROPDOWN_H
#define LV_CALENDAR_HEADER_DROPDOWN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../core/lv_obj.h"
#if LV_USE_CALENDAR && LV_USE_CALENDAR_HEADER_DROPDOWN

#if LV_USE_DROPDOWN == 0
#error "LV_USE_DROPDOWN needs to be enabled"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_calendar_header_dropdown_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создайте заголовок календаря с раскрывающимися списками для выбора года и месяца.
 * @param parent    указатель на объект календаря.
 * @return          созданный заголовок
 */
lv_obj_t * lv_calendar_add_header_dropdown(lv_obj_t * parent);

/**
 * Устанавливает собственный список календарных лет
 * @param parent        указатель на объект календаря
 * @param years_list    указатель на константный массив символов со списком лет, дополнительную информацию см. вlv_dropdownset_options.
 *                      например `const char *years = "2023\n2022\n2021\n2020\n2019"
 *                      Будет сохранен только указатель, поэтому эта переменная не может быть локальной и позже будет уничтожена.
 */
void lv_calendar_header_dropdown_set_year_list(lv_obj_t * parent, const char * years_list);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_CALENDAR_HEADER_ARROW*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CALENDAR_HEADER_DROPDOWN_H*/
