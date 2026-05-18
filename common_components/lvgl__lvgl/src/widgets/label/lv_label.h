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
 * @param parent    указатель на объект, он будет родительским элементом новой метки.
 * @return          указатель на созданную кнопку
 */
lv_obj_t * lv_label_create(lv_obj_t * parent);

/*=====================
 * Функции установки
 *====================*/

/**
 * Установите новый текст для метки. Для хранения текста метки будет выделена память.
 * @param obj           указатель на объект метки
 * @param text          Строка символов, завершающаяся '\0'.  NULL, чтобы обновить текущий текст.
 * @note Если`LV_USE_ARABIC_PERSIAN_CHARS`включен, текст будет изменен на правильный арабский язык.
 * персонажи в нем.
 */
void lv_label_set_text(lv_obj_t * obj, const char * text);

/**
 * Установите новый форматированный текст для метки. Для хранения текста метки будет выделена память.
 * @param obj           указатель на объект метки
 * @param fmt           Строка формата, подобная `printf`
 * Пример:
 * @code
 * lv_label_set_text_fmt (метка1, "Пользователь %d",user_num);
 * @endcode
 * @note Если`LV_USE_ARABIC_PERSIAN_CHARS`включен, текст будет изменен и будет содержать правильные арабские символы.
 */
void lv_label_set_text_fmt(lv_obj_t * obj, const char * fmt, ...) LV_FORMAT_ATTRIBUTE(2, 3);

/**
 * Установите новый форматированный текст для метки. Для хранения текста метки будет выделена память.
 * @param obj           указатель на объект метки
 * @param fmt           Строка формата, подобная `printf`
 * @param args          список вариативных аргументов
 *
 * Пример:
 * @code
 * va_list арг;
 * va_start (аргументы, fmt);
 * lv_label_set_text_vfmt (метка1, fmt, args);
 * va_end (аргументы);
 * @endcode
 * @note Он игнорирует `LV_USE_ARABIC_PERSIAN_CHARS`
 */
void lv_label_set_text_vfmt(lv_obj_t * obj, const char * fmt, va_list args);

/**
 * Установите статический текст. Он не будет сохранен с помощью метки, поэтому переменный текст
 * должен быть «живым», пока существует метка.
 * @param obj           указатель на объект метки
 * @param text          указатель на текст.  NULL, чтобы обновить текущий текст.
 * @note Он игнорирует `LV_USE_ARABIC_PERSIAN_CHARS`
 */
void lv_label_set_text_static(lv_obj_t * obj, const char * text);

/**
 * Установите поведение метки с текстом, длина которого превышает размер объекта.
 * @param obj           указатель на объект метки
 * @param long_mode     новый режим из перечисления lv_label_long_mode.
 *                      В LV_LONG_WRAP/DOT/SCROLL/SCROLL_CIRC размер метки должен быть установлен AFTER этой функцией
 */
void lv_label_set_long_mode(lv_obj_t * obj, lv_label_long_mode_t long_mode);

/**
 * Установите, где должно начинаться выделение текста
 * @param obj       указатель на объект метки
 * @param index     индекс символа, с которого должен начинаться выбор. `LV_LABEL_TEXT_SELECTION_OFF`для отсутствия выбора
 */
void lv_label_set_text_selection_start(lv_obj_t * obj, uint32_t index);

/**
 * Установите, где должно закончиться выделение текста
 * @param obj       указатель на объект метки
 * @param index     индекс символа, на котором должен закончиться выбор. `LV_LABEL_TEXT_SELECTION_OFF`для отсутствия выбора
 */
void lv_label_set_text_selection_end(lv_obj_t * obj, uint32_t index);

/**
 * Включите перекрашивание с помощью встроенных команд
 * @param obj           указатель на объект метки
 * @param en            true: включить перекрашивание, false: отключить
 * Example: «Это красное слово #ff0000»
 */
void lv_label_set_recolor(lv_obj_t * obj, bool en);

#if LV_USE_TRANSLATION

/**
 * Назначьте тег перевода для этой метки. Для хранения тега по метке будет выделена память.
 * Текст метки будет автоматически обновляться при открытии языка с помощью `lv_translation_set_language`.
 * @param obj           указатель на объект метки
 * @param tag          Строка символов, завершающаяся '\0'.
 */
void lv_label_set_translation_tag(lv_obj_t * obj, const char * tag);

