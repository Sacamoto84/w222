/**
 * @file lv_calendar.h
 *
 */

#ifndef LV_CALENDAR_H
#define LV_CALENDAR_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../buttonmatrix/lv_buttonmatrix.h"

#if LV_USE_CALENDAR

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Представляет дату в объекте календаря (независимо от платформы).
 */
typedef struct {
    uint16_t year;
    uint8_t month;  /**< 1..12 */
    uint8_t day;    /**< 1..31 */
} lv_calendar_date_t;

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_calendar_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создайте виджет календаря
 * @param parent    pointer to an object, it will be the parent of the new calendar
 * @return          pointer the created calendar
 */
lv_obj_t * lv_calendar_create(lv_obj_t * parent);

/*======================
 * Добавить/удалить функции
 *=====================*/

/*=====================
 * Функции установки
 *====================*/

/**
 * Установите текущий год, месяц и день одновременно
 * @param obj  pointer to a calendar object
 * @param year      today's year
 * @param month     today's month [1..12]
 * @param day       today's day [1..31]
 */
void lv_calendar_set_today_date(lv_obj_t * obj, uint32_t year, uint32_t month, uint32_t day);

/**
 * Установите текущий год
 * @param obj  pointer to a calendar object
 * @param year      today's year
 */
void lv_calendar_set_today_year(lv_obj_t * obj, uint32_t year);

/**
 * Установите текущий год
 * @param obj  pointer to a calendar object
 * @param month     today's month [1..12]
 */
void lv_calendar_set_today_month(lv_obj_t * obj, uint32_t month);

/**
 * Установите текущий год
 * @param obj  pointer to a calendar object
 * @param day       today's day [1..31]
 */
void lv_calendar_set_today_day(lv_obj_t * obj, uint32_t day);

/**
 * Установите текущий отображаемый год и месяц одновременно
 * @param obj           pointer to a calendar object
 * @param year          shown year
 * @param month         shown month [1..12]
 */
void lv_calendar_set_month_shown(lv_obj_t * obj, uint32_t year, uint32_t month);

/**
 * Установите текущий отображаемый год
 * @param obj           pointer to a calendar object
 * @param year          shown year
 */
void lv_calendar_set_shown_year(lv_obj_t * obj, uint32_t year);

/**
 * Установите отображаемый в данный момент месяц
 * @param obj           pointer to a calendar object
 * @param month         shown month [1..12]
 */
void lv_calendar_set_shown_month(lv_obj_t * obj, uint32_t month);

/**
 * Установите выделенные даты
 * @param obj           pointer to a calendar object
 * @param highlighted   pointer to an `lv_calendar_date_t` array containing the dates.
 *                      Будет сохранен только указатель, поэтому эта переменная не может быть локальной и позже будет уничтожена.
 * @param date_num number of dates in the array
 */
void lv_calendar_set_highlighted_dates(lv_obj_t * obj, lv_calendar_date_t highlighted[], size_t date_num);

/**
 * Установите название дней
 * @param obj           pointer to a calendar object
 * @param day_names     pointer to an array with the names.
 *                      например  `const char * days[7] = {"Sun", "Mon", ...}`
 *                      Будет сохранен только указатель, поэтому эта переменная не может быть локальной и позже будет уничтожена.
 */
void lv_calendar_set_day_names(lv_obj_t * obj, const char ** day_names);

/*=====================
 * Геттерные функции
 *====================*/

/**
 * Получите объект матрицы кнопок календаря.
 * Он показывает даты и названия дней.
 * @param obj       pointer to a calendar object
 * @return          pointer to a the button matrix
 */
lv_obj_t * lv_calendar_get_btnmatrix(const lv_obj_t * obj);

/**
 * Получить сегодняшнюю дату
 * @param calendar  pointer to a calendar object
 * @return          return pointer to an `lv_calendar_date_t` variable containing the date of today.
 */
const lv_calendar_date_t * lv_calendar_get_today_date(const lv_obj_t * calendar);

/**
 * Получить текущий показ
 * @param calendar  pointer to a calendar object
 * @return          pointer to an `lv_calendar_date_t` variable containing the date is being shown.
 */
const lv_calendar_date_t * lv_calendar_get_showed_date(const lv_obj_t * calendar);

/**
 * Получить выделенные даты
 * @param calendar  pointer to a calendar object
 * @return          pointer to an `lv_calendar_date_t` array containing the dates.
 */
lv_calendar_date_t * lv_calendar_get_highlighted_dates(const lv_obj_t * calendar);

/**
 * Получить количество выделенных дат
 * @param calendar  pointer to a calendar object
 * @return          number of highlighted days
 */
size_t lv_calendar_get_highlighted_dates_num(const lv_obj_t * calendar);

/**
 * Получить текущий нажатый день
 * @param calendar  pointer to a calendar object
 * @param date      store the pressed date here
 * @return          LV_RESULT_OK: there is a valid pressed date
 *                  LV_RESULT_INVALID: there is no pressed data
 */
lv_result_t lv_calendar_get_pressed_date(const lv_obj_t * calendar, lv_calendar_date_t * date);

/*=====================
 * Другие функции
 *====================*/

/**********************
 *      MACROS
 **********************/

#include "lv_calendar_header_arrow.h"
#include "lv_calendar_header_dropdown.h"
#include "lv_calendar_chinese.h"

#endif  /*LV_USE_CALENDAR*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_CALENDAR_H*/
