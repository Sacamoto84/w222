/**
 * @file lv_text.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_text_private.h"
#include "lv_text_ap.h"
#include "lv_math.h"
#include "lv_log.h"
#include "lv_assert.h"
#include "../stdlib/lv_mem.h"
#include "../stdlib/lv_string.h"
#include "../misc/lv_types.h"

/*********************
 *      DEFINES
 *********************/
#define NO_BREAK_FOUND UINT32_MAX

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

#if LV_TXT_ENC == LV_TXT_ENC_UTF8
    static uint8_t lv_text_utf8_size(const char * str);
    static uint32_t lv_text_unicode_to_utf8(uint32_t letter_uni);
    static uint32_t lv_text_utf8_conv_wc(uint32_t c);
    static uint32_t lv_text_utf8_next(const char * txt, uint32_t * i);
    static uint32_t lv_text_utf8_prev(const char * txt, uint32_t * i_start);
    static uint32_t lv_text_utf8_get_byte_id(const char * txt, uint32_t utf8_id);
    static uint32_t lv_text_utf8_get_char_id(const char * txt, uint32_t byte_id);
    static uint32_t lv_text_utf8_get_length(const char * txt);
#elif LV_TXT_ENC == LV_TXT_ENC_ASCII
    static uint8_t lv_text_iso8859_1_size(const char * str);
    static uint32_t lv_text_unicode_to_iso8859_1(uint32_t letter_uni);
    static uint32_t lv_text_iso8859_1_conv_wc(uint32_t c);
    static uint32_t lv_text_iso8859_1_next(const char * txt, uint32_t * i);
    static uint32_t lv_text_iso8859_1_prev(const char * txt, uint32_t * i_start);
    static uint32_t lv_text_iso8859_1_get_byte_id(const char * txt, uint32_t utf8_id);
    static uint32_t lv_text_iso8859_1_get_char_id(const char * txt, uint32_t byte_id);
    static uint32_t lv_text_iso8859_1_get_length(const char * txt);
#endif
/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *  GLOBAL VARIABLES
 **********************/
#if LV_TXT_ENC == LV_TXT_ENC_UTF8
    uint8_t (*const lv_text_encoded_size)(const char *)                   = lv_text_utf8_size;
    uint32_t (*const lv_text_unicode_to_encoded)(uint32_t)                = lv_text_unicode_to_utf8;
    uint32_t (*const lv_text_encoded_conv_wc)(uint32_t)                   = lv_text_utf8_conv_wc;
    uint32_t (*const lv_text_encoded_next)(const char *, uint32_t *)      = lv_text_utf8_next;
    uint32_t (*const lv_text_encoded_prev)(const char *, uint32_t *)      = lv_text_utf8_prev;
    uint32_t (*const lv_text_encoded_get_byte_id)(const char *, uint32_t) = lv_text_utf8_get_byte_id;
    uint32_t (*const lv_text_encoded_get_char_id)(const char *, uint32_t) = lv_text_utf8_get_char_id;
    uint32_t (*const lv_text_get_encoded_length)(const char *)            = lv_text_utf8_get_length;
#elif LV_TXT_ENC == LV_TXT_ENC_ASCII
    uint8_t (*const lv_text_encoded_size)(const char *)                   = lv_text_iso8859_1_size;
    uint32_t (*const lv_text_unicode_to_encoded)(uint32_t)                = lv_text_unicode_to_iso8859_1;
    uint32_t (*const lv_text_encoded_conv_wc)(uint32_t)                   = lv_text_iso8859_1_conv_wc;
    uint32_t (*const lv_text_encoded_next)(const char *, uint32_t *)      = lv_text_iso8859_1_next;
    uint32_t (*const lv_text_encoded_prev)(const char *, uint32_t *)      = lv_text_iso8859_1_prev;
    uint32_t (*const lv_text_encoded_get_byte_id)(const char *, uint32_t) = lv_text_iso8859_1_get_byte_id;
    uint32_t (*const lv_text_encoded_get_char_id)(const char *, uint32_t)     = lv_text_iso8859_1_get_char_id;
    uint32_t (*const lv_text_get_encoded_length)(const char *)            = lv_text_iso8859_1_get_length;

#endif

/**********************
 *      MACROS
 **********************/

#define LV_IS_ASCII(value)              ((value & 0x80U) == 0x00U)
#define LV_IS_2BYTES_UTF8_CODE(value)   ((value & 0xE0U) == 0xC0U)
#define LV_IS_3BYTES_UTF8_CODE(value)   ((value & 0xF0U) == 0xE0U)
#define LV_IS_4BYTES_UTF8_CODE(value)   ((value & 0xF8U) == 0xF0U)
#define LV_IS_INVALID_UTF8_CODE(value)  ((value & 0xC0U) != 0x80U)

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_text_attributes_init(lv_text_attributes_t * attributes)
{
    lv_memzero(attributes, sizeof(lv_text_attributes_t));
}

