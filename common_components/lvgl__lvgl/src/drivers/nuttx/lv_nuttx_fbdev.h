/**
 * @file lv_nuttx_fbdev.h
 *
 */

#ifndef LV_NUTTX_FBDEV_H
#define LV_NUTTX_FBDEV_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../display/lv_display.h"

#if LV_USE_NUTTX

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
 * Создайте новый дисплей с помощью бэкэнда NuttX.
 */
lv_display_t * lv_nuttx_fbdev_create(void);

/**
 * Инициализировать отображение с указанным устройством кадрового буфера
 * @param disp      pointer to display with NuttX backend
 * @param file      the name of framebuffer device
 */
int lv_nuttx_fbdev_set_file(lv_display_t * disp, const char * file);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_NUTTX */

#ifdef __cplusplus
} /* внешний "С" */
#endif

#endif /* LV_NUTTX_FBDEV_H */
