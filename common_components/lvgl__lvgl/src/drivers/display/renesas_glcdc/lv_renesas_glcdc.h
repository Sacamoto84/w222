/**
 * @file lv_renesas_glcdc.h
 *
 */

#ifndef LV_RENESAS_GLCDC_H
#define LV_RENESAS_GLCDC_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../display/lv_display.h"

#if LV_USE_RENESAS_GLCDC

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
 * Создайте дисплей с помощью периферийного устройства RenesasGLCDCв режиме рендеринга DIRECT.
 * @return  указатель на созданный дисплей
 */
lv_display_t * lv_renesas_glcdc_direct_create(void);

/**
 * Создайте дисплей с помощью периферийного устройства RenesasGLCDCв режиме рендеринга PARTIAL.
 * @param buf1      первый буфер
 * @param buf2      второй буфер (может быть`NULL`)
 * @param buf_size  размер буфера в байтах
 * @return          указатель на созданный дисплей
 */
lv_display_t * lv_renesas_glcdc_partial_create(void * buf1, void * buf2, size_t buf_size);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_RENESAS_GLCDC */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_RENESAS_GLCDC_H */