#endif /*LV_USE_TRANSLATION*/


/*=====================
 * Геттерные функции
 *====================*/

/**
 * Получить текст метки
 * @param obj       указатель на объект метки
 * @return          текст этикетки
 */
char * lv_label_get_text(const lv_obj_t * obj);

/**
 * Получить длинный режим метки
 * @param obj       указатель на объект метки
 * @return          текущий длинный режим
 */
lv_label_long_mode_t lv_label_get_long_mode(const lv_obj_t * obj);

/**
 * Получить относительные координаты x и y буквы
 * @param obj       указатель на объект метки
 * @param char_id   индекс символа [0... длина текста - 1].
 *                  Выражается в индексе символов, а не индексе байтов (отличается в UTF -8)
 * @param pos       сохраните результат здесь (например, index = 0 дает координаты 0;0, если текст выровнен по левому краю)
 */
void lv_label_get_letter_pos(const lv_obj_t * obj, uint32_t char_id, lv_point_t * pos);

/**
 * Получите индекс буквы в относительной точке метки.
 * @param obj       указатель на объект метки
 * @param pos_in    указатель на точку с координатами на этикетке
 * @param bidi      использовать ли обработку биди
 * @return          Индекс буквы в точке «pos_p» (например, в точке 0;0 — это буква 0., если выровнена по левому краю)
 *                  Выражается в символьном индексе, а не в байтовом индексе (отличается в UTF -8)
 */
uint32_t lv_label_get_letter_on(const lv_obj_t * obj, lv_point_t * pos_in, bool bidi);

/**
 * Проверьте, нарисован ли символ под точкой.
 * @param obj       указатель на объект метки
 * @param pos       Наведите указатель мыши на наличие символа под
 * @return          рисуется ли персонаж под точкой
 */
bool lv_label_is_char_under_pos(const lv_obj_t * obj, lv_point_t * pos);

/**
 * @brief Получите индекс начала выбора.
 * @param obj       указатель на объект метки.
 * @return          Начальный индекс выбора.  `LV_LABEL_TEXT_SELECTION_OFF`, если ничего не выбрано.
 */
uint32_t lv_label_get_text_selection_start(const lv_obj_t * obj);

/**
 * @brief Получите индекс конца выделения.
 * @param obj       указатель на объект метки.
 * @return          Конечный индекс выбора.  `LV_LABEL_TXT_SEL_OFF`, если ничего не выбрано.
 */
uint32_t lv_label_get_text_selection_end(const lv_obj_t * obj);

/**
 * @brief Получить атрибут перекрашивания
 * @param obj       указатель на объект метки.
 * @return          true: перекрашивание включено, false: перекрашивание отключено
 */
bool lv_label_get_recolor(const lv_obj_t * obj);

/*=====================
 * Другие функции
 *====================*/

#if LV_USE_OBSERVER
/**
 * Привяжите целое число, строку или указатель к метке.
 * @param obj       указатель на метку
 * @param subject   указатель на тему
 * @param fmt       необязательная строка формата в формате printf с 1 спецификатором формата (например, «%d °C»)
 *                  или NULL для прямой привязки к значению.
 * @return          указатель на вновь созданный наблюдатель
 * @note            Если будут показаны строки и указатели`fmt == NULL`(завершаемая строка `\0`)
 *                  в виде текста, целые числа — %d, числа с плавающей запятой — %0.1f.
 */
lv_observer_t * lv_label_bind_text(lv_obj_t * obj, lv_subject_t * subject, const char * fmt);
#endif


/**
 * Вставьте текст в метку. Текст метки не может быть статическим.
 * @param obj       указатель на объект метки
 * @param pos       индекс символа для вставки. Выражается в индексе символов, а не индексе байтов.
 *                  0: перед первым символом. LV_LABEL_POS_LAST: после последнего символа.
 * @param txt       указатель на текст для вставки
 */
void lv_label_ins_text(lv_obj_t * obj, uint32_t pos, const char * txt);

/**
 * Удаление символов из метки. Текст метки не может быть статическим.
 * @param obj       указатель на объект метки
 * @param pos       индекс символа, откуда вырезать. Выражается в индексе символов, а не индексе байтов.
 *                  0: начать перед первым символом
 * @param cnt       количество символов для вырезания
 */
void lv_label_cut_text(lv_obj_t * obj, uint32_t pos, uint32_t cnt);



/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_LABEL*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_LABEL_H*/
