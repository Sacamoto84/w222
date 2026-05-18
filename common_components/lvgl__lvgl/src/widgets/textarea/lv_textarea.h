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
 * @param parent    указатель на объект, он будет родителем новой текстовой области
 * @return          указатель на созданную текстовую область
 */
lv_obj_t * lv_textarea_create(lv_obj_t * parent);

/*======================
 * Добавить/удалить функции
 *=====================*/

/**
 * Вставьте символ в текущую позицию курсора.
 * Чтобы добавить широкий символ, например. 'Á' викор `lv_text_encoded_conv_wc('Á')`
 * @param obj       указатель на объект текстовой области
 * @param c         символ (например, «а»)
 */
void lv_textarea_add_char(lv_obj_t * obj, uint32_t c);

/**
 * Вставить текст в текущую позицию курсора
 * @param obj       указатель на объект текстовой области
 * @param txt       строка, завершающаяся '\0' для вставки
 */
void lv_textarea_add_text(lv_obj_t * obj, const char * txt);

/**
 * Удалить левый символ из текущей позиции курсора
 * @param obj       указатель на объект текстовой области
 */
void lv_textarea_delete_char(lv_obj_t * obj);

/**
 * Удалить правый символ из текущей позиции курсора
 * @param obj       указатель на объект текстовой области
 */
void lv_textarea_delete_char_forward(lv_obj_t * obj);

/*=====================
 * Функции установки
 *====================*/

/**
 * Установить текст текстовой области
 * @param obj       указатель на объект текстовой области
 * @param txt       указатель на текст
 */
void lv_textarea_set_text(lv_obj_t * obj, const char * txt);

/**
 * Установите текст-заполнитель текстовой области
 * @param obj       указатель на объект текстовой области
 * @param txt       указатель на текст
 */
void lv_textarea_set_placeholder_text(lv_obj_t * obj, const char * txt);

/**
 * Установите положение курсора
 * @param obj       указатель на объект текстовой области
 * @param pos       новая позиция курсора в индексе символов
 *                  <0: индекс с конца текста
 *                  LV_TEXTAREA_CURSOR_LAST: идти за последним символом
 */
void lv_textarea_set_cursor_pos(lv_obj_t * obj, int32_t pos);

/**
 * Включите/выключите позиционирование курсора, щелкнув текст в текстовой области.
 * @param obj       указатель на объект текстовой области
 * @param en        true: включить позиции кликов; ложь: отключить
 */
void lv_textarea_set_cursor_click_pos(lv_obj_t * obj, bool en);

/**
 * Включить/отключить режим пароля
 * @param obj       указатель на объект текстовой области
 * @param en        true: включить, false: отключить
 */
void lv_textarea_set_password_mode(lv_obj_t * obj, bool en);

/**
 * Установите символы замены для отображения в режиме пароля
 * @param obj       указатель на объект текстовой области
 * @param bullet    указатель на текст замены
 */
void lv_textarea_set_password_bullet(lv_obj_t * obj, const char * bullet);

/**
 * Настройте текстовую область на одну строку или верните ее в нормальное состояние.
 * @param obj       указатель на объект текстовой области
 * @param en        true: одна строка, false: нормально
 */
void lv_textarea_set_one_line(lv_obj_t * obj, bool en);

/**
 * Установите список персонажей. Только эти символы будут приняты текстовой областью.
 * @param obj       указатель на объект текстовой области
 * @param list      список персонажей. Копия сохранена. Пример: «+-.,0123456789»
 */
void lv_textarea_set_accepted_chars(lv_obj_t * obj, const char * list);

/**
 * Установите список персонажей. Только эти символы будут приняты текстовой областью.
 * @param obj       указатель на объект текстовой области
 * @param list      список персонажей. Сохраняется только указатель. Пример: «+-.,0123456789»
 */
void lv_textarea_set_accepted_chars_static(lv_obj_t * obj, const char * list);

/**
 * Установите максимальную длину текстовой области.
 * @param obj       указатель на объект текстовой области
 * @param num       можно добавить максимальное количество символов (`lv_textarea_set_text` игнорирует это)
 */
void lv_textarea_set_max_length(lv_obj_t * obj, uint32_t num);

/**
 * В `LV_EVENT_INSERT` текст, который планировалось вставить, можно заменить другим текстом.
 * Его можно использовать для добавления автоматического форматирования в текстовую область.
 * @param obj       указатель на объект текстовой области
 * @param txt       указатель на новую строку для вставки. Если `""`, текст не будет добавлен.
 *                  Переменная должна быть активна после запуска`event_cb`. (Должно быть`global`или`static`)
 */
void lv_textarea_set_insert_replace(lv_obj_t * obj, const char * txt);

/**
 * Включить/отключить режим выбора.
 * @param obj       указатель на объект текстовой области
 * @param en        true или false, чтобы включить/отключить режим выбора
 */
void lv_textarea_set_text_selection(lv_obj_t * obj, bool en);

