/**
 * @file lv_keyboard.h
 *
 */

#ifndef LV_KEYBOARD_H
#define LV_KEYBOARD_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../buttonmatrix/lv_buttonmatrix.h"

#if LV_USE_KEYBOARD

/*********************
 *      DEFINES
 *********************/
#define LV_KEYBOARD_CTRL_BUTTON_FLAGS (LV_BUTTONMATRIX_CTRL_NO_REPEAT | LV_BUTTONMATRIX_CTRL_CLICK_TRIG | LV_BUTTONMATRIX_CTRL_CHECKED)

/**********************
 *      TYPEDEFS
 **********************/

/** Текущий режим клавиатуры.*/
typedef enum {
    LV_KEYBOARD_MODE_TEXT_LOWER,
    LV_KEYBOARD_MODE_TEXT_UPPER,
    LV_KEYBOARD_MODE_SPECIAL,
    LV_KEYBOARD_MODE_NUMBER,
    LV_KEYBOARD_MODE_USER_1,
    LV_KEYBOARD_MODE_USER_2,
    LV_KEYBOARD_MODE_USER_3,
    LV_KEYBOARD_MODE_USER_4,
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
    LV_KEYBOARD_MODE_TEXT_ARABIC
#endif
} lv_keyboard_mode_t;

#if LV_USE_OBJ_PROPERTY
enum _lv_property_keyboard_id_t {
    LV_PROPERTY_ID(KEYBOARD, TEXTAREA,            LV_PROPERTY_TYPE_OBJ,   0),
    LV_PROPERTY_ID(KEYBOARD, MODE,                LV_PROPERTY_TYPE_INT,   1),
    LV_PROPERTY_ID(KEYBOARD, POPOVERS,            LV_PROPERTY_TYPE_INT,   2),
    LV_PROPERTY_ID(KEYBOARD, SELECTED_BUTTON,     LV_PROPERTY_TYPE_INT,   3),
    LV_PROPERTY_KEYBOARD_END,
};
#endif

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_keyboard_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создайте объект «Клавиатура»
 * @param parent    pointer to an object, it will be the parent of the new keyboard
 * @return          pointer to the created keyboard object
 */
lv_obj_t * lv_keyboard_create(lv_obj_t * parent);

/*=====================
 * Функции установки
 *====================*/

/**
 * Назначьте текстовую область на клавиатуру. Туда будут вставлены нажатые символы.
 * @param kb        pointer to a keyboard object
 * @param ta        pointer to a text area object to write into
 */
void lv_keyboard_set_textarea(lv_obj_t * kb, lv_obj_t * ta);

/**
 * Установите новый режим (например, текст, число, специальные символы).
 * @param kb        pointer to a keyboard object
 * @param mode      the desired mode (see 'lv_keyboard_mode_t')
 */
void lv_keyboard_set_mode(lv_obj_t * kb, lv_keyboard_mode_t mode);

/**
 * Включите или отключите всплывающие окна, показывающие заголовки кнопок при нажатии.
 * @param kb        pointer to a keyboard object
 * @param en        true to enable popovers; false to disable
 */
void lv_keyboard_set_popovers(lv_obj_t * kb, bool en);

/**
 * Установите пользовательскую схему кнопок для клавиатуры.
 * @param kb        pointer to a keyboard object
 * @param mode      the mode to assign the new map to (see 'lv_keyboard_mode_t')
 * @param map       pointer to a string array describing the button map
 *                  см. «lv_buttonmatrix_set_map ()» для получения более подробной информации.
 * @param ctrl_map  pointer to the control map. See 'lv_buttonmatrix_set_ctrl_map()'

 */
void lv_keyboard_set_map(lv_obj_t * kb, lv_keyboard_mode_t mode, const char * const map[],
                         const lv_buttonmatrix_ctrl_t ctrl_map[]);

/*=====================
 * Геттерные функции
 *====================*/

/**
 * Получите текстовую область, назначенную в данный момент клавиатуре.
 * @param kb        pointer to a keyboard object
 * @return          pointer to the assigned text area object
 */
lv_obj_t * lv_keyboard_get_textarea(const lv_obj_t * kb);

/**
 * Получить текущий режим клавиатуры.
 * @param kb        pointer to a keyboard object
 * @return          the current mode (see 'lv_keyboard_mode_t')
 */
lv_keyboard_mode_t lv_keyboard_get_mode(const lv_obj_t * kb);

/**
 * Проверьте, включены ли всплывающие окна на клавиатуре.
 * @param obj       pointer to a keyboard object
 * @return          true if popovers are enabled; false otherwise
 */
bool lv_keyboard_get_popovers(const lv_obj_t * obj);

/**
 * Получите текущую карту кнопок клавиатуры.
 * @param kb        pointer to a keyboard object
 * @return          pointer to the map array
 */
const char * const * lv_keyboard_get_map_array(const lv_obj_t * kb);

/**
 * Получить индекс последней выбранной кнопки (нажатой, отпущенной, сфокусированной и т. д.).
 * Полезно в `event_cb` для получения текста или свойств кнопки.
 * @param obj       pointer to a keyboard object
 * @return          index of the last interacted button
 *                  возвращает LV_BUTTONMATRIX_BUTTON_NONE, если не установлено
 */
uint32_t lv_keyboard_get_selected_button(const lv_obj_t * obj);

/**
 * Получить текст кнопки по индексу.
 * @param obj       pointer to a keyboard object
 * @param btn_id    index of the button (excluding newline characters)
 * @return          pointer to the text of the button
 */
const char * lv_keyboard_get_button_text(const lv_obj_t * obj, uint32_t btn_id);

/*=====================
 * Другие функции
 *====================*/

/**
 * Обратный вызов событий клавиатуры по умолчанию для обработки нажатий кнопок.
 * Добавляет символы в текстовую область и при необходимости переключает карту.
 * Если используется пользовательский `event_cb`, эту функцию можно вызвать внутри него.
 * @param e         the triggering event
 */
void lv_keyboard_def_event_cb(lv_event_t * e);

/**********************
 *      MACROS
 **********************/

#endif  /*LV_USE_KEYBOARD*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_KEYBOARD_H*/
