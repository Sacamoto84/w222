/**
 * @file lv_event.h
 *
 */

#ifndef LV_EVENT_H
#define LV_EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_types.h"
#include "../lv_conf_internal.h"

#include "lv_array.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef void (*lv_event_cb_t)(lv_event_t * e);

/**
 * Тип события, отправляемого в виджет
 */
typedef enum {
    LV_EVENT_ALL = 0,

    /** События устройства ввода*/
    LV_EVENT_PRESSED,             /**< Widget has been pressed */
    LV_EVENT_PRESSING,            /**< Widget is being pressed (sent continuously while pressing)*/
    LV_EVENT_PRESS_LOST,          /**< Widget is still being pressed but slid cursor/finger off Widget */
    LV_EVENT_SHORT_CLICKED,       /**< Widget was pressed for a short period of time, then released. Not sent if scrolled. */
    LV_EVENT_SINGLE_CLICKED,      /**< Sent for first short click within a small distance and short time */
    LV_EVENT_DOUBLE_CLICKED,      /**< Sent for second short click within small distance and short time */
    LV_EVENT_TRIPLE_CLICKED,      /**< Sent for third short click within small distance and short time */
    LV_EVENT_LONG_PRESSED,        /**< Object has been pressed for at least `long_press_time`.  Not sent if scrolled. */
    LV_EVENT_LONG_PRESSED_REPEAT, /**< Sent after `long_press_time` in every `long_press_repeat_time` ms.  Not sent if scrolled. */
    LV_EVENT_CLICKED,             /**< Sent on release if not scrolled (regardless to long press)*/
    LV_EVENT_RELEASED,            /**< Sent in every cases when Widget has been released */
    LV_EVENT_SCROLL_BEGIN,        /**< Scrolling begins. The event parameter is a pointer to the animation of the scroll. Can be modified */
    LV_EVENT_SCROLL_THROW_BEGIN,
    LV_EVENT_SCROLL_END,          /**< Scrolling ends */
    LV_EVENT_SCROLL,              /**< Scrolling */
    LV_EVENT_GESTURE,             /**< A gesture is detected. Get gesture with `lv_indev_get_gesture_dir(lv_indev_active());` */
    LV_EVENT_KEY,                 /**< A key is sent to Widget. Get key with `lv_indev_get_key(lv_indev_active());`*/
    LV_EVENT_ROTARY,              /**< An encoder or wheel was rotated. Get rotation count with `lv_event_get_rotary_diff(e);`*/
    LV_EVENT_FOCUSED,             /**< Widget received focus */
    LV_EVENT_DEFOCUSED,           /**< Widget's focus has been lost */
    LV_EVENT_LEAVE,               /**< Widget's focus has been lost but is still selected */
    LV_EVENT_HIT_TEST,            /**< Perform advanced hit-testing */
    LV_EVENT_INDEV_RESET,         /**< Indev has been reset */
    LV_EVENT_HOVER_OVER,          /**< Indev hover over object */
    LV_EVENT_HOVER_LEAVE,         /**< Indev hover leave object */

    /** Рисование событий */
    LV_EVENT_COVER_CHECK,         /**< Check if Widget fully covers an area. The event parameter is `lv_cover_check_info_t *`. */
    LV_EVENT_REFR_EXT_DRAW_SIZE,  /**< Get required extra draw area around Widget (e.g. for shadow). The event parameter is `int32_t *` to store the size. */
    LV_EVENT_DRAW_MAIN_BEGIN,     /**< Starting the main drawing phase */
    LV_EVENT_DRAW_MAIN,           /**< Perform the main drawing */
    LV_EVENT_DRAW_MAIN_END,       /**< Finishing the main drawing phase */
    LV_EVENT_DRAW_POST_BEGIN,     /**< Starting the post draw phase (when all children are drawn)*/
    LV_EVENT_DRAW_POST,           /**< Perform the post draw phase (when all children are drawn)*/
    LV_EVENT_DRAW_POST_END,       /**< Finishing the post draw phase (when all children are drawn)*/
    LV_EVENT_DRAW_TASK_ADDED,     /**< Adding a draw task. The `LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS` flag needs to be set */

    /** Специальные мероприятия */
    LV_EVENT_VALUE_CHANGED,       /**< Widget's value has changed (i.e. slider moved)*/
    LV_EVENT_INSERT,              /**< Text has been inserted into Widget. The event data is `char *` being inserted. */
    LV_EVENT_REFRESH,             /**< Notify Widget to refresh something on it (for user)*/
    LV_EVENT_READY,               /**< A process has finished */
    LV_EVENT_CANCEL,              /**< A process has been cancelled */
    LV_EVENT_STATE_CHANGED,       /**< The state of the widget changed*/

    /** Другие события */
    LV_EVENT_CREATE,              /**< Object is being created */
    LV_EVENT_DELETE,              /**< Object is being deleted */
    LV_EVENT_CHILD_CHANGED,       /**< Child was removed, added, or its size, position were changed */
    LV_EVENT_CHILD_CREATED,       /**< Child was created, always bubbles up to all parents */
    LV_EVENT_CHILD_DELETED,       /**< Child was deleted, always bubbles up to all parents */
    LV_EVENT_SCREEN_UNLOAD_START, /**< A screen unload started, fired immediately when scr_load is called */
    LV_EVENT_SCREEN_LOAD_START,   /**< A screen load started, fired when the screen change delay is expired */
    LV_EVENT_SCREEN_LOADED,       /**< A screen was loaded */
    LV_EVENT_SCREEN_UNLOADED,     /**< A screen was unloaded */
    LV_EVENT_SIZE_CHANGED,        /**< Object coordinates/size have changed */
    LV_EVENT_STYLE_CHANGED,       /**< Object's style has changed */
    LV_EVENT_LAYOUT_CHANGED,      /**< A child's position position has changed due to a layout recalculation */
    LV_EVENT_GET_SELF_SIZE,       /**< Get internal size of a widget */

    /** События дополнительных компонентов LVGL */
    LV_EVENT_INVALIDATE_AREA,     /**< Область признана недействительной (отмечена для перерисовки).   `lv_event_get_param(e)`
                                   * вернуть указатель на объект`lv_area_t`с координатами
                                   * область, которую необходимо признать недействительной.  При необходимости область можно свободно изменять.
                                   * адаптируйте его к особым требованиям дисплея. Обычно требуется с
                                   * монохромные дисплеи для аннулирования строк или столбцов `N x 8` за один проход. */
    LV_EVENT_RESOLUTION_CHANGED,  /**< Sent when the resolution changes due to `lv_display_set_resolution()` or `lv_display_set_rotation()`. */
    LV_EVENT_COLOR_FORMAT_CHANGED,/**< Sent as a result of any call to `lv_display_set_color_format()`. */
    LV_EVENT_REFR_REQUEST,        /**< Sent when something happened that requires redraw. */
    LV_EVENT_REFR_START,          /**< Sent before a refreshing cycle starts. Sent even if there is nothing to redraw. */
    LV_EVENT_REFR_READY,          /**< Sent when refreshing has been completed (after rendering and calling flush callback). Sent even if no redraw happened. */
    LV_EVENT_RENDER_START,        /**< Sent just before rendering begins. */
    LV_EVENT_RENDER_READY,        /**< Sent after rendering has been completed. */
    LV_EVENT_FLUSH_START,         /**< Sent before flush callback is called. */
    LV_EVENT_FLUSH_FINISH,        /**< Sent after flush callback call has returned. */
    LV_EVENT_FLUSH_WAIT_START,    /**< Sent before flush wait callback is called. */
    LV_EVENT_FLUSH_WAIT_FINISH,   /**< Sent after flush wait callback call has returned. */
    LV_EVENT_UPDATE_LAYOUT_COMPLETED,    /**< Sent after layout update completes*/

    LV_EVENT_VSYNC,
    LV_EVENT_VSYNC_REQUEST,
#if LV_USE_TRANSLATION
    LV_EVENT_TRANSLATION_LANGUAGE_CHANGED, /**< Sent when the translation language changed. */
#endif /*LV_USE_TRANSLATION*/

    LV_EVENT_LAST,                 /** Количество событий по умолчанию */

    LV_EVENT_PREPROCESS = 0x8000,   /** Это флаг, который можно установить вместе с событием, чтобы оно было обработано.
                                      перед обработкой событий класса по умолчанию */
    LV_EVENT_MARKED_DELETING = 0x10000,
} lv_event_code_t;

