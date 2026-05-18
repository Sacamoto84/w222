/**
 * @file lv_draw_label_private.h
 *
 */

#ifndef LV_DRAW_LABEL_PRIVATE_H
#define LV_DRAW_LABEL_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_label.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** Сохраните некоторую информацию, чтобы ускорить рисование очень больших текстов.
 * Получение первого видимого символа занимает много времени, потому что
 * все предыдущие символы необходимо проверить для расчета позиций.
 * Эта структура хранит более раннюю координату (например, -1000 пикселей) и индекс этой строки.
 * Поэтому расчеты можно начинать отсюда.*/
struct _lv_draw_label_hint_t {
    /** Индекс линии по координате `y`*/
    int32_t line_start;

    /** Укажите координату`y`первой буквы по индексу`line start`. Относительно координаты метки*/
    int32_t y;

    /** Координата «y1» метки при сохранении подсказки.
     * Используется для отмены подсказки, если метка сдвинулась слишком сильно.*/
    int32_t coord_y;
};

struct _lv_draw_glyph_dsc_t {
    /** В зависимости от полей`format`это может быть источником изображения или буфера рисования растровых или векторных данных. */
    const void * glyph_data;
    lv_font_glyph_format_t format;
    const lv_area_t * letter_coords;
    const lv_area_t * bg_coords;
    lv_font_glyph_dsc_t * g;
    lv_color_t color;
    lv_opa_t opa;
    lv_color_t outline_stroke_color;
    lv_opa_t outline_stroke_opa;
    int32_t outline_stroke_width;
    int32_t rotation;
    lv_point_t pivot;          /**< Rotation pivot point associated with total glyph including line_height */
    lv_draw_buf_t * _draw_buf; /**< a shared draw buf for get_bitmap, do not use it directly, use glyph_data instead */
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_LABEL_PRIVATE_H*/
