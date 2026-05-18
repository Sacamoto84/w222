/**
 * @file lv_text_private.h
 *
 */

#ifndef LV_TEXT_PRIVATE_H
#define LV_TEXT_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_text.h"

/*********************
 *      DEFINES
 *********************/

#ifndef LV_TXT_COLOR_CMD
#define LV_TXT_COLOR_CMD "#"
#endif

#define LV_TXT_ENC_UTF8 1
#define LV_TXT_ENC_ASCII 2

#define LV_TEXT_LEN_MAX UINT32_MAX

/**********************
 *      TYPEDEFS
 **********************/

/** Конечный автомат для рендеринга текста. */
typedef enum {
    LV_TEXT_CMD_STATE_WAIT, /**< Waiting for command*/
    LV_TEXT_CMD_STATE_PAR,  /**< Processing the parameter*/
    LV_TEXT_CMD_STATE_IN,   /**< Processing the command*/
} lv_text_cmd_state_t;

typedef struct {
    int32_t letter_space;   /**< Letter space between letters*/
    int32_t line_space;     /**< Space between lines of text*/
    int32_t max_width;      /**< Max width of the text (break the lines to fit this size). Set COORD_MAX to avoid*/
    lv_text_flag_t text_flags;
} lv_text_attributes_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Инициализировать дескриптор текстовых атрибутов
 * @param attributes дескриптор текстовых атрибутов для инициализации
 */
void lv_text_attributes_init(lv_text_attributes_t * attributes);

/**
 * Получить размер текста
 * @param size_res указатель на переменнуюpoint_tдля хранения результата
 * @param text указатель на текст
 * @param font указатель на шрифт текста
 * @param attributes текстовые атрибуты, флаги для поведения разрыва строки, интервал и т. д.
 */
void lv_text_get_size_attributes(lv_point_t * size_res, const char * text, const lv_font_t * font,
                                 lv_text_attributes_t * attributes);
/**
 * Укажите длину текста заданным шрифтом с помощью текстовых флагов.
 * @param txt завершающая строка '\0'
 * @param length длина txt в байтах, а не в символах (Á — 1 символ, но 2 байта в
 * UTF-8)
 * @param font указатель на шрифт текста
 * @param attributes текстовые атрибуты, флаги для поведения разрыва строки, интервал и т. д.
 * @return длина длинного текста char_num
 */
int32_t lv_text_get_width(const char * txt, uint32_t length, const lv_font_t * font,
                          const lv_text_attributes_t * attributes);

/**
 * Проверьте, является ли c состоянием команды
 * Состояние @param
 * @param c
 * @return Истинно, если c является состоянием
 */
bool lv_text_is_cmd(lv_text_cmd_state_t * state, uint32_t c);

/**
 * Получите следующую строку текста. Проверьте длину строки и разбейте символы.
 * @param txt строка, завершающаяся '\0'
 * @param len длина txt в байтах
 * @param font указатель на шрифт
 * @param used_width Когдаused_width!= NULL, сохраните ширину этой линии, если
 * flag ==LV_TEXT_FLAG_NONE, иначе сохраните -1.
 * @param attributes текстовые атрибуты, флаги для управления поведением разрыва строки, интервалом и т. д.
 * @return индекс первого символа новой строки
 *         (в байтовом индексе, а не в буквенном индексе. С UTF -8 они разные)
 */
uint32_t lv_text_get_next_line(const char * txt, uint32_t len, const lv_font_t * font, int32_t * used_width,
                               lv_text_attributes_t * attributes);

/**
 * Вставить строку в другую
 * @param txt_buf исходный текст (должен быть достаточно большим для текста результата и завершаться NULL)
 * @param pos позиция для вставки (0: перед исходным текстом, 1: после первого символа и т. д.)
 * @param ins_txt текст для вставки должен заканчиваться '\0'
 */
void lv_text_ins(char * txt_buf, uint32_t pos, const char * ins_txt);