typedef struct {
    lv_array_t array;
    uint8_t is_traversing: 1;          /**< True: the list is being nested traversed */
    uint8_t has_marked_deleting: 1;    /**< True: в списке отмечены удаляемые объекты
                                         когда некоторые события помечены как удаляемые */
} lv_event_list_t;

/**
 * @brief Обратный вызов события.
 * События используются для уведомления пользователя о некоторых действиях, предпринимаемых с виджетом.
 * Подробнее см.::lv_event_t.
 */

lv_result_t lv_event_send(lv_event_list_t * list, lv_event_t * e, bool preprocess);

lv_event_dsc_t * lv_event_add(lv_event_list_t * list, lv_event_cb_t cb, lv_event_code_t filter, void * user_data);
bool lv_event_remove_dsc(lv_event_list_t * list, lv_event_dsc_t * dsc);

uint32_t lv_event_get_count(lv_event_list_t * list);

lv_event_dsc_t * lv_event_get_dsc(lv_event_list_t * list, uint32_t index);

lv_event_cb_t lv_event_dsc_get_cb(lv_event_dsc_t * dsc);

void * lv_event_dsc_get_user_data(lv_event_dsc_t * dsc);

bool lv_event_remove(lv_event_list_t * list, uint32_t index);

void lv_event_remove_all(lv_event_list_t * list);

