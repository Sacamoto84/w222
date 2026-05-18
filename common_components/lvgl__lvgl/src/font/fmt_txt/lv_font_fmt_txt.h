/**
 * @file lv_font_fmt_txt.h
 *
 */

#ifndef LV_FONT_FMT_TXT_H
#define LV_FONT_FMT_TXT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_font.h"
#include "../../misc/lv_types.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** Это описывает глиф.*/
typedef struct {
#if LV_FONT_FMT_TXT_LARGE == 0
    uint32_t bitmap_index : 20;     /**< Start index of the bitmap. A font can be max 1 MB.*/
    uint32_t adv_w : 12;            /**< Draw the next glyph after this width. 8.4 format (real_value * 16 is stored).*/
    uint8_t box_w;                  /**< Width of the glyph's bounding box*/
    uint8_t box_h;                  /**< Height of the glyph's bounding box*/
    int8_t ofs_x;                   /**< x offset of the bounding box*/
    int8_t ofs_y;                   /**< y offset of the bounding box. Measured from the top of the line*/
#else
    uint32_t bitmap_index;          /**< Start index of the bitmap. A font can be max 4 GB.*/
    uint32_t adv_w;                 /**< Draw the next glyph after this width. 28.4 format (real_value * 16 is stored).*/
    uint16_t box_w;                 /**< Width of the glyph's bounding box*/
    uint16_t box_h;                 /**< Height of the glyph's bounding box*/
    int16_t ofs_x;                  /**< x offset of the bounding box*/
    int16_t ofs_y;                  /**< y offset of the bounding box. Measured from the top of the line*/
#endif
} lv_font_fmt_txt_glyph_dsc_t;

/** Формат карты символов шрифта.*/
typedef enum {
    LV_FONT_FMT_TXT_CMAP_FORMAT0_FULL,
    LV_FONT_FMT_TXT_CMAP_SPARSE_FULL,
    LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY,
    LV_FONT_FMT_TXT_CMAP_SPARSE_TINY,
} lv_font_fmt_txt_cmap_type_t;

/**
 * Сопоставьте кодовые точки с `glyph_dsc` s
 * Поддерживается несколько форматов для оптимизации использования памяти.
 * См. https://github.com/lvgl/lv_font_conv/blob/master/doc/font_spec.md.
 */
typedef struct {
    /** Первый символ Юникода для этого диапазона*/
    uint32_t range_start;

    /** Количество символов Юникода, относящихся к этому диапазону.
     * Последний символ Юникода = range_start + range_length - 1*/
    uint16_t range_length;

    /** Первый глиф ID (индекс массива `glyph_dsc`) для этого диапазона.*/
    uint16_t glyph_id_start;

    /*
    Согласно спецификации существует 4 формата:
        https://github.com/lvgl/lv_font_conv/blob/master/doc/font_spec.md

    Для простоты введите «относительную кодовую точку»:
        rcp = codepoint - range_start

    и функция поиска:
        выполните поиск «значения» в «массиве» и вернет индекс «значения».

    Формат 0 крошечный
        unicode_list == NULL && glyph_id_ofs_list == NULL
        glyph_id = glyph_id_start + rcp

    Формат 0 полный
        unicode_list == NULL && glyph_id_ofs_list != NULL
        glyph_id = glyph_id_start + glyph_id_ofs_list[rcp]

    Редкий крошечный
        unicode_list != NULL && glyph_id_ofs_list == NULL
        glyph_id = glyph_id_start + search(unicode_list, rcp)

    Редкий полный
        unicode_list != NULL && glyph_id_ofs_list != NULL
        glyph_id = glyph_id_start + glyph_id_ofs_list[search(unicode_list, rcp)]
    */

    const uint16_t * unicode_list;

    /** if(type == LV_FONT_FMT_TXT_CMAP_FORMAT0_ ...) это `uint8_t *`
     * if(type == LV_FONT_FMT_TXT_CMAP_SPARSE_ ...) это `uint16_t *`
     */
    const void * glyph_id_ofs_list;

    /** Длина `unicode_list` и/или `glyph_id_ofs_list`*/
    uint16_t list_length;

    /** Тип этой карты символов*/
    lv_font_fmt_txt_cmap_type_t type;
} lv_font_fmt_txt_cmap_t;

/** Простое сопоставление значений керна из пар*/
typedef struct {
    /*Чтобы получить значение керна из двух кодовых точек:
       1. Get the `glyph_id_left` and `glyph_id_right` from `lv_font_fmt_txt_cmap_t
       2. for(i = 0; i < pair_cnt * 2; i += 2)
             if( glyph_ids [i] == glyph_id_left &&
                glyph_ids [i+1] == glyph_id_right )
                 возвращаемые значения[i/2];
     */
    const void * glyph_ids;
    const int8_t * values;
    uint32_t pair_cnt   : 30;
    uint32_t glyph_ids_size : 2;    /**< 0: `glyph_ids` is stored as `uint8_t`; 1: as `uint16_t` */
} lv_font_fmt_txt_kern_pair_t;

