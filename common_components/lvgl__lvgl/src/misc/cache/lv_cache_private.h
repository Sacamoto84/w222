/**
* @file lv_cache_private.h
*
*/

#ifndef LV_CACHE_PRIVATE_H
#define LV_CACHE_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_types.h"
#include "../../osal/lv_os_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Результат обратного вызова условия резервирования кэша
 */
typedef enum {
    LV_CACHE_RESERVE_COND_OK,          /**< The condition is met and no entries need to be evicted */
    LV_CACHE_RESERVE_COND_TOO_LARGE,   /**< The condition is not met and the reserve size is too large */
    LV_CACHE_RESERVE_COND_NEED_VICTIM, /**< The condition is not met and a victim is needed to be evicted */
    LV_CACHE_RESERVE_COND_ERROR        /**< An error occurred while checking the condition */
} lv_cache_reserve_cond_res_t;

struct _lv_cache_ops_t;
struct _lv_cache_t;
struct _lv_cache_class_t;
struct _lv_cache_entry_t;

typedef struct _lv_cache_ops_t lv_cache_ops_t;
typedef struct _lv_cache_class_t lv_cache_class_t;

typedef int32_t lv_cache_compare_res_t;
typedef bool (*lv_cache_create_cb_t)(void * node, void * user_data);
typedef void (*lv_cache_free_cb_t)(void * node, void * user_data);
typedef lv_cache_compare_res_t (*lv_cache_compare_cb_t)(const void * a, const void * b);

/**
 * Функция выделения экземпляра кэша, используемая классом кэша для выделения памяти для экземпляров кэша.
 * @return Он должен вернуть указатель на выделенный экземпляр.
 */
typedef void * (*lv_cache_alloc_cb_t)(void);

/**
 * Функция инициализации экземпляра кэша, используемая классом кэша для инициализации экземпляра кэша.
 * @return Он должен возвращать true, если инициализация прошла успешно, и false в противном случае.
 */
typedef bool (*lv_cache_init_cb_t)(lv_cache_t * cache);

/**
 * Функция уничтожения экземпляра кэша, используемая классом кэша для уничтожения экземпляра кэша.
 */
typedef void (*lv_cache_destroy_cb_t)(lv_cache_t * cache, void * user_data);

/**
 * Функция получения кэша, используемая классом кэша для получения записи кэша по его ключу.
 * @return `NULL`, если ключ не найден.
 */
typedef lv_cache_entry_t * (*lv_cache_get_cb_t)(lv_cache_t * cache, const void * key, void * user_data);

/**
 * Функция добавления кэша, используемая классом кэша для добавления записи кэша с заданным ключом.
 * Эта функция заботится только о том, как добавить запись, она не проверяет, существует ли запись, и не заботится о том, является ли она жертвой или нет.
 * @return добавленная запись кэша или NULL, если запись не добавлена.
 */
typedef lv_cache_entry_t * (*lv_cache_add_cb_t)(lv_cache_t * cache, const void * key, void * user_data);

/**
 * Функция удаления кэша, используемая классом кэша для удаления записи из кэша, но не освобождает память.
 * Эта функция заботится только о том, как удалить запись, ее не волнует, является ли она жертвой или нет.
 */
typedef void (*lv_cache_remove_cb_t)(lv_cache_t * cache, lv_cache_entry_t * entry, void * user_data);

/**
 * Функция удаления кэша, используемая классом кэша для удаления записи кэша из кэша и освобождения памяти.
 */
typedef void (*lv_cache_drop_cb_t)(lv_cache_t * cache, const void * key, void * user_data);

/**
 * Функция удаления всех кэшей, используемая классом кэша для удаления всех записей кэша из кэша и освобождения памяти.
 */
typedef void (*lv_cache_drop_all_cb_t)(lv_cache_t * cache, void * user_data);

/**
 * Функция получения жертвы кэша, используемая классом кэша для удаления записи жертвы.
 */
