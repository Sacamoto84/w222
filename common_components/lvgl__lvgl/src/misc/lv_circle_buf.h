/**
* @file lv_circle_buf.h
*
 */


#ifndef LV_CIRCLE_BUF_H
#define LV_CIRCLE_BUF_H

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

typedef bool (*lv_circle_buf_fill_cb_t)(void * buf, uint32_t buff_len, int32_t index, void * user_data);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создайте круговой буфер
 * @param capacity максимальное количество элементов в буфере
 * @param element_size размер элемента в байтах
 * @return указатель на созданный буфер
 */
lv_circle_buf_t * lv_circle_buf_create(uint32_t capacity, uint32_t element_size);

/**
 * Создайте круговой буфер из существующего буфера
 * @param buf указатель на буфер
 * @param capacity максимальное количество элементов в буфере
 * @param element_size размер элемента в байтах
 * @return указатель на созданный буфер
 */
lv_circle_buf_t * lv_circle_buf_create_from_buf(void * buf, uint32_t capacity, uint32_t element_size);

/**
 * Создайте круговой буфер из существующего массива
 * @param array указатель на массив
 * @return указатель на созданный буфер
 */
lv_circle_buf_t * lv_circle_buf_create_from_array(const lv_array_t * array);

/**
 * Изменить размер буфера
 * @param circle_buf указатель на буфер
 * @param capacity новая емкость буфера
 * @return LV_RESULT_OK : размер буфера изменен;  LV_RESULT_INVALID: размер буфера не изменяется.
 */
lv_result_t lv_circle_buf_resize(lv_circle_buf_t * circle_buf, uint32_t capacity);

/**
 * Уничтожить буфер круга
 * @param circle_buf указатель на буфер
 */
void lv_circle_buf_destroy(lv_circle_buf_t * circle_buf);

/**
 * Получить размер буфера
 * @param circle_buf указатель на буфер
 * @return количество элементов в буфере
 */
uint32_t lv_circle_buf_size(const lv_circle_buf_t * circle_buf);

/**
 * Получить емкость буфера
 * @param circle_buf указатель на буфер
 * @return максимальное количество элементов в буфере
 */
uint32_t lv_circle_buf_capacity(const lv_circle_buf_t * circle_buf);

/**
 * Получить оставшееся место в буфере
 * @param circle_buf указатель на буфер
 * @return количество элементов, которые можно записать в буфер
 */
uint32_t lv_circle_buf_remain(const lv_circle_buf_t * circle_buf);

/**
 * Проверьте, пуст ли буфер
 * @param circle_buf указатель на буфер
 * @return true: буфер пуст; false: буфер не пуст
 */
bool lv_circle_buf_is_empty(const lv_circle_buf_t * circle_buf);

/**
 * Проверьте, заполнен ли буфер
 * @param circle_buf указатель на буфер
 * @return true: буфер заполнен; false: буфер не заполнен
 */
bool lv_circle_buf_is_full(const lv_circle_buf_t * circle_buf);

/**
 * Сбросить буфер
 * @param circle_buf указатель на буфер
 * @return LV_RESULT_OK : буфер сбрасывается;  LV_RESULT_INVALID: буфер не сбрасывается
 */
void lv_circle_buf_reset(lv_circle_buf_t * circle_buf);

/**
 * Получить голову буфера
 * @param circle_buf указатель на буфер
 * @return указатель на заголовок буфера
 */
void * lv_circle_buf_head(const lv_circle_buf_t * circle_buf);

/**
 * Получить хвост буфера
 * @param circle_buf указатель на буфер
 * @return указатель на конец буфера
 */
void * lv_circle_buf_tail(const lv_circle_buf_t * circle_buf);

/**
 * Чтение значения
 * @param circle_buf указатель на буфер
 * @param data указатель на переменную для хранения прочитанного значения
 * @return LV_RESULT_OK : значение считывается;  LV_RESULT_INVALID: значение не читается
 */
lv_result_t lv_circle_buf_read(lv_circle_buf_t * circle_buf, void * data);

/**
 * Напишите значение
 * @param circle_buf указатель на буфер
 * @param data указатель на значение для записи
 * @return LV_RESULT_OK: значение записывается;  LV_RESULT_INVALID: значение не записано
 */
lv_result_t lv_circle_buf_write(lv_circle_buf_t * circle_buf, const void * data);

/**
 * Заполните буфер значениями
 * @param circle_buf указатель на буфер
 * @param count количество значений для заполнения
 * @param fill_cb функция обратного вызова для заполнения буфера
 * @param user_data
 * @return количество заполненных значений
 */
uint32_t lv_circle_buf_fill(lv_circle_buf_t * circle_buf, uint32_t count, lv_circle_buf_fill_cb_t fill_cb,
                            void * user_data);

/**
 * Пропустить значение
 * @param circle_buf указатель на буфер
 * @return LV_RESULT_OK : значение пропускается;  LV_RESULT_INVALID: значение не пропускается
 */
lv_result_t lv_circle_buf_skip(lv_circle_buf_t * circle_buf);

/**
 * Посмотреть значение
 * @param circle_buf указатель на буфер
 * @param data указатель на переменную для хранения просмотренного значения
 * @return LV_RESULT_OK : значение просматривается;  LV_RESULT_INVALID: значение не просматривается
 */
lv_result_t lv_circle_buf_peek(const lv_circle_buf_t * circle_buf, void * data);

/**
 * Просмотр значения по индексу
 * @param circle_buf указатель на буфер
 * @param index индекс значения для просмотра. Если индекс больше размера буфера, он будет возвращаться циклически.
 * @param data указатель на переменную для хранения просмотренного значения
 * @return LV_RESULT_OK : значение просматривается;  LV_RESULT_INVALID: значение не просматривается
 */
lv_result_t lv_circle_buf_peek_at(const lv_circle_buf_t * circle_buf, uint32_t index, void * data);

/*************************
 *    GLOBAL VARIABLES
 *************************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CIRCLE_BUF_H*/
