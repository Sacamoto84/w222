/**
 * @file lv_arclabel_private.h
 *
 */

#ifndef LV_ARCLABEL_PRIVATE_H
#define LV_ARCLABEL_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../core/lv_obj_private.h"

#if LV_USE_ARCLABEL != 0
#include "lv_arclabel.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_arclabel_t {
    lv_obj_t obj;

    char * text;
    char dot[LV_ARCLABEL_DOT_NUM + 1]; /**< Bytes that have been replaced with dots */
    uint32_t dot_begin;                 /**< Offset where bytes have been replaced with dots */

    /**
     * @brief Начальный угол дуги в градусах.
     *
     * Эта переменная представляет начало углового диапазона дуги.
     * в плане градусов. Он используется вместе с`angle_size`для определения.
     * часть круга, которую охватывает дуга. Значения нормализованы
     * в диапазон [0, 360), обеспечивая совместимость с циклическими представлениями.
     *
     * Изменение этого значения влияет на визуальное отображение дуги и связанных с ней объектов.
     * индикаторы или метки, что приводит к необходимости последующего признания недействительными или обновления
     * отображение объекта для отражения изменений.
     *
     * @note При настройке этого значения программно убедитесь, что оно не превышает 360 градусов.
     * поскольку оно будет автоматически скорректировано в соответствии с допустимым диапазоном.
     */
    lv_value_precise_t angle_start;
    lv_value_precise_t angle_size;
    uint32_t offset;
    uint32_t radius;
    lv_point_t center_offset;
    lv_arclabel_dir_t dir;
    lv_arclabel_text_align_t text_align_v; /**< Vertical text alignment */
    lv_arclabel_text_align_t text_align_h; /**< Horizontal text alignment */
    uint8_t static_txt : 1;             /**< Flag to indicate the text is static */
    uint8_t recolor    : 1;             /**< Enable in-line letter re-coloring */
    uint8_t overflow   : 2;             /**< Overflow mode: 0=visible, 1=ellipsis, 2=clip */
    bool end_overlap   : 1;             /**< End overlap flag, false if prevent end overlap */
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_ARCLABEL != 0 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_ARCLABEL_PRIVATE_H*/
