/**
 * @file lv_canvas.h
 *
 */

#ifndef LV_CANVAS_H
#define LV_CANVAS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_CANVAS != 0

#include "../image/lv_image.h"
#include "../../draw/lv_draw_image.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_canvas_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создайте объект холста
 * @param parent     pointer to an object, it will be the parent of the new canvas
 * @return           pointer to the created canvas
 */
lv_obj_t * lv_canvas_create(lv_obj_t * parent);

/*=====================
 * Функции установки
 *====================*/

/**
 * Установите буфер для холста.
 *
 * Вместо этого используйте lv_canvas_set_draw_buf(), если вам нужно установить буфер с требованием выравнивания.
 *
 * @param obj    pointer to a canvas object
 * @param buf    buffer where content of canvas will be.
 *                 Требуемый размер (lv_image_color_format_get_px_size(cf)*w)/8*h)
 *                 Его можно выделить с помощью `lv_malloc()` или
 *                 это может быть статически выделенный массив (например, static lv_color_t buf[100*50]) или
 *                 это может быть адрес в RAM или внешний SRAM
 * @param w      width of canvas
 * @param h      height of canvas
 * @param cf     color format. `LV_COLOR_FORMAT...`
 */
void lv_canvas_set_buffer(lv_obj_t * obj, void * buf, int32_t w, int32_t h, lv_color_format_t cf);

/**
 * Установите буфер рисования для холста. Буфер отрисовки может быть выделен с помощью `lv_draw_buf_create()`.
 * или определяется статически `LV_DRAW_BUF_DEFINE_STATIC` . Когда начальный адрес буфера и шаг совпадают
 * требование, рекомендуется использовать `lv_draw_buf_create` .
 * @param obj       pointer to a canvas object
 * @param draw_buf  pointer to a draw buffer
 */
void lv_canvas_set_draw_buf(lv_obj_t * obj, lv_draw_buf_t * draw_buf);

/**
 * Установите цвет и непрозрачность пикселя
 * @param obj   pointer to a canvas
 * @param x     X coordinate of the pixel
 * @param y     Y coordinate of the pixel
 * @param color the color
 * @param opa   the opacity
 * @note        The following color formats are supported
 *              LV_COLOR_FORMAT_I1/2/4/8, LV_COLOR_FORMAT_A8,
 *              LV_COLOR_FORMAT_RGB565, LV_COLOR_FORMAT_RGB888,
 *              LV_COLOR_FORMAT_XRGB8888, LV_COLOR_FORMAT_ARGB8888
 * @note    this function invalidates the canvas object every time,
 *      для лучшей производительности, если вы меняете много пикселей в цикле,
 *      вызовите `lv_display_enable_invalidation` до начала цикла
 *      поэтому аннулирование не выполняется при каждом вызове
 */
void lv_canvas_set_px(lv_obj_t * obj, int32_t x, int32_t y, lv_color_t color, lv_opa_t opa);

/**
 * Установите цвет палитры холста для индексного формата. Действительно только для `LV_COLOR_FORMAT_I1/2/4/8`
 * @param obj       pointer to canvas object
 * @param index     the palette color to set:
 *                  - for `LV_COLOR_FORMAT_I1`: 0..1
 *                  - for `LV_COLOR_FORMAT_I2`: 0..3
 *                  - for `LV_COLOR_FORMAT_I4`: 0..15
 *                  - for `LV_COLOR_FORMAT_I8`: 0..255
 * @param color     the color to set
 */
void lv_canvas_set_palette(lv_obj_t * obj, uint8_t index, lv_color32_t color);

/*=====================
 * Геттерные функции
 *====================*/

lv_draw_buf_t * lv_canvas_get_draw_buf(lv_obj_t * obj);

/**
 * Получить цвет и непрозрачность пикселя
 * @param obj   pointer to a canvas
 * @param x     X coordinate of the pixel
 * @param y     Y coordinate of the pixel
 * @return      ARGB8888 color of the pixel
 */
lv_color32_t lv_canvas_get_px(lv_obj_t * obj, int32_t x, int32_t y);

/**
 * Получите изображение холста как указатель на переменную `lv_image_dsc_t`.
 * @param canvas    pointer to a canvas object
 * @return          pointer to the image descriptor.
 */
lv_image_dsc_t * lv_canvas_get_image(lv_obj_t * canvas);

/**
 * Верните указатель на буфер.
 * Рекомендуется использовать эту функцию вместо буфера из
 * возвращаемое значение lv_canvas_get_image () как есть, можно выровнять
 * @param canvas    pointer to a canvas object
 * @return          pointer to the buffer
 */
const void * lv_canvas_get_buf(lv_obj_t * canvas);

/*=====================
 * Другие функции
 *====================*/

/**
 * Копирование буфера на холст
 * @param obj           pointer to a canvas object
 * @param canvas_area   the area of the canvas to copy the new data to
 * @param src_buf       pointer to a buffer holding the source data
 * @param src_area      the area of the source buffer to copy from. If NULL, copy the whole buffer.
 * @note  canvas_area and src_area should be the same size. If canvas_area and the size of src_buf are the same,
 *        src_area можно оставить NULL.
 */
void lv_canvas_copy_buf(lv_obj_t * obj, const lv_area_t * canvas_area, lv_draw_buf_t * src_buf,
                        const lv_area_t * src_area);

/**
 * Заполните холст цветом
 * @param obj       pointer to a canvas
 * @param color     the background color
 * @param opa       the desired opacity
 */
void lv_canvas_fill_bg(lv_obj_t * obj, lv_color_t color, lv_opa_t opa);

/**
 * Инициализируйте слой, чтобы использовать общие функции рисования LVGL ( lv_draw_rect /label/...) на холсте.
 * Необходимо использовать в паре с `lv_canvas_finish_layer`.
 * @param canvas    pointer to a canvas
 * @param layer     pointer to a layer variable to initialize
 */
void lv_canvas_init_layer(lv_obj_t * canvas, lv_layer_t * layer);

/**
 * Подождите, пока все рисунки на слое не будут готовы.
 * Необходимо использовать в паре с `lv_canvas_init_layer`.
 * @param canvas    pointer to a canvas
 * @param layer     pointer to a layer to finalize
 */
void lv_canvas_finish_layer(lv_obj_t * canvas, lv_layer_t * layer);

/**********************
 *      MACROS
 **********************/

#define LV_CANVAS_BUF_SIZE(w, h, bpp, stride) (((((w * bpp + 7) >> 3) + stride - 1) & ~(stride - 1)) * h + LV_DRAW_BUF_ALIGN)

/**
 * Просто обертка для `LV_CANVAS_BUF_SIZE` для привязок.
 */
uint32_t lv_canvas_buf_size(int32_t w, int32_t h, uint8_t bpp, uint8_t stride);

#endif /*LV_USE_CANVAS*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_CANVAS_H*/
