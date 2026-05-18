/**
 * @file lv_nuttx_mouse.h
 *
 */

/*********************
 *      INCLUDES
 *********************/

#ifndef LV_NUTTX_MOUSE_H
#define LV_NUTTX_MOUSE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../indev/lv_indev.h"

#if LV_USE_NUTTX

#if LV_USE_NUTTX_MOUSE

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
 * Инициализируйте indev с помощью обычного ввода.
 * @param dev_path      путь устройства ввода
 */
lv_indev_t * lv_nuttx_mouse_create(const char * dev_path);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_NUTTX_MOUSE */

#endif /* LV_USE_NUTTX*/

#ifdef __cplusplus
} /* внешний "С" */
#endif

#endif /* LV_NUTTX_MOUSE_H */