void lv_text_get_size(lv_point_t * size_res, const char * text, const lv_font_t * font, int32_t letter_space,
                      int32_t line_space, int32_t max_width, lv_text_flag_t flag)
{
    lv_text_attributes_t attrs;
    lv_text_attributes_init(&attrs);
    attrs.line_space = line_space;
    attrs.max_width = max_width;
    attrs.text_flags = flag;
    attrs.letter_space = letter_space;
    lv_text_get_size_attributes(size_res, text, font, &attrs);
}

void lv_text_get_size_attributes(lv_point_t * size_res, const char * text, const lv_font_t * font,
                                 lv_text_attributes_t * attributes)
{
    uint32_t line_start     = 0;
    uint32_t new_line_start = 0;
    uint16_t letter_height  = 0;
    size_res->x = 0;
    size_res->y = 0;

    LV_ASSERT_NULL(attributes);
    LV_ASSERT_NULL(font);
    LV_ASSERT_NULL(text);

    letter_height = lv_font_get_line_height(font);

    if(attributes->text_flags & LV_TEXT_FLAG_EXPAND) {
        attributes->max_width = LV_COORD_MAX;
    }

    /*Расчет высота и самая длинная линия*/
    while(text[line_start] != '\0') {
        new_line_start += lv_text_get_next_line(
                              &text[line_start], LV_TEXT_LEN_MAX, font, NULL, attributes);

        if((unsigned long)size_res->y +
           (unsigned long)letter_height + (unsigned long)attributes->line_space > LV_MAX_OF(int32_t)) {
            LV_LOG_WARN("integer overflow while calculating text height");
            return;
        }
        else {
            size_res->y += letter_height;
            size_res->y += attributes->line_space;
        }

        /*Вычислить самую длинную линию*/
        int32_t act_line_length = lv_text_get_width(
                                      &text[line_start], new_line_start - line_start, font, attributes);

        size_res->x = LV_MAX(act_line_length, size_res->x);
        line_start  = new_line_start;
    }

    /*Сделайте текст на одну строку выше, если последний символ — «\n» или «\r».*/
    if((line_start != 0) && (text[line_start - 1] == '\n' || text[line_start - 1] == '\r')) {
        size_res->y += letter_height + attributes->line_space;
    }

    /*Исправление с последним межстрочным интервалом или установка высоты вручную, если текст пустой*/
    if(size_res->y == 0)
        size_res->y = letter_height;
    else
        size_res->y -= attributes->line_space;
}

bool lv_text_is_cmd(lv_text_cmd_state_t * state, uint32_t c)
{
    bool ret = false;

    if(c == (uint32_t)LV_TXT_COLOR_CMD[0]) {
        if(*state == LV_TEXT_CMD_STATE_WAIT) { /*Начать символ*/
            *state = LV_TEXT_CMD_STATE_PAR;
            ret = true;
        }
        /*Другой начальный символ в параметре отображается cmd. голец*/
        else if(*state == LV_TEXT_CMD_STATE_WAIT) {
            *state = LV_TEXT_CMD_STATE_WAIT;
        }
        /*Конец команды*/
        else if(*state == LV_TEXT_CMD_STATE_IN) {
            *state = LV_TEXT_CMD_STATE_WAIT;
            ret = true;
        }
    }

    /*Пропустите параметр цвета и подождите пробел после него.*/
    if(*state == LV_TEXT_CMD_STATE_PAR) {
        if(c == ' ') {
            *state = LV_TEXT_CMD_STATE_IN; /*После параметра текст находится в команде*/
        }
        ret = true;
    }

    return ret;
}

