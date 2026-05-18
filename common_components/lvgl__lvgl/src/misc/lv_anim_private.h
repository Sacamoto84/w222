/**
 * @file lv_anim_private.h
 *
 */

#ifndef LV_ANIM_PRIVATE_H
#define LV_ANIM_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_anim.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    bool anim_list_changed;
    bool anim_run_round;
    bool anim_vsync_registered;
    lv_timer_t * timer;
    lv_ll_t anim_ll;
} lv_anim_state_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Запустите модуль анимации
 */
void lv_anim_core_init(void);

/**
 * Деинитизировать модуль анимации
 */
void lv_anim_core_deinit(void);

/*
 * Установите для анимации режим вертикальной синхронизации.
 * @param enable true: использовать режим vsync, false: использовать режим таймера.
 */
void lv_anim_enable_vsync_mode(bool enable);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_ANIM_PRIVATE_H*/
