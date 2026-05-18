/**
 * @file lv_wl_keyboard.h
 *
 */

#ifndef LV_WL_KEYBOARD_H
#define LV_WL_KEYBOARD_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../indev/lv_indev.h"
#include "../../indev/lv_indev_gesture.h"
#if LV_USE_WAYLAND

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_indev_t * lv_wayland_keyboard_create(void);

/**
 * Получить устройство ввода с клавиатуры для данного дисплея LVGL.
 * @param display LVGL дисплей
 * @return устройство ввода, подключенное к клавиатуре, илиNULLпри ошибке
 */
lv_indev_t * lv_wayland_get_keyboard(lv_display_t * display);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_WAYLAND */

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_WL_KEYBOARD_H*/
