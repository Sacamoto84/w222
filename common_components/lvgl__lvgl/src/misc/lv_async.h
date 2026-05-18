/**
 * @file lv_async.h
 *
 */

#ifndef LV_ASYNC_H
#define LV_ASYNC_H

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

/**
 * Введите асинхронный обратный вызов.
 */
typedef void (*lv_async_cb_t)(void *);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Вызовите асинхронную функцию при следующем запускеlv_timer_handler(). Эта функция, скорее всего, вернет
 * **до того, как** действительно произойдет звонок!
 * @param async_xcb обратный вызов, который является самой задачей.
 *                 (знак «x» в имени аргумента указывает на то, что это не полностью универсальная функция, поскольку она не следует
 *                  соглашение`func_name(object, callback, ...)`)
 * @param user_data специальный параметр
 */
lv_result_t lv_async_call(lv_async_cb_t async_xcb, void * user_data);

/**
 * Отменить вызов асинхронной функции
 * @param async_xcb обратный вызов, который является самой задачей.
 * @param user_data специальный параметр
 */
lv_result_t lv_async_call_cancel(lv_async_cb_t async_xcb, void * user_data);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_ASYNC_H*/