/**
 * Получите следующее слово текста. Слово разделяется символами разрыва.
 *
 * Если слово не помещается в пространство max_width, подчиняйтесь правиламLV_TXT_LINE_BREAK_LONG_*.
 *
 * Если следующее слово ничего не может поместить, верните 0.
 *
 * Если первый символ является символом разрыва, возвращается следующий индекс.
 *
 * Пример вызовов из lv_text_get_next_line() при достаточном количествеmax_widthи
 * txt = "Тестовый текст\n"
 *        0123456789
 *
 * Звонки будут следующими:
 *     1. Верните i=4, указывая на символ прерывания ' ', для строки "Test"
 *     2. Верните i=5, так как i=4 был символом прерывания.
 *     3. Верните i=9, указывая на символ прерывания '\n'
 *     4. Родительскийlv_text_get_next_line() обнаружит последующий '\0'
 *
 * TODO: Возвращенныйword_w_ptrможет переоценить ширину возвращаемого слова, когда
 * max_width добился. При нынешнем использовании это не имеет никакого значения.
 *
 * @param txt строка, завершающаяся '\0'
 * @param font указатель на шрифт
 * @param letter_space буквенное пространство
 * @param max_width максимальная ширина текста (разорвите строки, чтобы они соответствовали этому размеру). Установите COORD_MAX, чтобы избежать разрывов строк.
 * @param flag настройки для текста из перечисления 'txt_flag_type'
 * @param [out] word_w_ptr ширина (в пикселях) анализируемого слова. Может быть NULL.
 * @param cmd_state Указатель на переменную lv_text_cmd_state_t, в которой хранится текущее состояние обработки команды.
 * @return индекс первого символа следующего слова (в индексе байта, а не индекса буквы. СUTF-8 они разные)
 */
static uint32_t lv_text_get_next_word(const char * txt, const lv_font_t * font,
                                      int32_t letter_space, int32_t max_width,
                                      lv_text_flag_t flag, uint32_t * word_w_ptr,
                                      lv_text_cmd_state_t * cmd_state)
{
    if(txt == NULL || txt[0] == '\0') return 0;
    if(font == NULL) return 0;

    if(flag & LV_TEXT_FLAG_EXPAND) max_width = LV_COORD_MAX;

    uint32_t i = 0, i_next = 0, i_next_next = 0;  /*Итерация индекса в txt*/
    uint32_t letter = 0;      /*Письмо в я*/
    uint32_t letter_next = 0; /*Письмо на i_next*/
    int32_t letter_w;
    int32_t cur_w = 0;  /*Пиксель Ширина пройденной строки*/
    uint32_t word_len = 0;   /*Количество символов в пройденном слове*/
    uint32_t break_index = NO_BREAK_FOUND; /*используется только для «длинных» слов*/
    uint32_t break_letter_count = 0; /*Количество символов до точки разрыва длинного слова*/

    letter = lv_text_encoded_next(txt, &i_next);
    i_next_next = i_next;

    /*Получите полное слово, независимо от того, подходит оно или нет в max_width.*/
    while(txt[i] != '\0') {
        letter_next = lv_text_encoded_next(txt, &i_next_next);
        word_len++;

        /*Обработка команды перекрашивания*/
        if((flag & LV_TEXT_FLAG_RECOLOR) != 0) {
            if(lv_text_is_cmd(cmd_state, letter)) {
                i = i_next;
                i_next = i_next_next;
                letter = letter_next;
                continue;   /*Пропустить букву, если она является частью команды*/
            }
        }

        letter_w = lv_font_get_glyph_width(font, letter, letter_next);
        cur_w += letter_w;

        if(letter_w > 0) {
            cur_w += letter_space;
        }

        /*Проверьте, находится ли этот символ в max_width.*/
        if(break_index == NO_BREAK_FOUND && (cur_w - letter_space) > max_width) {
            break_index = i;
            break_letter_count = word_len - 1;
            if(flag & LV_TEXT_FLAG_BREAK_ALL) {
                break;
            }
            /*break_index теперь указывает на неподходящий символ.*/
        }

        /*Проверьте наличие новых строк и символов разрыва.*/
        if(letter == '\n' || letter == '\r' || lv_text_is_break_char(letter)) {
            /*Обновите выходную ширину первого символа, если она подходит.
             *Это необходимо сделать здесь, если первая буква является символом разрыва.*/
            if(i == 0 && break_index == NO_BREAK_FOUND && word_w_ptr != NULL) *word_w_ptr = cur_w;
            word_len--;
            break;
        }
        else if(lv_text_is_a_word(letter_next) || lv_text_is_a_word(letter)) {
            /*Нашёл слово для одной буквы, обычно верно для CJK.*/
            *word_w_ptr = cur_w;
            i = i_next;
            break;
        }

        /*Обновить ширину вывода*/
        if(word_w_ptr != NULL && break_index == NO_BREAK_FOUND) *word_w_ptr = cur_w;

        i = i_next;
        i_next = i_next_next;
        letter = letter_next;
    }

    /*Все слово помещается в отведенное место*/
    if(break_index == NO_BREAK_FOUND) {
        if(word_len == 0 || (letter == '\r' && letter_next == '\n')) i = i_next;
        return i;
    }

#if LV_TXT_LINE_BREAK_LONG_LEN > 0
    /*Слово не помещается в отведенное место, но не является «длинным».*/
    if(word_len < LV_TXT_LINE_BREAK_LONG_LEN) {
        if(flag & LV_TEXT_FLAG_BREAK_ALL) return break_index;
        if(word_w_ptr != NULL) *word_w_ptr = 0; /*Не возвращайте ни слова*/
        return 0;
    }

    /*Слово «длинное», но в отведенном месте может поместиться недостаточное количество слов.*/
    if(break_letter_count < LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN) {
        if(flag & LV_TEXT_FLAG_BREAK_ALL) return break_index;
        if(word_w_ptr != NULL) *word_w_ptr = 0;
        return 0;
    }

    /*Слово «длинное», но буквы, возможно, нужно лучше распределить.*/
    {
        i = break_index;
        int32_t n_move = LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN - (word_len - break_letter_count);
        /*Переместить указатель «i» назад*/
        for(; n_move > 0; n_move--) {
            lv_text_encoded_prev(txt, &i);
            /**
             * TODO: было бы целесообразно обновить возвращенный
             * ширина слова здесьОднако при нынешнем использовании это ни на что не влияет.
             */
        }
    }
    return i;
#else
    if(flag & LV_TEXT_FLAG_BREAK_ALL) return break_index;
    if(word_w_ptr != NULL) *word_w_ptr = 0; /*Не возвращайте ни слова*/
    (void) break_letter_count;
    return 0;
#endif
}

