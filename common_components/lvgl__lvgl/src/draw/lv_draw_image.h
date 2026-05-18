/**
 * @file lv_draw_image.h
 *
 */

#ifndef LV_DRAW_IMAGE_H
#define LV_DRAW_IMAGE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw.h"
#include "lv_image_decoder.h"
#include "lv_draw_buf.h"
#include "../misc/lv_style.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      MACROS
 **********************/

struct _lv_draw_image_dsc_t {
    lv_draw_dsc_base_t base;

    /**The image source: pointer to `lv_image_dsc_t` or a path to a file*/
    const void * src;

    /**The header of the image. Initialized internally in `lv_draw_image` */
    lv_image_header_t header;

    /**Clip the corner of the image with this radius. Use `LV_RADIUS_CIRCLE` for max. radius */
    int32_t clip_radius;

    /**The rotation of the image in 0.1 degree unit. E.g. 234 means 23.4° */
    int32_t rotation;

    /**Горизонтальный масштаб (зум) изображения.
     * 256 ( LV_SCALE_NONE ): означает отсутствие масштабирования, 512 — двойной размер, 128 — половинный размер.*/
    int32_t scale_x;

    /**Same as `scale_y` but vertically*/
    int32_t scale_y;

    /**Parallelogram like transformation of the image horizontally in 0.1 degree unit. E.g. 456 means 45.6°.*/
    int32_t skew_x;

    /**Same as `skew_x` but vertically*/
    int32_t skew_y;

    /**Поворотная точка трансформации (масштаб и вращение).
     * 0;0 — верхний левый угол изображения. Может быть и за пределами изображения.*/
    lv_point_t pivot;

    /**Mix this color to the images. In case of `LV_COLOR_FORMAT_A8` it will be the color of the visible pixels*/
    lv_color_t recolor;

    /**The intensity of recoloring. 0 means, no recolor, 255 means full cover (transparent pixels remain transparent)*/
    lv_opa_t recolor_opa;

    /**Непрозрачность в диапазоне 0...255.
     * LV_OPA_TRANSP , LV_OPA_10 , LV_OPA_20 , .. LV_OPA_COVER также можно использовать*/
    lv_opa_t opa;

    /**Описывает, как смешать пиксели изображения с фоном.
     * См. `lv_blend_mode_t`для более подробной информации.
     */
    lv_blend_mode_t blend_mode : 4;

    /**1: perform the transformation with anti-aliasing */
    uint16_t antialias          : 1;

    /**Если изображение меньше поля`image_area``lv_draw_image_dsc_t`
     * расположите изображение плиткой (повторяйте как по горизонтали, так и по вертикали), чтобы заполнить
     * `image_area` область*/
    uint16_t tile               : 1;

    const lv_image_colorkey_t * colorkey;

    /**Used internally to store some information about the palette or the color of A8 images*/
    lv_draw_image_sup_t * sup;

    /** Используется для обозначения всей исходной, необрезанной области, где должно быть нарисовано изображение.
     * Это важно для:
     *  1. Рендеринг слоев, при котором может случиться так, что визуализируется только меньшая область слоя и, например.
     *     `clip_radius` должен знать, какое было исходное изображение.
     *  2. Мозаичные изображения, где целевая область рисования больше, чем изображение, подлежащее мозаике.
     */
    lv_area_t image_area;

    /**Указатель на дескриптор изображения A8 или L8, с помощью которого нужно замаскировать изображение.
     * Маска всегда выравнивается по центру. */
    const lv_image_dsc_t * bitmap_mask_src;
};

/**
 * Выполните фактический рендеринг декодированного изображения.
 * @param clipped_img_area                 указатель на задачу рисования
 * @param draw_dsc          дескриптор отрисовки изображения
 * @param decoder_dsc       указатель на дескриптор декодированного изображения
 * @param sup               дополнительные данные
 * @param img_coords        абсолютные координаты изображения
 * @param clipped_img_area  абсолютные координаты клипа
 */
typedef void (*lv_draw_image_core_cb)(lv_draw_task_t * t, const lv_draw_image_dsc_t * draw_dsc,
                                      const lv_image_decoder_dsc_t * decoder_dsc, lv_draw_image_sup_t * sup,
                                      const lv_area_t * img_coords, const lv_area_t * clipped_img_area);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Инициализируйте дескриптор отрисовки изображения.
 * @param dsc       указатель на дескриптор отрисовки
 */
void lv_draw_image_dsc_init(lv_draw_image_dsc_t * dsc);

/**
 * Попробуйте получить дескриптор отрисовки изображения из задачи рисования.
 * @param task      нарисовать задачу
 * @return          дескриптор отрисовки задачи или NULL, если задача не относится к типу LV_DRAW_TASK_TYPE_IMAGE
 */
lv_draw_image_dsc_t * lv_draw_task_get_image_dsc(lv_draw_task_t * task);

/**
 * Создать задачу рисования изображения
 * @param layer         указатель на слой
 * @param dsc           указатель на инициализированный дескриптор отрисовки
 * @param coords        координаты изображения
 * @note                `coords` может быть меньше реальной области изображения.
 *                      (если рендерится только часть изображения)
 *                      или может быть больше (в случае мозаичных изображений).   .
 */
void lv_draw_image(lv_layer_t * layer, const lv_draw_image_dsc_t * dsc, const lv_area_t * coords);

/**
 * Создайте задачу рисования, чтобы смешать один слой с другим слоем.
 * @param layer         указатель на слой
 * @param dsc           указатель на инициализированный дескриптор отрисовки. `src`должен быть установлен для слоя для смешивания.
 * @param coords        координаты слоя.
 * @note                `coords` может быть меньше общей области виджетов, из которой создается слой.
 *                      (если на слой была отрисована только часть виджета)
 */
void lv_draw_layer(lv_layer_t * layer, const lv_draw_image_dsc_t * dsc, const lv_area_t * coords);

/**
 * Получить тип источника изображения
 * @param src указатель на источник изображения:
 *  - указатель на переменнуюlv_image_t(изображение хранится внутри и компилируется в код)
 *  - путь к файлу (например, " S:/folder/image.bin")
 *  - или символ (например,LV_SYMBOL_CLOSE)
 * @return тип источника изображенияLV_IMAGE_SRC_VARIABLE/FILE/SYMBOL/ UNKNOWN
 */
lv_image_src_t lv_image_src_get_type(const void * src);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_IMAGE_H*/
