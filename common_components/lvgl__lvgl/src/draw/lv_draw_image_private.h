/**
 * @file lv_draw_image_private.h
 *
 */

#ifndef LV_DRAW_IMAGE_PRIVATE_H
#define LV_DRAW_IMAGE_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_image.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_draw_image_sup_t {
    lv_color_t alpha_color;
    const lv_color32_t * palette;
    uint32_t palette_size   : 9;
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Может использоваться блоками рисования для обработки декодирования и
 * подготовить все для фактического рендеринга изображения
 * @param t             указатель на задачу рисования
 * @param draw_dsc      дескриптор отрисовки изображения
 * @param coords        абсолютные координаты изображения
 * @param draw_core_cb  обратный вызов для выполнения фактического рендеринга
 * @param decoder_args  аргументы, передаваемые декодерам изображений, илиNULLпо умолчанию
 */
void lv_draw_image_normal_helper(lv_draw_task_t * t, const lv_draw_image_dsc_t * draw_dsc,
                                 const lv_area_t * coords, lv_draw_image_core_cb draw_core_cb,
                                 const lv_image_decoder_args_t * decoder_args);

/**
 * Может использоваться блоками рисования для изображений TILED для обработки декодирования и
 * подготовить все для фактического рендеринга изображения
 * @param t             указатель на задачу рисования
 * @param draw_dsc      дескриптор отрисовки изображения
 * @param coords        абсолютные координаты изображения
 * @param draw_core_cb  обратный вызов для выполнения фактического рендеринга
 * @param decoder_args  аргументы, передаваемые декодерам изображений, илиNULLпо умолчанию
 */
void lv_draw_image_tiled_helper(lv_draw_task_t * t, const lv_draw_image_dsc_t * draw_dsc,
                                const lv_area_t * coords, lv_draw_image_core_cb draw_core_cb,
                                const lv_image_decoder_args_t * decoder_args);

/**
 * Получить площадь прямоугольника, если он повернут и масштабирован
 * @param res сохраните координаты здесь
 * @param w ширина прямоугольника для преобразования
 * @param h высота прямоугольника для преобразования
 * @param angle угол поворота
 * @param scale_x масштабирование по оси X (256 без масштабирования)
 * @param scale_y масштабирование в направлении Y (256 без масштабирования)
 * @param pivot координаты поворота x,y
 */
void lv_image_buf_get_transformed_area(lv_area_t * res, int32_t w, int32_t h, int32_t angle,
                                       uint16_t scale_x, uint16_t scale_y, const lv_point_t * pivot);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_IMAGE_PRIVATE_H*/