uint32_t lv_text_get_next_line(const char * txt, uint32_t len,
                               const lv_font_t * font, int32_t * used_width, lv_text_attributes_t * attributes)
{

    if(used_width) *used_width = 0;

    if(txt == NULL) return 0;
    if(txt[0] == '\0') return 0;
    if(font == NULL) return 0;

    int32_t line_w = 0;

    /*Еслиmax_widthне имеет значения, просто найдите символ новой строки.
     *не думая о переносе слов*/
    if((attributes->text_flags & LV_TEXT_FLAG_EXPAND) ||
       (attributes->text_flags & LV_TEXT_FLAG_FIT)) {

        uint32_t i;
        for(i = 0; i < len && txt[i] != '\n' && txt[i] != '\r' && txt[i] != '\0'; i++) {
            /*Просто найдите новые символы строки или концы строк, увеличив `i`.*/
        }
        if(i < len && txt[i] != '\0') i++;    /*Чтобы выйти за пределы `\n`*/
        if(used_width) *used_width = -1;
        return i;
    }

    if(attributes->text_flags & LV_TEXT_FLAG_EXPAND) {
        attributes->max_width = LV_COORD_MAX;
    }
    lv_text_cmd_state_t cmd_state = LV_TEXT_CMD_STATE_WAIT;

    uint32_t i = 0;                                        /*Итерация индекса в txt*/
    uint32_t max_width = attributes->max_width;
    bool explicit_new_line = false;

    while(i < len && txt[i] != '\0' && max_width > 0) {
        lv_text_flag_t word_flag = attributes->text_flags;

        if(i == 0) word_flag |= LV_TEXT_FLAG_BREAK_ALL;

        uint32_t word_w = 0;
        uint32_t advance = lv_text_get_next_word(&txt[i], font, attributes->letter_space,
                                                 max_width, word_flag, &word_w, &cmd_state);
        max_width -= word_w;
        line_w += word_w;

        if(advance == 0) {
            break;
        }

        i += advance;

        if(txt[0] == '\n' || txt[0] == '\r') {
            explicit_new_line = true;
            break;
        }

        if(txt[i] == '\n' || txt[i] == '\r') {
            i++;  /*Включите следующую новую строку в текущую строку*/
            explicit_new_line = true;
            break;
        }
    }

    /*Всегда делайте хотя бы один шаг, чтобы избежать бесконечных циклов.*/
    if(i == 0) {
        uint32_t letter = lv_text_encoded_next(txt, &i);
        if(used_width != NULL) {
            line_w = lv_font_get_glyph_width(font, letter, '\0');
        }
    }

    if(used_width != NULL) {
        *used_width = line_w;
    }

    /*Пропускать начальные пробелы следующей строки только для автоматического переноса слов.*/
    if(!explicit_new_line) {
        while(i < len && txt[i] == ' ') {
            i++;
        }
    }

    return i;
}

