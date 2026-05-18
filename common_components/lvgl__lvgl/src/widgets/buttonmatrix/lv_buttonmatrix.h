/**
 * @file lv_buttonmatrix.h
 *
 */

#ifndef LV_BUTTONMATRIX_H
#define LV_BUTTONMATRIX_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_BUTTONMATRIX != 0

#include "../../core/lv_obj.h"
#include "../../core/lv_obj_property.h"

/*********************
 *      DEFINES
 *********************/
#define LV_BUTTONMATRIX_BUTTON_NONE 0xFFFF
LV_EXPORT_CONST_INT(LV_BUTTONMATRIX_BUTTON_NONE);

/**********************
 *      TYPEDEFS
 **********************/

/** Введите для сохранения флагов управления кнопками (отключено, скрыто и т. д.).
 *  Младшие 4 бита используются для хранения пропорций ширины кнопок в диапазоне [1..15]. */
typedef enum {
    LV_BUTTONMATRIX_CTRL_NONE         = 0x0000, /**< No extra control, use the default settings*/
    LV_BUTTONMATRIX_CTRL_WIDTH_1      = 0x0001, /**< Set the width to 1 relative to the other buttons in the same row */
    LV_BUTTONMATRIX_CTRL_WIDTH_2      = 0x0002, /**< Set the width to 2 relative to the other buttons in the same row */
    LV_BUTTONMATRIX_CTRL_WIDTH_3      = 0x0003, /**< Set the width to 3 relative to the other buttons in the same row */
    LV_BUTTONMATRIX_CTRL_WIDTH_4      = 0x0004, /**< Set the width to 4 relative to the other buttons in the same row */
    LV_BUTTONMATRIX_CTRL_WIDTH_5      = 0x0005, /**< Set the width to 5 relative to the other buttons in the same row */
    LV_BUTTONMATRIX_CTRL_WIDTH_6      = 0x0006, /**< Set the width to 6 relative to the other buttons in the same row */
    LV_BUTTONMATRIX_CTRL_WIDTH_7      = 0x0007, /**< Set the width to 7 relative to the other buttons in the same row */
    LV_BUTTONMATRIX_CTRL_WIDTH_8      = 0x0008, /**< Set the width to 8 relative to the other buttons in the same row */
    LV_BUTTONMATRIX_CTRL_WIDTH_9      = 0x0009, /**< Set the width to 9 relative to the other buttons in the same row */
    LV_BUTTONMATRIX_CTRL_WIDTH_10     = 0x000A, /**< Set the width to 10 relative to the other buttons in the same row */
    LV_BUTTONMATRIX_CTRL_WIDTH_11     = 0x000B, /**< Set the width to 11 relative to the other buttons in the same row */
    LV_BUTTONMATRIX_CTRL_WIDTH_12     = 0x000C, /**< Set the width to 12 relative to the other buttons in the same row */
    LV_BUTTONMATRIX_CTRL_WIDTH_13     = 0x000D, /**< Set the width to 13 relative to the other buttons in the same row */
    LV_BUTTONMATRIX_CTRL_WIDTH_14     = 0x000E, /**< Set the width to 14 relative to the other buttons in the same row */
    LV_BUTTONMATRIX_CTRL_WIDTH_15     = 0x000F, /**< Set the width to 15 relative to the other buttons in the same row */
    LV_BUTTONMATRIX_CTRL_HIDDEN       = 0x0010, /**< Hides button; it continues to hold its space in layout. */
    LV_BUTTONMATRIX_CTRL_NO_REPEAT    = 0x0020, /**< Do not emit LV_EVENT_LONG_PRESSED_REPEAT events while button is long-pressed. */
    LV_BUTTONMATRIX_CTRL_DISABLED     = 0x0040, /**< Disables button like LV_STATE_DISABLED on normal Widgets. */
    LV_BUTTONMATRIX_CTRL_CHECKABLE    = 0x0080, /**< Enable toggling of LV_STATE_CHECKED when clicked. */
    LV_BUTTONMATRIX_CTRL_CHECKED      = 0x0100, /**< Make the button checked. It will use the :cpp:enumerator:`LV_STATE_CHECHKED` styles. */
    LV_BUTTONMATRIX_CTRL_CLICK_TRIG   = 0x0200, /**< 1: Enables sending LV_EVENT_VALUE_CHANGE on CLICK, 0: sends LV_EVENT_VALUE_CHANGE on PRESS. */
    LV_BUTTONMATRIX_CTRL_POPOVER      = 0x0400, /**< Show button text in a pop-over while being pressed. */
    LV_BUTTONMATRIX_CTRL_RECOLOR      = 0x0800, /**< Enable text recoloring with `#color` */
    LV_BUTTONMATRIX_CTRL_RESERVED_1   = 0x1000, /**< Reserved for later use */
    LV_BUTTONMATRIX_CTRL_RESERVED_2   = 0x2000, /**< Reserved for later use */
    LV_BUTTONMATRIX_CTRL_CUSTOM_1     = 0x4000, /**< Custom free-to-use flag */
    LV_BUTTONMATRIX_CTRL_CUSTOM_2     = 0x8000, /**< Custom free-to-use flag */
} lv_buttonmatrix_ctrl_t;

