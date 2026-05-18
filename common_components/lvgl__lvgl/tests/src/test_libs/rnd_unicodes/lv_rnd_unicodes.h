/**
* @file lv_rnd_unicodes.h
*
*/
#if LV_BUILD_TEST

#ifndef LV_RND_UNICODES_H
#define LV_RND_UNICODES_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../../lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

extern const uint32_t LV_RND_UNICODE_ALPHANUM_AND_CJK_TABLE[];
extern const uint32_t LV_RND_UNICODE_ALPHANUM_AND_CJK_TABLE_LEN;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Генерировать случайные символы UTF -8 в заданных диапазонах.
 * @param buf         буфер для хранения сгенерированных символов
 * @param buf_len     длина буфера
 * @param ranges      массив диапазонов на выбор
 * @param range_num   количество диапазонов
 * @param char_num    количество символов для генерации
 * @return количество сгенерированных символов в байтах
 */
int lv_random_utf8_chars(uint8_t * buf, int buf_len, const uint32_t * ranges, uint32_t range_num, int char_num);

/*************************
 *    GLOBAL VARIABLES
 *************************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_RND_UNICODES_H*/

#endif /*LV_BUILD_TEST*/
