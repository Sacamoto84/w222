/**
 * @file lv_label.h
 *
 */

#ifndef LV_LABEL_H
#define LV_LABEL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_LABEL != 0

#include "../../misc/lv_types.h"
#include "../../core/lv_obj.h"
#include "../../font/lv_font.h"
#include "../../font/lv_symbol_def.h"
#include "../../misc/lv_text.h"
#include "../../draw/lv_draw.h"
#include "../../core/lv_observer.h"

/*********************
 *      DEFINES
 *********************/
#define LV_LABEL_DOT_NUM 3
#define LV_LABEL_POS_LAST 0xFFFF
#define LV_LABEL_TEXT_SELECTION_OFF LV_DRAW_LABEL_NO_TXT_SEL
#if LV_WIDGETS_HAS_DEFAULT_VALUE
#define LV_LABEL_DEFAULT_TEXT "Text"
#else
#define LV_LABEL_DEFAULT_TEXT ""
#endif

LV_EXPORT_CONST_INT(LV_LABEL_DOT_NUM);
LV_EXPORT_CONST_INT(LV_LABEL_POS_LAST);
LV_EXPORT_CONST_INT(LV_LABEL_TEXT_SELECTION_OFF);

/**********************
 *      TYPEDEFS
 **********************/

/** Поведение в длительном режиме. Используется в 'lv_label_ext_t'*/
typedef enum {
    LV_LABEL_LONG_MODE_WRAP,             /**< Keep the object width, wrap lines longer than object width and expand the object height*/
    LV_LABEL_LONG_MODE_DOTS,             /**< Keep the size and write dots at the end if the text is too long*/
    LV_LABEL_LONG_MODE_SCROLL,           /**< Keep the size and roll the text back and forth*/
    LV_LABEL_LONG_MODE_SCROLL_CIRCULAR,  /**< Keep the size and roll the text circularly*/
    LV_LABEL_LONG_MODE_CLIP,             /**< Keep the size and clip the text out of it*/
} lv_label_long_mode_t;

#if LV_USE_OBJ_PROPERTY
enum _lv_property_label_id_t {
    LV_PROPERTY_ID(LABEL, TEXT,                   LV_PROPERTY_TYPE_TEXT,      0),
    LV_PROPERTY_ID(LABEL, LONG_MODE,              LV_PROPERTY_TYPE_INT,       1),
    LV_PROPERTY_ID(LABEL, TEXT_SELECTION_START,   LV_PROPERTY_TYPE_INT,       2),
    LV_PROPERTY_ID(LABEL, TEXT_SELECTION_END,     LV_PROPERTY_TYPE_INT,       3),
    LV_PROPERTY_LABEL_END,
};
#endif

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_label_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создать объект метки
 * @param parent    pointer to an object, it will be the parent of the new label.
 * @return          pointer to the created button
 */
lv_obj_t * lv_label_create(lv_obj_t * parent);

/*=====================
 * Функции установки
 *====================*/

/**
 * Установите новый текст для метки. Для хранения текста метки будет выделена память.
 * @param obj           pointer to a label object
 * @param text          '\0' terminated character string. NULL to refresh with the current text.
 * @note If `LV_USE_ARABIC_PERSIAN_CHARS` is enabled the text will be modified to have the correct Arabic
 * персонажи в нем.
 */
void lv_label_set_text(lv_obj_t * obj, const char * text);

/**
 * Установите новый форматированный текст для метки. Для хранения текста метки будет выделена память.
 * @param obj           pointer to a label object
 * @param fmt           `printf`-like format string
 * Пример:
 * @code
 * lv_label_set_text_fmt (метка1, "Пользователь %d", user_num );
 * @endcode
 * @note If `LV_USE_ARABIC_PERSIAN_CHARS` is enabled the text will be modified to have the correct Arabic characters in it.
 */
void lv_label_set_text_fmt(lv_obj_t * obj, const char * fmt, ...) LV_FORMAT_ATTRIBUTE(2, 3);

/**
 * Установите новый форматированный текст для метки. Для хранения текста метки будет выделена память.
 * @param obj           pointer to a label object
 * @param fmt           `printf`-like format string
 * @param args          variadic arguments list
 *
 * Пример:
 * @code
 * va_list арг;
 * va_start (аргументы, fmt);
 * lv_label_set_text_vfmt (метка1, fmt, args);
 * va_end (аргументы);
 * @endcode
 * @note It ignores `LV_USE_ARABIC_PERSIAN_CHARS`
 */
void lv_label_set_text_vfmt(lv_obj_t * obj, const char * fmt, va_list args);

/**
 * Установите статический текст. Он не будет сохранен с помощью метки, поэтому переменная text
 * должен быть «живым», пока существует метка.
 * @param obj           pointer to a label object
 * @param text          pointer to a text. NULL to refresh with the current text.
 * @note It ignores `LV_USE_ARABIC_PERSIAN_CHARS`
 */
void lv_label_set_text_static(lv_obj_t * obj, const char * text);

/**
 * Установите поведение метки с текстом, длина которого превышает размер объекта.
 * @param obj           pointer to a label object
 * @param long_mode     the new mode from 'lv_label_long_mode' enum.
 *                      В LV_LONG_WRAP/DOT/SCROLL/SCROLL_CIRC размер метки должен быть установлен AFTER этой функцией
 */
void lv_label_set_long_mode(lv_obj_t * obj, lv_label_long_mode_t long_mode);

