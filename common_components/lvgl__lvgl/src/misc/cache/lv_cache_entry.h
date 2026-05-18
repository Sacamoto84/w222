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
 * @param node_size     Размер узла в кэше.
 * @return              Размер записи кэша.
 */
uint32_t lv_cache_entry_get_size(const uint32_t node_size);

/**
 * Получите счетчик ссылок записи кэша.
 * @param entry        Запись кэша, для которой нужно получить счетчик ссылок.
 * @return             Счетчик ссылок записи кэша.
 */
int32_t  lv_cache_entry_get_ref(lv_cache_entry_t * entry);

/**
 * Получите размер узла записи кэша. Это тот же размер, что и в параметреlv_cache_entry_get_size()node_size.
 * @param entry        Запись кэша, для которой нужно получить размер узла.
 * @return             Размер узла записи кэша.
 */
uint32_t lv_cache_entry_get_node_size(lv_cache_entry_t * entry);

/**
 * Проверьте, недействительна ли запись в кэше.
 * @param entry        Запись кэша, которую необходимо проверить.
 * @return             Верно: запись в кэше недействительна. Ложь: запись в кэше действительна.
 */
bool     lv_cache_entry_is_invalid(lv_cache_entry_t * entry);

/**
 * Получите данные записи кэша.
 * @param entry        Запись кэша, данные из которой нужно получить.
 * @return             Указатель на данные записи кэша.
 */
void  *  lv_cache_entry_get_data(lv_cache_entry_t * entry);

/**
 * Получите экземпляр кэша записи кэша.
 * @param entry        Запись кэша, из которой требуется получить экземпляр кэша.
 * @return             Указатель на экземпляр кэша записи кэша.
 */
const lv_cache_t * lv_cache_entry_get_cache(const lv_cache_entry_t * entry);

/**
 * Получите запись кэша данных. Данные должны быть выделены экземпляром кэша.
 * @param data         Данные, из которых требуется получить запись в кэше.
 * @param node_size    Размер узла в кэше.
 * @return             Указатель на запись кэша данных.
 */
lv_cache_entry_t * lv_cache_entry_get_entry(void * data, const uint32_t node_size);

/**
 * Выделите запись в кэше.
 * @param node_size    Размер узла в кэше.
 * @param cache        Экземпляр кэша, из которого будет выделена запись кэша.
 * @return             Указатель на выделенную запись кэша.
 */
lv_cache_entry_t * lv_cache_entry_alloc(const uint32_t node_size, const lv_cache_t * cache);

/**
 * Инициализируйте запись кэша.
 * @param entry        Запись кэша для инициализации.
 * @param cache        Экземпляр кэша, из которого будет выделена запись кэша.
 * @param node_size    Размер узла в кэше.
 */
void lv_cache_entry_init(lv_cache_entry_t * entry, const lv_cache_t * cache, const uint32_t node_size);

/**
 * Освободить запись кэша. И данные записи кэша будут освобождены.
 * @param entry        Запись кэша, которую необходимо освободить.
 */
void lv_cache_entry_delete(lv_cache_entry_t * entry);
/*************************
 *    GLOBAL VARIABLES
 *************************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CACHE_ENTRY_H*/
