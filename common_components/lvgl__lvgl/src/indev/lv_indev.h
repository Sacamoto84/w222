/**
 * @file lv_indev.h
 *
 */

#ifndef LV_INDEV_H
#define LV_INDEV_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../core/lv_group.h"
#include "../misc/lv_area.h"
#include "../misc/lv_timer.h"
#include "../misc/lv_event.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** Возможные типы устройств ввода*/
typedef enum {
    LV_INDEV_TYPE_NONE,    /**< Uninitialized state*/
    LV_INDEV_TYPE_POINTER, /**< Touch pad, mouse, external button*/
    LV_INDEV_TYPE_KEYPAD,  /**< Keypad or keyboard*/
    LV_INDEV_TYPE_BUTTON,  /**< External (hardware button) which is assigned to a specific point of the screen*/
    LV_INDEV_TYPE_ENCODER, /**< Encoder with only Left, Right turn and a Button*/
} lv_indev_type_t;

/** Состояния для устройств ввода*/
typedef enum {
    LV_INDEV_STATE_RELEASED = 0,
    LV_INDEV_STATE_PRESSED
} lv_indev_state_t;

typedef enum {
    LV_INDEV_MODE_NONE = 0,
    LV_INDEV_MODE_TIMER,
    LV_INDEV_MODE_EVENT,
} lv_indev_mode_t;


/* Поддерживаемые типы жестов */
typedef enum {
    LV_INDEV_GESTURE_NONE = 0,
    LV_INDEV_GESTURE_PINCH,
    LV_INDEV_GESTURE_SWIPE,
    LV_INDEV_GESTURE_ROTATE,
    LV_INDEV_GESTURE_TWO_FINGERS_SWIPE,
    LV_INDEV_GESTURE_SCROLL,            /* Используется с колесами прокрутки */
    LV_INDEV_GESTURE_CNT,               /* Общее количество типов жестов */
} lv_indev_gesture_type_t;

/** Структура данных передается драйверу ввода для заполнения*/
typedef struct {
    lv_indev_gesture_type_t gesture_type[LV_INDEV_GESTURE_CNT]; /* Текущие типы жестов, по жесту */
    void * gesture_data[LV_INDEV_GESTURE_CNT]; /* Используется для хранения данных для каждого жеста */

    lv_indev_state_t state; /**< LV_INDEV_STATE_RELEASED or LV_INDEV_STATE_PRESSED*/

    lv_point_t point; /**< For LV_INDEV_TYPE_POINTER the currently pressed point*/
    uint32_t key;     /**< For LV_INDEV_TYPE_KEYPAD the currently pressed key*/
    uint32_t btn_id;  /**< For LV_INDEV_TYPE_BUTTON the currently pressed button*/
    int16_t enc_diff; /**< For LV_INDEV_TYPE_ENCODER number of steps since the previous read*/

    uint32_t timestamp; /**< Initialized to lv_tick_get(). Driver may provide more accurate timestamp for buffered events*/
    bool continue_reading;  /**< If set to true, the read callback is invoked again, unless the device is in event-driven mode*/
} lv_indev_data_t;

typedef void (*lv_indev_read_cb_t)(lv_indev_t * indev, lv_indev_data_t * data);

/** Обратный вызов переназначения ключей Indev */
typedef lv_key_t (*lv_indev_key_remap_cb_t)(lv_indev_t * indev, lv_key_t key);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Создать индев.
 * @return Указатель на созданный indev или NULL, если выделение не удалось.
 */
lv_indev_t * lv_indev_create(void);

/**
 * Удалите предоставленное устройство ввода. Обязательно больше не используйте предоставленное устройство ввода.
 * @param indev указатель на удаление
 */
void lv_indev_delete(lv_indev_t * indev);

/**
 * Получите следующее устройство ввода.
 * @param indev указатель на текущее устройство ввода. NULLдля инициализации.
 * @return следующее устройство ввода или NULL, если их больше нет. Предоставьте первое устройство ввода, когда
 * параметр NULL
 */
