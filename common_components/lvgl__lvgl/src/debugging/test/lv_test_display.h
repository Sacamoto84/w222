/**
 * @file lv_test_display.h
 *
 */

#ifndef LV_TEST_DISPLAY_H
#define LV_TEST_DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#if LV_USE_TEST

#include "../../misc/lv_types.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/***
 * Создайте фиктивный дисплей для тестов.
 * @param hor_res   максимальное горизонтальное разрешение
 * @param ver_res   максимальное вертикальное разрешение
 * @return          созданный дисплей
 *
 * @note            Разрешение можно изменить на любое меньшее значение позже.
 *                  с помощью `lv_display_set_resolution`
 *                  Цветовой формат можно позже свободно изменить с помощью `lv_display_set_color_format`.
 */
lv_display_t * lv_test_display_create(int32_t hor_res, int32_t ver_res);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_TEST*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TEST_DISPLAY_H*/
