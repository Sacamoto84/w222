/**
* @file lv_cache_entry.h
*
 */

#ifndef LV_CACHE_ENTRY_H
#define LV_CACHE_ENTRY_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_types.h"

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
 * Получите размер записи кэша.
 * @param node_size     The size of the node in the cache.
 * @return              The size of the cache entry.
 */
uint32_t lv_cache_entry_get_size(const uint32_t node_size);

/**
 * Получите счетчик ссылок записи кэша.
 * @param entry        The cache entry to get the reference count of.
 * @return             The reference count of the cache entry.
 */
int32_t  lv_cache_entry_get_ref(lv_cache_entry_t * entry);

/**
 * Получите размер узла записи кэша. Это тот же размер, что и у параметра lv_cache_entry_get_size() node_size.
 * @param entry        The cache entry to get the node size of.
 * @return             The node size of the cache entry.
 */
uint32_t lv_cache_entry_get_node_size(lv_cache_entry_t * entry);

/**
 * Проверьте, недействительна ли запись в кэше.
 * @param entry        The cache entry to check.
 * @return             True: the cache entry is invalid. False: the cache entry is valid.
 */
bool     lv_cache_entry_is_invalid(lv_cache_entry_t * entry);

/**
 * Получите данные записи кэша.
 * @param entry        The cache entry to get the data of.
 * @return             The pointer to the data of the cache entry.
 */
void  *  lv_cache_entry_get_data(lv_cache_entry_t * entry);

/**
 * Получите экземпляр кэша записи кэша.
 * @param entry        The cache entry to get the cache instance of.
 * @return             The pointer to the cache instance of the cache entry.
 */
const lv_cache_t * lv_cache_entry_get_cache(const lv_cache_entry_t * entry);

/**
 * Получите запись кэша данных. Данные должны быть выделены экземпляром кэша.
 * @param data         The data to get the cache entry of.
 * @param node_size    The size of the node in the cache.
 * @return             The pointer to the cache entry of the data.
 */
lv_cache_entry_t * lv_cache_entry_get_entry(void * data, const uint32_t node_size);

/**
 * Выделите запись в кэше.
 * @param node_size    The size of the node in the cache.
 * @param cache        The cache instance to allocate the cache entry from.
 * @return             The pointer to the allocated cache entry.
 */
lv_cache_entry_t * lv_cache_entry_alloc(const uint32_t node_size, const lv_cache_t * cache);

/**
 * Инициализируйте запись кэша.
 * @param entry        The cache entry to initialize.
 * @param cache        The cache instance to allocate the cache entry from.
 * @param node_size    The size of the node in the cache.
 */
void lv_cache_entry_init(lv_cache_entry_t * entry, const lv_cache_t * cache, const uint32_t node_size);

/**
 * Освободить запись кэша. И данные записи кэша будут освобождены.
 * @param entry        The cache entry to deallocate.
 */
void lv_cache_entry_delete(lv_cache_entry_t * entry);
/*************************
 *    GLOBAL VARIABLES
 *************************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_CACHE_ENTRY_H*/
