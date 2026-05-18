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
 * @param buf_i1              pointer to buffer with I1 formatted render
 * @param buf_argb8888        pointer to buffer for ARGB8888 render
 * @param width               width in pixels of the area.
 *                            должно быть кратно 8.
 * @param height              height in pixels of the area
 * @param buf_i1_stride       stride of i1 buffer in bytes
 * @param buf_argb8888_stride stride of argb8888 buffer in bytes
 * @param index0_color        color of the 0 bits of i1 buf
 * @param index1_color        color of the 1 bits of i1 buf
 */
void lv_draw_sw_i1_to_argb8888(const void * buf_i1, void * buf_argb8888, uint32_t width, uint32_t height,
                               uint32_t buf_i1_stride, uint32_t buf_argb8888_stride, uint32_t index0_color, uint32_t index1_color);

/**
 * Поменяйте местами верхний и нижний байт буфера RGB565.
 * Может потребоваться, если 8-битный параллельный порт или порт SPI отправляют байты в неправильном порядке.
 * Байты будут заменены местами.
 * @param buf           pointer to buffer
 * @param buf_size_px   number of pixels in the buffer
 */
void lv_draw_sw_rgb565_swap(void * buf, uint32_t buf_size_px);

/**
 * Инвертируйте буфер рисования в цветовом формате I1.
 * Обычно во время смешивания бит устанавливается в 1, если яркость больше 127.
 * В зависимости от используемого контроллера дисплея вам может потребоваться разное поведение.
 * Инверсия будет выполнена на месте.
 * @param buf          pointer to the buffer to be inverted
 * @param buf_size     size of the buffer in bytes
 */
void lv_draw_sw_i1_invert(void * buf, uint32_t buf_size);


/**
 * Преобразование буфера рисования в цветовом формате I1 из htiled (построчно)
 * к макету буфера с vtiled (по столбцам). Преобразование предполагает, что ширина буфера
 * а высота округляется до кратного 8.
 * @param buf           pointer to the buffer to be converted
 * @param buf_size      size of the buffer in bytes
 * @param width         width of the buffer
 * @param height        height of the buffer
 * @param out_buf       pointer to the output buffer
 * @param out_buf_size  size of the output buffer in bytes
 * @param bit_order_lsb bit order of the resulting vtiled buffer
 */
void lv_draw_sw_i1_convert_to_vtiled(const void * buf, uint32_t buf_size, uint32_t width, uint32_t height,
                                     void * out_buf,
                                     uint32_t out_buf_size, bool bit_order_lsb);

/**
 * Поворот буфера в другой буфер
 * @param src           the source buffer
 * @param dest          the destination buffer
 * @param src_width     source width in pixels
 * @param src_height    source height in pixels
 * @param src_stride     source stride in bytes (number of bytes in a row)
 * @param dest_stride   destination stride in bytes (number of bytes in a row)
 * @param rotation      LV_DISPLAY_ROTATION_0/90/180/270
 * @param color_format  LV_COLOR_FORMAT_RGB565/RGB888/XRGB8888/ARGB8888
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
