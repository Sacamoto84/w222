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
 * @param parent    указатель на объект, он будет родительским для нового календаря
 * @return          указатель созданного календаря
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
 * @param obj  указатель на объект календаря
 * @param year      сегодняшний год
 * @param month     сегодняшний месяц [1..12]
 * @param day       сегодняшний день [1..31]
 */
void lv_calendar_set_today_date(lv_obj_t * obj, uint32_t year, uint32_t month, uint32_t day);

/**
 * Установите текущий год
 * @param obj  указатель на объект календаря
 * @param year      сегодняшний год
 */
void lv_calendar_set_today_year(lv_obj_t * obj, uint32_t year);

/**
 * Установите текущий год
 * @param obj  указатель на объект календаря
 * @param month     сегодняшний месяц [1..12]
 */
void lv_calendar_set_today_month(lv_obj_t * obj, uint32_t month);

/**
 * Установите текущий год
 * @param obj  указатель на объект календаря
 * @param day       сегодняшний день [1..31]
 */
void lv_calendar_set_today_day(lv_obj_t * obj, uint32_t day);

/**
 * Установите текущий отображаемый год и месяц одновременно
 * @param obj           указатель на объект календаря
 * @param year          показанный год
 * @param month         показан месяц [1..12]
 */
void lv_calendar_set_month_shown(lv_obj_t * obj, uint32_t year, uint32_t month);

/**
 * Установите текущий отображаемый год
 * @param obj           указатель на объект календаря
 * @param year          показанный год
 */
void lv_calendar_set_shown_year(lv_obj_t * obj, uint32_t year);

/**
 * Установите отображаемый в данный момент месяц
 * @param obj           указатель на объект календаря
 * @param month         показан месяц [1..12]
 */
void lv_calendar_set_shown_month(lv_obj_t * obj, uint32_t month);

/**
 * Установите выделенные даты
 * @param obj           указатель на объект календаря
 * @param highlighted   указатель на массив `lv_calendar_date_t`, содержащий даты.
 *                      Будет сохранен только указатель, поэтому эта переменная не может быть локальной и позже будет уничтожена.
 * @param date_num количество дат в массиве
 */
void lv_calendar_set_highlighted_dates(lv_obj_t * obj, lv_calendar_date_t highlighted[], size_t date_num);

/**
 * Установите название дней
 * @param obj           указатель на объект календаря
 * @param day_names     указатель на массив с именами.
 *                      например `const char * days[7] = {"Sun", "Mon", ...}`
 *                      Будет сохранен только указатель, поэтому эта переменная не может быть локальной и позже будет уничтожена.
 */
void lv_calendar_set_day_names(lv_obj_t * obj, const char ** day_names);

/*=====================
 * Геттерные функции
 *====================*/

/**
 * Получите объект матрицы кнопок календаря.
 * Он показывает даты и названия дней.
 * @param obj       указатель на объект календаря
 * @return          указатель на матрицу кнопок
 */
lv_obj_t * lv_calendar_get_btnmatrix(const lv_obj_t * obj);

/**
 * Получить сегодняшнюю дату
 * @param calendar  указатель на объект календаря
 * @return          вернуть указатель на переменную `lv_calendar_date_t`, содержащую сегодняшнюю дату.
 */
const lv_calendar_date_t * lv_calendar_get_today_date(const lv_obj_t * calendar);

/**
 * Получить текущий показ
 * @param calendar  указатель на объект календаря
 * @return          отображается указатель на переменную `lv_calendar_date_t`, содержащую дату.
 */
const lv_calendar_date_t * lv_calendar_get_showed_date(const lv_obj_t * calendar);

/**
 * Получить выделенные даты
 * @param calendar  указатель на объект календаря
 * @return          указатель на массив `lv_calendar_date_t`, содержащий даты.
 */
lv_calendar_date_t * lv_calendar_get_highlighted_dates(const lv_obj_t * calendar);

/**
 * Получить количество выделенных дат
 * @param calendar  указатель на объект календаря
 * @return          количество выделенных дней
 */
size_t lv_calendar_get_highlighted_dates_num(const lv_obj_t * calendar);

/**
 * Получить текущий нажатый день
 * @param calendar  указатель на объект календаря
 * @param date      сохраните нажатую дату здесь
 * @return          LV_RESULT_OK: есть действительная нажатая дата.
 *                  LV_RESULT_INVALID: нет нажатых данных
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
