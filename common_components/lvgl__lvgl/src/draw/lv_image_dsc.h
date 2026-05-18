/**
 * @file lv_image_dsc.h
 *
 */

#ifndef LV_IMAGE_DSC_H
#define LV_IMAGE_DSC_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

/*********************
 *      DEFINES
 *********************/

/** Магическое число для изображения lvgl, 9 — версия lvgl 9.
 *  Он не должен быть ни допустимым символом ASCII, ни больше, чем 0x80. См. `lv_image_src_get_type`.
 */
#define LV_IMAGE_HEADER_MAGIC (0x19)
LV_EXPORT_CONST_INT(LV_IMAGE_HEADER_MAGIC);

/**
 * Флаги зарезервированы для пользователя, lvgl не будет использовать эти биты.
 */
#define LV_IMAGE_FLAGS_USER_MASK (0xFF00)

/**********************
 *      TYPEDEFS
 **********************/

typedef enum _lvimage_flags_t {
    /**
     * Для карты RGB данных изображения отметьте, если она предварительно умножена на альфу.
     * Для индексированного изображения этот бит указывает данные палитры, предварительно умножаемые на альфу.
     */
    LV_IMAGE_FLAGS_PREMULTIPLIED    = 0x0001,
    /**
     * Данные изображения сжаты, поэтому декодеру необходимо сначала декодировать изображение.
     * Если этот флаг установлен, все изображение будет распаковано при декодировании.
     * `get_area_cb` не понадобится.
     */
    LV_IMAGE_FLAGS_COMPRESSED       = 0x0008,

    /*Нижеуказанные флаги применимы только для заголовка буфера отрисовки.*/

    /**
     * Изображение выделяется из кучи, поэтому должно быть освобождено после использования.
     */
    LV_IMAGE_FLAGS_ALLOCATED        = 0x0010,

    /**
     * Если данные изображения распределены по памяти и могут быть обработаны на месте.
     * При постобработке декодера изображений этот флаг означает, что мы модифицируем его на месте.
     */
    LV_IMAGE_FLAGS_MODIFIABLE       = 0x0020,

    /**
     * Изображение имеет собственные методы рисования.
     */
    LV_IMAGE_FLAGS_CUSTOM_DRAW      = 0x0040,

    /**
     * Флаги зарезервированы для пользователя, lvgl не будет использовать эти биты.
     */
    LV_IMAGE_FLAGS_USER1            = 0x0100,
    LV_IMAGE_FLAGS_USER2            = 0x0200,
    LV_IMAGE_FLAGS_USER3            = 0x0400,
    LV_IMAGE_FLAGS_USER4            = 0x0800,
    LV_IMAGE_FLAGS_USER5            = 0x1000,
    LV_IMAGE_FLAGS_USER6            = 0x2000,
    LV_IMAGE_FLAGS_USER7            = 0x4000,
    LV_IMAGE_FLAGS_USER8            = 0x8000,
} lv_image_flags_t;

typedef enum {
    LV_IMAGE_COMPRESS_NONE = 0,
    LV_IMAGE_COMPRESS_RLE,      /**< LVGL custom RLE compression */
    LV_IMAGE_COMPRESS_LZ4,
} lv_image_compress_t;

#if LV_BIG_ENDIAN_SYSTEM
typedef struct {
    uint32_t reserved_2: 16;    /**< Reserved to be used later*/
    uint32_t stride: 16;        /**< Number of bytes in a row*/
    uint32_t h: 16;
    uint32_t w: 16;
    uint32_t flags: 16;         /**< Image flags, see `lv_image_flags_t`*/
    uint32_t cf : 8;            /**< Color format: See `lv_color_format_t`*/
    uint32_t magic: 8;          /**< Magic number. Must be LV_IMAGE_HEADER_MAGIC*/
} lv_image_header_t;
#else
typedef struct {
    uint32_t magic: 8;          /**< Magic number. Must be LV_IMAGE_HEADER_MAGIC*/
    uint32_t cf : 8;            /**< Color format: See `lv_color_format_t`*/
    uint32_t flags: 16;         /**< Image flags, see `lv_image_flags_t`*/

    uint32_t w: 16;
    uint32_t h: 16;
    uint32_t stride: 16;        /**< Number of bytes in a row*/
    uint32_t reserved_2: 16;    /**< Reserved to be used later*/
} lv_image_header_t;
#endif

typedef struct {
    void * buf;
    uint32_t stride;            /**< Number of bytes in a row*/
} lv_yuv_plane_t;

typedef union {
    lv_yuv_plane_t yuv;         /**< packed format*/
    struct {
        lv_yuv_plane_t y;
        lv_yuv_plane_t u;
        lv_yuv_plane_t v;
    } planar;                   /**< planar format with 3 plane*/
    struct {
        lv_yuv_plane_t y;
        lv_yuv_plane_t uv;
    } semi_planar;              /**< planar format with 2 plane*/
} lv_yuv_buf_t;

/**
 * Структура для описания постоянного ресурса изображения.
 * Он похож на lv_draw_buf_t, но данные постоянны.
 */
typedef struct {
    lv_image_header_t header;   /**< A header describing the basics of the image*/
    uint32_t data_size;         /**< Size of the image in bytes*/
    const uint8_t * data;       /**< Pointer to the data of the image*/
    const void * reserved;      /**< A reserved field to make it has same size as lv_draw_buf_t*/
    const void * reserved_2;    /**< A reserved field to make it has same size as lv_draw_buf_t*/
} lv_image_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_IMAGE_DSC_H*/