typedef bool (*lv_buttonmatrix_button_draw_cb_t)(lv_obj_t * btnm, uint32_t btn_id, const lv_area_t * draw_area,
                                                 const lv_area_t * clip_area);

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_buttonmatrix_class;

#if LV_USE_OBJ_PROPERTY
enum _lv_property_buttonmatrix_id_t {
    LV_PROPERTY_ID(BUTTONMATRIX, SELECTED_BUTTON, LV_PROPERTY_TYPE_INT, 0),
    LV_PROPERTY_ID(BUTTONMATRIX, ONE_CHECKED,     LV_PROPERTY_TYPE_BOOL, 1),
    LV_PROPERTY_BUTTONMATRIX_END,
};
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создайте объект матрицы кнопок
 * @param parent    указатель на объект, он будет родителем новой матрицы кнопок
 * @return          указатель на созданную матрицу кнопок
 */
lv_obj_t * lv_buttonmatrix_create(lv_obj_t * parent);

/*=====================
 * Функции установки
 *====================*/

/**
 * Установить новую карту. Кнопки будут созданы/удалены в соответствии с картой.
 * матрица кнопок сохраняет ссылку на карту, поэтому массив строк не должен
 * быть освобождены в течение срока службы матрицы.
 * @param obj       указатель на объект матрицы кнопки
 * @param map       указатель на массив строк. Последняя строка должна быть: «». Используйте «\n», чтобы сделать разрыв строки.
 */
void lv_buttonmatrix_set_map(lv_obj_t * obj, const char * const map[]);

/**
 * Установите карту управления кнопками (скрытая, отключенная и т. д.) для матрицы кнопок.
 * Массив карты управления будет скопирован и после этого может быть освобожден.
 * функция возвращает.
 * @param obj       указатель на объект матрицы кнопки
 * @param ctrl_map  указатель на массив управляющих байтов `lv_button_ctrl_t`.
 *                  длина массива и положение элементов должны совпадать
 *                  количество и порядок отдельных кнопок (т.е. исключает
 *                  записи новой строки).
 *                  Элемент карты должен выглядеть, например, так:
 *                 `ctrl_map[0] = width | LV_BUTTONMATRIX_CTRL_NO_REPEAT |  LV_BUTTONMATRIX_CTRL_TGL_ENABLE`
 */
void lv_buttonmatrix_set_ctrl_map(lv_obj_t * obj, const lv_buttonmatrix_ctrl_t ctrl_map[]);

/**
 * Установить выбранные кнопки
 * @param obj        указатель на объект матрицы кнопок
 * @param btn_id     Индекс кнопки, которую нужно изменить, основан на 0. (не считая новых строк)
 */
void lv_buttonmatrix_set_selected_button(lv_obj_t * obj, uint32_t btn_id);

/**
 * Установите атрибуты кнопки матрицы кнопок
 * @param obj       указатель на объект матрицы кнопок
 * @param btn_id    Индекс кнопки, которую нужно изменить, основан на 0. (не считая новых строк)
 * @param ctrl      OR - атрибуты. Например.  `LV_BUTTONMATRIX_CTRL_NO_REPEAT | LV_BUTTONMATRIX_CTRL_CHECKABLE`
 */
void lv_buttonmatrix_set_button_ctrl(lv_obj_t * obj, uint32_t btn_id, lv_buttonmatrix_ctrl_t ctrl);

/**
 * Очистить атрибуты кнопки матрицы кнопок.
 * @param obj       указатель на объект матрицы кнопок
 * @param btn_id    Индекс кнопки, которую нужно изменить, основан на 0. (не считая новых строк)
 * @param ctrl      OR - атрибуты. Например.  `LV_BUTTONMATRIX_CTRL_NO_REPEAT | LV_BUTTONMATRIX_CTRL_CHECKABLE`
 */
