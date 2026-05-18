/**
 * @file lv_utils.h
 *
 */

#ifndef LV_UTILS_H
#define LV_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_types.h"
#include "../draw/lv_draw_buf.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/** Ищет от base[0] до base[n - 1] элемента, соответствующего *key.
 *
 * @note Функция cmp должна возвращать отрицательное значение, если она первая.
 *  аргумент (ключ поиска) меньше второго (запись таблицы),
 *  ноль, если они равны, и положительный, если больше.
 *
 * @note Элементы массива должны располагаться в порядке возрастания.
 *
 * @param key    Указатель на искомый элемент
 * @param base   Указатель на первый элемент для поиска
 * @param n      Количество элементов
 * @param size   Размер каждого элемента
 * @param cmp    Указатель на функцию сравнения (см. unicode_list_compare()
 *                 в качестве примера функции сравнения)
 *
 * @return указатель на соответствующий элемент или NULL, если его не существует.
 */
void * lv_utils_bsearch(const void * key, const void * base, size_t n, size_t size,
                        int (*cmp)(const void * pRef, const void * pElement));

/**
 * Сохраните рисунок в файл
 * @param draw_buf  указатель на буфер рисования
 * @param path      путь к файлу для сохранения
 * @return          LV_RESULT_OK: успех;  LV_RESULT_INVALID: ошибка
 */
lv_result_t lv_draw_buf_save_to_file(const lv_draw_buf_t * draw_buf, const char * path);

/**
 * Измените порядок байтов в 32-битном значении.
 * @param x     32-битное значение.
 * @return      значение`x`с обратным порядком байтов.
 */
static inline uint32_t lv_swap_bytes_32(uint32_t x)
{
    return (x << 24)
           | ((x & 0x0000ff00U) <<  8)
           | ((x & 0x00ff0000U) >>  8)
           | (x >> 24);
}

/**
 * Измените порядок байтов в 16-битном значении.
 * @param x     16-битное значение.
 * @return      значение`x`с обратным порядком байтов.
 */
static inline uint16_t lv_swap_bytes_16(uint16_t x)
{
    return (x << 8) | (x >> 8);
}

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
