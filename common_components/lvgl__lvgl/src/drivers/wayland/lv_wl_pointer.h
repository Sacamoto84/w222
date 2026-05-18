
/**
 * @file lv_wl_pointer.h
 *
 */

#ifndef LV_WL_POINTER_H
#define LV_WL_POINTER_H

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

lv_indev_t * lv_wayland_pointer_create(void);

/**
 * Получает устройство ввода указателя мыши.
 * @note Используется для создания группы ввода при запуске приложения.
 * @param disp Ссылка на дисплей LVGL, связанный с окном.
 * @return Устройство ввода
 */
lv_indev_t * lv_wayland_get_pointer(lv_display_t * disp);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_WAYLAND */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_WL_POINTER_H*/
