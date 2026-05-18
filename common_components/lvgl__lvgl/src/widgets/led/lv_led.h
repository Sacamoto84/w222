/**
 * @file lv_led.h
 *
 */

#ifndef LV_LED_H
#define LV_LED_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../core/lv_obj.h"

#if LV_USE_LED

/*********************
 *      DEFINES
 *********************/
/** Яркость при LED, если OFF */
#ifndef LV_LED_BRIGHT_MIN
# define LV_LED_BRIGHT_MIN 80
#endif

/** Яркость при LED, если ON */
#ifndef LV_LED_BRIGHT_MAX
# define LV_LED_BRIGHT_MAX 255
#endif

/**********************
 *      TYPEDEFS
 **********************/

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_led_class;

#if LV_USE_OBJ_PROPERTY
enum _lv_property_led_id_t {
    LV_PROPERTY_ID(LED, COLOR,      LV_PROPERTY_TYPE_COLOR, 0),
    LV_PROPERTY_ID(LED, BRIGHTNESS, LV_PROPERTY_TYPE_INT,   1),
    LV_PROPERTY_LED_END,
};
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создать светодиодный объект
 * @param parent    указатель на объект, он будет родителем нового светодиода
 * @return          указатель на созданный светодиод
 */
lv_obj_t * lv_led_create(lv_obj_t * parent);

/**
 * Установите цвет LED
 * @param led       указатель на объект LED
 * @param color     цвет LED
 */
void lv_led_set_color(lv_obj_t * led, lv_color_t color);

/**
 * Установите яркость объекта LED
 * @param led       указатель на объект LED
 * @param bright    LV_LED_BRIGHT_MIN (макс. темный) ...LV_LED_BRIGHT_MAX(макс. светлый)
 */
void lv_led_set_brightness(lv_obj_t * led, uint8_t bright);

/**
 * Свет на LED
 * @param led       указатель на объект LED
 */
void lv_led_on(lv_obj_t * led);

/**
 * Выключите LED
 * @param led       указатель на объект LED
 */
void lv_led_off(lv_obj_t * led);

/**
 * Переключить состояние LED
 * @param led       указатель на объект LED
 */
void lv_led_toggle(lv_obj_t * led);

/**
 * Получите яркость объекта LED.
 * @param obj       указатель на объект LED
 * @return яркий 0 (макс. темный) ... 255 (макс. светлый)
 */
uint8_t lv_led_get_brightness(const lv_obj_t * obj);

/**
 * Получить цвет объекта LED
 * @param obj       указатель на объект LED
 * @return цвет цвет LED
 */
lv_color_t lv_led_get_color(const lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_LED*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_LED_H*/