/**
 * Установите, где должно начинаться выделение текста
 * @param obj       pointer to a label object
 * @param index     character index from where selection should start. `LV_LABEL_TEXT_SELECTION_OFF` for no selection
 */
void lv_label_set_text_selection_start(lv_obj_t * obj, uint32_t index);

/**
 * Установите, где должно закончиться выделение текста
 * @param obj       pointer to a label object
 * @param index     character index where selection should end. `LV_LABEL_TEXT_SELECTION_OFF` for no selection
 */
void lv_label_set_text_selection_end(lv_obj_t * obj, uint32_t index);

/**
 * Включите перекрашивание с помощью встроенных команд
 * @param obj           pointer to a label object
 * @param en            true: enable recoloring, false: disable
 * Example: "This is a #ff0000 red# word"
 */
void lv_label_set_recolor(lv_obj_t * obj, bool en);

#if LV_USE_TRANSLATION

/**
 * Назначьте тег перевода для этой метки. Для хранения тега по метке будет выделена память.
 * Текст метки будет автоматически обновляться при изменении языка с помощью `lv_translation_set_language`.
 * @param obj           pointer to a label object
 * @param tag          '\0' terminated character string.
 */
void lv_label_set_translation_tag(lv_obj_t * obj, const char * tag);

#endif /*LV_USE_TRANSLATION*/


/*=====================
 * Геттерные функции
 *====================*/

/**
 * Получить текст метки
 * @param obj       pointer to a label object
 * @return          the text of the label
 */
char * lv_label_get_text(const lv_obj_t * obj);

/**
 * Получить длинный режим метки
 * @param obj       pointer to a label object
 * @return          the current long mode
 */
lv_label_long_mode_t lv_label_get_long_mode(const lv_obj_t * obj);

/**
 * Получить относительные координаты x и y буквы
 * @param obj       pointer to a label object
 * @param char_id   index of the character [0 ... text length - 1].
 *                  Выражается в индексе символов, а не индексе байтов (отличается в UTF -8)
 * @param pos       store the result here (E.g. index = 0 gives 0;0 coordinates if the text if aligned to the left)
 */
void lv_label_get_letter_pos(const lv_obj_t * obj, uint32_t char_id, lv_point_t * pos);

/**
 * Получите индекс буквы в относительной точке метки.
 * @param obj       pointer to label object
 * @param pos_in    pointer to point with coordinates on a the label
 * @param bidi      whether to use bidi processed
 * @return          The index of the letter on the 'pos_p' point (E.g. on 0;0 is the 0. letter if aligned to the left)
 *                  Выражается в символьном индексе, а не в байтовом индексе (отличается в UTF -8)
 */
uint32_t lv_label_get_letter_on(const lv_obj_t * obj, lv_point_t * pos_in, bool bidi);

/**
 * Проверьте, нарисован ли символ под точкой.
 * @param obj       pointer to a label object
 * @param pos       Point to check for character under
 * @return          whether a character is drawn under the point
 */
bool lv_label_is_char_under_pos(const lv_obj_t * obj, lv_point_t * pos);

/**
 * @brief Get the selection start index.
 * @param obj       pointer to a label object.
 * @return          selection start index. `LV_LABEL_TEXT_SELECTION_OFF` if nothing is selected.
 */
uint32_t lv_label_get_text_selection_start(const lv_obj_t * obj);

/**
 * @brief Get the selection end index.
 * @param obj       pointer to a label object.
 * @return          selection end index. `LV_LABEL_TXT_SEL_OFF` if nothing is selected.
 */
uint32_t lv_label_get_text_selection_end(const lv_obj_t * obj);

/**
 * @brief Get the recoloring attribute
 * @param obj       pointer to a label object.
 * @return          true: recoloring is enabled, false: recoloring is disabled
 */
bool lv_label_get_recolor(const lv_obj_t * obj);

/*=====================
 * Другие функции
 *====================*/

#if LV_USE_OBSERVER
/**
 * Привяжите целое число, строку или указатель к метке.
 * @param obj       pointer to Label
 * @param subject   pointer to Subject
 * @param fmt       optional printf-like format string with 1 format specifier (e.g. "%d °C")
 *                  или NULL для прямой привязки к значению.
 * @return          pointer to newly-created Observer
 * @note            If `fmt == NULL` strings and pointers (`\0` terminated string) will be shown
 *                  в виде текста, целые числа — %d, числа с плавающей запятой — %0.1f.
 */
lv_observer_t * lv_label_bind_text(lv_obj_t * obj, lv_subject_t * subject, const char * fmt);
#endif


/**
 * Вставьте текст в метку. Текст метки не может быть статическим.
 * @param obj       pointer to a label object
 * @param pos       character index to insert. Expressed in character index and not byte index.
 *                  0: before first char. LV_LABEL_POS_LAST: after last char.
 * @param txt       pointer to the text to insert
 */
void lv_label_ins_text(lv_obj_t * obj, uint32_t pos, const char * txt);

/**
 * Удаление символов из метки. Текст метки не может быть статическим.
 * @param obj       pointer to a label object
 * @param pos       character index from where to cut. Expressed in character index and not byte index.
 *                  0: start in front of the first character
 * @param cnt       number of characters to cut
 */
void lv_label_cut_text(lv_obj_t * obj, uint32_t pos, uint32_t cnt);



/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_LABEL*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_LABEL_H*/
