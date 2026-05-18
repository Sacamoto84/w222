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
 * @param parent    указатель на родительский виджет
 * @return          указатель на созданный виджет анимации Лотти
 */
lv_obj_t  * lv_lottie_create(lv_obj_t * parent);

/**
 * Установите буфер для анимации. Он также определяет размер анимации.
 * @param obj       указатель на виджет лотереи
 * @param w         ширина анимации и буфера
 * @param h         высота анимации и буфера
 * @param buf       статический буфер размером`width x height x 4`в байтах
 */
void lv_lottie_set_buffer(lv_obj_t * obj, int32_t w, int32_t h, void * buf);

/**
 * Установите буфер прорисовки для анимации. Он также определяет размер анимации.
 * @param obj       указатель на виджет лотереи
 * @param draw_buf  инициализированный буфер отрисовки с цветовым форматом ARGB8888
 */
void lv_lottie_set_draw_buf(lv_obj_t * obj, lv_draw_buf_t * draw_buf);

/**
 * Установите источник анимации в виде массива.
 * @param obj       указатель на виджет лотереи
 * @param src       анимация лотереи преобразована в массив с нулевым завершением
 * @param src_size  размер исходного массива в байтах
 */
void lv_lottie_set_src_data(lv_obj_t * obj, const void * src, size_t src_size);

/**
 * Установите источник анимации в качестве пути.
 * Лотти не использует файловую систему LVGL API.
 * @param obj       указатель на виджет лотереи
 * @param src       путь к файлу json, например. "путь/к/file.json"
 */
void lv_lottie_set_src_file(lv_obj_t * obj, const char * src);

/**
 * Получите анимацию LVGL, которая управляет анимацией лотереи.
 * @param obj       указатель на виджет лотереи
 * @return          анимация LVGL
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
