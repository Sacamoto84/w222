/**
 * @file lv_qrcode.h
 *
 */

#ifndef LV_QRCODE_H
#define LV_QRCODE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#include "../../misc/lv_color.h"
#include "../../misc/lv_types.h"
#include "../../widgets/canvas/lv_canvas.h"
#include LV_STDBOOL_INCLUDE
#include LV_STDINT_INCLUDE
#if LV_USE_QRCODE

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_qrcode_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создайте пустой объект кода QR (`lv_canvas`).
 * @param parent point to an object where to create the QR code
 * @return pointer to the created QR code object
 */
lv_obj_t * lv_qrcode_create(lv_obj_t * parent);

/**
 * Установите размер кода QR.
 * @param obj pointer to a QR code object
 * @param size width and height of the QR code
 */
void lv_qrcode_set_size(lv_obj_t * obj, int32_t size);

/**
 * Установите темный цвет кода QR.
 * @param obj pointer to a QR code object
 * @param color dark color of the QR code
 */
void lv_qrcode_set_dark_color(lv_obj_t * obj, lv_color_t color);

/**
 * Установите цвет подсветки кода QR.
 * @param obj pointer to a QR code object
 * @param color light color of the QR code
 */
void lv_qrcode_set_light_color(lv_obj_t * obj, lv_color_t color);

/**
 * Установите данные объекта кода QR.
 * @param obj pointer to a QR code object
 * @param data data to display
 * @param data_len length of data in bytes
 * @return LV_RESULT_OK: if no error; LV_RESULT_INVALID: on error
 */
lv_result_t lv_qrcode_update(lv_obj_t * obj, const void * data, uint32_t data_len);

/**
 * Вспомогательная функция для установки данных объекта кода QR
 * @param obj pointer to a QR code object
 * @param data data to display as a string
 */
void lv_qrcode_set_data(lv_obj_t * obj, const char * data);

/**
 * Включить или отключить тихую зону.
 * Тихая зона — это область вокруг кода QR, где данные не кодируются.
 * @param obj pointer to a QR code object
 * @param enable true: enable quiet zone; false: disable quiet zone
 */
void lv_qrcode_set_quiet_zone(lv_obj_t * obj, bool enable);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_QRCODE*/

#ifdef __cplusplus
} /* внешний "С" */
#endif

#endif /*LV_QRCODE_H*/
