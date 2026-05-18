/**
 * @file lv_textarea.h
 *
 */

#ifndef LV_TEXTAREA_H
#define LV_TEXTAREA_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../label/lv_label.h"

#if LV_USE_TEXTAREA != 0

/*Тестирование зависимостей*/
#if LV_USE_LABEL == 0
#error "lv_textarea: lv_label is required. Enable it in lv_conf.h (LV_USE_LABEL 1)"
#endif

/*********************
 *      DEFINES
 *********************/
#define LV_TEXTAREA_CURSOR_LAST (0x7FFF) /*Поместите курсор после последнего символа*/
LV_EXPORT_CONST_INT(LV_TEXTAREA_CURSOR_LAST);

#define LV_PART_TEXTAREA_PLACEHOLDER LV_PART_CUSTOM_FIRST
LV_EXPORT_CONST_INT(LV_PART_TEXTAREA_PLACEHOLDER);

/**********************
 *      TYPEDEFS
 **********************/

#if LV_USE_OBJ_PROPERTY
enum _lv_property_textarea_id_t {
    LV_PROPERTY_ID(TEXTAREA, TEXT,              LV_PROPERTY_TYPE_TEXT,  0),
    LV_PROPERTY_ID(TEXTAREA, PLACEHOLDER_TEXT,  LV_PROPERTY_TYPE_TEXT,  1),
    LV_PROPERTY_ID(TEXTAREA, CURSOR_POS,        LV_PROPERTY_TYPE_INT,   2),
    LV_PROPERTY_ID(TEXTAREA, CURSOR_CLICK_POS,  LV_PROPERTY_TYPE_INT,   3),
    LV_PROPERTY_ID(TEXTAREA, PASSWORD_MODE,     LV_PROPERTY_TYPE_INT,   4),
    LV_PROPERTY_ID(TEXTAREA, PASSWORD_BULLET,   LV_PROPERTY_TYPE_TEXT,  5),
    LV_PROPERTY_ID(TEXTAREA, ONE_LINE,          LV_PROPERTY_TYPE_BOOL,  6),
    LV_PROPERTY_ID(TEXTAREA, ACCEPTED_CHARS,    LV_PROPERTY_TYPE_TEXT,  7),
    LV_PROPERTY_ID(TEXTAREA, MAX_LENGTH,        LV_PROPERTY_TYPE_INT,   8),
    LV_PROPERTY_ID(TEXTAREA, TEXT_SELECTION,    LV_PROPERTY_TYPE_BOOL,  9),
    LV_PROPERTY_ID(TEXTAREA, PASSWORD_SHOW_TIME, LV_PROPERTY_TYPE_INT,  10),
    LV_PROPERTY_ID(TEXTAREA, LABEL,             LV_PROPERTY_TYPE_OBJ,   11),
    LV_PROPERTY_ID(TEXTAREA, TEXT_IS_SELECTED,  LV_PROPERTY_TYPE_INT,   12),
    LV_PROPERTY_ID(TEXTAREA, CURRENT_CHAR,      LV_PROPERTY_TYPE_INT,   13),
    LV_PROPERTY_TEXTAREA_END,
};
#endif

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_textarea_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создание объекта текстовой области
 * @param parent    pointer to an object, it will be the parent of the new text area
 * @return          pointer to the created text area
 */
lv_obj_t * lv_textarea_create(lv_obj_t * parent);

/*======================
 * Добавить/удалить функции
 *=====================*/

/**
 * Вставьте символ в текущую позицию курсора.
 * Чтобы добавить широкий символ, например. 'Á' используйте `lv_text_encoded_conv_wc('Á')`
 * @param obj       pointer to a text area object
 * @param c         a character (e.g. 'a')
 */
void lv_textarea_add_char(lv_obj_t * obj, uint32_t c);

/**
 * Вставить текст в текущую позицию курсора
 * @param obj       pointer to a text area object
 * @param txt       a '\0' terminated string to insert
 */
void lv_textarea_add_text(lv_obj_t * obj, const char * txt);

/**
 * Удалить левый символ из текущей позиции курсора
 * @param obj       pointer to a text area object
 */
void lv_textarea_delete_char(lv_obj_t * obj);