lv_indev_t * lv_indev_get_next(lv_indev_t * indev);

/**
 * Чтение данных с устройства ввода.
 * @param indev указатель на устройство ввода
 */
void lv_indev_read(lv_indev_t * indev);

/**
 * Вызывается периодически для чтения устройств ввода
 * @param timer указатель на таймер для чтения
 */
void lv_indev_read_timer_cb(lv_timer_t * timer);

/**
 * Включить или отключить одно или все устройства ввода (по умолчанию включено)
 * @param indev указатель на устройство ввода или NULL, чтобы включить/отключить их все
 * @param enable true для включения, false для отключения
 */
void lv_indev_enable(lv_indev_t * indev, bool enable);

/**
 * Получите текущее обрабатываемое устройство ввода. Также может использоваться в функциях действий.
 * @return указатель на обрабатываемое в данный момент устройство ввода или NULL, если обработка устройства ввода отсутствует
 * прямо сейчас
 */
lv_indev_t * lv_indev_active(void);

/**
 * Установите тип устройства ввода
 * @param indev указатель на устройство ввода
 * @param indev_type тип устройства ввода из`lv_indev_type_t`(`LV_INDEV_TYPE_...`)
 */
void lv_indev_set_type(lv_indev_t * indev, lv_indev_type_t indev_type);

/**
 * Установите функцию обратного вызова для чтения данных входного устройства в indev.
 * @param indev указатель на устройство ввода
 * @param read_cb указатель на функцию обратного вызова для чтения данных устройства ввода
 */
void lv_indev_set_read_cb(lv_indev_t * indev,  lv_indev_read_cb_t read_cb);

/**
 * Установите пользовательские данные в indev
 * @param indev указатель на устройство ввода
 * @param user_data указатель на данные пользователя
 */
void lv_indev_set_user_data(lv_indev_t * indev, void * user_data);

/**
 * Установите данные драйвера в indev
 * @param indev указатель на устройство ввода
 * @param driver_data указатель на данные водителя
 */
void lv_indev_set_driver_data(lv_indev_t * indev, void * driver_data);

/**
 * Назначить значение indev
 * @param indev указатель на устройство ввода
 * @param disp указатель на дисплей
 */
void lv_indev_set_display(lv_indev_t * indev, struct _lv_display_t * disp);

/**
 * Установите время длительного нажатия на индев.
 * @param  indev            указатель на устройство ввода
 * @param  long_press_time  время длительного нажатия, мс
 */
void lv_indev_set_long_press_time(lv_indev_t * indev, uint16_t long_press_time);

/**
 * Установите время повторения длительного воздействия на индев.
 * @param  indev            указатель на устройство ввода
 * @param  long_press_repeat_time  время повтора длительного нажатия в мс
 */
void lv_indev_set_long_press_repeat_time(lv_indev_t * indev, uint16_t long_press_repeat_time);

/**
 * Установить ограничение прокрутки для устройства ввода
 * @param indev указатель на устройство ввода
 * @param scroll_limit количество пикселей, на которое нужно сдвинуть объект перед фактическим перетаскиванием
 */
void lv_indev_set_scroll_limit(lv_indev_t * indev, uint8_t scroll_limit);

/**
 * Установите замедление прокрутки на индев. Большее значение означает более быстрое замедление.
 * @param indev указатель на устройство ввода
 * @param scroll_throw замедление в [%]
 */
void lv_indev_set_scroll_throw(lv_indev_t * indev, uint8_t scroll_throw);

/**
 * Установите минимальный порог скорости для обнаружения жестов.
 * Разница между последовательными точками должна превышать это значение (в пикселях).
 * чтобы движение считалось достаточно быстрым, чтобы вызвать жест.
 *
 * @param indev указатель на устройство ввода
 * @param min_velocity минимальный порог скорости в пикселях (по умолчанию: 3)
 */
