/**
 * @file lv_observer_private.h
 *
 */

#ifndef LV_OBSERVER_PRIVATE_H
#define LV_OBSERVER_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_observer.h"

#if LV_USE_OBSERVER

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Объект наблюдателя: дескриптор, возвращаемый при подписке виджетов LVGL на субъектов.
 */
struct _lv_observer_t {
    lv_subject_t * subject;             /**< Наблюдаемый субъект */
    lv_observer_cb_t cb;                /**< Обратный вызов при изменении значения */
    void * target;                      /**< Цель наблюдателя: виджет или любой указатель */
    void * user_data;                   /**< Дополнительный параметр при подписке */
    uint32_t auto_free_user_data : 1;   /**< Автоматически освобождать user_data при удалении наблюдателя */
    uint32_t notified : 1;              /**< Наблюдатель уже был уведомлен */
    uint32_t for_obj : 1;               /**< `target` указывает на виджет (`lv_obj_t *`) */
};

/**
 * Дескриптор создан `lv_obj_add_subject_increment_event()`
 */
struct _lv_subject_increment_dsc_t {
    lv_subject_t * subject; /**< Субъект, который нужно изменять*/
    int32_t step;           /**< Шаг, добавляемый к субъекту */
    bool rollover;          /**< Переходить на другой край при выходе за границу*/
    int32_t min_value;      /**< Не устанавливать значение меньше этого */
    int32_t max_value;      /**< Не устанавливать значение больше этого */
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_OBSERVER */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OBSERVER_PRIVATE_H*/