/**
 * Удалить правый символ из текущей позиции курсора
 * @param obj       pointer to a text area object
 */
void lv_textarea_delete_char_forward(lv_obj_t * obj);

/*=====================
 * Функции установки
 *====================*/

/**
 * Установить текст текстовой области
 * @param obj       pointer to a text area object
 * @param txt       pointer to the text
 */
void lv_textarea_set_text(lv_obj_t * obj, const char * txt);

/**
 * Установите текст-заполнитель текстовой области
 * @param obj       pointer to a text area object
 * @param txt       pointer to the text
 */
void lv_textarea_set_placeholder_text(lv_obj_t * obj, const char * txt);

/**
 * Установите положение курсора
 * @param obj       pointer to a text area object
 * @param pos       the new cursor position in character index
 *                  <0: индекс с конца текста
 *                  LV_TEXTAREA_CURSOR_LAST: go after the last character
 */
void lv_textarea_set_cursor_pos(lv_obj_t * obj, int32_t pos);

/**
 * Включите/выключите позиционирование курсора, щелкнув текст в текстовой области.
 * @param obj       pointer to a text area object
 * @param en        true: enable click positions; false: disable
 */
void lv_textarea_set_cursor_click_pos(lv_obj_t * obj, bool en);

/**
 * Включить/отключить режим пароля
 * @param obj       pointer to a text area object
 * @param en        true: enable, false: disable
 */
void lv_textarea_set_password_mode(lv_obj_t * obj, bool en);

/**
 * Установите символы замены для отображения в режиме пароля
 * @param obj       pointer to a text area object
 * @param bullet    pointer to the replacement text
 */
void lv_textarea_set_password_bullet(lv_obj_t * obj, const char * bullet);

/**
 * Настройте текстовую область на одну строку или верните ее в нормальное состояние.
 * @param obj       pointer to a text area object
 * @param en        true: one line, false: normal
 */
void lv_textarea_set_one_line(lv_obj_t * obj, bool en);

/**
 * Установите список персонажей. Только эти символы будут приняты текстовой областью.
 * @param obj       pointer to a text area object
 * @param list      list of characters. A copy is saved. Example: "+-.,0123456789"
 */
void lv_textarea_set_accepted_chars(lv_obj_t * obj, const char * list);

/**
 * Установите список персонажей. Только эти символы будут приняты текстовой областью.
 * @param obj       pointer to a text area object
 * @param list      list of characters. Only the pointer is saved. Example: "+-.,0123456789"
 */
void lv_textarea_set_accepted_chars_static(lv_obj_t * obj, const char * list);

/**
 * Установите максимальную длину текстовой области.
 * @param obj       pointer to a text area object
 * @param num       the maximal number of characters can be added (`lv_textarea_set_text` ignores it)
 */
void lv_textarea_set_max_length(lv_obj_t * obj, uint32_t num);

/**
 * В `LV_EVENT_INSERT` текст, который планировалось вставить, можно заменить другим текстом.
 * Его можно использовать для добавления автоматического форматирования в текстовую область.
 * @param obj       pointer to a text area object
 * @param txt       pointer to a new string to insert. If `""` no text will be added.
 *                  Переменная должна быть активной после существования `event_cb`. (Должно быть `global` или `static` )
 */
void lv_textarea_set_insert_replace(lv_obj_t * obj, const char * txt);

/**
 * Включить/отключить режим выбора.
 * @param obj       pointer to a text area object
 * @param en        true or false to enable/disable selection mode
 */
void lv_textarea_set_text_selection(lv_obj_t * obj, bool en);

/**
 * Установите, как долго будет отображаться пароль, прежде чем изменить его на «*».
 * @param obj       pointer to a text area object
 * @param time      show time in milliseconds. 0: hide immediately.
 */
void lv_textarea_set_password_show_time(lv_obj_t * obj, uint32_t time);

/**
 * @deprecated Use the normal text_align style property instead
 * Установите выравнивание метки.
 * Он устанавливает, где выравнивается метка (в однострочном режиме она может быть меньше текстовой области)
 * и как выравниваются линии области в случае многострочной текстовой области
 * @param obj       pointer to a text area object
 * @param align     the align mode from ::lv_text_align_t
 */