void lv_indev_set_gesture_min_velocity(lv_indev_t * indev, uint8_t min_velocity);

/**
 * Установите порог минимального расстояния для обнаружения жестов.
 * Общее расстояние от первой точки до текущей точки должно превышать
 * это значение (в пикселях), чтобы движение считалось достаточно большим
 * чтобы вызвать жест.
 *
 * @param indev указатель на устройство ввода
 * @param min_distance порог минимального расстояния в пикселях (по умолчанию: 50)
 */
void lv_indev_set_gesture_min_distance(lv_indev_t * indev, uint8_t min_distance);

/**
 * Получить тип устройства ввода
 * @param indev указатель на устройство ввода
 * @return тип устройства ввода из`lv_hal_indev_type_t`(`LV_INDEV_TYPE_...`)
 */
lv_indev_type_t lv_indev_get_type(const lv_indev_t * indev);

/**
 * Получите функцию обратного вызова для чтения данных входного устройства в индев.
 * @param indev указатель на устройство ввода
 * @return Указатель на функцию обратного вызова для чтения данных устройства ввода или NULL, если indev равен NULL
 */
lv_indev_read_cb_t lv_indev_get_read_cb(lv_indev_t * indev);

/**
 * Получить состояние разработки
 * @param indev указатель на устройство ввода
 * @return Состояние Indev или LV_INDEV_STATE_RELEASED, если indev равно NULL
 */
lv_indev_state_t lv_indev_get_state(const lv_indev_t * indev);

/**
 * Получить группу, назначенную разработчиком
 * @param indev указатель на устройство ввода
 * @return Указатель на группу, назначенную indev, или NULL, если indev равен NULL.
 */
lv_group_t * lv_indev_get_group(const lv_indev_t * indev);

/**
 * Получить указатель на назначенное оборудование Indev
 * @param indev указатель на устройство ввода
 * @return указатель на назначенный дисплей или NULL, если indev равен NULL
 */
lv_display_t * lv_indev_get_display(const lv_indev_t * indev);

/**
 * Получить указатель на пользовательские данные indev
 * @param indev указатель на устройство ввода
 * @return указатель на пользовательские данные или NULL, если indev равен NULL
 */
void * lv_indev_get_user_data(const lv_indev_t * indev);

/**
 * Получить указатель данных драйвера indev
 * @param indev указатель на устройство ввода
 * @return указатель на данные драйвера или NULL, если indev равен NULL
 */
void * lv_indev_get_driver_data(const lv_indev_t * indev);

/**
 * Решение, перемещается ли инициатива при выступлении
 * @param indev указатель на устройство ввода
 * @return true: indev перемещается при нажатии; false: indev не перемещается при нажатии
 */
bool lv_indev_get_press_moved(const lv_indev_t * indev);

/**
 * Сброс одного или всех устройств ввода
 * @param indev указатель на устройство ввода для сброса илиNULLдля сброса всех из них
 * @param obj указатель на объект, который запускает сброс.
 */
void lv_indev_reset(lv_indev_t * indev, lv_obj_t * obj);

/**
 * События, связанные с касанием и клавишами, сначала отправляются на устройство ввода, а затем в виджет.
 * Если эта функция появится в событии indev, событие не будет отправлено в виджет.
 * @param indev указатель на устройство ввода
 */
void lv_indev_stop_processing(lv_indev_t * indev);

/**
 * Сброс состояния длительного нажатия устройства ввода
 * @param indev указатель на устройство ввода
 */
void lv_indev_reset_long_press(lv_indev_t * indev);

/**
 * Установите курсор для устройства ввода указателя (для LV_INPUT_TYPE_POINTER и LV_INPUT_TYPE_BUTTON)
 * @param indev указатель на устройство ввода
 * @param cur_obj указатель на объект, который будет использоваться в качестве курсора
 */
void lv_indev_set_cursor(lv_indev_t * indev, lv_obj_t * cur_obj);

