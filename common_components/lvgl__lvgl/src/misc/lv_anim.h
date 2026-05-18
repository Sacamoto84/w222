/**
 * @file lv_anim.h
 *
 */

#ifndef LV_ANIM_H
#define LV_ANIM_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"
#include "lv_ext_data.h"
#include "lv_types.h"
#include "lv_math.h"
#include "lv_timer.h"
#include "lv_ll.h"

/*********************
 *      DEFINES
 *********************/

#define LV_ANIM_REPEAT_INFINITE      0xFFFFFFFF
#define LV_ANIM_PLAYTIME_INFINITE    0xFFFFFFFF
#define LV_ANIM_PAUSE_FOREVER        0xFFFFFFFF

/*
 * Макросы, используемые для установки параметра анимации кубической кривой Безье.
 * Параметры берутся из https://easings.net/
 *
 * Использование:
 *
 * lv_anim_t а;
 * lv_anim_init (&а);
 * ...
 * lv_anim_set_path_cb (&a,lv_anim_path_custom_bezier3);
 * LV_ANIM_SET_EASE_IN_SINE (&а); //Устанавливаем параметр кубического анимации Без значения easyInSine
 * ...
 * lv_anim_start (&а);
 */

#define _PARA(a, x1, y1, x2, y2) ((a)->parameter.bezier3 =                                  \
(lv_anim_bezier3_para_t) {                      \
    LV_BEZIER_VAL_FLOAT(x1), LV_BEZIER_VAL_FLOAT(y1),   \
    LV_BEZIER_VAL_FLOAT(x2), LV_BEZIER_VAL_FLOAT(y2) }  \
                                 )

#define LV_ANIM_SET_EASE_IN_SINE(a) _PARA(a, 0.12, 0, 0.39, 0)
#define LV_ANIM_SET_EASE_OUT_SINE(a) _PARA(a, 0.61, 1, 0.88, 1)
#define LV_ANIM_SET_EASE_IN_OUT_SINE(a) _PARA(a, 0.37, 0, 0.63, 1)
#define LV_ANIM_SET_EASE_IN_QUAD(a) _PARA(a, 0.11, 0, 0.5, 0)
#define LV_ANIM_SET_EASE_OUT_QUAD(a) _PARA(a, 0.5, 1, 0.89, 1)
#define LV_ANIM_SET_EASE_IN_OUT_QUAD(a) _PARA(a, 0.45, 0, 0.55, 1)
#define LV_ANIM_SET_EASE_IN_CUBIC(a) _PARA(a, 0.32, 0, 0.67, 0)
#define LV_ANIM_SET_EASE_OUT_CUBIC(a) _PARA(a, 0.33, 1, 0.68, 1)
#define LV_ANIM_SET_EASE_IN_OUT_CUBIC(a) _PARA(a, 0.65, 0, 0.35, 1)
#define LV_ANIM_SET_EASE_IN_QUART(a) _PARA(a, 0.5, 0, 0.75, 0)
#define LV_ANIM_SET_EASE_OUT_QUART(a) _PARA(a, 0.25, 1, 0.5, 1)
#define LV_ANIM_SET_EASE_IN_OUT_QUART(a) _PARA(a, 0.76, 0, 0.24, 1)
#define LV_ANIM_SET_EASE_IN_QUINT(a) _PARA(a, 0.64, 0, 0.78, 0)
#define LV_ANIM_SET_EASE_OUT_QUINT(a) _PARA(a, 0.22, 1, 0.36, 1)
#define LV_ANIM_SET_EASE_IN_OUT_QUINT(a) _PARA(a, 0.83, 0, 0.17, 1)
#define LV_ANIM_SET_EASE_IN_EXPO(a) _PARA(a, 0.7, 0, 0.84, 0)
#define LV_ANIM_SET_EASE_OUT_EXPO(a) _PARA(a, 0.16, 1, 0.3, 1)
#define LV_ANIM_SET_EASE_IN_OUT_EXPO(a) _PARA(a, 0.87, 0, 0.13, 1)
#define LV_ANIM_SET_EASE_IN_CIRC(a) _PARA(a, 0.55, 0, 1, 0.45)
#define LV_ANIM_SET_EASE_OUT_CIRC(a) _PARA(a, 0, 0.55, 0.45, 1)
#define LV_ANIM_SET_EASE_IN_OUT_CIRC(a) _PARA(a, 0.85, 0, 0.15, 1)
#define LV_ANIM_SET_EASE_IN_BACK(a) _PARA(a, 0.36, 0, 0.66, -0.56)
#define LV_ANIM_SET_EASE_OUT_BACK(a) _PARA(a, 0.34, 1.56, 0.64, 1)
#define LV_ANIM_SET_EASE_IN_OUT_BACK(a) _PARA(a, 0.68, -0.6, 0.32, 1.6)

