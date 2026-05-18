/**
 * @file lv_nanovg_image_cache.h
 *
 */

#ifndef LV_NANOVG_IMAGE_CACHE_H
#define LV_NANOVG_IMAGE_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_DRAW_NANOVG

#include "../lv_draw_image_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_draw_nanovg_unit_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * @brief Инициализируйте кэш изображений
 * @param u указатель на блок nanovg
 */
void lv_nanovg_image_cache_init(struct _lv_draw_nanovg_unit_t * u);

/**
 * @brief Деинициализировать кэш изображений
 * @param u указатель на блок nanovg
 */
void lv_nanovg_image_cache_deinit(struct _lv_draw_nanovg_unit_t * u);

/**
 * @brief Получите дескриптор изображения из кеша, создайте новый, если не найден.
 * @param u указатель на блок nanovg
 * @param src исходные данные изображения
 * @param color цвет для нанесения
 * @param image_flags флаги изображений
 * @param header заголовок изображения для заполнения (может бытьNULL)
 * @return дескриптор изображения или -1 в случае неудачи
 */
int lv_nanovg_image_cache_get_handle(struct _lv_draw_nanovg_unit_t * u,
                                     const void * src,
                                     lv_color32_t color,
                                     int image_flags,
                                     lv_image_header_t * header);

/**
 * @brief Удалить изображение из кеша
 * @param u указатель на блок nanovg
 * @param src исходные данные изображения
 */
void lv_nanovg_image_cache_drop(struct _lv_draw_nanovg_unit_t * u, const void * src);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_NANOVG*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_NANOVG_IMAGE_CACHE_H*/