/**
 * Установите группу назначения для устройства ввода с клавиатуры (для LV_INDEV_TYPE_KEYPAD)
 * @param indev указатель на устройство ввода
 * @param group указатель на группу
 */
void lv_indev_set_group(lv_indev_t * indev, lv_group_t * group);

/**
 * Установите массив точек для LV_INDEV_TYPE_BUTTON.
 * Эти точки будут назначены кнопкам для нажатия определенной точки на экране.
 * @param indev указатель на устройство ввода
 * @param points массив точек
 */
void lv_indev_set_button_points(lv_indev_t * indev, const lv_point_t points[]);

/**
 * Получить последнюю точку устройства ввода (для LV_INDEV_TYPE_POINTER и LV_INDEV_TYPE_BUTTON)
 * @param indev указатель на устройство ввода
 * @param point указатель на точку для сохранения результата
 */
void lv_indev_get_point(const lv_indev_t * indev, lv_point_t * point);

/**
* Получить текущий жест напрямую
* @param indev указатель на устройство ввода
* @return текущий жест прямой
*/
lv_dir_t lv_indev_get_gesture_dir(const lv_indev_t * indev);

/**
 * Получить последнюю нажатую клавишу устройства ввода (для LV_INDEV_TYPE_KEYPAD )
 * @param indev указатель на устройство ввода
 * @return последняя нажатая клавиша (0 в случае ошибки)
 */
uint32_t lv_indev_get_key(const lv_indev_t * indev);


/**
 * Получите счетчик последовательных кликов на небольшом расстоянии и во времени.
 * Счетчик обновляется до запуска LV_EVENT_SHORT_CLICKED.
 * @param indev указатель на устройство ввода
 * @return счетчик коротких кликов
 */
uint8_t lv_indev_get_short_click_streak(const lv_indev_t * indev);

/**
 * Проверьте текущее направление прокрутки устройства ввода (для LV_INDEV_TYPE_POINTER и
 * LV_INDEV_TYPE_BUTTON)
 * @param indev указатель на устройство ввода
 * @return LV_DIR_NONE : прокрутки больше нет
 *         LV_DIR_HOR/VER
 */
lv_dir_t lv_indev_get_scroll_dir(const lv_indev_t * indev);

/**
 * Получить текущий прокручиваемый объект (для LV_INDEV_TYPE_POINTER и
 * LV_INDEV_TYPE_BUTTON)
 * @param indev указатель на устройство ввода
 * @return указатель на прокручиваемый в данный момент объект или NULL, если прокрутка по этому индеву отсутствует
 */
lv_obj_t * lv_indev_get_scroll_obj(const lv_indev_t * indev);

/**
 * Получить вектор движения устройства ввода (для LV_INDEV_TYPE_POINTER и
 * LV_INDEV_TYPE_BUTTON)
 * @param indev указатель на устройство ввода
 * @param point указатель на точку для хранения типов.pointer.vector
 */
void lv_indev_get_vect(const lv_indev_t * indev, lv_point_t * point);

/**
 * Получить объект курсора устройства ввода (только для LV_INDEV_TYPE_POINTER)
 * @param indev указатель на устройство ввода
 * @return указатель на объект курсора
 */
lv_obj_t * lv_indev_get_cursor(lv_indev_t * indev);

/**
 * Ничего не делать до следующего выпуска
 * @param indev указатель на устройство ввода
 */
void lv_indev_wait_release(lv_indev_t * indev);

/**
 * Получает указатель на текущий активный объект в обрабатываемом в данный момент устройстве ввода.
 * @return указатель на текущий активный объект или NULL, если активного объекта нет
 */
lv_obj_t * lv_indev_get_active_obj(void);

/**
 * Получите указатель на таймер чтения indev для
 * измените его параметры с помощью функции `lv_timer_...`.
 * @param indev указатель на устройство ввода
 * @return указатель на таймер обновления чтения indev. (NULLпри ошибке)
 */
lv_timer_t * lv_indev_get_read_timer(lv_indev_t * indev);

