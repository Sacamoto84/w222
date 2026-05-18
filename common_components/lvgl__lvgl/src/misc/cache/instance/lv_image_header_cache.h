/**
* @file lv_image_header_cache.h
*
 */

#ifndef LV_IMAGE_HEADER_CACHE_H
#define LV_IMAGE_HEADER_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_types.h"

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
 * Инициализируйте кеш заголовка изображения.
 * @param  count initial size of the cache in count of image headers.
 * @return LV_RESULT_OK: initialization succeeded, LV_RESULT_INVALID: failed.
 */
lv_result_t lv_image_header_cache_init(uint32_t count);

/**
 * Изменить размер кеша заголовка изображения.
 * Если установлено значение 0, кэш отключен.
 * @param count  new max count of cached image headers.
 * @param evict_now true: evict the image headers should be removed by the eviction policy, false: wait for the next cache cleanup.
 */
void lv_image_header_cache_resize(uint32_t count, bool evict_now);

/**
 * Недействительный кеш заголовка изображения. Используйте NULL, чтобы сделать недействительными все заголовки изображений.
 * Он также автоматически вызывается, когда изображение становится недействительным.
 * @param src pointer to an image source.
 */
void lv_image_header_cache_drop(const void * src);

/**
 * Возвращайте true, если кэш заголовка изображения включен.
 * @return true: enabled, false: disabled.
 */
bool lv_image_header_cache_is_enabled(void);

/**
 * Создайте итератор для перебора кеша заголовка изображения.
 * @return an iterator to iterate over the image header cache.
 */
lv_iter_t * lv_image_header_cache_iter_create(void);

/**
 * Дамп содержимого кэша заголовка изображения в удобочитаемом формате с указанием порядка кэширования.
 */
void lv_image_header_cache_dump(void);

/*************************
 *    GLOBAL VARIABLES
 *************************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_IMAGE_HEADER_CACHE_H*/