/**
 * Установите, как долго будет отображаться пароль, прежде чем изменить его на «*».
 * @param obj       указатель на объект текстовой области
 * @param time      показывать время в миллисекундах. 0: немедленно скрыть.
 */
void lv_textarea_set_password_show_time(lv_obj_t * obj, uint32_t time);

/**
 * @deprecated Вместо этого используйте обычное свойство стиля text_align.
 * Установите выравнивание метки.
 * Он устанавливает, где выравнивается метка (в однострочном режиме она может быть меньше текстовой области)
 * и как выравниваются линии области в случае многострочной текстовой области
 * @param obj       указатель на объект текстовой области
 * @param align     режим выравнивания из :: lv_text_align_t
 */
void lv_textarea_set_align(lv_obj_t * obj, lv_text_align_t align);

/*=====================
 * Геттерные функции
 *====================*/

/**
 * Получить текст текстовой области. В режиме пароля он дает реальный текст (а не *).
 * @param obj       указатель на объект текстовой области
 * @return          указатель на текст
 */
const char * lv_textarea_get_text(const lv_obj_t * obj);

/**
 * Получить текст-заполнитель текстовой области
 * @param obj       указатель на объект текстовой области
 * @return          указатель на текст
 */
const char * lv_textarea_get_placeholder_text(lv_obj_t * obj);

/**
 * Получить метку текстовой области
 * @param obj       указатель на объект текстовой области
 * @return          указатель на объект метки
 */
lv_obj_t * lv_textarea_get_label(const lv_obj_t * obj);

/**
 * Получить текущую позицию курсора в индексе символов
 * @param obj       указатель на объект текстовой области
 * @return          позиция курсора
 */
uint32_t lv_textarea_get_cursor_pos(const lv_obj_t * obj);

/**
 * Узнайте, включено или нет позиционирование щелчка курсора.
 * @param obj       указатель на объект текстовой области
 * @return          true: включить позиции кликов; ложь: отключить
 */
bool lv_textarea_get_cursor_click_pos(lv_obj_t * obj);

/**
 * Получить атрибут режима пароля
 * @param obj       указатель на объект текстовой области
 * @return          true: режим пароля включен, false: отключен
 */
bool lv_textarea_get_password_mode(const lv_obj_t * obj);

/**
 * Получить замещающие символы для отображения в режиме пароля
 * @param obj       указатель на объект текстовой области
 * @return          указатель на текст замены
 */
const char * lv_textarea_get_password_bullet(lv_obj_t * obj);

/**
 * Получите однострочный атрибут конфигурации
 * @param obj       указатель на объект текстовой области
 * @return          true: однострочная конфигурация включена, false: отключена
 */
bool lv_textarea_get_one_line(const lv_obj_t * obj);

/**
 * Получите список принятых символов.
 * @param obj       указатель на объект текстовой области
 * @return          список акцентированных символов.
 */
const char * lv_textarea_get_accepted_chars(lv_obj_t * obj);

/**
 * Получить максимальную длину текстовой области.
 * @param obj       указатель на объект текстовой области
 * @return          максимальное количество добавляемых символов
 */
uint32_t lv_textarea_get_max_length(lv_obj_t * obj);

/**
 * Узнайте, выделен ли текст или нет.
 * @param obj       указатель на объект текстовой области
 * @return          выделен ли текст или нет
 */
bool lv_textarea_text_is_selected(const lv_obj_t * obj);

/**
 * Проверьте, включен ли режим выбора.
 * @param obj       указатель на объект текстовой области
 * @return          true: режим выбора включен, false: отключен
 */
bool lv_textarea_get_text_selection(lv_obj_t * obj);

/**
 * Установите, как долго будет отображаться пароль, прежде чем изменить его на «*».
 * @param obj       указатель на объект текстовой области
 * @return          показывать время в миллисекундах. 0: немедленно скрыть.
 */
uint32_t lv_textarea_get_password_show_time(lv_obj_t * obj);

/**
 * Получить символ из текущей позиции курсора
 * @param obj       указатель на объект текстовой области
 * @return          символ или 0
 */
uint32_t lv_textarea_get_current_char(lv_obj_t * obj);

/*=====================
 * Другие функции
 *====================*/

/**
 * Снимите выделение в текстовой области.
 * @param obj       указатель на объект текстовой области
 */
void lv_textarea_clear_selection(lv_obj_t * obj);

/**
 * Переместить курсор на один символ вправо
 * @param obj       указатель на объект текстовой области
 */
void lv_textarea_cursor_right(lv_obj_t * obj);

/**
 * Переместить курсор на один символ влево
 * @param obj       указатель на объект текстовой области
 */
void lv_textarea_cursor_left(lv_obj_t * obj);

/**
 * Переместить курсор на одну строку вниз
 * @param obj       указатель на объект текстовой области
 */
void lv_textarea_cursor_down(lv_obj_t * obj);

/**
 * Переместить курсор на одну строку вверх
 * @param obj       указатель на объект текстовой области
 */
void lv_textarea_cursor_up(lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_TEXTAREA_H*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TEXTAREA_H*/