LV_EXPORT_CONST_INT(LV_ANIM_REPEAT_INFINITE);
LV_EXPORT_CONST_INT(LV_ANIM_PLAYTIME_INFINITE);

/**********************
 *      TYPEDEFS
 **********************/

/** Может использоваться для указания, включена или отключена анимация в конкретном случае.*/
#define LV_ANIM_OFF false
#define LV_ANIM_ON true
typedef bool lv_anim_enable_t;

/** Получить текущее значение во время анимации*/
typedef int32_t (*lv_anim_path_cb_t)(const lv_anim_t *);

/** Общий прототип функций «аниматора».
 * Первый параметр — это переменная для анимации.
 * Второй параметр — это значение, которое нужно установить.
 * Совместимость с механизмами `lv_xxx_set_yyy(obj, value)`.
 * `x` в`_xcb_t`означает, что это не полностью универсальный прототип, потому что
 * он не получает`lv_anim_t *`в качестве первого аргумента*/
typedef void (*lv_anim_exec_xcb_t)(void *, int32_t);

/** То же самое, что `lv_anim_exec_xcb_t`, но принимает`lv_anim_t *`в качестве первого параметра.
 * Это более последовательно, но менее удобно. Может использоваться привязкой функций генератора.*/
typedef void (*lv_anim_custom_exec_cb_t)(lv_anim_t *, int32_t);

/** Обратный вызов для вызова, когда анимация будет готова*/
typedef void (*lv_anim_completed_cb_t)(lv_anim_t *);

/** Обратный вызов для вызова, когда анимация действительно начинается (учитывая`delay`)*/
typedef void (*lv_anim_start_cb_t)(lv_anim_t *);

/** Обратный вызов используется, когда значения анимации являются относительными для получения текущего значения.*/
typedef int32_t (*lv_anim_get_value_cb_t)(lv_anim_t *);

/** Обратный вызов, используемый при удалении анимации*/
typedef void (*lv_anim_deleted_cb_t)(lv_anim_t *);

/** Параметр, когда путь светитсяcustom_bezier. */
typedef struct {
    int16_t x1;
    int16_t y1;
    int16_t x2;
    int16_t y2;
} lv_anim_bezier3_para_t;