/**
* Установите модель событий устройства ввода: управляемый событиями режим или режим таймера.
* @param indev указатель на устройство ввода
* @param mode режим устройства ввода
*/
void lv_indev_set_mode(lv_indev_t * indev, lv_indev_mode_t mode);

/**
 * Получите рабочий режим устройства ввода.
 * @param indev указатель на устройство ввода
 * @return режим работы для указанного устройства ввода.
 */
lv_indev_mode_t lv_indev_get_mode(lv_indev_t * indev);

/**
 * Поиск по точке самого верхнего кликабельного объекта
 * @param obj указатель на начальный объект, обычно экран
 * @param point указатель на точку для поиска самого верхнего дочернего элемента
 * @return указатель на найденный объект или NULL, если подходящего объекта не нашлось
 */
lv_obj_t * lv_indev_search_obj(lv_obj_t * obj, lv_point_t * point);

/**
 * добавить обработчик событий в indev
 * @param indev          указатель на indev
 * @param event_cb      обратный вызов события
 * @param filter        код события для реакции или `LV_EVENT_ALL`
 * @param user_data     опционально user_data
 */
void lv_indev_add_event_cb(lv_indev_t * indev, lv_event_cb_t event_cb, lv_event_code_t filter, void * user_data);

/**
 * Получить количество событий, прикрепленных к устройствам
 * @param indev          указатель на indev
 * @return              количество событий
 */
uint32_t lv_indev_get_event_count(lv_indev_t * indev);

/**
 * Получить дескриптор события для события
 * @param indev          указатель на indev
 * @param index         индекс события
 * @return              дескриптор события
 */
lv_event_dsc_t * lv_indev_get_event_dsc(lv_indev_t * indev, uint32_t index);

/**
 * Удалить событие
 * @param indev         указатель на indev
 * @param index         индекс события, которое нужно удалить
 * @return              true: и событие было удалено; false: ни одно событие не было удалено
 */
bool lv_indev_remove_event(lv_indev_t * indev, uint32_t index);

/**
 * Удалитеevent_cbс помощью user_data.
 * @param indev         указатель на indev
 * @param event_cb      event_cb события, которое нужно удалить
 * @param user_data user_data
 * @return              количество событий удалено
 */
uint32_t lv_indev_remove_event_cb_with_user_data(lv_indev_t * indev, lv_event_cb_t event_cb, void * user_data);

/**
 * Отправить событие разработчику
 * @param indev         указатель на indev
 * @param code          код события.  LV_EVENT_...
 * @param param         необязательный параметр
 * @return              LV_RESULT_OK: indev не был удален в этом событии.
 */
lv_result_t lv_indev_send_event(lv_indev_t * indev, lv_event_code_t code, void * param);

/**
 * Установить обратный вызов переназначения клавиш ( LV_INDEV_TYPE_KEYPAD )
 * @param indev         указатель на indev
 * @param remap_cb      обратный вызов функции переназначения. Используйте NULL, чтобы отключить обратный вызов.
 */
void lv_indev_set_key_remap_cb(lv_indev_t * indev, lv_indev_key_remap_cb_t remap_cb);

#if LV_USE_EXT_DATA
/**
 * @brief Прикрепляет внешние пользовательские данные и обратный вызов деструктора к indev.
 *
 * Связывает пользовательские данные с индевом LVGL и определяет функцию деструктора.
 * который будет автоматически включаться при удалении indev для логической очистки
 * связанные ресурсы.
 *
 * @param indev      Указатель на indev
 * @param data       Определяемый пользователем указатель данных для связи с indev
 * @param free_cb    Функция обратного вызова для очисткиext_dataпри удалении indev.
 *                   Получаетext_dataв качестве параметра.  NULLозначает, что очистка не требуется.
 */
void lv_indev_set_external_data(lv_indev_t * indev, void * data, void (* free_cb)(void * data));
#endif

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_INDEV_H*/
