/**
 * @file lv_ime_pinyin_private.h
 *
 */

#ifndef LV_IME_PINYIN_PRIVATE_H
#define LV_IME_PINYIN_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../core/lv_obj_private.h"
#include "lv_ime_pinyin.h"

#if LV_USE_IME_PINYIN != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Данные lv_ime_pinyin*/
struct _lv_ime_pinyin_t {
    lv_obj_t obj;
    lv_obj_t * kb;
    lv_obj_t * cand_panel;
    const lv_pinyin_dict_t * dict;
    lv_ll_t k9_legal_py_ll;
    char * cand_str;            /* Строка-кандидат */
    char   input_char[16];      /* Символ поля ввода */
#if LV_IME_PINYIN_USE_K9_MODE
    char   k9_input_str[LV_IME_PINYIN_K9_MAX_INPUT + 1]; /* Строка ввода режима 9-клавишного ввода (k9) */
    uint16_t k9_py_ll_pos;      /* Текущие страницы карты пиньинь (k9) */
    uint16_t k9_legal_py_count; /* Подсчет разрешенных номеров Пиньинь (k9) */
    uint16_t k9_input_str_len;  /* 9-клавишный режим ввода (k9) строка ввода макс. длина */
#endif
    uint16_t ta_count;          /* Количество символов, введенных в текстовое поле на этот раз */
    uint16_t cand_num;          /* Количество кандидатов */
    uint16_t py_page;           /* Текущие страницы карты пиньинь (k26) */
    uint16_t py_num[26];        /* Количество и длина пиньинь */
    uint16_t py_pos[26];        /* Позиция Пиньинь */
    lv_ime_pinyin_mode_t  mode; /* Режим установки: 1: 26-кнопочный ввод (k26), 0: 9-кнопочный ввод (k9). По умолчанию: 1. */
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_IME_PINYIN != 0 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_IME_PINYIN_PRIVATE_H*/
