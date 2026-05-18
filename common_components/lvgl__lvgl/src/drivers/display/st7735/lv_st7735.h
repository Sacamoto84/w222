/**
 * @file lv_st7735.h
 *
 * Этот драйвер представляет собой всего лишь оболочку универсального драйвера контроллера LCD, совместимого с MIPI.
 *
 */

#ifndef LV_ST7735_H
#define LV_ST7735_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../lcd/lv_lcd_generic_mipi.h"

#if LV_USE_ST7735

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef lv_lcd_send_cmd_cb_t lv_st7735_send_cmd_cb_t;
typedef lv_lcd_send_color_cb_t lv_st7735_send_color_cb_t;

/**********************
 *  GLOBAL PROTOTYPES
 **********************/

/**
 * Создайте дисплей LCD с помощью драйвера ST7735.
 * @param hor_res       горизонтальное разрешение
 * @param ver_res       вертикальное разрешение
 * @param flags         настройки конфигурации по умолчанию (зеркало, порядокRGBи т. д.)
 * @param send_cmd_cb      платформо-зависимая функция для отправки команды контроллеруLCD(обычно используется передача по опросу)
 * @param send_color_cb    зависящая от платформы функция для отправки данных пикселей на контроллерLCD(обычно используется передача DMA: необходимо реализовать «готовый» обратный вызов)
 * @return              указатель на созданный дисплей
 */
lv_display_t * lv_st7735_create(uint32_t hor_res, uint32_t ver_res, lv_lcd_flag_t flags,
                                lv_st7735_send_cmd_cb_t send_cmd_cb, lv_st7735_send_color_cb_t send_color_cb);

/**
 * Установите зазор, то есть смещение пикселя (0,0) в VRAM.
 * @param disp          объект отображения
 * @param x             смещение по оси x
 * @param y             смещение по оси y
 */
void lv_st7735_set_gap(lv_display_t * disp, uint16_t x, uint16_t y);

/**
 * Установить инверсию цвета
 * @param disp          объект отображения
 * @param invert        false: нормально, true: инвертировать
 */
void lv_st7735_set_invert(lv_display_t * disp, bool invert);

/**
 * Установить гамма-кривую
 * @param disp          объект отображения
 * @param gamma         гамма-кривая
 */
void lv_st7735_set_gamma_curve(lv_display_t * disp, uint8_t gamma);

/**
 * Отправить список команд.
 * @param disp          объект отображения
 * @param cmd_list      команды контроллера и панели
 */
void lv_st7735_send_cmd_list(lv_display_t * disp, const uint8_t * cmd_list);

/**********************
 *      OTHERS
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_ST7735*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_ST7735_H*/