/**
 * Удалить часть строки
 * @param txt строка, которую нужно изменить, должна заканчиваться '\0' и указывать на кадр кучи или стека, а не на память только для чтения.
 * @param pos позиция, с которой следует начать удаление (0: перед первым символом, 1: после первого символа).
 * чар и т. д.)
 * @param len количество символов для удаления
 */
void lv_text_cut(char * txt, uint32_t pos, uint32_t len);

/**
 * Вернуть новый форматированный текст. Для хранения текста будет выделена память.
 * @param fmt `printf`-подобный формат
 * @param ap предметы для печати

 * @return указатель на выделенную текстовую строку.
 */
char * lv_text_set_text_vfmt(const char * fmt, va_list ap) LV_FORMAT_ATTRIBUTE(1, 0);

/**
 * Декодировать два закодированных символа из строки.
 * @param txt указатель на строку, завершающуюся '\0'
 * @param letter первый декодированный символ Юникода или 0 при неверном коде данных
 * @param letter_next второй декодированный символ Юникода или 0 при неверном коде данных
 * @param ofs индекс начала в «txt», с чего начать.
 *                После вызова он укажет следующий закодированный символ в формате «txt».
 *                NULL для использования txt[0] в качестве индекса
 */
void lv_text_encoded_letter_next_2(const char * txt, uint32_t * letter, uint32_t * letter_next, uint32_t * ofs);

/**
 * Проверьте, является ли символ разбиением на символ или нет (текст может быть сломан здесь или нет)
 * @param letter письмо
 * @return false: «буква» не является разрывным символом
 */
static inline bool lv_text_is_break_char(uint32_t letter)
{
    uint8_t i;
    bool ret = false;

    /*Сравните букву с TXT_BREAK_CHARS.*/
    for(i = 0; LV_TXT_BREAK_CHARS[i] != '\0'; i++) {
        if(letter == (uint32_t)LV_TXT_BREAK_CHARS[i]) {
            ret = true; /*Если совпадение, то это разрыв символа*/
            break;
        }
    }

    return ret;
}

/**
 * Проверьте, является ли символ разбиением на символ или нет (текст может быть сломан здесь или нет)
 * @param letter письмо
 * @return false: «буква» не является разрывным символом
 */
static inline bool lv_text_is_a_word(uint32_t letter)
{
    /*Дешевый чек на недействительную букву*/
    if(letter == 0) return false;

    /*CJK унифицированные иероглифы*/
    if(letter >= 0x4E00 && letter <= 0x9FFF) {
        return true;
    }

    /*Полноширинные варианты ASCII*/
    if(letter >= 0xFF01 && letter <= 0xFF5E) {
        return true;
    }

    /*CJK символы и знаки препинания*/
    if(letter >= 0x3000 && letter <= 0x303F) {
        return true;
    }

    /*CJK Добавка радикалов*/
    if(letter >= 0x2E80 && letter <= 0x2EFF) {
        return true;
    }

    /*CJK Удары*/
    if(letter >= 0x31C0 && letter <= 0x31EF) {
        return true;
    }

    /*Хирагана и Катакана*/
    if(letter >= 0x3040 && letter <= 0x30FF) {
        return true;
    }

    /*Китайские вертикальные формы*/
    if(letter >= 0xFE10 && letter <= 0xFE1F) {
        return true;
    }

    /*CJK Формы совместимости*/
    if(letter >= 0xFE30 && letter <= 0xFE4F) {
        return true;
    }

    return false;
}

/**
 * Проверьте, можно ли рассматривать символ как маркер и не нужно ли его отображать.
 * Обратите внимание, это не полный список. Добавьте свои выводы в список.
 *
 * @param letter письмо
 * @return правда, если так
 */
