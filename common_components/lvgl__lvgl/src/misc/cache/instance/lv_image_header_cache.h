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
 * @param  count начальный размер кэша по количеству заголовков изображений.
 * @return LV_RESULT_OK: инициализация прошла успешно, LV_RESULT_INVALID: не удалось.
 */
lv_result_t lv_image_header_cache_init(uint32_t count);

/**
 * Изменить размер кеша заголовка изображения.
 * Если установлено значение 0, кэш отключен.
 * @param count  новое максимальное количество заголовков кэшированных изображений.
 * @param evict_now true: вытеснить заголовки изображений должны быть удалены политикой вытеснения, false: дождаться следующей очистки кэша.
 */
void lv_image_header_cache_resize(uint32_t count, bool evict_now);

/**
 * Недействительный кеш заголовка изображения. Используйте NULL, чтобы сделать недействительными все заголовки изображений.
 * Он также автоматически вызывается, когда изображение становится недействительным.
 * @param src указатель на источник изображения.
 */
void lv_image_header_cache_drop(const void * src);

/**
 * Возвращает true, если кэш заголовка изображения включен.
 * @return true: включено, false: отключено.
 */
bool lv_image_header_cache_is_enabled(void);

/**
 * Создайте итератор для перебора кеша заголовка изображения.
 * @return итератор для перебора кэша заголовка изображения.
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
} /*extern "C"*/
#endif

#endif /*LV_IMAGE_HEADER_CACHE_H*/
