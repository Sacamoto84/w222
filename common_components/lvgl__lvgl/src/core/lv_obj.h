/**
 * @file lv_obj.h
 *
 */

#ifndef LV_OBJ_H
#define LV_OBJ_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#include "../misc/lv_types.h"
#include "../misc/lv_style.h"
#include "../misc/lv_area.h"
#include "../misc/lv_color.h"
#include "../misc/lv_assert.h"

#include "lv_obj_tree.h"
#include "lv_obj_pos.h"
#include "lv_obj_scroll.h"
#include "lv_obj_style.h"
#include "lv_obj_draw.h"
#include "lv_obj_class.h"
#include "lv_obj_event.h"
#include "lv_obj_property.h"
#include "lv_group.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/**
 * Функции включения/выключения, управляющие поведением объекта.
 * Значения можно объединять побитовым ИЛИ.
 *
 * Note: при добавлении/удалении флагов или изменении их битов
 * нужно обновить соответствующие свойства ниже.
 */
typedef enum {
    LV_OBJ_FLAG_HIDDEN          = (1u << 0),  /**< Скрыть объект, как будто его нет*/
    LV_OBJ_FLAG_CLICKABLE       = (1u << 1),  /**< Сделать объект кликабельным для устройств ввода*/
    LV_OBJ_FLAG_CLICK_FOCUSABLE = (1u << 2),  /**< Добавлять состояние фокуса при нажатии*/
    LV_OBJ_FLAG_CHECKABLE       = (1u << 3),  /**< Переключать состояние checked при нажатии*/
    LV_OBJ_FLAG_SCROLLABLE      = (1u << 4),  /**< Разрешить прокрутку объекта*/
    LV_OBJ_FLAG_SCROLL_ELASTIC  = (1u << 5),  /**< Разрешить упругую прокрутку внутри объекта*/
    LV_OBJ_FLAG_SCROLL_MOMENTUM = (1u << 6),  /**< Продолжать прокрутку по инерции*/
    LV_OBJ_FLAG_SCROLL_ONE      = (1u << 7),  /**< Прокручивать только один привязываемый дочерний объект*/
    LV_OBJ_FLAG_SCROLL_CHAIN_HOR = (1u << 8), /**< Передавать горизонтальную прокрутку родителю*/
    LV_OBJ_FLAG_SCROLL_CHAIN_VER = (1u << 9), /**< Передавать вертикальную прокрутку родителю*/
    LV_OBJ_FLAG_SCROLL_CHAIN     = (LV_OBJ_FLAG_SCROLL_CHAIN_HOR | LV_OBJ_FLAG_SCROLL_CHAIN_VER),
    LV_OBJ_FLAG_SCROLL_ON_FOCUS = (1u << 10),  /**< Автоматически прокручивать к объекту при получении фокуса*/
    LV_OBJ_FLAG_SCROLL_WITH_ARROW  = (1u << 11), /**< Разрешить прокрутку сфокусированного объекта стрелками*/
    LV_OBJ_FLAG_SNAPPABLE       = (1u << 12), /**< Если у родителя включена привязка прокрутки, он может привязаться к этому объекту*/
    LV_OBJ_FLAG_PRESS_LOCK      = (1u << 13), /**< Сохранять состояние нажатия, даже если указатель сместился с объекта*/
    LV_OBJ_FLAG_EVENT_BUBBLE    = (1u << 14), /**< Передавать события также родителю*/
    LV_OBJ_FLAG_GESTURE_BUBBLE  = (1u << 15), /**< Передавать жесты родителю*/
    LV_OBJ_FLAG_ADV_HITTEST     = (1u << 16), /**< Выполнять более точную проверку попадания, например с учетом скругленных углов*/
    LV_OBJ_FLAG_IGNORE_LAYOUT   = (1u << 17), /**< Не позиционировать объект через макет*/
    LV_OBJ_FLAG_FLOATING        = (1u << 18), /**< Не прокручивать объект вместе с родителем и игнорировать макет*/
    LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS = (1u << 19), /**< Отправлять события `LV_EVENT_DRAW_TASK_ADDED`*/
    LV_OBJ_FLAG_OVERFLOW_VISIBLE = (1u << 20),/**< Не обрезать дочерние объекты по расширенной области отрисовки родителя*/
    LV_OBJ_FLAG_EVENT_TRICKLE   = (1u << 21), /**< Передавать события также дочерним объектам*/
    LV_OBJ_FLAG_STATE_TRICKLE   = (1u << 22), /**< Передавать состояния также дочерним объектам*/

    LV_OBJ_FLAG_LAYOUT_1        = (1u << 23), /**< Пользовательский флаг для макетов*/
    LV_OBJ_FLAG_LAYOUT_2        = (1u << 24), /**< Пользовательский флаг для макетов*/
#if LV_USE_FLEX
    LV_OBJ_FLAG_FLEX_IN_NEW_TRACK = LV_OBJ_FLAG_LAYOUT_1,     /**< Начать новую flex-строку с этого элемента*/
#endif

    LV_OBJ_FLAG_WIDGET_1        = (1u << 25), /**< Пользовательский флаг для виджета*/
    LV_OBJ_FLAG_WIDGET_2        = (1u << 26), /**< Пользовательский флаг для виджета*/
    LV_OBJ_FLAG_USER_1          = (1u << 27), /**< Пользовательский флаг для прикладного кода*/
    LV_OBJ_FLAG_USER_2          = (1u << 28), /**< Пользовательский флаг для прикладного кода*/
    LV_OBJ_FLAG_USER_3          = (1u << 29), /**< Пользовательский флаг для прикладного кода*/
    LV_OBJ_FLAG_USER_4          = (1u << 30), /**< Пользовательский флаг для прикладного кода*/
} lv_obj_flag_t;