int32_t lv_text_get_width(const char * txt, uint32_t length, const lv_font_t * font,
                          const lv_text_attributes_t * attributes)
{
    if(txt == NULL) return 0;
    if(font == NULL) return 0;
    if(txt[0] == '\0') return 0;

    uint32_t i                = 0;
    int32_t width             = 0;
    lv_text_cmd_state_t cmd_state = LV_TEXT_CMD_STATE_WAIT;

    if(length != 0) {
        while(txt[i] != '\0' && i < length) {

            uint32_t letter;
            uint32_t letter_next;

            lv_text_encoded_letter_next_2(txt, &letter, &letter_next, &i);

            if((attributes->text_flags & LV_TEXT_FLAG_RECOLOR) != 0) {
                if(lv_text_is_cmd(&cmd_state, letter) != false) {
                    continue;
                }
            }

            int32_t char_width = lv_font_get_glyph_width(font, letter, letter_next);
            if(char_width > 0) {
                width += char_width;
                width += attributes->letter_space;
            }
        }

        if(width > 0) {
            width -= attributes->letter_space; /*Обрежьте последний пробел между буквами. Важно, если текст находится по центру
                                      выровнен*/
        }
    }

    return width;
}

void lv_text_ins(char * txt_buf, uint32_t pos, const char * ins_txt)
{
    if(txt_buf == NULL || ins_txt == NULL) return;

    size_t old_len = lv_strlen(txt_buf);
    size_t ins_len = lv_strlen(ins_txt);
    if(ins_len == 0) return;

    size_t new_len = ins_len + old_len;
    pos              = lv_text_encoded_get_byte_id(txt_buf, pos); /*Преобразование в байтовый индекс вместо буквенного индекса*/

    /*Скопируйте вторую часть в конец, чтобы освободить место для текста для вставки.*/
    size_t i;
    for(i = new_len; i >= pos + ins_len; i--) {
        txt_buf[i] = txt_buf[i - ins_len];
    }

    /*Скопируйте текст в новое место*/
    lv_memcpy(txt_buf + pos, ins_txt, ins_len);
}

void lv_text_cut(char * txt, uint32_t pos, uint32_t len)
{
    if(txt == NULL) return;

    size_t old_len = lv_strlen(txt);

    pos = lv_text_encoded_get_byte_id(txt, pos); /*Преобразование в байтовый индекс вместо буквенного индекса*/
    len = lv_text_encoded_get_byte_id(&txt[pos], len);

    /*Скопируйте вторую часть в конец, чтобы освободить место для текста для вставки.*/
    uint32_t i;
    for(i = pos; i <= old_len - len; i++) {
        txt[i] = txt[i + len];
    }
}

char * lv_text_set_text_vfmt(const char * fmt, va_list ap)
{
    /*Выделите место для нового текста, используя трюк из стандартного раздела C99 7.19.6.12.*/
    va_list ap_copy;
    va_copy(ap_copy, ap);
    uint32_t len = lv_vsnprintf(NULL, 0, fmt, ap_copy);
    va_end(ap_copy);

    char * text = 0;
#if LV_USE_ARABIC_PERSIAN_CHARS
    /*Соберите текст в соответствии со строкой формата*/
    char * raw_txt = lv_malloc(len + 1);
    LV_ASSERT_MALLOC(raw_txt);
    if(raw_txt == NULL) {
        return NULL;
    }

    lv_vsnprintf(raw_txt, len + 1, fmt, ap);

    /*Получите размер арабского текста и обработайте его.*/
    size_t len_ap = lv_text_ap_calc_bytes_count(raw_txt);
    text = lv_malloc(len_ap + 1);
    LV_ASSERT_MALLOC(text);
    if(text == NULL) {
        return NULL;
    }
    lv_text_ap_proc(raw_txt, text);

    lv_free(raw_txt);
#else
    text = lv_malloc(len + 1);
    LV_ASSERT_MALLOC(text);
    if(text == NULL) {
        return NULL;
    }

    lv_vsnprintf(text, len + 1, fmt, ap);
#endif

    return text;
}

void lv_text_encoded_letter_next_2(const char * txt, uint32_t * letter, uint32_t * letter_next, uint32_t * ofs)
{
    *letter = lv_text_encoded_next(txt, ofs);
    *letter_next = *letter != '\0' ? lv_text_encoded_next(&txt[*ofs], NULL) : 0;
}

#if LV_TXT_ENC == LV_TXT_ENC_UTF8
/*******************************
 *   UTF-8 ENCODER/DECODER
 ******************************/

/**
 * Укажите размер кодированного символа UTF -8.
 * @param str указатель на символ в строке
 * @return длина символаUTF-8 (1,2,3 или 4), 0 для неверного кода.
 */
