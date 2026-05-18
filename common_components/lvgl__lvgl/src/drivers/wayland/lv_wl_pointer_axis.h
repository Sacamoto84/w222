/**
 * @file lv_wl_pointer_axis.h
 *
 */

#ifndef LV_WL_POINTER_AXIS_H
#define LV_WL_POINTER_AXIS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../indev/lv_indev.h"
#include "../../indev/lv_indev_gesture.h"

#if LV_USE_WAYLAND

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_indev_t * lv_wayland_pointer_axis_create(void);

/**
 * Получает устройство ввода кодера
 * @note Используется для создания группы ввода при запуске приложения.
 * @param display Ссылка на дисплей LVGL, связанный с окном.
 * @return Устройство ввода
 */
lv_indev_t * lv_wayland_get_pointeraxis(lv_display_t * display);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_WAYLAND */

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_WL_POINTER_AXIS_H*/
