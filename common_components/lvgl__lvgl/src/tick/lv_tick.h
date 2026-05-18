/**
 * @file lv_tick.h
 * Обеспечить доступ к системному тику с разрешением 1 миллисекунду
 */

#ifndef LV_TICK_H
#define LV_TICK_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#include "../misc/lv_types.h"

/*********************
 *      DEFINES
 *********************/
#ifndef LV_ATTRIBUTE_TICK_INC
#define LV_ATTRIBUTE_TICK_INC
#endif

/**********************
 *      TYPEDEFS
 **********************/
typedef uint32_t (*lv_tick_get_cb_t)(void);

typedef void (*lv_delay_cb_t)(uint32_t ms);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Вам придется периодически вызывать эту функцию.
 * Обычно безопасно вызывать из обработчика прерывания или другого потока.
 * @param tick_period   период вызова этой функции в миллисекундах
 */
LV_ATTRIBUTE_TICK_INC void lv_tick_inc(uint32_t tick_period);

/**
 * Получите количество миллисекунд, прошедших с момента запуска
 * @return          прошедшие миллисекунды
 */
uint32_t lv_tick_get(void);

/**
 * Получите количество миллисекунд, прошедших с момента предыдущей отметки времени.
 * @param prev_tick     предыдущая отметка времени (возвращаемое значениеlv_tick_get())
 * @return              количество миллисекунд, прошедших с момента 'prev_tick'
 */
uint32_t lv_tick_elaps(uint32_t prev_tick);

/**
 * Получите прошедшее миллисекунды между двумя отметками времени
 * @param tick          отметка времени
 * @param prev_tick     отметка времени перед `tick`
 * @return              прошедшие миллисекунды между`prev_tick`и `tick`
 */
uint32_t lv_tick_diff(uint32_t tick, uint32_t prev_tick);

/**
 * Задержка для заданных миллисекунд.
 * По умолчанию это блокирующая задержка, но с `lv_delay_set_cb()`
 * также можно установить пользовательскую функцию задержки
 * @param ms        количество миллисекунд задержки
 */
void lv_delay_ms(uint32_t ms);

/**
 * Установите обратный вызов для задержки блокировки
 * @param cb        указатель на обратный вызов
 */
void lv_delay_set_cb(lv_delay_cb_t cb);

/**
 * Установите пользовательский обратный вызов для «lv_tick_get»
 * @param cb        вызовите этот обратный вызов на 'lv_tick_get'
 */
void lv_tick_set_cb(lv_tick_get_cb_t cb);

/**
 * Получите собственный обратный вызов для «lv_tick_get»
 * @return      вызовите этот обратный вызов на 'lv_tick_get'
 */
lv_tick_get_cb_t lv_tick_get_cb(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_TICK_H*/
