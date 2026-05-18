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
 * @param parent     указатель на объект, он будет родителем нового холста
 * @return           указатель на созданный холст
 */
lv_obj_t * lv_canvas_create(lv_obj_t * parent);

/*=====================
 * Функции установки
 *====================*/

/**
 * Установите буфер для холста.
 *
 * Вместо этого воспользуйтесьlv_canvas_set_draw_buf(), если вам нужно установить буфер с требованиями спортсмена.
 *
 * @param obj    указатель на объект холста
 * @param buf    буфер, в котором будет находиться содержимое холста.
 *                 Требуемый размер (lv_image_color_format_get_px_size(cf)*w)/8*h)
 *                 Его можно выделить с помощью`lv_malloc()`или
 *                 это может быть статически выделенный массив (например, staticlv_color_tbuf[100*50]) или
 *                 это может быть адрес в RAM или внешний SRAM
 * @param w      ширина холста
 * @param h      высота холста
 * @param cf     цветовой формат.  `LV_COLOR_FORMAT...`
 */
void lv_canvas_set_buffer(lv_obj_t * obj, void * buf, int32_t w, int32_t h, lv_color_format_t cf);

/**
 * Установите буфер рисования для холста. Буфер отрисовки можно выделить с помощью `lv_draw_buf_create()`.
 * или определяется статически `LV_DRAW_BUF_DEFINE_STATIC` . Когда начальный адрес буфера и шаг совпадают
 * требование, рекомендуется использовать`lv_draw_buf_create`.
 * @param obj       указатель на объект холста
 * @param draw_buf  указатель на буфер рисования
 */
void lv_canvas_set_draw_buf(lv_obj_t * obj, lv_draw_buf_t * draw_buf);

/**
 * Установите цвет и непрозрачность пикселя
 * @param obj   указатель на холст
 * @param x     Координата X пикселя
 * @param y     Координата Y пикселя
 * @param color цвет
 * @param opa   непрозрачность
 * @note        Поддерживаются следующие цветовые форматы
 *              LV_COLOR_FORMAT_I1/2/4/8, LV_COLOR_FORMAT_A8,
 *              LV_COLOR_FORMAT_RGB565, LV_COLOR_FORMAT_RGB888,
 *              LV_COLOR_FORMAT_XRGB8888, LV_COLOR_FORMAT_ARGB8888
 * @note    эта функция каждый раз делает объект холста недействительным,
 *      для лучшей производительности, если вы меняете много пикселей в цикле,
 *      вызовите`lv_display_enable_invalidation`до начала цикла
 *      поэтому аннулирование не выполняется при каждом вызове
 */
void lv_canvas_set_px(lv_obj_t * obj, int32_t x, int32_t y, lv_color_t color, lv_opa_t opa);

/**
 * Установите цвет палитры холста для индексного формата. Действительно только для `LV_COLOR_FORMAT_I1/2/4/8`
 * @param obj       указатель на объект холста
 * @param index     цвет палитры для установки:
 *                  - для `LV_COLOR_FORMAT_I1`: 0..1
 *                  - для `LV_COLOR_FORMAT_I2`: 0..3
 *                  - для `LV_COLOR_FORMAT_I4`: 0..15
 *                  - для `LV_COLOR_FORMAT_I8`: 0..255
 * @param color     цвет для установки
 */
void lv_canvas_set_palette(lv_obj_t * obj, uint8_t index, lv_color32_t color);

/*=====================
 * Геттерные функции
 *====================*/

lv_draw_buf_t * lv_canvas_get_draw_buf(lv_obj_t * obj);

/**
 * Получить цвет и непрозрачность пикселя
 * @param obj   указатель на холст
 * @param x     Координата X пикселя
 * @param y     Координата Y пикселя
 * @return      ARGB8888 цвет пикселя
 */
lv_color32_t lv_canvas_get_px(lv_obj_t * obj, int32_t x, int32_t y);

/**
 * Получите изображение холста как указатель на переменную `lv_image_dsc_t`.
 * @param canvas    указатель на объект холста
 * @return          указатель на дескриптор изображения.
 */
lv_image_dsc_t * lv_canvas_get_image(lv_obj_t * canvas);

/**
 * Верните указатель на буфер.
 * Рекомендуется использовать эту функцию вместо буфера из
 * возвращаемое значениеlv_canvas_get_image() как есть, можно выровнять
 * @param canvas    указатель на объект холста
 * @return          указатель на буфер
 */
const void * lv_canvas_get_buf(lv_obj_t * canvas);

/*=====================
 * Другие функции
 *====================*/

/**
 * Копирование буфера на холст
 * @param obj           указатель на объект холста
 * @param canvas_area   область холста, в которую нужно скопировать новые данные
 * @param src_buf       указатель на буфер, содержащий исходные данные
 * @param src_area      область исходного буфера, из которой осуществляется копирование. ЕслиNULL, скопируйте весь буфер.
 * @note  canvas_area иsrc_areaдолжны быть одинакового размера. Еслиcanvas_areaи размерsrc_bufодинаковы,
 *        src_area можно оставитьNULL.
 */
void lv_canvas_copy_buf(lv_obj_t * obj, const lv_area_t * canvas_area, lv_draw_buf_t * src_buf,
                        const lv_area_t * src_area);

/**
 * Заполните холст цветом
 * @param obj       указатель на холст
 * @param color     цвет фона
 * @param opa       желаемая непрозрачность
 */
void lv_canvas_fill_bg(lv_obj_t * obj, lv_color_t color, lv_opa_t opa);

/**
 * Инициализируйте слой, чтобы использовать общие функции рисованияLVGL(lv_draw_rect/label/...) на холсте.
 * Необходимо использовать вместе с `lv_canvas_finish_layer`.
 * @param canvas    указатель на холст
 * @param layer     указатель на переменную слоя для инициализации
 */
void lv_canvas_init_layer(lv_obj_t * canvas, lv_layer_t * layer);

/**
 * Подождите, пока все рисунки на слое не будут готовы.
 * Необходимо использовать вместе с `lv_canvas_init_layer`.
 * @param canvas    указатель на холст
 * @param layer     указатель на слой для финализации
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
} /*extern "C"*/
#endif

#endif /*LV_CANVAS_H*/
