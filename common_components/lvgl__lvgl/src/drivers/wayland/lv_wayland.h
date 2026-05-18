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
 * @note Должен вызываться в цикле выполнения приложения вместо
 * обычныйlv_timer_handlerпредоставлен LVGL
 * @return время до следующего запуска (в мс)
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
} /*extern "C"*/
#endif

#endif /* LV_WAYLAND_H */