static inline bool lv_text_is_marker(uint32_t letter)
{
    if(letter < 0x20) return true;

    /*U+061CARABICLETTERMARK, см.  https://www.compart.com/en/unicode/block/U+0600*/
    if(letter == 0x061C) return true;

    /*U+115FHANGULCHOSEONGFILLER, см.  https://www.compart.com/en/unicode/block/U+1100*/
    if(letter == 0x115F) return true;
    /*U+1160 HANGUL JUNGSEONG FILLER*/
    if(letter == 0x1160) return true;

    /*См.  https://www.compart.com/en/unicode/block/U+1800.*/
    if(letter >= 0x180B && letter <= 0x180E) return true;

    /*См.  https://www.compart.com/en/unicode/block/U+2000.*/
    if(letter >= 0x200B && letter <= 0x200F) return true;
    if(letter >= 0x2028 && letter <= 0x202F) return true;
    if(letter >= 0x205F && letter <= 0x206F) return true;

    /*U+FEFFZEROWIDTHNO -BREAKSPACE , см.  https://www.compart.com/en/unicode/block/U+FE70*/
    if(letter == 0xFEFF) return true;

    if(letter == 0xF8FF) return true; /*LV_SYMBOL_DUMMY*/

    return false;
}

/***************************************************************
 *  GLOBAL FUNCTION POINTERS FOR CHARACTER ENCODING INTERFACE
 ***************************************************************/

/**
 * Укажите размер закодированного символа
 * @param txt  указатель на символ в строке
 * @return длина закодированного символа (1,2,3...). О недействительно
 */
extern uint8_t (*const lv_text_encoded_size)(const char * txt);

/**
 * Преобразовать букву Юникода в закодированную
 * @param letter_uni буква Юникода
 * @return Символ, закодированный в Little Endian, для совместимости с символами C (например, 'Á', 'Ü').
 */
extern uint32_t (*const lv_text_unicode_to_encoded)(uint32_t letter_uni);

/**
 * Преобразование широкого символа, например. 'Á' с прямым порядком байтов для совместимости с закодированным форматом.
 * @param c широкий характер
 * @return `c` в закодированном формате
 */
extern uint32_t (*const lv_text_encoded_conv_wc)(uint32_t c);

/**
 * Декодируйте следующий закодированный символ строки.
 * @param i_start      указатель на строку, завершающуюся '\0'
 * @param i_start  индекс начала в «txt», с чего начать.
 *                 После вызова он укажет следующий закодированный символ в формате «txt».
 *                 NULL для использования txt[0] в качестве индекса
 * @return декодированный символ Юникода или 0 при неверном коде данных
 */
extern uint32_t (*const lv_text_encoded_next)(const char * txt, uint32_t * i_start);

/**
 * Получить предыдущий закодированный символ из строки.
 *
 * @param  i_start      указатель на строку, завершающуюся '\0'
 * @param  i_start  индекс в «txt», с чего начать. После вызова он укажет на предыдущий
 *                    закодированный символ в формате «txt».
 *
 * @return декодированный символ Юникода или 0 для неверных данных
 */
extern uint32_t (*const lv_text_encoded_prev)(const char * txt, uint32_t * i_start);

/**
 * Преобразовать буквенный индекс (в закодированном тексте) в байтовый индекс.
 * например в UTF -8 индекс «AÁRT» для «R» равен 2, но начинается с байта 3, потому что длина «Á» составляет 2 байта.
 * @param utf8_id строкаUTF-8, завершающаяся '\0'
 * @param utf8_id индекс символов
 * @return индекс байта 'enc_id'-ой буквы
 */
extern uint32_t (*const lv_text_encoded_get_byte_id)(const char * txt, uint32_t utf8_id);

/**
 * Преобразуйте индекс байта (в закодированном тексте) в индекс символа.
 * например в UTF -8 индекс «AÁRT» для «R» равен 2, но начинается с байта 3, потому что длина «Á» составляет 2 байта.
 * @param byte_id строкаUTF-8, завершающаяся '\0'
 * @param byte_id байтовый индекс
 * @return индекс символа буквы в 'byte_id'-ой позиции
 */
extern uint32_t (*const lv_text_encoded_get_char_id)(const char * txt, uint32_t byte_id);

/**
 * Получите количество символов (и байтов NOT) в строке.
 * например в UTF -8 "ABC" - 3 символа (но 4 байта)
 * @param txt символьная строка, завершающаяся '\0'
 * @return количество символов
 */
extern uint32_t (*const lv_text_get_encoded_length)(const char * txt);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_TEXT_PRIVATE_H*/