#if LV_USE_OBJ_PROPERTY
enum _lv_signed_prop_id_t {
    /*Свойства флага OBJ */
    LV_PROPERTY_ID(OBJ, FLAG_START,                 LV_PROPERTY_TYPE_INT,       0),
    LV_PROPERTY_ID(OBJ, FLAG_HIDDEN,                LV_PROPERTY_TYPE_INT,       0),
    LV_PROPERTY_ID(OBJ, FLAG_CLICKABLE,             LV_PROPERTY_TYPE_INT,       1),
    LV_PROPERTY_ID(OBJ, FLAG_CLICK_FOCUSABLE,       LV_PROPERTY_TYPE_INT,       2),
    LV_PROPERTY_ID(OBJ, FLAG_CHECKABLE,             LV_PROPERTY_TYPE_INT,       3),
    LV_PROPERTY_ID(OBJ, FLAG_SCROLLABLE,            LV_PROPERTY_TYPE_INT,       4),
    LV_PROPERTY_ID(OBJ, FLAG_SCROLL_ELASTIC,        LV_PROPERTY_TYPE_INT,       5),
    LV_PROPERTY_ID(OBJ, FLAG_SCROLL_MOMENTUM,       LV_PROPERTY_TYPE_INT,       6),
    LV_PROPERTY_ID(OBJ, FLAG_SCROLL_ONE,            LV_PROPERTY_TYPE_INT,       7),
    LV_PROPERTY_ID(OBJ, FLAG_SCROLL_CHAIN_HOR,      LV_PROPERTY_TYPE_INT,       8),
    LV_PROPERTY_ID(OBJ, FLAG_SCROLL_CHAIN_VER,      LV_PROPERTY_TYPE_INT,       9),
    LV_PROPERTY_ID(OBJ, FLAG_SCROLL_ON_FOCUS,       LV_PROPERTY_TYPE_INT,       10),
    LV_PROPERTY_ID(OBJ, FLAG_SCROLL_WITH_ARROW,     LV_PROPERTY_TYPE_INT,       11),
    LV_PROPERTY_ID(OBJ, FLAG_SNAPPABLE,             LV_PROPERTY_TYPE_INT,       12),
    LV_PROPERTY_ID(OBJ, FLAG_PRESS_LOCK,            LV_PROPERTY_TYPE_INT,       13),
    LV_PROPERTY_ID(OBJ, FLAG_EVENT_BUBBLE,          LV_PROPERTY_TYPE_INT,       14),
    LV_PROPERTY_ID(OBJ, FLAG_GESTURE_BUBBLE,        LV_PROPERTY_TYPE_INT,       15),
    LV_PROPERTY_ID(OBJ, FLAG_ADV_HITTEST,           LV_PROPERTY_TYPE_INT,       16),
    LV_PROPERTY_ID(OBJ, FLAG_IGNORE_LAYOUT,         LV_PROPERTY_TYPE_INT,       17),
    LV_PROPERTY_ID(OBJ, FLAG_FLOATING,              LV_PROPERTY_TYPE_INT,       18),
    LV_PROPERTY_ID(OBJ, FLAG_SEND_DRAW_TASK_EVENTS, LV_PROPERTY_TYPE_INT,       19),
    LV_PROPERTY_ID(OBJ, FLAG_OVERFLOW_VISIBLE,      LV_PROPERTY_TYPE_INT,       20),
    LV_PROPERTY_ID(OBJ, FLAG_EVENT_TRICKLE,         LV_PROPERTY_TYPE_INT,       21),
    LV_PROPERTY_ID(OBJ, FLAG_STATE_TRICKLE,         LV_PROPERTY_TYPE_INT,       22),
    LV_PROPERTY_ID(OBJ, FLAG_LAYOUT_1,              LV_PROPERTY_TYPE_INT,       23),
    LV_PROPERTY_ID(OBJ, FLAG_LAYOUT_2,              LV_PROPERTY_TYPE_INT,       24),
    LV_PROPERTY_ID(OBJ, FLAG_FLEX_IN_NEW_TRACK,     LV_PROPERTY_TYPE_INT,       23), /*Сопоставлено с FLAG_LAYOUT_1*/
    LV_PROPERTY_ID(OBJ, FLAG_WIDGET_1,              LV_PROPERTY_TYPE_INT,       25),
    LV_PROPERTY_ID(OBJ, FLAG_WIDGET_2,              LV_PROPERTY_TYPE_INT,       26),
    LV_PROPERTY_ID(OBJ, FLAG_USER_1,                LV_PROPERTY_TYPE_INT,       27),
    LV_PROPERTY_ID(OBJ, FLAG_USER_2,                LV_PROPERTY_TYPE_INT,       28),
    LV_PROPERTY_ID(OBJ, FLAG_USER_3,                LV_PROPERTY_TYPE_INT,       29),
    LV_PROPERTY_ID(OBJ, FLAG_USER_4,                LV_PROPERTY_TYPE_INT,       30),
    LV_PROPERTY_ID(OBJ, FLAG_END,                   LV_PROPERTY_TYPE_INT,       30),