/**
 * Получите виджет, изначально нацеленный на событие. То же самое, даже если событие было всплывшим.
 * @param e     указатель на дескриптор события
 * @return      цель event_code
 */
void * lv_event_get_target(lv_event_t * e);

/**
 * Получить текущую цель события. Это виджет, для которого вызывается обработчик событий.
 * Если событие не всплывает, это то же самое, что и «обычная» цель.
 * @param e     указатель на дескриптор события
 * @return      указатель на текущую цель event_code
 */
void * lv_event_get_current_target(lv_event_t * e);

/**
 * Получить код события.
 * @param e     указатель на дескриптор события
 * @return      код события. (Например, `LV_EVENT_CLICKED`,`LV_EVENT_FOCUSED`и т. д.)
 */
lv_event_code_t lv_event_get_code(lv_event_t * e);

/**
 * Получить параметр, переданный при отправке события.
 * @param e     указатель на дескриптор события
 * @return      указатель на параметр
 */
void * lv_event_get_param(lv_event_t * e);

/**
 * Получите user_data, когда событие было зарегистрировано в виджете.
 * @param e     указатель на дескриптор события
 * @return      указатель на user_data
 */
void * lv_event_get_user_data(lv_event_t * e);

/**
 * Остановите событие от всплытия.
 * Это допустимо только при вызове в середине цепочки обработки событий.
 * @param e     указатель на дескриптор события
 */
void lv_event_stop_bubbling(lv_event_t * e);

/**
 * Не позволяйте событиям дойти до детей.
 * Это допустимо только при вызове в середине цепочки обработки событий.
 * @param e     указатель на дескриптор события
 */
void lv_event_stop_trickling(lv_event_t * e);

/**
 * Прекратите обработку этого события.
 * Это допустимо только при вызове в середине цепочки обработки событий.
 * @param e     указатель на дескриптор события
 */
void lv_event_stop_processing(lv_event_t * e);

/**
 * Вспомогательная функция, обычно используемая в LV_EVENT_DELETE
 * посмотретьuser_dataсобытия
 * @param e     указатель на дескриптор события
 */
void lv_event_free_user_data_cb(lv_event_t * e);


/**
 * Зарегистрируйте новое пользовательское событие ID.
 * Его можно использовать так же, как, например.  `LV_EVENT_CLICKED` для отправки пользовательских событий
 * @return      новый идентификатор события
 *
 * Пример:
 * @code
 * uint32_t LV_EVENT_MINE = 0;
 * ...
 * e = lv_event_register_id ();
 * ...
 * lv_obj_send_event (obj,LV_EVENT_MINEиsome_data);
 * @endcode
 */
uint32_t lv_event_register_id(void);

/**
 * Получите имя кода события.
 * @param code  код события
 * @return      имя кода события в виде строки
 */
const char * lv_event_code_get_name(lv_event_code_t code);

#if LV_USE_EXT_DATA
/**
 * Установите внешние данные и их деструктор для дескриптора события.
 * Это позволяет связать пользовательские данные с обратным вызовом события, который будет автоматически очищен.
 * когда дескриптор события удаляется или уничтожается.
 * @param dsc         указатель на дескриптор события (из lv_obj_add_event_cb)
 * @param data        указатель на внешние данные, которые нужно связать с дескриптором события
 * @param free_cb     указатель функции на деструктор, который будет вызываться для очистки внешних данных.
 *                    Деструктор получит указатель данных в качестве параметра.
 */
void lv_event_desc_set_external_data(lv_event_dsc_t * dsc, void * data, void (* free_cb)(void * data));
#endif

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_EVENT_H */
