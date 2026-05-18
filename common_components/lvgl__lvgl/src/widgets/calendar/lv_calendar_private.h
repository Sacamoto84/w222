/**
 * @file lv_calendar_private.h
 *
 */

#ifndef LV_CALENDAR_PRIVATE_H
#define LV_CALENDAR_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../core/lv_obj_private.h"
#include "lv_calendar.h"

#if LV_USE_CALENDAR

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** Данные календаря */
struct _lv_calendar_t {
    lv_obj_t obj;
    /* Новые данные для этого типа */
    lv_obj_t * btnm;
    lv_calendar_date_t today;                /**< Date of today */
    lv_calendar_date_t showed_date;          /**< Currently visible month (day is ignored) */
    lv_calendar_date_t * highlighted_dates;  /**< Apply different style on these days (pointer to user-defined array) */
    size_t highlighted_dates_num;            /**< Number of elements in `highlighted_days` */
    const char * map[8 * 7];
#ifdef LV_USE_CALENDAR_CHINESE
    bool use_chinese_calendar;

    /** 7 * 6: В неделе 7 дней, а всего в календаре отображается 6 недель.
     * 20: Including the number of dates, line breaks, names for each day,
     * и резервируя несколько мест для адресов. */
    char nums [7 * 6][20];
#else
    /** 7 * 6: В неделе 7 дней, а всего в календаре отображается 6 недель.
     * 6: Including the number of dates, and reserving several spaces for
     * адреса.*/
    char nums [7 * 6][4];
#endif
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_CALENDAR */

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_CALENDAR_PRIVATE_H*/
