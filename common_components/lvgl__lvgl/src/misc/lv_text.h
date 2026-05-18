/**
 * @file lv_text.h
 *
 */

#ifndef LV_TEXT_H
#define LV_TEXT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"
#include "lv_types.h"
#include "lv_area.h"
#include "../font/lv_font.h"
#include "../stdlib/lv_sprintf.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Параметры рендеринга текста.
 */
typedef enum {
    LV_TEXT_FLAG_NONE      = 0x00,

    /*Игнорируйте максимальную ширину, чтобы избежать автоматического переноса слов.*/
    LV_TEXT_FLAG_EXPAND    = 0x01,

    /**Max-width is already equal to the longest line. (Used to skip some calculation)*/
    LV_TEXT_FLAG_FIT       = 0x02,

    /**Чтобы предотвратить переполнение, вставляйте разрывы между любыми двумя символами.
    В противном случае разрывы вставляются по границам слов, как настроено через LV_TXT_BREAK_CHARS.
    или согласно LV_TXT_LINE_BREAK_LONG_LEN, LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN,
    и LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN.*/
    LV_TEXT_FLAG_BREAK_ALL = 0x04,

    /**Enable parsing of recolor command*/
    LV_TEXT_FLAG_RECOLOR   = 0x08,

} lv_text_flag_t;

/** Политика выравнивания меток*/
typedef enum {
    LV_TEXT_ALIGN_AUTO, /**< Align text auto*/
    LV_TEXT_ALIGN_LEFT, /**< Align text to left*/
    LV_TEXT_ALIGN_CENTER, /**< Align text to center*/
    LV_TEXT_ALIGN_RIGHT, /**< Align text to right*/
} lv_text_align_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Получить размер текста
 * @param size_res указатель на переменнуюpoint_tдля хранения результата
 * @param text указатель на текст
 * @param font указатель на шрифт текста
 * @param letter_space расстояние между буквами текста
 * @param line_space межстрочное пространство текста
 * @param max_width максимальная ширина текста (разорвите строки, чтобы они соответствовали этому размеру). Установите COORD_MAX, чтобы избежать
 * @param flag настройки текста из::lv_text_flag_t
 */
void lv_text_get_size(lv_point_t * size_res, const char * text, const lv_font_t * font, int32_t letter_space,
                      int32_t line_space, int32_t max_width, lv_text_flag_t flag);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TEXT_H*/
