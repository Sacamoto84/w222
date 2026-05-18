/**
 * @file lv_barcode.h
 *
 */

#ifndef LV_BARCODE_H
#define LV_BARCODE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#include "../../misc/lv_types.h"
#include "../../misc/lv_color.h"
#include "../../widgets/canvas/lv_canvas.h"

#if LV_USE_BARCODE

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    /**
     * Код 128 с кодировкой GS1. Полосы `[FCN1]` и пробелы.
     */
    LV_BARCODE_ENCODING_CODE128_GS1,
    /**
     * Код 128 с необработанной кодировкой.
     */
    LV_BARCODE_ENCODING_CODE128_RAW,
} lv_barcode_encoding_t;

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_barcode_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создайте пустой объект штрих-кода (`lv_canvas`).
 * @param parent point to an object where to create the barcode
 * @return pointer to the created barcode object
 */
lv_obj_t * lv_barcode_create(lv_obj_t * parent);

/**
 * Установите темный цвет объекта штрих-кода
 * @param obj pointer to barcode object
 * @param color dark color of the barcode
 */
void lv_barcode_set_dark_color(lv_obj_t * obj, lv_color_t color);

/**
 * Установите цвет света объекта штрих-кода
 * @param obj pointer to barcode object
 * @param color light color of the barcode
 */
void lv_barcode_set_light_color(lv_obj_t * obj, lv_color_t color);

/**
 * Установите масштаб объекта штрих-кода
 * @param obj pointer to barcode object
 * @param scale scale factor
 */
void lv_barcode_set_scale(lv_obj_t * obj, uint16_t scale);

/**
 * Задайте направление объекта штрих-кода
 * @param obj pointer to barcode object
 * @param direction draw direction (`LV_DIR_HOR` or `LB_DIR_VER`)
 */
void lv_barcode_set_direction(lv_obj_t * obj, lv_dir_t direction);

/**
 * Установите мозаичный режим объекта штрих-кода
 * @param obj pointer to barcode object
 * @param tiled true: tiled mode, false: normal mode (default)
 */
void lv_barcode_set_tiled(lv_obj_t * obj, bool tiled);

/**
 * Установите кодировку объекта штрих-кода
 * @param obj pointer to barcode object
 * @param encoding encoding (default is `LV_BARCODE_CODE128_GS1`)
 */
void lv_barcode_set_encoding(lv_obj_t * obj, lv_barcode_encoding_t encoding);

/**
 * Установите данные объекта штрих-кода
 * @param obj pointer to barcode object
 * @param data data to display
 * @return LV_RESULT_OK: if no error; LV_RESULT_INVALID: on error
 */
lv_result_t lv_barcode_update(lv_obj_t * obj, const char * data);

/**
 * Получить темный цвет объекта штрих-кода
 * @param obj pointer to barcode object
 * @return dark color of the barcode
 */
lv_color_t lv_barcode_get_dark_color(lv_obj_t * obj);

/**
 * Получить светлый цвет объекта штрих-кода
 * @param obj pointer to barcode object
 * @return light color of the barcode
 */
lv_color_t lv_barcode_get_light_color(lv_obj_t * obj);

/**
 * Получить масштаб объекта штрих-кода
 * @param obj pointer to barcode object
 * @return scale factor
 */
uint16_t lv_barcode_get_scale(lv_obj_t * obj);

/**
 * Получить кодировку объекта штрих-кода
 * @param obj pointer to barcode object
 * @return encoding
 */
lv_barcode_encoding_t lv_barcode_get_encoding(const lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_BARCODE*/

#ifdef __cplusplus
} /* внешний "С" */
#endif

#endif /*LV_BARCODE_H*/
