/**
 * @file lv_lottie.h
 *
 */

#ifndef LV_LOTTIE_H
#define LV_LOTTIE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../misc/lv_types.h"
#if LV_USE_LOTTIE

/*Тестирование зависимостей*/
#if LV_USE_CANVAS == 0
#error "lv_lottie: lv_canvas is required. Enable it in lv_conf.h (LV_USE_CANVAS 1)"
#endif

#if LV_USE_THORVG == 0
#error "lv_lottie: ThorVG is required. Enable it in lv_conf.h (LV_USE_THORVG_INTERNAL/EXTERNAL 1)"
#endif

#include "../../draw/lv_draw_buf.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создать анимацию лотереи
 * @param parent    pointer to the parent widget
 * @return          pointer to the created Lottie animation widget
 */
lv_obj_t  * lv_lottie_create(lv_obj_t * parent);

/**
 * Установите буфер для анимации. Он также определяет размер анимации.
 * @param obj       pointer to a lottie widget
 * @param w         width of the animation and buffer
 * @param h         height of the animation and buffer
 * @param buf       a static buffer with `width x height x 4` byte size
 */
void lv_lottie_set_buffer(lv_obj_t * obj, int32_t w, int32_t h, void * buf);

/**
 * Установите буфер прорисовки для анимации. Он также определяет размер анимации.
 * @param obj       pointer to a lottie widget
 * @param draw_buf  an initialized draw buffer with ARGB8888 color format
 */
void lv_lottie_set_draw_buf(lv_obj_t * obj, lv_draw_buf_t * draw_buf);

/**
 * Установите источник анимации в виде массива.
 * @param obj       pointer to a lottie widget
 * @param src       the lottie animation converted to an nul terminated array
 * @param src_size  size of the source array in bytes
 */
void lv_lottie_set_src_data(lv_obj_t * obj, const void * src, size_t src_size);

/**
 * Установите источник анимации в качестве пути.
 * Лотти не использует файловую систему LVGL API.
 * @param obj       pointer to a lottie widget
 * @param src       path to a json file, e.g. "path/to/file.json"
 */
void lv_lottie_set_src_file(lv_obj_t * obj, const char * src);

/**
 * Получите анимацию LVGL, которая управляет анимацией лотереи.
 * @param obj       pointer to a lottie widget
 * @return          the LVGL animation
 */
lv_anim_t * lv_lottie_get_anim(lv_obj_t * obj);

/**********************
 * GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_LOTTIE*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_LOTTIE_H*/
