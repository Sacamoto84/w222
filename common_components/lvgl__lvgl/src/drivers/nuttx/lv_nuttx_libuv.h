/**
 * @file lv_nuttx_libuv.h
 *
 */

#ifndef LV_NUTTX_LIBUV_H
#define LV_NUTTX_LIBUV_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../display/lv_display.h"
#include "../../indev/lv_indev.h"

#if LV_USE_NUTTX

#if LV_USE_NUTTX_LIBUV

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    void * loop;
    lv_display_t * disp;
    lv_indev_t * indev;
} lv_nuttx_uv_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Инициализируйте uv_loop, используя предоставленную информацию о конфигурации.
 * @param uv_info Указатель на структуру lv_nuttx_uv_t, которую необходимо инициализировать.
 */
void * lv_nuttx_uv_init(lv_nuttx_uv_t * uv_info);

/**
 * Деинициализируйте конфигурациюuv_loopдля уровня портирования NuttX.
 * @param data Указатель на пользовательские данные.
 */
void lv_nuttx_uv_deinit(void ** data);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_NUTTX_LIBUV*/

#endif /*LV_USE_NUTTX*/

#ifdef __cplusplus
} /* внешний "С" */
#endif

#endif /*LV_NUTTX_LIBUV_H*/