    LV_PROPERTY_ID(OBJ, STATE_START,                LV_PROPERTY_TYPE_INT,       31),
    LV_PROPERTY_ID(OBJ, STATE_ALT,                  LV_PROPERTY_TYPE_INT,       31),
    /*1 зарезервировано*/
    LV_PROPERTY_ID(OBJ, STATE_CHECKED,              LV_PROPERTY_TYPE_INT,       33),
    LV_PROPERTY_ID(OBJ, STATE_FOCUSED,              LV_PROPERTY_TYPE_INT,       34),
    LV_PROPERTY_ID(OBJ, STATE_FOCUS_KEY,            LV_PROPERTY_TYPE_INT,       35),
    LV_PROPERTY_ID(OBJ, STATE_EDITED,               LV_PROPERTY_TYPE_INT,       36),
    LV_PROPERTY_ID(OBJ, STATE_HOVERED,              LV_PROPERTY_TYPE_INT,       37),
    LV_PROPERTY_ID(OBJ, STATE_PRESSED,              LV_PROPERTY_TYPE_INT,       38),
    LV_PROPERTY_ID(OBJ, STATE_SCROLLED,             LV_PROPERTY_TYPE_INT,       39),
    LV_PROPERTY_ID(OBJ, STATE_DISABLED,             LV_PROPERTY_TYPE_INT,       40),
    /*2 зарезервировано*/
    LV_PROPERTY_ID(OBJ, STATE_USER_1,               LV_PROPERTY_TYPE_INT,       43),
    LV_PROPERTY_ID(OBJ, STATE_USER_2,               LV_PROPERTY_TYPE_INT,       44),
    LV_PROPERTY_ID(OBJ, STATE_USER_3,               LV_PROPERTY_TYPE_INT,       45),
    LV_PROPERTY_ID(OBJ, STATE_USER_4,               LV_PROPERTY_TYPE_INT,       46),
    LV_PROPERTY_ID(OBJ, STATE_ANY,                  LV_PROPERTY_TYPE_INT,       47),
    LV_PROPERTY_ID(OBJ, STATE_END,                  LV_PROPERTY_TYPE_INT,       47),