/** Описывает анимацию*/
struct _lv_anim_t {
#if LV_USE_EXT_DATA
    lv_ext_data_t ext_data;
#endif
    void * var;                               /**< Variable (Widget or other user-provided object) to animate */
    lv_anim_exec_xcb_t exec_cb;               /**< Function to execute to animate */
    lv_anim_custom_exec_cb_t custom_exec_cb;  /**< Функция, которую нужно выполнить для анимации,
                                               * та же цель, что иexec_cb, но другие параметры */
    lv_anim_start_cb_t start_cb;              /**< Call it when animation is starts (considering `delay`) */
    lv_anim_completed_cb_t completed_cb;      /**< Call it when animation is fully completed */
    lv_anim_deleted_cb_t deleted_cb;          /**< Call it when animation is deleted */
    lv_anim_get_value_cb_t get_value_cb;      /**< Get current value in relative mode */
    void * user_data;                         /**< Custom user data */
    lv_anim_path_cb_t path_cb;                /**< Provides path (curve) of animation */
    int32_t start_value;                      /**< Start value */
    int32_t current_value;                    /**< Current value */
    int32_t end_value;                        /**< End value */
    int32_t duration;                         /**< Animation duration in ms */
    int32_t act_time;                         /**< Ms elapsed since animation started. Set to negative to make delay. */
    uint32_t reverse_delay;                   /**< Wait (in ms) after forward play ends and before reverse play begins. */
    uint32_t reverse_duration;                /**< Reverse animation duration in ms */
    uint32_t repeat_delay;                    /**< Wait before repeating */
    uint32_t repeat_cnt;                      /**< Repeat count for animation */
    union _lv_anim_path_para_t {
        lv_anim_bezier3_para_t bezier3;       /**< Parameter used when path is custom_bezier */
    } parameter;

