/**
 * @file lv_wayland.h
 */

#ifndef LV_WAYLAND_H
#define LV_WAYLAND_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_WAYLAND

#include "lv_wl_keyboard.h"
#include "lv_wl_pointer.h"
#include "lv_wl_touch.h"
#include "lv_wl_window.h"
#include "lv_wl_pointer_axis.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Обертка вокруг lv_timer_handler
 * @note Must be called in the application run loop instead of the
 * обычный lv_timer_handler предоставлен LVGL
 * @return time till it needs to be run next (in ms)
 */
uint32_t lv_wayland_timer_handler(void);

/**
 * Получает файловый дескриптор сокета Wayland.
 */
int lv_wayland_get_fd(void);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_WAYLAND */

#ifdef __cplusplus
} /* внешний "С" */
#endif

#endif /* LV_WAYLAND_H */
