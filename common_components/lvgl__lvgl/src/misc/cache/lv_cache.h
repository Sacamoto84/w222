/**
 * @file lv_cache.h
 *
 */

#ifndef LV_CACHE_H
#define LV_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../lv_types.h"

#include "lv_cache_entry.h"

#include "class/lv_cache_class.h"
#include "instance/lv_cache_instance.h"

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
 * Создайте объект кэша с заданными параметрами.
 * @param cache_class   The class of the cache. Currently only support one two builtin classes:
 *                        - lv_cache_class_lru_rb_count for LRU-based cache with count-based eviction policy.
 *                        - lv_cache_class_lru_rb_size for LRU-based cache with size-based eviction policy.
 * @param node_size     The node size is the size of the data stored in the cache..
 * @param max_size      The max size is the maximum amount of memory or count that the cache can hold.
 *                        - lv_cache_class_lru_rb_count: max_size is the maximum count of nodes in the cache.
 *                        - lv_cache_class_lru_rb_size: max_size is the maximum size of the cache in bytes.
 * @param ops           A set of operations that can be performed on the cache. See lv_cache_ops_t for details.
 * @return              Returns a pointer to the created cache object on success, `NULL` on error.
 */
lv_cache_t * lv_cache_create(const lv_cache_class_t * cache_class,
                             size_t node_size, size_t max_size,
                             lv_cache_ops_t ops);

/**
 * Уничтожить объект кэша.
 * @param cache         The cache object pointer to destroy.
 * @param user_data     A user data pointer that will be passed to the free callback.
 */
void lv_cache_destroy(lv_cache_t * cache, void * user_data);

/**
 * Получите запись кэша с заданным ключом. Если записи нет в кеше, возвращается `NULL` (не найден).
 * Если запись найдена, ее приоритет будет изменен политикой кэша. И `lv_cache_entry_t::ref_cnt` будет увеличен.
 * @param cache         The cache object pointer to acquire the entry.
 * @param key           The key of the entry to acquire.
 * @param user_data     A user data pointer that will be passed to the create callback.
 * @return              Returns a pointer to the acquired cache entry on success with `lv_cache_entry_t::ref_cnt` incremented, `NULL` on error.
 */
lv_cache_entry_t * lv_cache_acquire(lv_cache_t * cache, const void * key, void * user_data);

/**
 * Получите запись кэша с заданным ключом. Если записи нет в кеше, будет создана новая запись с заданным ключом.
 * Если запись найдена, ее приоритет будет изменен политикой кэша. И `lv_cache_entry_t::ref_cnt` будет увеличен.
 * Если вы хотите использовать API для упрощения кода, вам следует предоставить `lv_cache_ops_t::create_cb`, который создает новую запись с данным ключом.
 * Этот API представляет собой комбинацию lv_cache_acquire() и lv_cache_add(). Эффект тот же, что и при вызове lv_cache_acquire() и lv_cache_add() по отдельности.
 * И внутреннее воздействие на кеш также соответствует этим двум API.
 * @param cache         The cache object pointer to acquire the entry.
 * @param key           The key of the entry to acquire or create.
 * @param user_data     A user data pointer that will be passed to the create callback.
 * @return              Returns a pointer to the acquired or created cache entry on success with `lv_cache_entry_t::ref_cnt` incremented, `NULL` on error.
 */
lv_cache_entry_t * lv_cache_acquire_or_create(lv_cache_t * cache, const void * key, void * user_data);

/**
 * Добавьте новую запись в кэш с указанным ключом и данными. Если кеш заполнен, для удаления записи будет использоваться политика кеша.
 * @param cache         The cache object pointer to add the entry.
 * @param key           The key of the entry to add.
 * @param user_data     A user data pointer that will be passed to the create callback.
 * @return              Returns a pointer to the added cache entry on success with `lv_cache_entry_t::ref_cnt` incremented, `NULL` on error.
 */
lv_cache_entry_t * lv_cache_add(lv_cache_t * cache, const void * key, void * user_data);

/**
 * Освободите запись кэша. `lv_cache_entry_t::ref_cnt` будет уменьшен. Если `lv_cache_entry_t::ref_cnt` равен нулю, произойдет ошибка.
 * Если запись, переданная в эту функцию, является последней ссылкой на данные и запись помечена как недействительная, для удаления записи будет использоваться политика кэша.
 * @param cache         The cache object pointer to release the entry.
 * @param entry         The cache entry pointer to release.
 * @param user_data     A user data pointer that will be passed to the free callback.
 */
void lv_cache_release(lv_cache_t * cache, lv_cache_entry_t * entry, void * user_data);

/**
 * Зарезервируйте определенный объем памяти/счета в кеше. Эта функция полезна, когда вы хотите заранее зарезервировать определенный объем памяти/счета.
 * например, когда вы знаете, что он понадобится вам позже.
 * Если текущий размер кэша превышает зарезервированный размер, функция будет удалять записи до тех пор, пока не будет достигнут зарезервированный размер.
 * @param cache         The cache object pointer to reserve.
 * @param reserved_size The amount of memory/count to reserve.
 * @param user_data     A user data pointer that will be passed to the free callback.
 */
void lv_cache_reserve(lv_cache_t * cache, uint32_t reserved_size, void * user_data);