typedef lv_cache_entry_t * (*lv_cache_get_victim_cb)(lv_cache_t * cache, void * user_data);

/**
 * Функция условия резерва кэша, используемая классом кэша для проверки возможности добавления новой записи в кэш без превышения его максимального размера.
 * Возможные результаты см. в lv_cache_reserve_cond_res_t.
 */
typedef lv_cache_reserve_cond_res_t (*lv_cache_reserve_cond_cb)(lv_cache_t * cache, const void * key, size_t size,
                                                                void * user_data);

/**
 * Функция создания итератора кэша, используемая классом кэша для создания итератора для кэша.
 * @return Указатель на созданный итератор или NULL, если итератор невозможно создать.
 */
typedef lv_iter_t * (*lv_cache_iter_create_cb)(lv_cache_t * cache);

/**
 * Структура операций с кэшем
 */
struct _lv_cache_ops_t {
    lv_cache_compare_cb_t compare_cb;    /**< Compare function for keys */
    lv_cache_create_cb_t create_cb;      /**< Create function for nodes */
    lv_cache_free_cb_t free_cb;          /**< Free function for nodes */
};

/**
 * Структура записи кэша
 */
struct _lv_cache_t {
    const lv_cache_class_t * clz;     /**<Класс кэша. Есть два встроенных класса:
                                       * - lv_cache_class_lru_rb_count для кэша на основеLRUс политикой вытеснения на основе подсчета.
                                       * - lv_cache_class_lru_rb_size для кеша на основеLRUс политикой вытеснения на основе размера. */

    uint32_t node_size;               /**< Size of a node */

    uint32_t max_size;                /**< Maximum size of the cache */
    uint32_t size;                    /**< Current size of the cache */

    lv_cache_ops_t ops;               /**< Cache operations struct _lv_cache_ops_t */

    lv_mutex_t lock;                  /**< Cache lock used to protect the cache in multithreading environments */

    const char * name;                /**< Name of the cache */
};

/**
 * Структура класса кэша для создания пользовательских классов кэша
 *
 * Примеры:
 * - lv_cache_class_lru_rb_count для кэша на основеLRUс политикой вытеснения на основе подсчета.
 * - lv_cache_class_lru_rb_size для кеша на основеLRUс политикой вытеснения на основе размера.
 */
struct _lv_cache_class_t {
    lv_cache_alloc_cb_t alloc_cb;                 /**< The allocation function for cache entries */
    lv_cache_init_cb_t init_cb;                   /**< The initialization function for cache entries */
    lv_cache_destroy_cb_t destroy_cb;             /**< The destruction function for cache entries */

    lv_cache_get_cb_t get_cb;                     /**< The get function for cache entries */
    lv_cache_add_cb_t add_cb;                     /**< The add function for cache entries */
    lv_cache_remove_cb_t remove_cb;               /**< The remove function for cache entries */
    lv_cache_drop_cb_t drop_cb;                   /**< The drop function for cache entries */
    lv_cache_drop_all_cb_t drop_all_cb;           /**< The drop all function for cache entries */
    lv_cache_get_victim_cb get_victim_cb;         /**< The get victim function for cache entries */
    lv_cache_reserve_cond_cb reserve_cond_cb;     /**< The reserve condition function for cache entries */

    lv_cache_iter_create_cb iter_create_cb;       /**< The iterator creation function for cache entries */
};

/*-----------------
 * Слот входа в кэш
 *----------------*/

struct _lv_cache_slot_size_t;

typedef struct _lv_cache_slot_size_t lv_cache_slot_size_t;

/**
 * Структура слота входа в кэш
 *
 * Чтобы добавить новые поля в запись кэша, добавьте их в новую структуру и добавьте ее в первую
 * поле структуры данных кэша.  А это размер слота для записи кэша.
 */
struct _lv_cache_slot_size_t {
    size_t size;
};
/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*************************
 *    GLOBAL VARIABLES
 *************************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_CACHE_PRIVATE_H*/
