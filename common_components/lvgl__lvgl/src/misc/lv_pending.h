/**
 * @file lv_pending.h
 *
 */

#ifndef LV_PENDING_H
#define LV_PENDING_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_types.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct _lv_pending_t lv_pending_t;

typedef void (*lv_pending_free_cb_t)(void * obj, void * user_data);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создать список ожидания
 * @param obj_size размер объектов в списке
 * @param capacity_default емкость списка по умолчанию
 * @return указатель на ожидающий список
 */
lv_pending_t * lv_pending_create(size_t obj_size, uint32_t capacity_default);

/**
 * Уничтожить ожидающий список
 * @param pending указатель на список ожидания
 */
void lv_pending_destroy(lv_pending_t * pending);

/**
 * Установить бесплатный обратный вызов для списка ожидающих
 * @param pending указатель на список ожидания
 * @param free_cb бесплатный обратный звонок
 * @param user_data пользовательские данные для передачи в бесплатный обратный вызов
 */
void lv_pending_set_free_cb(lv_pending_t * pending, lv_pending_free_cb_t free_cb,
                            void * user_data);

/**
 * Добавить объект в список ожидающих
 * @param pending указатель на список ожидания
 * @param obj указатель на объект, который нужно добавить
 */
void lv_pending_add(lv_pending_t * pending, void * obj);

/**
 * Удалить все объекты из обоих ожидающих списков
 * @param pending указатель на список ожидания
 */
void lv_pending_remove_all(lv_pending_t * pending);

/**
 * Удалите все ссылки на старые объекты и замените ссылки на новые объекты.
 * @param pending указатель на список ожидания
 */
void lv_pending_swap(lv_pending_t * pending);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_PENDING_H*/
