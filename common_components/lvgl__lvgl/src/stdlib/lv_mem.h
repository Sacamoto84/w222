/**
 * @file lv_mem.h
 *
 */

#ifndef LV_MEM_H
#define LV_MEM_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#include "lv_string.h"

#include "../misc/lv_types.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef void * lv_mem_pool_t;

/**
 * Информационная структура кучи.
 */
typedef struct {
    size_t total_size;  /**< Total heap size */
    size_t free_cnt;
    size_t free_size;   /**< Size of available memory */
    size_t free_biggest_size;
    size_t used_cnt;
    size_t max_used;    /**< Max size of Heap memory used */
    uint8_t used_pct;   /**< Percentage used */
    uint8_t frag_pct;   /**< Amount of fragmentation */
} lv_mem_monitor_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Инициализируйте, чтобы использовать malloc/free/realloc и т. д. д.
 */
void lv_mem_init(void);

/**
 * Удалите всю динамически выделенную память и сбросьте состояние пулов памяти.
 */
void lv_mem_deinit(void);

lv_mem_pool_t lv_mem_add_pool(void * mem, size_t bytes);

void lv_mem_remove_pool(lv_mem_pool_t pool);

/**
 * Выделять память динамически
 * @param size запрошенный размер в байтах
 * @return указатель на выделенную неинициализированную память илиNULLв случае сбоя
 */
void * lv_malloc(size_t size);

/**
 * Динамически выделять блок обнулённой памяти
 * @param num запрошенное количество элементов, которые будут выделены.
 * @param size запрашиваемый размер каждого элемента в байтах.
 * @return указатель на выделенную обнуленную память илиNULLв случае сбоя
 */
void * lv_calloc(size_t num, size_t size);

/**
 * Динамически выделять обнуленную память
 * @param size запрошенный размер в байтах
 * @return указатель на выделенную обнуленную память илиNULLв случае сбоя
 */
void * lv_zalloc(size_t size);

/**
 * Динамически выделять обнуленную память
 * @param size запрошенный размер в байтах
 * @return указатель на выделенную обнуленную память илиNULLв случае сбоя
 */
void * lv_malloc_zeroed(size_t size);

/**
 * Освободить выделенные данные
 * @param data указатель на выделенную память
 */
void lv_free(void * data);

/**
 * Перераспределить память с новым размером. Старый контент сохранится.
 * @param data_p указатель на выделенную память.
 *               Его содержимое будет скопировано в новый блок памяти и освобождено.
 * @param new_size желаемый новый размер в байтах
 * @return указатель на новую память,NULLв случае сбоя
 */
void * lv_realloc(void * data_p, size_t new_size);

/**
 * Перераспределить память с новым размером. Старый контент сохранится.
 * В случае неудачи старый указатель освобождается.
 * @param data_p указатель на выделенную память.
 *               Его содержимое будет скопировано в новый блок памяти и освобождено.
 * @param new_size желаемый новый размер в байтах
 * @return указатель на новую память,NULLв случае сбоя
 */
void * lv_reallocf(void * data_p, size_t new_size);

/**
 * Используется внутри для выполнения простой операции`malloc`.
 * @param size      размер в байтах до `malloc`
 */
void * lv_malloc_core(size_t size);

/**
 * Используется внутри для выполнения простой операции`free`.
 * @param p      адрес памяти для освобождения
 */
void lv_free_core(void * p);

/**
 * Используется внутри для выполнения простой операции перераспределения.
 * @param p         адрес памяти для перераспределения
 * @param new_size  размер в байтах для перераспределения
 */
void * lv_realloc_core(void * p, size_t new_size);

/**
 * Используется внутриlv_mem_monitor() для сбора информации о состоянии кучи LVGL.
 * @param mon_p      указатель на объект lv_mem_monitor_t, который необходимо заполнить.
 */
void lv_mem_monitor_core(lv_mem_monitor_t * mon_p);

lv_result_t lv_mem_test_core(void);

/**
 * @brief Тестирует систему распределения памяти, выделяя и освобождая блок памяти.
 * @return LV_RESULT_OK, если система распределения памяти работает правильно, или LV_RESULT_INVALID, если возникла ошибка.
 */
lv_result_t lv_mem_test(void);

/**
 * Дайте информацию о рабочей памяти динамического распределения.
 * @param mon_p указатель на переменную lv_mem_monitor_t,
 *              результат анализа будет храниться здесь
 */
void lv_mem_monitor(lv_mem_monitor_t * mon_p);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_MEM_H*/