    /*OBJ нормальные свойства*/
    LV_PROPERTY_ID(OBJ, PARENT,                     LV_PROPERTY_TYPE_OBJ,       48),
    LV_PROPERTY_ID(OBJ, X,                          LV_PROPERTY_TYPE_INT,       49),
    LV_PROPERTY_ID(OBJ, Y,                          LV_PROPERTY_TYPE_INT,       50),
    LV_PROPERTY_ID(OBJ, W,                          LV_PROPERTY_TYPE_INT,       51),
    LV_PROPERTY_ID(OBJ, H,                          LV_PROPERTY_TYPE_INT,       52),
    LV_PROPERTY_ID(OBJ, CONTENT_WIDTH,              LV_PROPERTY_TYPE_INT,       53),
    LV_PROPERTY_ID(OBJ, CONTENT_HEIGHT,             LV_PROPERTY_TYPE_INT,       54),
    LV_PROPERTY_ID(OBJ, LAYOUT,                     LV_PROPERTY_TYPE_INT,       55),
    LV_PROPERTY_ID(OBJ, ALIGN,                      LV_PROPERTY_TYPE_INT,       56),
    LV_PROPERTY_ID(OBJ, SCROLLBAR_MODE,             LV_PROPERTY_TYPE_INT,       57),
    LV_PROPERTY_ID(OBJ, SCROLL_DIR,                 LV_PROPERTY_TYPE_INT,       58),
    LV_PROPERTY_ID(OBJ, SCROLL_SNAP_X,              LV_PROPERTY_TYPE_INT,       59),
    LV_PROPERTY_ID(OBJ, SCROLL_SNAP_Y,              LV_PROPERTY_TYPE_INT,       60),
    LV_PROPERTY_ID(OBJ, SCROLL_X,                   LV_PROPERTY_TYPE_INT,       61),
    LV_PROPERTY_ID(OBJ, SCROLL_Y,                   LV_PROPERTY_TYPE_INT,       62),
    LV_PROPERTY_ID(OBJ, SCROLL_TOP,                 LV_PROPERTY_TYPE_INT,       63),
    LV_PROPERTY_ID(OBJ, SCROLL_BOTTOM,              LV_PROPERTY_TYPE_INT,       64),
    LV_PROPERTY_ID(OBJ, SCROLL_LEFT,                LV_PROPERTY_TYPE_INT,       65),
    LV_PROPERTY_ID(OBJ, SCROLL_RIGHT,               LV_PROPERTY_TYPE_INT,       66),
    LV_PROPERTY_ID(OBJ, SCROLL_END,                 LV_PROPERTY_TYPE_POINT,     67),
    LV_PROPERTY_ID(OBJ, EXT_DRAW_SIZE,              LV_PROPERTY_TYPE_INT,       68),
    LV_PROPERTY_ID(OBJ, EVENT_COUNT,                LV_PROPERTY_TYPE_INT,       69),
    LV_PROPERTY_ID(OBJ, SCREEN,                     LV_PROPERTY_TYPE_OBJ,       70),
    LV_PROPERTY_ID(OBJ, DISPLAY,                    LV_PROPERTY_TYPE_POINTER,   71),
    LV_PROPERTY_ID(OBJ, CHILD_COUNT,                LV_PROPERTY_TYPE_INT,       72),
    LV_PROPERTY_ID(OBJ, INDEX,                      LV_PROPERTY_TYPE_INT,       73),

    LV_PROPERTY_OBJ_END,
};
#endif

/**
 * Класс базового объекта, доступный извне.
 */
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_obj_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создайте базовый объект (прямоугольник)
 * @param parent    указатель на родительский объект. Если NULL, то будет создан экран.
 * @return          указатель на новый объект
 */
lv_obj_t * lv_obj_create(lv_obj_t * parent);

/*=====================
 * Функции установки
 *====================*/

