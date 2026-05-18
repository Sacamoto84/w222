/**
* @file lv_image_cache.h
*
 */

#ifndef LV_IMAGE_CACHE_H
#define LV_IMAGE_CACHE_H

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
 * Инициализируйте кэш изображений.
 * @param  size размер кэша в байтах.
 * @return LV_RESULT_OK: инициализация прошла успешно, LV_RESULT_INVALID: не удалось.
 */
lv_result_t lv_image_cache_init(uint32_t size);

/**
 * Изменить размер кэша изображений.
 * Если установлено значение 0, кэш будет отключен.
 * @param new_size  новый размер кэша в байтах.
 * @param evict_now true: вытеснить изображения должны быть удалены политикой вытеснения, false: дождаться следующей очистки кэша.
 */
void lv_image_cache_resize(uint32_t new_size, bool evict_now);

/**
 * Недействительный кеш изображений. Используйте NULL, чтобы сделать все изображения недействительными.
 * @param src указатель на источник изображения.
 */
void lv_image_cache_drop(const void * src);

/**
 * Возвращаете true, если кэш изображений включен.
 * @return true: включено, false: отключено.
 */
bool lv_image_cache_is_enabled(void);

/**
 * Создайте итератор для перебора кэша изображений.
 * @return итератор для перебора кэша изображений.
 */
lv_iter_t * lv_image_cache_iter_create(void);

/**
 * Дамп содержимого кэша изображений в удобочитаемом формате с указанием порядка кэширования.
 */
void lv_image_cache_dump(void);

/*************************
 *    GLOBAL VARIABLES
 *************************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_IMAGE_CACHE_H*/
