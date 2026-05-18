/**
 * @file lv_anim_timeline_private.h
 *
 */

#ifndef LV_ANIM_TIMELINE_PRIVATE_H
#define LV_ANIM_TIMELINE_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_anim_timeline.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_anim_timeline_dsc_t;

/*Данные anim_timeline*/
struct _lv_anim_timeline_t {
    /** Динамически выделенный массив anim dsc*/
    struct _lv_anim_timeline_dsc_t * anim_dsc;

    /** Длина массива anim dsc*/
    uint32_t anim_dsc_cnt;

    /** Текущее время анимации*/
    uint32_t act_time;

    /** Обратное воспроизведение*/
    bool reverse;

    /** Задержка перед запуском анимации с любого конца*/
    uint32_t delay;

    /** Повторить счет*/
    uint32_t repeat_count;

    /** Подождите, прежде чем повторить*/
    uint32_t repeat_delay;

    /** Для любых пользовательских данных*/
    void * user_data;
};

/**********************
* GLOBAL PROTOTYPES
**********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_ANIM_TIMELINE_PRIVATE_H*/