/**
 * Установите один или несколько флагов
 * @param obj   указатель на объект
 * @param f     объединенные побитовым ИЛИ значения из`lv_obj_flag_t`для установки.
 */
void lv_obj_add_flag(lv_obj_t * obj, lv_obj_flag_t f);

/**
 * Удалить один или несколько флагов
 * @param obj   указатель на объект
 * @param f     объединенные побитовым ИЛИ значения из`lv_obj_flag_t`для очистки.
 */
void lv_obj_remove_flag(lv_obj_t * obj, lv_obj_flag_t f);

/**
 * Добавить или удалить один или несколько флагов.
 * @param obj   указатель на объект
 * @param f     объединенные побитовым ИЛИ значения из`lv_obj_flag_t`для обновления.
 * @param v     true: добавить флаги; false: удалить флаги
 */
void lv_obj_set_flag(lv_obj_t * obj, lv_obj_flag_t f, bool v);

/**
 * Добавьте к объекту одно или несколько состояний. Остальные биты состояния останутся неизменными.
 * Если указано в стилях, будет запущена анимация перехода из предыдущего состояния в текущее.
 * @param obj       указатель на объект
 * @param state     состояния, которые нужно добавить. Например, `LV_STATE_PRESSED | LV_STATE_FOCUSED`
 */
void lv_obj_add_state(lv_obj_t * obj, lv_state_t state);

/**
 * Удалите одно или несколько состояний объекта. Остальные биты состояния останутся неизменными.
 * Если указано в стилях, будет запущена анимация перехода из предыдущего состояния в текущее.
 * @param obj       указатель на объект
 * @param state     состояния, которые нужно добавить. Например, `LV_STATE_PRESSED | LV_STATE_FOCUSED`
 */
void lv_obj_remove_state(lv_obj_t * obj, lv_state_t state);

/**
 * Добавьте или удалите одно или несколько состояний объекта. Остальные биты состояния останутся неизменными.
 * @param obj       указатель на объект
 * @param state     состояния, которые нужно добавить. Например, `LV_STATE_PRESSED | LV_STATE_FOCUSED`
 * @param v         true: добавить состояния; false: удалить состояния
 */
void lv_obj_set_state(lv_obj_t * obj, lv_state_t state, bool v);

/**
 * Установите поле объекта user_data.
 * @param obj   указатель на объект
 * @param user_data   указатель на новый user_data.
 */
void lv_obj_set_user_data(lv_obj_t * obj, void * user_data);


/** Разрешить проверку только одного родственного RADIO_BUTTON
 * @param obj     указатель на виджет
 * @param en      включить или отключить поведение переключателя
 */
void lv_obj_set_radio_button(lv_obj_t * obj, bool en);

/*=======================
 * Геттерные функции
 *======================*/

/**
 * Проверьте, установлен ли данный флаг или все данные флаги на объекте.
 * @param obj   указатель на объект
 * @param f     флаг(а) для проверки (можно передавать несколько флагов через побитовое ИЛИ)
 * @return      true: все флаги установлены; false: не все флаги установлены
 */
bool lv_obj_has_flag(const lv_obj_t * obj, lv_obj_flag_t f);

/**
 * Проверьте, установлен ли данный флаг или какой-либо из флагов на объекте.
 * @param obj   указатель на объект
 * @param f     флаг(а) для проверки (можно передавать несколько флагов через побитовое ИЛИ)
 * @return      true: установлен хотя бы один флаг; false: ни один из флагов не установлен
 */
bool lv_obj_has_flag_any(const lv_obj_t * obj, lv_obj_flag_t f);

/**
 * Получить состояние объекта
 * @param obj   указатель на объект
 * @return      состояние (значения, объединенные побитовым ИЛИ, из `lv_state_t`)
 */
lv_state_t lv_obj_get_state(const lv_obj_t * obj);

/**
 * Проверьте, находится ли объект в заданном состоянии или нет.
 * @param obj       указатель на объект
 * @param state     состояние или комбинация состояний для проверки
 * @return          true:`obj`находится в`state`; false:`obj`нет в `state`
 */
bool lv_obj_has_state(const lv_obj_t * obj, lv_state_t state);

/** Узнайте, является ли объект переключателем
 * @param obj     указатель на виджет
 * @return        true, если включено поведение переключателя
 */
