/**
 * @file lv_bidi_private.h
 *
 */

#ifndef LV_BIDI_PRIVATE_H
#define LV_BIDI_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_bidi.h"
#if LV_USE_BIDI

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Преобразуйте текст, чтобы расположить символы в правильном визуальном порядке в соответствии с
 * Двунаправленный алгоритм Unicode
 * @param str_in текст для обработки
 * @param str_out сохраните результат здесь. Имеет длину `strlen(str_in)`
 * @param base_dir `LV_BASE_DIR_LTR` или `LV_BASE_DIR_RTL`
 */
void lv_bidi_process(const char * str_in, char * str_out, lv_base_dir_t base_dir);

/**
 * Автоматическое определение направления текста по первому сильному символу
 * @param txt текст для обработки
 * @return `LV_BASE_DIR_LTR` или `LV_BASE_DIR_RTL`
 */
lv_base_dir_t lv_bidi_detect_base_dir(const char * txt);

/**
 * Получить логическое положение символа в строке
 * @param str_in входная строка. Может быть только одна строка.
 * @param bidi_txt внутри текст обрабатывается двунаправленным текстом, какой буфер можно получить здесь.
 * Если большего не требуется, необходимо поднять с помощью `lv_free()`.
 * Может быть `NULL` не используется
 * @param len длина строки в количестве символов
 * @param base_dir базовое направление текста:`LV_BASE_DIR_LTR`или `LV_BASE_DIR_RTL`
 * @param visual_pos позиция визуального персонажа, какую логическую позицию следует получить
 * @param is_rtl сообщите, что символ в`visual_pos`является контекстомRTLили LTR
 * @return логическая позиция персонажа
 */
uint16_t lv_bidi_get_logical_pos(const char * str_in, char ** bidi_txt, uint32_t len, lv_base_dir_t base_dir,
                                 uint32_t visual_pos, bool * is_rtl);

/**
 * Получить визуальное положение персонажа в строке
 * @param str_in входная строка. Может быть только одна строка.
 * @param bidi_txt внутри текст обрабатывается двунаправленным текстом, какой буфер можно получить здесь.
 * Если большего не требуется, необходимо поднять с помощью `lv_free()`.
 * Может быть `NULL` не используется
 * @param len длина строки в количестве символов
 * @param base_dir базовое направление текста:`LV_BASE_DIR_LTR`или `LV_BASE_DIR_RTL`
 * @param logical_pos логическая позиция персонажа, какую визуальную позицию следует получить
 * @param is_rtl сообщите, что символ в`logical_pos`является контекстомRTLили LTR
 * @return визуальное положение персонажа
 */
uint16_t lv_bidi_get_visual_pos(const char * str_in, char ** bidi_txt, uint16_t len, lv_base_dir_t base_dir,
                                uint32_t logical_pos, bool * is_rtl);

/**
 * Биди обрабатывает абзац текста
 * @param str_in строка для обработки
 * @param str_out сохранить результат здесь
 * @param len длина текста
 * @param base_dir базовая директория текста
 * @param pos_conv_out массив`uint16_t`для хранения соответствующей логической позиции символа.
 * Может быть `NULL` не используется
 * @param pos_conv_len длина`pos_conv_out`в количестве элементов
 */
void lv_bidi_process_paragraph(const char * str_in, char * str_out, uint32_t len, lv_base_dir_t base_dir,
                               uint16_t * pos_conv_out, uint16_t pos_conv_len);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_BIDI*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_BIDI_PRIVATE_H*/
