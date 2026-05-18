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
 * @param parent    pointer to an object, it will be the parent of the new led
 * @return          pointer to the created led
 */
lv_obj_t * lv_led_create(lv_obj_t * parent);

/**
 * Установите цвет LED
 * @param led       pointer to a LED object
 * @param color     the color of the LED
 */
void lv_led_set_color(lv_obj_t * led, lv_color_t color);

/**
 * Установите яркость объекта LED
 * @param led       pointer to a LED object
 * @param bright    LV_LED_BRIGHT_MIN (max. dark) ... LV_LED_BRIGHT_MAX (max. light)
 */
void lv_led_set_brightness(lv_obj_t * led, uint8_t bright);

/**
 * Свет на LED
 * @param led       pointer to a LED object
 */
void lv_led_on(lv_obj_t * led);

/**
 * Выключите LED
 * @param led       pointer to a LED object
 */
void lv_led_off(lv_obj_t * led);

/**
 * Переключить состояние LED
 * @param led       pointer to a LED object
 */
void lv_led_toggle(lv_obj_t * led);

/**
 * Получите яркость объекта LED.
 * @param obj       pointer to LED object
 * @return bright   0 (max. dark) ... 255 (max. light)
 */
uint8_t lv_led_get_brightness(const lv_obj_t * obj);

/**
 * Получить цвет объекта LED
 * @param obj       pointer to LED object
 * @return color    color of the LED
 */
lv_color_t lv_led_get_color(const lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_LED*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_LED_H*/
