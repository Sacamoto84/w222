/**
 * @file lv_vg_lite_pending.h
 *
 */

#ifndef LV_VG_LITE_PENDING_H
#define LV_VG_LITE_PENDING_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../misc/lv_types.h"

#if LV_USE_DRAW_VG_LITE

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct _lv_vg_lite_pending_t lv_vg_lite_pending_t;

typedef void (*lv_vg_lite_pending_free_cb_t)(void * obj, void * user_data);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создать список ожидания
 * @param obj_size the size of the objects in the list
 * @param capacity_default the default capacity of the list
 * @return a pointer to the pending list
 */
lv_vg_lite_pending_t * lv_vg_lite_pending_create(size_t obj_size, uint32_t capacity_default);

/**
 * Уничтожить ожидающий список
 * @param pending pointer to the pending list
 */
void lv_vg_lite_pending_destroy(lv_vg_lite_pending_t * pending);

/**
 * Установить бесплатный обратный вызов для списка ожидающих
 * @param pending pointer to the pending list
 * @param free_cb the free callback
 * @param user_data user data to pass to the free callback
 */
void lv_vg_lite_pending_set_free_cb(lv_vg_lite_pending_t * pending, lv_vg_lite_pending_free_cb_t free_cb,
                                    void * user_data);

/**
 * Добавить объект в список ожидающих
 * @param pending pointer to the pending list
 * @param obj pointer to the object to add
 */
void lv_vg_lite_pending_add(lv_vg_lite_pending_t * pending, void * obj);

/**
 * Удалить все объекты из списка активных ожидающих выполнения
 * @param pending pointer to the pending list
 */
void lv_vg_lite_pending_remove_all(lv_vg_lite_pending_t * pending);

/**
 * Удалите все ссылки на старые объекты и замените ссылки на новые объекты.
 * @param pending pointer to the pending list
 */
void lv_vg_lite_pending_swap(lv_vg_lite_pending_t * pending);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_VG_LITE*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_VG_LITE_PENDING_H*/