bool lv_obj_is_radio_button(const lv_obj_t * obj);

/**
 * Получить группу объекта
 * @param       obj указатель на объект
 * @return      указатель на группу объекта
 */
lv_group_t * lv_obj_get_group(const lv_obj_t * obj);

/**
 * Получить поле user_data объекта.
 * @param obj   указатель на объект
 * @return      указатель наuser_dataобъекта
 */
void * lv_obj_get_user_data(lv_obj_t * obj);

/*=======================
 * Другие функции
 *======================*/

/**
 * Выделите специальные данные для объекта, если они еще не выделены.
 * @param obj   указатель на объект
 */
void lv_obj_allocate_spec_attr(lv_obj_t * obj);

/**
 * Проверьте тип объекта.
 * @param obj       указатель на объект
 * @param class_p   класс для проверки (например,`lv_slider_class`)
 * @return          true:`class_p`— это класс `obj`.
 */
bool lv_obj_check_type(const lv_obj_t * obj, const lv_obj_class_t * class_p);

/**
 * Проверьте, имеет ли какой-либо объект заданный класс (тип).
 * Он также проверяет классы-предки.
 * @param obj       указатель на объект
 * @param class_p   класс для проверки (например,`lv_slider_class`)
 * @return          true:`obj`имеет заданный класс
 */
bool lv_obj_has_class(const lv_obj_t * obj, const lv_obj_class_t * class_p);

/**
 * Получить класс (тип) объекта
 * @param obj   указатель на объект
 * @return      класс (тип) объекта
 */
const lv_obj_class_t * lv_obj_get_class(const lv_obj_t * obj);

/**
 * Проверьте, жив ли еще какой-либо объект.
 * @param obj       указатель на объект
 * @return          true: действительный
 */
bool lv_obj_is_valid(const lv_obj_t * obj);

/**
 * Утилита для установки ссылки на объект NULL при его удалении.
 * Ссылка должна находиться в месте, которое не станет недействительным.
 * во время жизни объекта, т. е. статический или выделенный.
 * @param obj_ptr   указатель на указатель на объект
 */
void lv_obj_null_on_delete(lv_obj_t ** obj_ptr);

/**
 * Добавьте в виджет обработчик событий, который будет загружать экран по триггеру.
 * @param obj           указатель на виджет, который должен загрузить экран
 * @param trigger       код события, например.  `LV_EVENT_CLICKED`
 * @param screen        экран для загрузки (должен быть действительный виджет)
 * @param anim_type     элемент`lv_screen_load_anim_t`анимация загрузки экрана
 * @param duration      продолжительность анимации в миллисекундах
 * @param delay         задержка перед загрузкой экрана в миллисекундах
 */
void lv_obj_add_screen_load_event(lv_obj_t * obj, lv_event_code_t trigger, lv_obj_t * screen,
                                  lv_screen_load_anim_t anim_type, uint32_t duration, uint32_t delay);

/**
 * Добавьте в виджет обработчик событий, который будет создавать экран по триггеру.
 * Созданный экран будет удален при выгрузке.
 * @param obj               указатель на виджет, который должен загрузить экран
 * @param trigger           код события, например.  `LV_EVENT_CLICKED`
 * @param screen_create_cb  обратный вызов для создания экрана, например.  `lv_obj_t * myscreen_create(void)`
 * @param anim_type         элемент`lv_screen_load_anim_t`анимация загрузки экрана
 * @param duration          продолжительность анимации в миллисекундах
 * @param delay             задержка перед загрузкой экрана в миллисекундах
 */
void lv_obj_add_screen_create_event(lv_obj_t * obj, lv_event_code_t trigger, lv_screen_create_cb_t screen_create_cb,
                                    lv_screen_load_anim_t anim_type, uint32_t duration, uint32_t delay);


/**
 * Воспроизведение анимации временной шкалы по триггеру
 * @param obj               указатель на виджет, который должен запускать воспроизведение анимации
 * @param trigger           код события, например.  `LV_EVENT_CLICKED`
 * @param at                указатель на временную шкалу анимации
 * @param delay             время ожидания перед запуском анимации
 * @param reverse           true: играть наоборот
 */