static uint8_t lv_text_utf8_size(const char * str)
{
    if(LV_IS_ASCII(str[0]))
        return 1;
    else if(LV_IS_2BYTES_UTF8_CODE(str[0]))
        return 2;
    else if(LV_IS_3BYTES_UTF8_CODE(str[0]))
        return 3;
    else if(LV_IS_4BYTES_UTF8_CODE(str[0]))
        return 4;
    return 0;
}

/**
 * Преобразуйте букву Юникода в UTF -8.
 * @param letter_uni буква Юникода
 * @return UTF -8 закодированный символ в Little Endian для совместимости с символами C (например, 'Á', 'Ű')
 */
static uint32_t lv_text_unicode_to_utf8(uint32_t letter_uni)
{
    if(letter_uni < 128) return letter_uni;
    uint8_t bytes[4];

    if(letter_uni < 0x0800) {
        bytes[0] = ((letter_uni >> 6) & 0x1F) | 0xC0;
        bytes[1] = ((letter_uni >> 0) & 0x3F) | 0x80;
        bytes[2] = 0;
        bytes[3] = 0;
    }
    else if(letter_uni < 0x010000) {
        bytes[0] = ((letter_uni >> 12) & 0x0F) | 0xE0;
        bytes[1] = ((letter_uni >> 6) & 0x3F) | 0x80;
        bytes[2] = ((letter_uni >> 0) & 0x3F) | 0x80;
        bytes[3] = 0;
    }
    else if(letter_uni < 0x110000) {
        bytes[0] = ((letter_uni >> 18) & 0x07) | 0xF0;
        bytes[1] = ((letter_uni >> 12) & 0x3F) | 0x80;
        bytes[2] = ((letter_uni >> 6) & 0x3F) | 0x80;
        bytes[3] = ((letter_uni >> 0) & 0x3F) | 0x80;
    }
    else {
        return 0;
    }

    uint32_t * res_p = (uint32_t *)bytes;
    return *res_p;
}

/**
 * Преобразование широкого символа, например. 'Á' с прямым порядком байтов будет совместим с UTF -8
 * @param c широкий символ или число с прямым порядком байтов
 * @return `c` с прямым порядком байтов
 */
static uint32_t lv_text_utf8_conv_wc(uint32_t c)
{
#if LV_BIG_ENDIAN_SYSTEM == 0
    /*Поменяйте местами байты ( UTF -8 — с прямым порядком байтов, но у MCU — с прямым порядком байтов)*/
    if((c & 0x80) != 0) {
        uint32_t swapped;
        uint8_t c8[4];
        lv_memcpy(c8, &c, 4);
        swapped = (c8[0] << 24) + (c8[1] << 16) + (c8[2] << 8) + (c8[3]);
        uint8_t i;
        for(i = 0; i < 4; i++) {
            if((swapped & 0xFF) == 0)
                swapped = (swapped >> 8); /*Игнорировать ведущие нули (они изначально были в конце)*/
        }
        c = swapped;
    }
#endif
    return c;
}

/**
 * Декодируйте символ UTF -8 из строки.
 * @param txt указатель на строку, завершающуюся '\0'
 * @param i Индекс начального байта в «txt», с чего начать.
 *          После вызова он будет указывать на следующий символ UTF -8 в текстовом формате.
 *          NULL для использования txt[0] в качестве индекса
 * @return декодированный символ Юникода или 0 в недопустимом кодеUTF-8
 */