/** Более сложное, но более оптимальное хранилище значений керна на основе классов.*/
typedef struct {
    /*Чтобы получить значение керна из двух кодовых точек:
          1. Get the `glyph_id_left` and `glyph_id_right` from `lv_font_fmt_txt_cmap_t
          2. Get the class of the left and right glyphs as `left_class` and `right_class`
              left_class = left_class_mapping[glyph_id_left];
              right_class = right_class_mapping[glyph_id_right];
          3. value = class_pair_values[(left_class-1)*right_class_cnt + (right_class-1)]
        */

    const int8_t * class_pair_values;     /**< left_class_cnt * right_class_cnt value */
    const uint8_t * left_class_mapping;   /**< Map the glyph_ids to classes: index -> glyph_id -> class_id */
    const uint8_t * right_class_mapping;  /**< Map the glyph_ids to classes: index -> glyph_id -> class_id */
    uint8_t left_class_cnt;
    uint8_t right_class_cnt;
} lv_font_fmt_txt_kern_classes_t;

/** Растровые форматы*/
typedef enum {
    LV_FONT_FMT_TXT_PLAIN      = 0,
    LV_FONT_FMT_TXT_COMPRESSED = 1,
    LV_FONT_FMT_TXT_COMPRESSED_NO_PREFILTER = 2,
} lv_font_fmt_txt_bitmap_format_t;

/** Опишите хранилище дополнительных данных для шрифтов. */
typedef struct {
    /** Растровые изображения всех глифов */
    const uint8_t * glyph_bitmap;

    /** Опишите глифы */
    const lv_font_fmt_txt_glyph_dsc_t * glyph_dsc;

    /** Сопоставьте глифы с символами Юникода.
     *Массив переменных `lv_font_cmap_fmt_txt_t` */
    const lv_font_fmt_txt_cmap_t * cmaps;

    /**
     * Сохраните значения кернинга.
     * Может быть `lv_font_fmt_txt_kern_pair_t *  or ` lv_font_kern_classes_fmt_txt_t *`
     * в зависимости от `kern_classes`
     */
    const void * kern_dsc;

    /** Масштабируйте значения керна в формате 12.4. */
    uint16_t kern_scale;

    /** Количество таблиц cmap */
    uint16_t cmap_num       : 9;

    /** Бит на пиксель: 1, 2, 3, 4, 8 */
    uint16_t bpp            : 4;

    /** Тип `kern_dsc` */
    uint16_t kern_classes   : 1;

    /**
     * формат хранения растрового изображения
     * от `lv_font_fmt_txt_bitmap_format_t`
     */
    uint16_t bitmap_format  : 2;

    /**
     * Байты, до которых дополняется каждая строка.
     * 0: means no align and padding
     * 1: e.g. with bpp=4 lines are aligned to 1 byte, so there can be a 4 bits of padding
     * 4, 8, 16, 32, 64: каждая строка дополняется до заданных границ байта.
     */
    uint8_t stride;
} lv_font_fmt_txt_dsc_t;

typedef struct {
    const lv_font_t * font_p; /**< Pointer to built-in font*/
    uint32_t size; /** < Размер встроенного шрифта*/
} lv_builtin_font_src_t;

LV_ATTRIBUTE_EXTERN_DATA extern const lv_font_class_t lv_builtin_font_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Используется как обратный вызов `get_glyph_bitmap` в собственном формате шрифта lvgl, если шрифт несжат.
 * @param g_dsc         the glyph descriptor including which font to use, which supply the glyph_index and format.
 * @param draw_buf      a draw buffer that can be used to store the bitmap of the glyph, it's OK not to use it.
 * @return pointer to an A8 bitmap (not necessarily bitmap_out) or NULL if `unicode_letter` not found
 */
const void * lv_font_get_bitmap_fmt_txt(lv_font_glyph_dsc_t * g_dsc, lv_draw_buf_t * draw_buf);

/**
 * Используется как обратный вызов `get_glyph_dsc` в собственном формате шрифта lvgl, если шрифт несжат.
 * @param font pointer to font
 * @param dsc_out store the result descriptor here
 * @param unicode_letter a UNICODE letter code
 * @param unicode_letter_next the unicode letter succeeding the letter under test
 * @return true: descriptor is successfully loaded into `dsc_out`.
 *         false: the letter was not found, no data is loaded to `dsc_out`
 */
bool lv_font_get_glyph_dsc_fmt_txt(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out, uint32_t unicode_letter,
                                   uint32_t unicode_letter_next);

/**********************
 *      MACROS
 **********************/

/**********************
 * ADD BUILT IN FONTS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_FONT_FMT_TXT_H*/
