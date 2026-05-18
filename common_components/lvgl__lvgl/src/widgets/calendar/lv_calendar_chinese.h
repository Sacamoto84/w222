/**
 * @file lv_calendar_chinese.h
 *
 */

#ifndef LV_CALENDAR_CHINESE_H
#define LV_CALENDAR_CHINESE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../core/lv_obj.h"
#include "lv_calendar.h"
#if LV_USE_CALENDAR && LV_USE_CALENDAR_CHINESE

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_calendar_date_t today;
    bool leep_month;
} lv_calendar_chinese_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Включите китайский календарь.
 * @param obj   указатель на объект календаря.
 * @param en    true: включить китайский календарь; ложь: отключить
 */
void lv_calendar_set_chinese_mode(lv_obj_t * obj, bool en);

/**
 * Узнайте название дня
 * @param gregorian   чтобы получить григорианское время для имени
 * @return            вернуть название дня
 */
const char * lv_calendar_get_day_name(lv_calendar_date_t * gregorian);

/**
 * Получите китайское время по григорианскому времени (ссылка: https://www.cnblogs.com/liyang31tg/p/4123171.html)
 * @param gregorian_time   нужно перевести китайское время в григорианское время
 * @param chinese_time     китайское время конвертируется из григорианского времени
 */
void lv_calendar_gregorian_to_chinese(lv_calendar_date_t * gregorian_time, lv_calendar_chinese_t * chinese_time);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_CALENDAR_CHINESE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CALENDAR_CHINESE_H*/