static uint32_t lv_text_utf8_next(const char * txt, uint32_t * i)
{
    /**
     * Юникод в UTF -8
     * 00000000 00000000 00000000 0xxxxxxx -> 0xxxxxxx
     * 00000000 00000000 00000yyy yyxxxxxx -> 110yyyyy 10xxxxxx
     * 00000000 00000000 zzzzyyyy yyxxxxxx -> 1110zzzz 10yyyyyy 10xxxxxx
     * 00000000 000wwwzz zzzzyyyy yyxxxxxx -> 11110www 10zzzzzz 10yyyyyy 10xxxxxx
     */

    uint32_t result = 0;

    /*Требуется фиктивный указатель «i».*/
    uint32_t i_tmp = 0;
    if(i == NULL) i = &i_tmp;

    /* Убедитесь, что строка не равна нулю */
    if(txt == NULL || txt[*i] == '\0') {
        return result;
    }

    /*Обычный ASCII*/
    if(LV_IS_ASCII(txt[*i])) {
        result = txt[*i];
        (*i)++;
    }
    /*Настоящее декодирование UTF -8*/
    else {
        /*2 байта кода UTF -8*/
        if(LV_IS_2BYTES_UTF8_CODE(txt[*i])) {
            result = (uint32_t)(txt[*i] & 0x1F) << 6;
            (*i)++;
            if(LV_IS_INVALID_UTF8_CODE(txt[*i])) return 0;
            result += (txt[*i] & 0x3F);
            (*i)++;
        }
        /*3 байта кода UTF -8*/
        else if(LV_IS_3BYTES_UTF8_CODE(txt[*i])) {
            result = (uint32_t)(txt[*i] & 0x0F) << 12;
            (*i)++;

            if(LV_IS_INVALID_UTF8_CODE(txt[*i])) return 0;
            result += (uint32_t)(txt[*i] & 0x3F) << 6;
            (*i)++;

            if(LV_IS_INVALID_UTF8_CODE(txt[*i])) return 0;
            result += (txt[*i] & 0x3F);
            (*i)++;
        }
        /*4 байта кода UTF -8*/
        else if(LV_IS_4BYTES_UTF8_CODE(txt[*i])) {
            result = (uint32_t)(txt[*i] & 0x07) << 18;
            (*i)++;

            if(LV_IS_INVALID_UTF8_CODE(txt[*i])) return 0;
            result += (uint32_t)(txt[*i] & 0x3F) << 12;
            (*i)++;

            if(LV_IS_INVALID_UTF8_CODE(txt[*i])) return 0;
            result += (uint32_t)(txt[*i] & 0x3F) << 6;
            (*i)++;

            if(LV_IS_INVALID_UTF8_CODE(txt[*i])) return 0;
            result += txt[*i] & 0x3F;
            (*i)++;
        }
        else {
            (*i)++; /*Не UTF -8 символов. Иди следующий.*/
        }
    }
    return result;
}

/**
 * Получить предыдущий символ UTF -8 из строки.
 * @param txt указатель на строку, завершающуюся '\0'
 * @param i Индекс начального байта в «txt», с чего начать. После вызова он укажет на предыдущий
 * UTF -8 символов в формате «txt».
 * @return декодированный символ Юникода или 0 в недопустимом кодеUTF-8
 */
static uint32_t lv_text_utf8_prev(const char * txt, uint32_t * i)
{
    uint8_t c_size;
    uint8_t cnt = 0;

    /*Попробуйте найти длинный символ !0 UTF -8, отойдя на один символ назад.*/
    (*i)--;
    do {
        if(cnt >= 4) return 0; /*Перед начальным символом UTF -8 не обнаружено.*/

        c_size = lv_text_encoded_size(&txt[*i]);
        if(c_size == 0) {
            if(*i != 0)
                (*i)--;
            else
                return 0;
        }
        cnt++;
    } while(c_size == 0);

    uint32_t i_tmp  = *i;
    uint32_t letter = lv_text_encoded_next(txt, &i_tmp); /*Персонаж найден, получите его*/

    return letter;
}

/**
 * Преобразуйте индекс символа (в тексте UTF -8) в индекс байта.
 * например в «AÁRT» индекс «R» равен 2-му символу, но начинается с байта 3, поскольку длина «Á» составляет 2 байта.
 * @param txt строкаUTF-8, завершающаяся '\0'
 * @param utf8_id индекс символов
 * @return индекс байта 'utf8_id'-ой буквы
 */
static uint32_t lv_text_utf8_get_byte_id(const char * txt, uint32_t utf8_id)
{
    uint32_t i;
    uint32_t byte_cnt = 0;
    for(i = 0; i < utf8_id && txt[byte_cnt] != '\0'; i++) {
        uint8_t c_size = lv_text_encoded_size(&txt[byte_cnt]);
        /* Если символ недействителен, сообщите, что его длина составляет 1 байт.*/
        byte_cnt += c_size ? c_size : 1;
    }

    return byte_cnt;
}

/**
 * Преобразуйте индекс байта (в тексте UTF -8) в индекс символа.
 * например в «AÁRT» индекс «R» равен 2-му символу, но начинается с байта 3, поскольку длина «Á» составляет 2 байта.
 * @param txt строкаUTF-8, завершающаяся '\0'
 * @param byte_id байтовый индекс
 * @return индекс символа буквы в 'byte_id'-ой позиции
 */
static uint32_t lv_text_utf8_get_char_id(const char * txt, uint32_t byte_id)
{
    uint32_t i        = 0;
    uint32_t char_cnt = 0;

    while(i < byte_id) {
        lv_text_encoded_next(txt, &i); /*«i» указывает на следующую букву, поэтому используйте предыдущую. ценность*/
        char_cnt++;
    }

    return char_cnt;
}

/**
 * Получите количество символов (и байтов NOT) в строке. Декодируйте его с помощью UTF -8, если он включен.
 * Например: «ABC» — 3 символа (но 4 байта).
 * @param txt символьная строка, завершающаяся '\0'
 * @return количество символов
 */
