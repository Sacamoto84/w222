/**
 * @file lv_vg_lite_grad.h
 *
 */

#ifndef LV_VG_LITE_GRAD_H
#define LV_VG_LITE_GRAD_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_DRAW_VG_LITE && LV_USE_VECTOR_GRAPHIC

#include "lv_vg_lite_utils.h"

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
 * @brief Создайте контекст градиента
 * @param cache_cnt количество записей кэша
 * @param unit блок рисования
 */
struct _lv_vg_lite_grad_ctx_t * lv_vg_lite_grad_ctx_create(uint32_t cache_cnt, struct _lv_draw_vg_lite_unit_t * unit);

/**
 * @brief Удаление контекста градиента
 * @param ctx контекст градиента, который нужно удалить
 */
void lv_vg_lite_grad_ctx_delete(struct _lv_vg_lite_grad_ctx_t * ctx);

/**
 * @brief Получить ожидающий список элементов градиента
 * @param ctx контекст градиента
 */
struct _lv_vg_lite_pending_t * lv_vg_lite_grad_ctx_get_pending(struct _lv_vg_lite_grad_ctx_t * ctx);

/**
 * @brief Получить кеш элементов градиента
 * @param ctx контекст градиента
 */
struct _lv_cache_t * lv_vg_lite_grad_ctx_get_cache(struct _lv_vg_lite_grad_ctx_t * ctx);

/**
 * @brief Нарисуйте градиент
 * @param ctx контекст градиента
 * @param buffer целевой буфер
 * @param path путь для рисования градиента
 * @param grad дескриптор градиента
 * @param grad_matrix матрица градиента
 * @param matrix матрица, применяемая к градиенту
 * @param fill правило заполнения
 * @param blend режим наложения
 * @return true: успех, false: неудача
 */
bool lv_vg_lite_draw_grad(
    struct _lv_vg_lite_grad_ctx_t * ctx,
    vg_lite_buffer_t * buffer,
    vg_lite_path_t * path,
    const lv_vector_gradient_t * grad,
    const vg_lite_matrix_t * grad_matrix,
    const vg_lite_matrix_t * matrix,
    vg_lite_fill_t fill,
    vg_lite_blend_t blend);

/**
 * @brief Нарисуйте помощник по градиенту
 * @param ctx контекст градиента
 * @param buffer целевой буфер
 * @param path путь для рисования градиента
 * @param area область для рисования градиента
 * @param grad_dsc дескриптор градиента
 * @param matrix матрица, применяемая к градиенту
 * @param fill правило заполнения
 * @param blend режим наложения
 * @return true: успех, false: неудача
 */
bool lv_vg_lite_draw_grad_helper(
    struct _lv_vg_lite_grad_ctx_t * ctx,
    vg_lite_buffer_t * buffer,
    vg_lite_path_t * path,
    const lv_area_t * area,
    const lv_grad_dsc_t * grad_dsc,
    const vg_lite_matrix_t * matrix,
    vg_lite_fill_t fill,
    vg_lite_blend_t blend);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_VG_LITE && LV_USE_VECTOR_GRAPHIC*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_VG_LITE_GRAD_H*/
