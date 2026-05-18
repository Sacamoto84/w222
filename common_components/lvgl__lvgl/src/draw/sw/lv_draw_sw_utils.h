/**
 * @file lv_draw_sw_utils.h
 *
 */

#ifndef LV_DRAW_SW_UTILS_H
#define LV_DRAW_SW_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#if LV_USE_DRAW_SW

#include "../../misc/lv_area.h"
#include "../../misc/lv_color.h"
#include "../../display/lv_display.h"


/*********************
 *      DEFINES
 *********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Преобразует буфер I1 в формат ARGB8888.
 * @param buf_i1              указатель на буфер с рендерингом в формате I1
 * @param buf_argb8888        указатель на буфер для рендеринга ARGB8888
 * @param width               ширина области в пикселях.
 *                            должно быть кратно 8.
 * @param height              высота в пикселях области
 * @param buf_i1_stride       шаг буфера i1 в байтах
 * @param buf_argb8888_stride шаг буфера argb8888 в байтах
 * @param index0_color        цвет 0 бит i1 buf
 * @param index1_color        цвет 1 бит i1 buf
 */
void lv_draw_sw_i1_to_argb8888(const void * buf_i1, void * buf_argb8888, uint32_t width, uint32_t height,
                               uint32_t buf_i1_stride, uint32_t buf_argb8888_stride, uint32_t index0_color, uint32_t index1_color);

/**
 * Поменяйте местами верхний и нижний байт буфера RGB565.
 * Может потребоваться, если 8-битный параллельный порт или порт SPI отправляют байты в неправильном порядке.
 * Байты будут заменены местами.
 * @param buf           указатель на буфер
 * @param buf_size_px   количество пикселей в буфере
 */
void lv_draw_sw_rgb565_swap(void * buf, uint32_t buf_size_px);

/**
 * Инвертируйте буфер рисования в цветовом формате I1.
 * Обычно во время смешивания бит устанавливается в 1, если яркость больше 127.
 * В зависимости от используемого контроллера дисплея вам может потребоваться разное поведение.
 * Инверсия будет выполнена на месте.
 * @param buf          указатель на буфер, который нужно инвертировать
 * @param buf_size     размер буфера в байтах
 */
void lv_draw_sw_i1_invert(void * buf, uint32_t buf_size);


/**
 * Преобразование буфера рисования в цвета в форматеI1из htiled (построчно)
 * к макету буфера с плиткой (по столбцам). Предварительное образование предусматривает, что ширина буфера
 * а высота округляется до кратного 8.
 * @param buf           указатель на буфер, который нужно преобразовать
 * @param buf_size      размер буфера в байтах
 * @param width         ширина буфера
 * @param height        высота буфера
 * @param out_buf       указатель на выходной буфер
 * @param out_buf_size  размер выходного буфера в байтах
 * @param bit_order_lsb порядок бит результирующего vtiled буфера
 */
void lv_draw_sw_i1_convert_to_vtiled(const void * buf, uint32_t buf_size, uint32_t width, uint32_t height,
                                     void * out_buf,
                                     uint32_t out_buf_size, bool bit_order_lsb);

/**
 * Поворот буфера в другой буфер
 * @param src           исходный буфер
 * @param dest          буфер назначения
 * @param src_width     ширина источника в пикселях
 * @param src_height    высота источника в пикселях
 * @param src_stride     исходный шаг в байтах (количество байтов в строке)
 * @param dest_stride   шаг назначения в байтах (количество байтов в строке)
 * @param rotation LV_DISPLAY_ROTATION_0 /90/180/270
 * @param color_format LV_COLOR_FORMAT_RGB565 / RGB888 / XRGB8888 / ARGB8888
 */
void lv_draw_sw_rotate(const void * src, void * dest, int32_t src_width, int32_t src_height, int32_t src_stride,
                       int32_t dest_stride, lv_display_rotation_t rotation, lv_color_format_t color_format);

/***********************
 * GLOBAL VARIABLES
 ***********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_SW*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_DRAW_SW_UTILS_H*/
