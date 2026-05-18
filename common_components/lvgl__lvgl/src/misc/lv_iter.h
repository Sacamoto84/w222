/**
* @file lv_iter.h
*
 */


#ifndef LV_ITER_H
#define LV_ITER_H

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

typedef lv_result_t (*lv_iter_next_cb)(void * instance, void * context, void * elem);
typedef void (*lv_iter_inspect_cb)(void * elem);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создайте итератор на основе экземпляра, а затем следующий элемент итератора можно будет получить черезlv_iter_next,
 * Чтобы получить следующую операцию унифицированным и абстрактным способом.
 * @param instance       Экземпляр для итерации
 * @param elem_size      Размер элемента, который будет повторяться в байтах
 * @param context_size   Размер контекста, передаваемого в next_cb, в байтах.
 * @param next_cb        Функция обратного вызова для получения следующего элемента
 * @return               Объект итератора
 */
lv_iter_t * lv_iter_create(void * instance, uint32_t elem_size, uint32_t context_size, lv_iter_next_cb next_cb);

/**
 * Получите контекст итератора. Вы можете использовать его для хранения некоторых временных переменных, связанных с текущим итератором.
 * @param iter           Объект`lv_iter_t`создан ранее
 * @return контекст iter
 */
void * lv_iter_get_context(const lv_iter_t * iter);

/**
 * Уничтожьте объект итератора и освободите контекст. Другие ресурсы, выделенные пользователем, не освобождаются.
 * Пользователю необходимо освободить его самостоятельно.
 * @param iter          Объект`lv_iter_t`создан ранее
 */
void lv_iter_destroy(lv_iter_t * iter);

/**
 * Получите следующий элемент итератора.
 * @param iter          Объект`lv_iter_t`создан ранее
 * @param elem          Указатель для сохранения следующего элемента
 * @return              LV_RESULT_OK: успешно получить следующий элемент.
 *                      LV_RESULT_INVALID: Следующий элемент недействителен
 */
lv_result_t lv_iter_next(lv_iter_t * iter, void * elem);

/**
 * Сделайте итератор доступным для просмотра, что означает, что пользователь может просматривать следующий элемент, не перемещая итератор.
 * @param iter          Объект`lv_iter_t`создан ранее
 * @param capacity      Емкость буфера просмотра
 */
void lv_iter_make_peekable(lv_iter_t * iter, uint32_t capacity);

/**
 * Просмотрите следующий элемент итератора, не перемещая итератор вперед.
 * @param iter          Объект`lv_iter_t`создан ранее
 * @param elem          Указатель для сохранения следующего элемента
 * @return              LV_RESULT_OK: успешно просмотреть следующий элемент.
 *                      LV_RESULT_INVALID: Следующий элемент недействителен
 */
lv_result_t lv_iter_peek(lv_iter_t * iter, void * elem);

/**
 * Только продвигайте итератор без получения следующего элемента.
 * @param iter          Объект`lv_iter_t`создан ранее
 * @return              LV_RESULT_OK: успешно просмотреть следующий элемент.
 *                      LV_RESULT_INVALID: Следующий элемент недействителен
 */
lv_result_t lv_iter_peek_advance(lv_iter_t * iter);

/**
 * Сбросьте взгляд на одежду `next`.
 * @param iter          Объект`lv_iter_t`создан ранее
 * @return              LV_RESULT_OK: успешно сбросить буфер просмотра.
 *                      LV_RESULT_INVALID: Буфер просмотра недействителен.
 */
lv_result_t lv_iter_peek_reset(lv_iter_t * iter);

/**
 * Проверьте элемент итератора. Функция обратного вызова будет вызываться для каждого элемента итератора.
 * @param iter          Объект`lv_iter_t`создан ранее
 * @param inspect_cb    Функция обратного вызова для проверки элемента
 */
void lv_iter_inspect(lv_iter_t * iter, lv_iter_inspect_cb inspect_cb);

/*************************
 *    GLOBAL VARIABLES
 *************************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_ITER_H*/