static uint32_t lv_text_utf8_get_length(const char * txt)
{
    uint32_t len = 0;
    uint32_t i   = 0;

    while(txt[i] != '\0') {
        lv_text_encoded_next(txt, &i);
        len++;
    }

    return len;
}

#elif LV_TXT_ENC == LV_TXT_ENC_ASCII
/*******************************
 *  ASCII ENCODER/DECODER
 ******************************/

/**
 * Укажите размер кодированного символа ISO8859 -1.
 * @param str указатель на символ в строке
 * @return длина кодированного символаISO8859-1 всегда будет равна 1.
 */
static uint8_t lv_text_iso8859_1_size(const char * str)
{
    LV_UNUSED(str); /*Неиспользованный*/
    return 1;
}

/**
 * Преобразуйте букву Юникода в ISO8859 -1.
 * @param letter_uni буква Юникода
 * @return ISO8859 -1 закодированный символ в Little Endian для совместимости с символами C (например, 'Á', 'Ű')
 */
static uint32_t lv_text_unicode_to_iso8859_1(uint32_t letter_uni)
{
    if(letter_uni < 256)
        return letter_uni;
    else
        return ' ';
}

/**
 * Преобразуйте широкие символы в ASCII , однако широкие символы в диапазоне ASCII (например, «A») по умолчанию совместимы с ASCII.
 * Таким образом, эта функция ничего не делает, а просто возвращает `c`.
 * @param c персонаж, например 'А'
 * @return то же, что `c`
 */
static uint32_t lv_text_iso8859_1_conv_wc(uint32_t c)
{
    return c;
}

/**
 * Декодируйте символ ISO8859 -1 из строки.
 * @param txt указатель на строку, завершающуюся '\0'
 * @param i Индекс начального байта в «txt», с чего начать.
 *          После вызова он будет следовать далее закодированный символISO8859-1 в формате «txt».
 *          NULL для использования txt[0] в качестве индекса
 * @return декодированный символISO8859-1.
 */
static uint32_t lv_text_iso8859_1_next(const char * txt, uint32_t * i)
{
    if(i == NULL) return txt[0]; /*Получить следующий символ*/

    uint8_t letter = txt[*i];
    (*i)++;
    return letter;
}

/**
 * Получить предыдущий символ ISO8859 -1 из строки.
 * @param txt указатель на строку, завершающуюся '\0'
 * @param i Индекс начального байта в «txt», с чего начать. После вызова он будет указывать на предыдущий закодированный символISO8859-1 в «txt».
 * @return декодированный символISO8859-1.
 */
static uint32_t lv_text_iso8859_1_prev(const char * txt, uint32_t * i)
{
    if(i == NULL) return *(txt - 1); /*Получите пред. голец*/

    (*i)--;
    uint8_t letter = txt[*i];

    return letter;
}

/**
 * Преобразуйте индекс символа (в тексте ISO8859 -1) в индекс байта.
 * Кодировка ISO8859 -1 совместима с ASCII, поэтому индексы символов совпадают с индексами байтов.
 * @param txt символьная строка, завершающаяся '\0'
 * @param utf8_id индекс символов
 * @return индекс байта 'utf8_id'-ой буквы
 */
static uint32_t lv_text_iso8859_1_get_byte_id(const char * txt, uint32_t utf8_id)
{
    LV_UNUSED(txt); /*Неиспользованный*/
    return utf8_id; /*В некодированном состоянии нет разницы*/
}

/**
 * Преобразуйте индекс байта (в тексте ISO8859 -1) в индекс символа.
 * Кодировка ISO8859 -1 совместима с ASCII, поэтому индексы символов совпадают с индексами байтов.
 * @param txt символьная строка, завершающаяся '\0'
 * @param byte_id байтовый индекс
 * @return индекс символа буквы в 'byte_id'-ой позиции
 */
static uint32_t lv_text_iso8859_1_get_char_id(const char * txt, uint32_t byte_id)
{
    LV_UNUSED(txt); /*Неиспользованный*/
    return byte_id; /*В некодированном состоянии нет разницы*/
}

/**
 * Получите количество символов (и байтов NOT) в строке.
 * Кодировка ISO8859 -1 совместима с ASCII, поэтому количество символов равно количеству байтов.
 * @param txt символьная строка, завершающаяся '\0'
 * @return количество символов
 */
static uint32_t lv_text_iso8859_1_get_length(const char * txt)
{
    return lv_strlen(txt);
}
#else

#error "Invalid character encoding. See `LV_TXT_ENC` in `lv_conf.h`"

#endif
