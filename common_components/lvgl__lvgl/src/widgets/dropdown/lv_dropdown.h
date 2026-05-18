/**
 * @file lv_dropdown.h
 *
 */

#ifndef LV_DROPDOWN_H
#define LV_DROPDOWN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_DROPDOWN != 0

/*Тестирование зависимостей*/

#if LV_USE_LABEL == 0
#error "lv_dropdown: lv_label is required. Enable it in lv_conf.h (LV_USE_LABEL 1)"
#endif

#include "../label/lv_label.h"

/*********************
 *      DEFINES
 *********************/
#define LV_DROPDOWN_POS_LAST 0xFFFF
LV_EXPORT_CONST_INT(LV_DROPDOWN_POS_LAST);

#if LV_USE_OBJ_PROPERTY
enum _lv_property_dropdown_id_t {
    LV_PROPERTY_ID(DROPDOWN, TEXT,                LV_PROPERTY_TYPE_TEXT,   0),
    LV_PROPERTY_ID(DROPDOWN, OPTIONS,             LV_PROPERTY_TYPE_TEXT,   1),
    LV_PROPERTY_ID(DROPDOWN, OPTION_COUNT,        LV_PROPERTY_TYPE_INT,    2),
    LV_PROPERTY_ID(DROPDOWN, SELECTED,            LV_PROPERTY_TYPE_INT,    3),
    // LV_PROPERTY_ID(DROPDOWN, SELECTED_STR,        LV_PROPERTY_TYPE_TEXT,   4),
    LV_PROPERTY_ID(DROPDOWN, DIR,                 LV_PROPERTY_TYPE_INT,    5),
    LV_PROPERTY_ID(DROPDOWN, SYMBOL,              LV_PROPERTY_TYPE_IMGSRC, 6),
    LV_PROPERTY_ID(DROPDOWN, SELECTED_HIGHLIGHT,  LV_PROPERTY_TYPE_INT,    7),
    LV_PROPERTY_ID(DROPDOWN, LIST,                LV_PROPERTY_TYPE_OBJ,    8),
    LV_PROPERTY_ID(DROPDOWN, IS_OPEN,             LV_PROPERTY_TYPE_BOOL,   9),
    LV_PROPERTY_DROPDOWN_END,
};
#endif

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_dropdown_class;
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_dropdownlist_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создать объект раскрывающегося списка
 * @param parent pointer to an object, it will be the parent of the new drop-down list
 * @return pointer to the created drop-down list
 */
lv_obj_t * lv_dropdown_create(lv_obj_t * parent);

/*=====================
 * Функции установки
 *====================*/

/**
 * Задайте текст кнопки раскрывающегося списка.
 * Если установлено значение `NULL`, текст выбранной опции будет отображаться на кнопке.
 * Если установлен определенный текст, этот текст будет отображаться независимо от выбранного параметра.
 * @param obj       pointer to a drop-down list object
 * @param text      the text as a string (Copy is saved)
 */
void lv_dropdown_set_text(lv_obj_t * obj, const char * text);

/**
 * Задайте текст кнопки раскрывающегося списка.
 * Если установлено значение `NULL`, текст выбранной опции будет отображаться на кнопке.
 * Если установлен определенный текст, этот текст будет отображаться независимо от выбранного параметра.
 * @param obj       pointer to a drop-down list object
 * @param text      the text as a string (Only its pointer is saved)
 */
void lv_dropdown_set_text_static(lv_obj_t * obj, const char * text);

/**
 * Установите параметры в раскрывающемся списке из строки.
 * Параметры будут скопированы и сохранены в объекте, поэтому `options` можно будет уничтожить после вызова этой функции.
 * @param obj       pointer to drop-down list object
 * @param options   a string with '\n' separated options. E.g. "One\nTwo\nThree"
 */
void lv_dropdown_set_options(lv_obj_t * obj, const char * options);

/**
 * Установите параметры в раскрывающемся списке из статической строки (глобальной, статической или динамически выделяемой).
 * Будет сохранен только указатель строки параметра.
 * @param obj       pointer to drop-down list object
 * @param options   a static string with '\n' separated options. E.g. "One\nTwo\nThree"
 */
void lv_dropdown_set_options_static(lv_obj_t * obj, const char * options);

/**
 * Добавьте параметры в раскрывающийся список из строки.  Работает только для нестатических опций.
 * @param obj       pointer to drop-down list object
 * @param option    a string without '\n'. E.g. "Four"
 * @param pos       the insert position, indexed from 0, LV_DROPDOWN_POS_LAST = end of string
 */
void lv_dropdown_add_option(lv_obj_t * obj, const char * option, uint32_t pos);

/**
 * Очистите все параметры в раскрывающемся списке.  Работает как со статическими, так и с динамическими параметрами.
 * @param obj       pointer to drop-down list object
 */
void lv_dropdown_clear_options(lv_obj_t * obj);

/**
 * Установить выбранный вариант
 * @param obj       pointer to drop-down list object
 * @param sel_opt   id of the selected option (0 ... number of option - 1);
 */