void lv_obj_add_play_timeline_event(lv_obj_t * obj, lv_event_code_t trigger, lv_anim_timeline_t * at, uint32_t delay,
                                    bool reverse);

#if LV_USE_OBJ_ID
/**
 * Установите идентификатор объекта.
 * @param obj   указатель на объект
 * @param id    идентификатор объекта
 */
void lv_obj_set_id(lv_obj_t * obj, void * id);

/**
 * Получить идентификатор объекта.
 * @param obj   указатель на объект
 * @return      идентификатор объекта
 */
void * lv_obj_get_id(const lv_obj_t * obj);

/**
 * Идентификаторы DEPRECATED используются только для печати деревьев виджетов.
 * Чтобы найти виджет, используйте `lv_obj_find_by_name`.
 *
 * Получите дочерний объект по его идентификатору.
 * Он будет рекурсивно проверять детей и внуков.
 * Функция`lv_obj_id_compare`используется для объединения идентификатора объекта с заданным идентификатором.
 *
 * @param obj       указатель на объект
 * @param id        идентификатор дочернего объекта
 * @return          указатель на дочерний объект или NULL, если не найден
 */
lv_obj_t * lv_obj_find_by_id(const lv_obj_t * obj, const void * id);

/**
 * Присвойте идентификатор объекту, если он не был назначен ранее.
 * Эта функция вызывается автоматически, когда LV_OBJ_ID_AUTO_ASSIGN включен.
 *
 * Установите `LV_USE_OBJ_ID_BUILTIN`, чтобы использовать встроенный метод для создания объекта ID.
 * В противном случае эти функции, включая`lv_obj_[set|assign|free|stringify]_id`и
 * `lv_obj_id_compare` должен быть реализован извне.
 *
 * @param class_p   класс, к которому принадлежит этот объект. Обратите внимание, что obj->class_p— это класс, который создается в данный момент.
 * @param obj   указатель на объект
 */
void lv_obj_assign_id(const lv_obj_class_t * class_p, lv_obj_t * obj);

/**
 * Бесплатные ресурсы, выделенные`lv_obj_assign_id`или`lv_obj_set_id`.
 * Эта функция также вызывается автоматически при удалении объекта.
 * @param obj   указатель на объект
 */
void lv_obj_free_id(lv_obj_t * obj);

/**
 * Сравните два идентификатора объекта и верните 0, если они равны.
 *
 * Установите `LV_USE_OBJ_ID_BUILTIN`, чтобы использовать встроенный метод сравнения.
 * В противном случае его необходимо реализовать извне.
 *
 * @param id1 первый идентификатор
 * @param id2 идентификатор второго
 * @return     0, если они равны, и ненулевое в противном случае.
 */
int lv_obj_id_compare(const void * id1, const void * id2);

/**
 * Форматирует идентификатор объекта в строку.
 * @param obj   указатель на объект
 * @param buf   буфер для записи строки в
 * @param len   длина буфера
 */
const char * lv_obj_stringify_id(lv_obj_t * obj, char * buf, uint32_t len);

#if LV_USE_OBJ_ID_BUILTIN
/**
 * Бесплатные ресурсы, используемые встроенным генератором ID.
 */
void lv_objid_builtin_destroy(void);
#endif

#endif /*LV_USE_OBJ_ID*/

/**********************
 *      MACROS
 **********************/

#if LV_USE_ASSERT_OBJ
#  define LV_ASSERT_OBJ(obj_p, obj_class)                                                               \
    do {                                                                                                \
        LV_ASSERT_MSG(obj_p != NULL, "The object is NULL");                                             \
        LV_ASSERT_MSG(lv_obj_has_class(obj_p, obj_class) == true, "Incompatible object type.");         \
        LV_ASSERT_MSG(lv_obj_is_valid(obj_p)  == true, "The object is invalid, deleted or corrupted?"); \
    } while(0)
# else
#  define LV_ASSERT_OBJ(obj_p, obj_class) LV_ASSERT_NULL(obj_p)
#endif

#if LV_USE_LOG && LV_LOG_TRACE_OBJ_CREATE
#  define LV_TRACE_OBJ_CREATE(...) LV_LOG_TRACE(__VA_ARGS__)
#else
#  define LV_TRACE_OBJ_CREATE(...)
#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OBJ_H*/