void lv_textarea_set_align(lv_obj_t * obj, lv_text_align_t align);

/*=====================
 * Геттерные функции
 *====================*/

/**
 * Получить текст текстовой области. В режиме пароля он дает реальный текст (а не *).
 * @param obj       pointer to a text area object
 * @return          pointer to the text
 */
const char * lv_textarea_get_text(const lv_obj_t * obj);

/**
 * Получить текст-заполнитель текстовой области
 * @param obj       pointer to a text area object
 * @return          pointer to the text
 */
const char * lv_textarea_get_placeholder_text(lv_obj_t * obj);

/**
 * Получить метку текстовой области
 * @param obj       pointer to a text area object
 * @return          pointer to the label object
 */
lv_obj_t * lv_textarea_get_label(const lv_obj_t * obj);

/**
 * Получить текущую позицию курсора в индексе символов
 * @param obj       pointer to a text area object
 * @return          the cursor position
 */
uint32_t lv_textarea_get_cursor_pos(const lv_obj_t * obj);

/**
 * Узнайте, включено или нет позиционирование щелчка курсора.
 * @param obj       pointer to a text area object
 * @return          true: enable click positions; false: disable
 */
bool lv_textarea_get_cursor_click_pos(lv_obj_t * obj);

/**
 * Получить атрибут режима пароля
 * @param obj       pointer to a text area object
 * @return          true: password mode is enabled, false: disabled
 */
bool lv_textarea_get_password_mode(const lv_obj_t * obj);

/**
 * Получить замещающие символы для отображения в режиме пароля
 * @param obj       pointer to a text area object
 * @return          pointer to the replacement text
 */
const char * lv_textarea_get_password_bullet(lv_obj_t * obj);

/**
 * Получите однострочный атрибут конфигурации
 * @param obj       pointer to a text area object
 * @return          true: one line configuration is enabled, false: disabled
 */
bool lv_textarea_get_one_line(const lv_obj_t * obj);

/**
 * Получите список принятых символов.
 * @param obj       pointer to a text area object
 * @return          list of accented characters.
 */
const char * lv_textarea_get_accepted_chars(lv_obj_t * obj);

/**
 * Получить максимальную длину текстовой области.
 * @param obj       pointer to a text area object
 * @return          the maximal number of characters to be add
 */
uint32_t lv_textarea_get_max_length(lv_obj_t * obj);

/**
 * Узнайте, выделен ли текст или нет.
 * @param obj       pointer to a text area object
 * @return          whether text is selected or not
 */
bool lv_textarea_text_is_selected(const lv_obj_t * obj);

/**
 * Проверьте, включен ли режим выбора.
 * @param obj       pointer to a text area object
 * @return          true: selection mode is enabled, false: disabled
 */
bool lv_textarea_get_text_selection(lv_obj_t * obj);

/**
 * Установите, как долго будет отображаться пароль, прежде чем изменить его на «*».
 * @param obj       pointer to a text area object
 * @return          show time in milliseconds. 0: hide immediately.
 */
uint32_t lv_textarea_get_password_show_time(lv_obj_t * obj);

/**
 * Получить символ из текущей позиции курсора
 * @param obj       pointer to a text area object
 * @return          a the character or 0
 */
uint32_t lv_textarea_get_current_char(lv_obj_t * obj);

/*=====================
 * Другие функции
 *====================*/

/**
 * Снимите выделение в текстовой области.
 * @param obj       pointer to a text area object
 */
void lv_textarea_clear_selection(lv_obj_t * obj);

/**
 * Переместить курсор на один символ вправо
 * @param obj       pointer to a text area object
 */
void lv_textarea_cursor_right(lv_obj_t * obj);

/**
 * Переместить курсор на один символ влево
 * @param obj       pointer to a text area object
 */
void lv_textarea_cursor_left(lv_obj_t * obj);

/**
 * Переместить курсор на одну строку вниз
 * @param obj       pointer to a text area object
 */
void lv_textarea_cursor_down(lv_obj_t * obj);

/**
 * Переместить курсор на одну строку вверх
 * @param obj       pointer to a text area object
 */
void lv_textarea_cursor_up(lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_TEXTAREA_H*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_TEXTAREA_H*/