void lv_dropdown_set_selected(lv_obj_t * obj, uint32_t sel_opt);

/**
 * Установите направление раскрывающегося списка
 * @param obj       pointer to a drop-down list object
 * @param dir       LV_DIR_LEFT/RIGHT/TOP/BOTTOM
 */
void lv_dropdown_set_dir(lv_obj_t * obj, lv_dir_t dir);

/**
 * Установите стрелку или другой символ, который будет отображаться при нажатии на кнопку раскрывающегося списка. Обычно это каретка вниз или стрелка.
 * @param obj       pointer to drop-down list object
 * @param symbol    a text like `LV_SYMBOL_DOWN`, an image (pointer or path) or NULL to not draw symbol icon
 * @note angle and zoom transformation can be applied if the symbol is an image.
 * например когда раскрывающийся список отмечен (открыт), поверните символ на 180 градусов
 */
void lv_dropdown_set_symbol(lv_obj_t * obj, const void * symbol);

/**
 * Установите, должна ли выбранная опция в списке быть выделена или нет
 * @param obj       pointer to drop-down list object
 * @param en        true: highlight enabled; false: disabled
 */
void lv_dropdown_set_selected_highlight(lv_obj_t * obj, bool en);

/*=====================
 * Геттерные функции
 *====================*/

/**
 * Получите список раскрывающегося списка, в котором можно разрешить стилизацию или другие изменения.
 * @param obj       pointer to a drop-down list object
 * @return          pointer to the list of the drop-down
 */
lv_obj_t * lv_dropdown_get_list(lv_obj_t * obj);

/**
 * Получить текст кнопки раскрывающегося списка.
 * @param obj   pointer to a drop-down list object
 * @return      the text as string, `NULL` if no text
 */
const char * lv_dropdown_get_text(lv_obj_t * obj);

/**
 * Получить параметры раскрывающегося списка
 * @param obj       pointer to drop-down list object
 * @return          the options separated by '\n'-s (E.g. "Option1\nOption2\nOption3")
 */
const char * lv_dropdown_get_options(const lv_obj_t * obj);

/**
 * Получить индекс выбранного варианта
 * @param obj       pointer to drop-down list object
 * @return          index of the selected option (0 ... number of option - 1);
 */
uint32_t lv_dropdown_get_selected(const lv_obj_t * obj);

/**
 * Получить общее количество вариантов
 * @param obj       pointer to drop-down list object
 * @return          the total number of options in the list
 */
uint32_t lv_dropdown_get_option_count(const lv_obj_t * obj);

/**
 * Получить текущий выбранный вариант в виде строки
 * @param obj       pointer to drop-down object
 * @param buf       pointer to an array to store the string
 * @param buf_size  size of `buf` in bytes. 0: to ignore it.
 */
void lv_dropdown_get_selected_str(const lv_obj_t * obj, char * buf, uint32_t buf_size);

/**
 * Получить индекс опциона.
 * @param obj       pointer to drop-down object
 * @param option    an option as string
 * @return          index of `option` in the list of all options. -1 if not found.
 */
int32_t lv_dropdown_get_option_index(lv_obj_t * obj, const char * option);

/**
 * Получите символ из раскрывающегося списка. Обычно это каретка вниз или стрелка.
 * @param obj       pointer to drop-down list object
 * @return          the symbol or NULL if not enabled
 */
const char * lv_dropdown_get_symbol(lv_obj_t * obj);

/**
 * Узнайте, следует ли выделять выбранную опцию в списке или нет.
 * @param obj       pointer to drop-down list object
 * @return          true: highlight enabled; false: disabled
 */
bool lv_dropdown_get_selected_highlight(lv_obj_t * obj);

/**
 * Получить направление раскрывающегося списка
 * @param obj       pointer to a drop-down list object
 * @return          LV_DIR_LEF/RIGHT/TOP/BOTTOM
 */
lv_dir_t lv_dropdown_get_dir(const lv_obj_t * obj);

/*=====================
 * Другие функции
 *====================*/

/**
 * Открыть раскрывающийся список
 * @param dropdown_obj       pointer to drop-down list object
 */
void lv_dropdown_open(lv_obj_t * dropdown_obj);

/**
 * Закрыть (Свернуть) раскрывающийся список
 * @param obj       pointer to drop-down list object
 */
void lv_dropdown_close(lv_obj_t * obj);

/**
 * Сообщает, открыт ли список или нет
 * @param obj       pointer to a drop-down list object
 * @return          true if the list os opened
 */
bool lv_dropdown_is_open(lv_obj_t * obj);


#if LV_USE_OBSERVER
/**
 * Привяжите целое число к значению раскрывающегося списка.
 * @param obj       pointer to Dropdown
 * @param subject   pointer to Subject
 * @return          pointer to newly-created Observer
 */
lv_observer_t * lv_dropdown_bind_value(lv_obj_t * obj, lv_subject_t * subject);
#endif

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DROPDOWN*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_DROPDOWN_H*/