void lv_buttonmatrix_clear_button_ctrl(lv_obj_t * obj, uint32_t btn_id, lv_buttonmatrix_ctrl_t ctrl);

/**
 * Установить атрибуты всех кнопок матрицы кнопок
 * @param obj       указатель на объект матрицы кнопки
 * @param ctrl      атрибут(ы) для установки из`lv_buttonmatrix_ctrl_t`. Значения могут быть объединены OR.
 */
void lv_buttonmatrix_set_button_ctrl_all(lv_obj_t * obj, lv_buttonmatrix_ctrl_t ctrl);

/**
 * Очистить атрибуты всех кнопок матрицы кнопок.
 * @param obj       указатель на объект матрицы кнопки
 * @param ctrl      атрибут(ы) для установки из`lv_buttonmatrix_ctrl_t`. Значения могут быть объединены OR.
 */
void lv_buttonmatrix_clear_button_ctrl_all(lv_obj_t * obj, lv_buttonmatrix_ctrl_t ctrl);

/**
 * Установите относительную ширину одной кнопки.
 * Этот метод приведет к регенерации матрицы и является относительно
 * дорогая операция. Рекомендуется указывать начальную ширину с помощью
 * `lv_buttonmatrix_set_ctrl_map` и этот метод можно использовать только для стимулирования изменений.
 * @param obj       указатель на объект матрицы кнопок
 * @param btn_id    Индекс кнопки, которую нужно изменить, основан на 0.
 * @param width     относительная ширина по сравнению с кнопками в том же ряду. [1..15]
 */
void lv_buttonmatrix_set_button_width(lv_obj_t * obj, uint32_t btn_id, uint32_t width);

/**
 * Сделайте матрицу кнопок похожей на виджет выбора (одновременно можно проверять только одну кнопку).
 * `LV_BUTTONMATRIX_CTRL_CHECKABLE` должен быть включен на кнопках, которые будут выбраны с помощью
 * `lv_buttonmatrix_set_ctrl()` или`lv_buttonmatrix_set_button_ctrl_all()`.
 * @param obj       указатель на объект матрицы кнопки
 * @param en        включен ли режим «одной проверки»
 */
void lv_buttonmatrix_set_one_checked(lv_obj_t * obj, bool en);

/*=====================
 * Геттерные функции
 *====================*/

/**
 * Получить текущую карту матрицы кнопок
 * @param obj       указатель на объект матрицы кнопки
 * @return          текущая карта
 */
const char * const * lv_buttonmatrix_get_map(const lv_obj_t * obj);

/**
 * Получить индекс последней «активированной» пользователем кнопки (нажатой, отпущенной, сфокусированной и т. д.)
 * Полезно в`event_cb`, чтобы получить текст кнопки, проверить, скрыта ли она и т. д. д.
 * @param obj       указатель на объект матрицы кнопок
 * @return          индекс последней отпущенной кнопки (LV_BUTTONMATRIX_BUTTON_NONE: если не установлено)
 */
uint32_t lv_buttonmatrix_get_selected_button(const lv_obj_t * obj);

/**
 * Получить текст кнопки
 * @param obj       указатель на объект матрицы кнопок
 * @param btn_id    индексная кнопка, не учитывающая символы новой строки.
 * @return          текст кнопкиbtn_index`
 */
const char * lv_buttonmatrix_get_button_text(const lv_obj_t * obj, uint32_t btn_id);

/**
 * Узнайте, включено или отключено значение элемента управления для кнопки матрицы кнопок.
 * @param obj       указатель на объект матрицы кнопки
 * @param btn_id    индекс кнопки без учета символов новой строки.
 * @param ctrl      контрольные значения для проверки (можно использовать значение OR)
 * @return          true: атрибут управления включен false: отключен
 */
bool lv_buttonmatrix_has_button_ctrl(lv_obj_t * obj, uint32_t btn_id, lv_buttonmatrix_ctrl_t ctrl);

/**
 * Скажите, включен режим «одной проверки» или нет.
 * @param obj       Объект матрицы кнопок
 * @return          true: включен режим «одной проверки»; ложь: отключено
 */
bool lv_buttonmatrix_get_one_checked(const lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_BUTTONMATRIX*/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_BUTTONMATRIX_H*/