/**
 * Удалить запись кэша с заданным ключом. Если записи нет в кеше, с ней ничего не произойдет.
 * Если запись найдена, она будет удалена из кэша, а ее данные будут освобождены при освобождении последней ссылки на нее.
 * @note The data will not be freed immediately but when the last reference to it is released. But this entry will not be found by lv_cache_acquire().
 *       Если вы хотите снова кэшировать тот же ключ, вам следует использовать lv_cache_add () или lv_cache_acquire_or_create ().
 * @param cache         The cache object pointer to drop the entry.
 * @param key           The key of the entry to drop.
 * @param user_data     A user data pointer that will be passed to the free callback.
 */
void lv_cache_drop(lv_cache_t * cache, const void * key, void * user_data);

/**
 * Удалите все записи кэша. Все записи будут удалены из кэша, а их данные будут освобождены, когда будет выпущена последняя ссылка на них.
 * @note If some entries are still referenced by other objects, it will issue an error. And this case shouldn't happen in normal cases..
 * @param cache         The cache object pointer to drop all entries.
 * @param user_data     A user data pointer that will be passed to the free callback.
 */
void lv_cache_drop_all(lv_cache_t * cache, void * user_data);

/**
 * Удалить одну запись из кеша. Политика выселения будет использоваться для выбора записи для выселения.
 * @param cache         The cache object pointer to evict an entry.
 * @param user_data     A user data pointer that will be passed to the free callback.
 * @return              Returns true if an entry is evicted, false if no entry is evicted.
 */
bool lv_cache_evict_one(lv_cache_t * cache, void * user_data);

/**
 * Установите максимальный размер кэша.
 * Если текущий размер кэша превышает новый максимальный размер, политика кэша будет использоваться для удаления записей до тех пор, пока не будет достигнут новый максимальный размер.
 * Если установлено значение 0, кэш будет отключен.
 * @note But this behavior will happen only new entries are added to the cache.
 * @param cache         The cache object pointer to set the maximum size.
 * @param max_size      The new maximum size of the cache.
 * @param user_data     A user data pointer that will be passed to the free callback.
 */
void   lv_cache_set_max_size(lv_cache_t * cache, size_t max_size, void * user_data);

/**
 * Получите максимальный размер кэша.
 * @param cache         The cache object pointer to get the maximum size.
 * @param user_data     A user data pointer that will be passed to the free callback.
 * @return              Returns the maximum size of the cache.
 */
size_t lv_cache_get_max_size(lv_cache_t * cache, void * user_data);

/**
 * Получить текущий размер кэша.
 * @param cache         The cache object pointer to get the current size.
 * @param user_data     A user data pointer that will be passed to the free callback.
 * @return              Returns the current size of the cache.
 */
size_t lv_cache_get_size(lv_cache_t * cache, void * user_data);

/**
 * Получите свободный размер кэша.
 * @param cache         The cache object pointer to get the free size.
 * @param user_data     A user data pointer that will be passed to the free callback.
 * @return              Returns the free size of the cache.
 */
size_t lv_cache_get_free_size(lv_cache_t * cache, void * user_data);

/**
 * Верните true, если кэш включен.
 * Отключенный кеш означает, что max_size кеша равен 0. В этом случае все операции с кешем будут простаивать.
 * @param cache         The cache object pointer to check if it's disabled.
 * @return              Returns true if the cache is enabled, false otherwise.
 */
bool lv_cache_is_enabled(lv_cache_t * cache);

/**
 * Установите обратный вызов сравнения кеша.
 * @param cache         The cache object pointer to set the compare callback.
 * @param compare_cb    The compare callback to set.
 * @param user_data     A user data pointer.
 */
void   lv_cache_set_compare_cb(lv_cache_t * cache, lv_cache_compare_cb_t compare_cb, void * user_data);

/**
 * Установите обратный вызов создания кеша.
 * @param cache         The cache object pointer to set the create callback.
 * @param alloc_cb      The create callback to set.
 * @param user_data     A user data pointer.
 */
void   lv_cache_set_create_cb(lv_cache_t * cache, lv_cache_create_cb_t alloc_cb, void * user_data);

/**
 * Установите бесплатный обратный вызов кеша.
 * @param cache         The cache object pointer to set the free callback.
 * @param free_cb       The free callback to set.
 * @param user_data     A user data pointer.
 */
void   lv_cache_set_free_cb(lv_cache_t * cache, lv_cache_free_cb_t free_cb, void * user_data);

/**
 * Дайте имя объекту кэша. Сохраняется только указатель строки.
 * @param cache         The cache object pointer to set the name.
 * @param name          The name of the cache.
 */
void lv_cache_set_name(lv_cache_t * cache, const char * name);

/**
 * Получите имя объекта кэша.
 * @param cache         The cache object pointer to get the name.
 * @return              Returns the name of the cache.
 */
const char * lv_cache_get_name(lv_cache_t * cache);

/**
 * Создайте итератор для объекта кэша. Итератор используется для перебора всех записей кэша.
 * @param cache         The cache object pointer to create the iterator.
 * @return              Returns a pointer to the created iterator on success, `NULL` on error.
 */
lv_iter_t * lv_cache_iter_create(lv_cache_t * cache);

/*************************
 *    GLOBAL VARIABLES
 *************************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /* LV_CACHE_H */
