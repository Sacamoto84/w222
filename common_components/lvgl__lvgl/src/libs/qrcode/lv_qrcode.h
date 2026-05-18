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
 * Создайте пустой объект-кодQR(`lv_canvas`).
 * @param parent указать на объект, где создать код QR
 * @return указатель на созданный объект кода QR
 */
lv_obj_t * lv_qrcode_create(lv_obj_t * parent);

/**
 * Установите размер кода QR.
 * @param obj указатель на объект кода QR
 * @param size ширина и высота кода QR
 */
void lv_qrcode_set_size(lv_obj_t * obj, int32_t size);

/**
 * Установите темный цвет кода QR.
 * @param obj указатель на объект кода QR
 * @param color темный цвет кода QR
 */
void lv_qrcode_set_dark_color(lv_obj_t * obj, lv_color_t color);

/**
 * Установите цвет подсветки кода QR.
 * @param obj указатель на объект кода QR
 * @param color светлый цвет кода QR
 */
void lv_qrcode_set_light_color(lv_obj_t * obj, lv_color_t color);

/**
 * Установите данные объекта кода QR.
 * @param obj указатель на объект кода QR
 * @param data данные для отображения
 * @param data_len длина данных в байтах
 * @return LV_RESULT_OK : если нет ошибок; LV_RESULT_INVALID: при ошибке
 */
lv_result_t lv_qrcode_update(lv_obj_t * obj, const void * data, uint32_t data_len);

/**
 * Вспомогательная функция для установки данных объекта кода QR
 * @param obj указатель на объект кода QR
 * @param data данные для отображения в виде строки
 */
void lv_qrcode_set_data(lv_obj_t * obj, const char * data);

/**
 * Включить или отключить тихую зону.
 * Тихая зона — это область вокруг кода QR, где данные не кодируются.
 * @param obj указатель на объект кода QR
 * @param enable true: включить тихую зону; false: отключить тихую зону
 */
void lv_qrcode_set_quiet_zone(lv_obj_t * obj, bool enable);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_QRCODE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_QRCODE_H*/
