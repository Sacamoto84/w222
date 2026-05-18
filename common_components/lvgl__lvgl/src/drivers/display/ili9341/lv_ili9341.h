/**
 * @file lv_ili9341.h
 *
 * Этот драйвер представляет собой всего лишь оболочку универсального драйвера контроллера LCD, совместимого с MIPI.
 *
 */

#ifndef LV_ILI9341_H
#define LV_ILI9341_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../lcd/lv_lcd_generic_mipi.h"

#if LV_USE_ILI9341

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef lv_lcd_send_cmd_cb_t lv_ili9341_send_cmd_cb_t;
typedef lv_lcd_send_color_cb_t lv_ili9341_send_color_cb_t;

/**********************
 *  GLOBAL PROTOTYPES
 **********************/

/**
 * Создайте дисплей LCD с помощью драйвера ILI9341.
 * @param hor_res       horizontal resolution
 * @param ver_res       vertical resolution
 * @param flags         default configuration settings (mirror, RGB ordering, etc.)
 * @param send_cmd      platform-dependent function to send a command to the LCD controller (usually uses polling transfer)
 * @param send_color    platform-dependent function to send pixel data to the LCD controller (usually uses DMA transfer: must implement a 'ready' callback)
 * @return              pointer to the created display
 */
lv_display_t * lv_ili9341_create(uint32_t hor_res, uint32_t ver_res, lv_lcd_flag_t flags,
                                 lv_ili9341_send_cmd_cb_t send_cmd_cb, lv_ili9341_send_color_cb_t send_color_cb);

/**
 * Установите зазор, то есть смещение пикселя (0,0) в VRAM.
 * @param disp          display object
 * @param x             x offset
 * @param y             y offset
 */
void lv_ili9341_set_gap(lv_display_t * disp, uint16_t x, uint16_t y);

/**
 * Установить инверсию цвета
 * @param disp          display object
 * @param invert        false: normal, true: invert
 */
void lv_ili9341_set_invert(lv_display_t * disp, bool invert);

/**
 * Установить гамма-кривую
 * @param disp          display object
 * @param gamma         gamma curve
 */
void lv_ili9341_set_gamma_curve(lv_display_t * disp, uint8_t gamma);

/**
 * Отправить список команд.
 * @param disp          display object
 * @param cmd_list      controller and panel-specific commands
 */
void lv_ili9341_send_cmd_list(lv_display_t * disp, const uint8_t * cmd_list);

/**********************
 *      OTHERS
 **********************/

/**********************
 *      MACROS
 **********************/


#endif /*LV_USE_ILI9341*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_ILI9341_H*/