    /* Их использует система анимации — пользователь не должен устанавливать */
    uint32_t last_timer_run;
    uint32_t pause_time;                      /**<The time when the animation was paused*/
    uint32_t pause_duration;                  /**<The amount of the time the animation must stay paused for*/
    uint8_t is_paused : 1;                    /**<Indicates that the animation is paused */
    uint8_t reverse_play_in_progress : 1;     /**< Reverse play is in progress */
    uint8_t run_round : 1;                    /**< Когда не равно глобальному. anim_state. anim_run_round(который переключает каждый
                                               * таймер анимации времени выполняется), указывает на необходимость обновления этой анимации. */
    uint8_t start_cb_called : 1;              /**< Indicates that `start_cb` was already called */
    uint8_t early_apply  : 1;                 /**< 1: Apply start value immediately even is there is a `delay` */
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Инициализируйте переменную анимации.
 * E.g.:
 * lv_anim_t а;
 * lv_anim_init (&а);
 * lv_anim_set_ ...(&а);
 * lv_anim_start (&а);
 * @param a     указатель на переменную`lv_anim_t`для инициализации
 */
void lv_anim_init(lv_anim_t * a);

/**
 * Установите переменную для анимации
 * @param a     указатель на инициализированную переменную `lv_anim_t`
 * @param var   указатель на переменную для анимации
 */
void lv_anim_set_var(lv_anim_t * a, void * var);

/**
 * Установить функцию для анимации `var`
 * @param a         указатель на инициализированную переменную `lv_anim_t`
 * @param exec_cb   функция, выполняемая во время анимации
 *                  Можно использовать встроенные функции LVGL.
 *                  например lv_obj_set_x
 */
void lv_anim_set_exec_cb(lv_anim_t * a, lv_anim_exec_xcb_t exec_cb);

/**
 * Установить продолжительность анимации
 * @param a         указатель на инициализированную переменную `lv_anim_t`
 * @param duration  продолжительность анимации в миллисекундах
 */
void lv_anim_set_duration(lv_anim_t * a, uint32_t duration);

/**
 * Установите задержку перед запуском анимации
 * @param a         указатель на инициализированную переменную `lv_anim_t`
 * @param delay     задержка перед анимацией в миллисекундах
 */
void lv_anim_set_delay(lv_anim_t * a, uint32_t delay);

/**
 * Возобновляет приостановленную анимацию
 * @param a         указатель на инициализированную переменную `lv_anim_t`
 */
void lv_anim_resume(lv_anim_t * a);

/**
 * Приостанавливает анимацию
 * @param a         указатель на инициализированную переменную `lv_anim_t`
 */
void lv_anim_pause(lv_anim_t * a);

/**
 * Приостанавливает анимацию на миллисекунды мс.
 * @param a         указатель на инициализированную переменную `lv_anim_t`
 * @param ms        время паузы в миллисекундах
 */
void lv_anim_pause_for(lv_anim_t * a, uint32_t ms);

/**
 * Проверьте, приостановлена ли анимация
 * @param a         указатель на инициализированную переменную `lv_anim_t`
 * @return          true, если анимация приостановлена, иначе false
 */
bool lv_anim_is_paused(lv_anim_t * a);

/**
 * Установите начальное и конечное значения анимации
 * @param a         указатель на инициализированную переменную `lv_anim_t`
 * @param start     начальное значение
 * @param end       конечная стоимость
 */
void lv_anim_set_values(lv_anim_t * a, int32_t start, int32_t end);

/**
 * Аналогично`lv_anim_set_exec_cb`, но`lv_anim_custom_exec_cb_t`получает
 * `lv_anim_t * ` в качестве первого места вместо `void *`.
 * Эту функцию можно использовать, когда LVGL привязан к другим языкам, поскольку
 * более одновременно иметь`lv_anim_t *`в качестве первого параметра.
 * @param a         указатель на инициализированную переменную `lv_anim_t`
 * @param exec_cb   функция для выполнения.
 */
void lv_anim_set_custom_exec_cb(lv_anim_t * a, lv_anim_custom_exec_cb_t exec_cb);

/**
 * Установите путь (кривую) анимации.
 * @param a         указатель на инициализированную переменную `lv_anim_t`
 * @param path_cb функция для установки текущего значения анимации.
 */
void lv_anim_set_path_cb(lv_anim_t * a, lv_anim_path_cb_t path_cb);

/**
 * Установите функцию вызова, когда анимация действительно запускается (учитывая`delay`)
 * @param a         указатель на инициализированную переменную `lv_anim_t`
 * @param start_cb  вызов функции при запуске анимации
 */
void lv_anim_set_start_cb(lv_anim_t * a, lv_anim_start_cb_t start_cb);

/**
 * Установите функцию, которая будет использовать текущее значение переменной и создавать начальное и конечное значения.
 * относительно возвращаемого текущего значения.
 * @param a             указатель на инициализированную переменную `lv_anim_t`
 * @param get_value_cb  вызов функции при запуске анимации
 */
void lv_anim_set_get_value_cb(lv_anim_t * a, lv_anim_get_value_cb_t get_value_cb);

/**
 * Установите вызов функции после завершения анимации
 * @param a             указатель на инициализированную переменную `lv_anim_t`
 * @param completed_cb  вызов функции, когда анимация полностью завершена
 */
void lv_anim_set_completed_cb(lv_anim_t * a, lv_anim_completed_cb_t completed_cb);

/**
 * Установите вызов функции при удалении анимации.
 * @param a         указатель на инициализированную переменную `lv_anim_t`
 * @param deleted_cb  вызов функции при удалении анимации
 */
void lv_anim_set_deleted_cb(lv_anim_t * a, lv_anim_deleted_cb_t deleted_cb);

/**
 * Создайте анимацию, которая будет воспроизводиться, когда будет готово направление вперед.
 * @param a         указатель на инициализированную переменную `lv_anim_t`
 * @param duration  продолжительность воспроизведения анимации в миллисекундах. 0: отключить воспроизведение
 */
void lv_anim_set_reverse_duration(lv_anim_t * a, uint32_t duration);

/**
 * Устаревший`lv_anim_set_reverse_time`API скоро будет удален, вместо него викорируйте`lv_anim_set_reverse_duration`.
 */
void lv_anim_set_reverse_time(lv_anim_t * a, uint32_t duration);

/**
 * Создайте анимацию, которая будет воспроизводиться, когда будет готово направление вперед.
 * @param a         указатель на инициализированную переменную `lv_anim_t`
 * @param delay     задержка в миллисекундах перед началом воспроизведения анимации.
 */
void lv_anim_set_reverse_delay(lv_anim_t * a, uint32_t delay);

/**
 * Заставьте анимацию повторяться.
 * @param a         указатель на инициализированную переменную `lv_anim_t`
 * @param cnt       количество повторов или`LV_ANIM_REPEAT_INFINITE`для бесконечного повторения. 0: отключить повторение.
 */
void lv_anim_set_repeat_count(lv_anim_t * a, uint32_t cnt);

/**
 * Установите задержку перед повторением анимации.
 * @param a         указатель на инициализированную переменную `lv_anim_t`
 * @param delay     задержка в миллисекундах перед повторением анимации.
 */
void lv_anim_set_repeat_delay(lv_anim_t * a, uint32_t delay);

/**
 * Установите, должна ли анимация применяться немедленно или только по истечении задержки.
 * @param a         указатель на инициализированную переменную `lv_anim_t`
 * @param en        true: немедленно применить начальное значение в `lv_anim_start`;
 *                  false: применять начальное значение только по истечении`delay`мс и анимация действительно запускается
 */
void lv_anim_set_early_apply(lv_anim_t * a, bool en);

/**
 * Set the custom user data field of the animation.
 * @param a           указатель на инициализированную переменную `lv_anim_t`
 * @param user_data   указатель на новый user_data.
 */
void lv_anim_set_user_data(lv_anim_t * a, void * user_data);

/**
 * Установите параметр для кубического пути Безье
 * @param a         указатель на инициализированную переменную `lv_anim_t`
 * @param x1        первая контрольная точка X
 * @param y1        первая контрольная точка Y
 * @param x2        вторая контрольная точка X
 * @param y2        вторая контрольная точка Y
 */
void lv_anim_set_bezier3_param(lv_anim_t * a, int16_t x1, int16_t y1, int16_t x2, int16_t y2);

/**
 * Создать анимацию
 * @param a         инициализированная переменная anim_t. Не требуется после звонка.
 * @return          указатель на созданную анимацию (отличный от параметра `a`)
 */
lv_anim_t * lv_anim_start(const lv_anim_t * a);

/**
 * Получить задержку перед запуском анимации
 * @param a указатель на инициализированную переменную `lv_anim_t`
 * @return задержка перед анимацией в миллисекундах
 */
uint32_t lv_anim_get_delay(const lv_anim_t * a);

/**
 * Получите время, используемое для воспроизведения анимации.
 * @param a указатель на анимацию.
 * @return время воспроизведения в миллисекундах.
 */
uint32_t lv_anim_get_playtime(const lv_anim_t * a);

/**
 * Получить продолжительность анимации
 * @param a         указатель на инициализированную переменную `lv_anim_t`
 * @return продолжительность анимации в миллисекундах
 */
uint32_t lv_anim_get_time(const lv_anim_t * a);

/**
 * Get the repeat count of the animation.
 * @param a         указатель на инициализированную переменную `lv_anim_t`
 * @return количество повторов или`LV_ANIM_REPEAT_INFINITE`для бесконечного повторения. 0: повтор отключен.
 */
uint32_t lv_anim_get_repeat_count(const lv_anim_t * a);

/**
 * Получите полеuser_dataанимации.
 * @param   a указатель на инициализированную переменную `lv_anim_t`
 * @return  указатель на пользовательскийuser_dataанимации
 */
void * lv_anim_get_user_data(const lv_anim_t * a);

/**
 * Удалить анимацию(и) переменной с помощью заданной функции аниматора.
 * @param var       указатель на переменную
 * @param exec_cb   указатель функции, который анимирует 'var',
 *                  илиNULL, чтобы стоять рядом с ним и удалить все анимации 'var
 * @return          true: удалена как минимум 1 анимация, false: ни одна анимация не удалена
 */
bool lv_anim_delete(void * var, lv_anim_exec_xcb_t exec_cb);

/**
 * Удалить все анимации
 */
void lv_anim_delete_all(void);

/**
 * Получите переменную анимацию и ее `exec_cb`.
 * @param var       указатель на переменную
 * @param exec_cb   указатель на функцию, которая анимирует «var», илиNULLдля возврата первого соответствующего «var».
 * @return          указатель на анимацию.
 */
lv_anim_t * lv_anim_get(void * var, lv_anim_exec_xcb_t exec_cb);

/**
 * Получите глобальный таймер обновления анимации.
 * @return указатель на таймер обновления анимации.
 */
lv_timer_t * lv_anim_get_timer(void);

/**
 * Удалите анимацию, получив анимированную переменную из `a`.
 * Будут удалены только анимации с `exec_cb`.
 * Эта функция существует, потому что логично, что все животные. функция получает
 * `lv_anim_t` в качестве первого параметра. Это непрактично в C, но можно сделать
 * API более последователен и упрощает создание привязок.
 * @param a         указатель на анимацию.
 * @param exec_cb   указатель функции, который анимирует 'var',
 *                  илиNULL, чтобы стоять рядом с ним и удалить все анимации 'var
 * @return          true: удалена как минимум 1 анимация, false: ни одна анимация не удалена
 */
bool lv_anim_custom_delete(lv_anim_t * a, lv_anim_custom_exec_cb_t exec_cb);

/**
 * Получите переменную анимацию и ее `exec_cb`.
 * Эта функция существует, потому что логично, что все животные. функция получает
 * `lv_anim_t` в качестве первого параметра. Это непрактично в C, но можно сделать
 * API более последователен и упрощает создание привязок.
 * @param a         указатель на анимацию.
 * @param exec_cb   указатель на функцию, которая анимирует «var», илиNULLдля возврата первого соответствующего «var».
 * @return          указатель на анимацию.
 */
lv_anim_t * lv_anim_custom_get(lv_anim_t * a, lv_anim_custom_exec_cb_t exec_cb);

/**
 * Получить количество запущенных в данный момент анимаций
 * @return      количество запущенных анимаций
 */
uint16_t lv_anim_count_running(void);

/**
 * Сохраните скорость как специальное значение, которое можно использовать как время в анимации.
 * Оно будет внутренне преобразовано во время на основе начального и конечного значений.
 * Возвращаемое значение можно использовать как константу с несколькими анимациями.
 * и позвольте LVGL преобразовать скорость во время на основе фактических значений.
 * LIMITATION: максимальное время, сохраненное таким образом, может составлять 10 000 мс.
 * @param speed         скорость анимации с разрешением единицы/сек в диапазоне 0..10к
 * @return              специальное значение, которое можно использовать в качестве времени анимации
 * @note                внутренняя скорость хранится как 10 единиц/сек.
 */
uint32_t lv_anim_speed(uint32_t speed);

/**
 * Сохраните скорость как специальное значение, которое можно использовать как время в анимации.
 * Оно будет внутренне преобразовано во время на основе начального и конечного значений.
 * Возвращаемое значение можно использовать как константу с несколькими анимациями.
 * и позвольте LVGL преобразовать скорость во время на основе фактических значений.
 * @param speed         скорость анимации в единицах измерения в секунду, разрешение в диапазоне 0..10k.
 * @param min_time      минимальное время в диапазоне 0..10k
 * @param max_time      максимальное время в диапазоне 0..10k
 * @return              специальное значение, в котором хранятся все три значения и которое можно использовать в качестве времени анимации.
 * @note                внутренняя скорость хранится как 10 единиц/сек.
 * @note                внутренне мин/max_time хранятся с шагом 10 мс
 *
 */
uint32_t lv_anim_speed_clamped(uint32_t speed, uint32_t min_time, uint32_t max_time);

/**
 * Приведите скорость (созданную с помощью`lv_anim_speed`или `lv_anim_speed_clamped`) ко времени.
 * на основе начального и конечного значений.
 * @param speed     возвращаемые значения`lv_anim_speed`или `lv_anim_speed_clamped`
 * @param start     начальное значение анимации
 * @param end       конечное значение анимации
 * @return          время, необходимое для перехода от`start`к`end`с заданной настройкой `speed`
 */
uint32_t lv_anim_resolve_speed(uint32_t speed, int32_t start, int32_t end);

/**
 * Рассчитайте время анимации на основе ее скорости, начального и конечного значений.
 * Это проще, чем`lv_anim_speed`или `lv_anim_speed_clamped`, поскольку он конвертирует
 * скорость, начало и конец времени немедленно.
 * Поскольку это проще, максимальное время не ограничено.
 * @param speed         скорость анимации
 * @param start         начальное значение
 * @param end           конечная стоимость
 * @return              время анимации в миллисекундах
 */
uint32_t lv_anim_speed_to_time(uint32_t speed, int32_t start, int32_t end);


/**
 * Вручную обновите состояние анимации.
 * Полезно для запуска анимации в процессе блокировки, где
 * `lv_timer_handler` некоторое время не может работать.
 * Не следует использовать напрямую, поскольку он появляется в `lv_refr_now()`.
 */
void lv_anim_refr_now(void);

/**
 * Рассчитать текущее значение анимации, применяя линейную характеристику
 * @param a     указатель на анимацию
 * @return      текущее значение для установки
 */
int32_t lv_anim_path_linear(const lv_anim_t * a);

/**
 * Вычислить текущее значение анимации, замедляющей начальную фазу
 * @param a     указатель на анимацию
 * @return      текущее значение для установки
 */
int32_t lv_anim_path_ease_in(const lv_anim_t * a);

/**
 * Вычислить текущее значение анимации, замедляющей конечную фазу
 * @param a     указатель на анимацию
 * @return      текущее значение для установки
 */
int32_t lv_anim_path_ease_out(const lv_anim_t * a);

/**
 * Вычислить текущее значение анимации, применяя характеристику «S» (косинус).
 * @param a     указатель на анимацию
 * @return      текущее значение для установки
 */
int32_t lv_anim_path_ease_in_out(const lv_anim_t * a);

/**
 * Вычислить текущее значение анимации с выбросом в конце
 * @param a     указатель на анимацию
 * @return      текущее значение для установки
 */
int32_t lv_anim_path_overshoot(const lv_anim_t * a);

/**
 * Рассчитать текущее значение анимации с 3 отскоками
 * @param a     указатель на анимацию
 * @return      текущее значение для установки
 */
int32_t lv_anim_path_bounce(const lv_anim_t * a);

/**
 * Вычислить текущее значение анимации, применяя ступенчатую характеристику.
 * (Установите конечное значение в конце анимации)
 * @param a     указатель на анимацию
 * @return      текущее значение для установки
 */
int32_t lv_anim_path_step(const lv_anim_t * a);

/**
 * Пользовательский путь анимации кубической Безье, необходимо указать кубические параметры в a->parameter.bezier3
 * @param a     указатель на анимацию
 * @return      текущее значение для установки
 */
int32_t lv_anim_path_custom_bezier3(const lv_anim_t * a);

#if LV_USE_EXT_DATA
/**
 * @brief Связывает внешние пользовательские данные с экземпляром анимации.
 *
 * Прикрепляет произвольные пользовательские данные к объекту анимации LVGL вместе с необязательным
 * обратный вызов деструктора, который будет автоматически вызываться после завершения анимации
 * или удаляется, обеспечивая надлежащую очистку ресурса.
 *
 * @param anim       Указатель на объект анимации для настройки
 * @param data       Пользовательский указатель данных для связи
 * @param free_cb    Обратный вызов очистки, который получает ext_data, когда:
 *                   - Анимация завершается естественным образом
 *                   - Анимация удаляется преждевременно
 *                   - Новые данные заменяют текущую ассоциацию
 *                   NULL указывает, что очистка не требуется.
 */
void lv_anim_set_external_data(lv_anim_t * anim, void * data, void (* free_cb)(void * data));
#endif

/**********************
 *   GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_ANIM_H*/
