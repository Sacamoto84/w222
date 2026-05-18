/**
 * @file lv_timer.h
 */

#ifndef LV_TIMER_H
#define LV_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"
#include "../tick/lv_tick.h"
#include "lv_types.h"
#include "lv_ll.h"

/*********************
 *      DEFINES
 *********************/
#ifndef LV_ATTRIBUTE_TIMER_HANDLER
#define LV_ATTRIBUTE_TIMER_HANDLER
#endif

#define LV_NO_TIMER_READY 0xFFFFFFFF

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Таймеры выполняют этот тип функций.
 */
typedef void (*lv_timer_cb_t)(lv_timer_t *);

/**
 * Обработчик таймера возобновляет работу этого типа.
 */
typedef void (*lv_timer_handler_resume_cb_t)(void * data);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Периодически вызывайте его для обработки lv_timers.
 * @return время до следующего запуска (в мс)
 */
LV_ATTRIBUTE_TIMER_HANDLER uint32_t lv_timer_handler(void);

/**
 * Вызовите его в суперциклеmain()или потоках. Он будет запускатьlv_timer_handler()
 * с заданным периодом в мс. Вы можете использовать его со сном или задержкой в ​​среде OS.
 * Эта функция используется для упрощения портирования.
 * @param period период запуска lv_timer_handler()
 * @return время, через которое его необходимо вызвать снова
 */
LV_ATTRIBUTE_TIMER_HANDLER uint32_t lv_timer_handler_run_in_period(uint32_t period);

/**
 * Вызовите его в суперциклеmain()или потоках. Он автоматически вызываетlv_timer_handler() в нужный момент.
 * Эта функция используется для упрощения портирования.
 */
LV_ATTRIBUTE_TIMER_HANDLER void lv_timer_periodic_handler(void);

/**
 * Установите обратный вызов возобновления для обработчика таймера
 * @param cb функция, вызываемая при возобновлении обработчика таймера
 * @param data указатель на данные резюме
 */
void lv_timer_handler_set_resume_cb(lv_timer_handler_resume_cb_t cb, void * data);

/**
 * Создайте «пустой» таймер. Его необходимо инициализировать как минимум
 * `lv_timer_set_cb` и `lv_timer_set_period`
 * @return указатель на созданный таймер
 */
lv_timer_t * lv_timer_create_basic(void);

/**
 * создать новый lv_timer.
 * @param timer_xcb обратный вызов для периодического вызова.
 *                 (знак «x» в имени аргумента указывает на то, что это не полностью универсальная функция, поскольку она не следует
 *                  соглашение`func_name(object, callback, ...)`)
 * @param period период звонка в мс
 * @param user_data специальный параметр
 * @return указатель на новый таймер
 */
lv_timer_t * lv_timer_create(lv_timer_cb_t timer_xcb, uint32_t period, void * user_data);

/**
 * Удалить lv_timer
 * @param timer указатель на lv_timer
 */
void lv_timer_delete(lv_timer_t * timer);

/**
 * Приостановите таймер.
 * Обычно безопасно вызывать из обработчика прерывания или другого потока.
 * @param timer указатель на lv_timer
 */
void lv_timer_pause(lv_timer_t * timer);

/**
 * Возобновите таймер.
 * @param timer указатель на lv_timer
 */
void lv_timer_resume(lv_timer_t * timer);

/**
 * Установите обратный вызов на таймер (функция для периодического вызова)
 * @param timer указатель на таймер
 * @param timer_cb функция, вызываемая периодически
 */
void lv_timer_set_cb(lv_timer_t * timer, lv_timer_cb_t timer_cb);

/**
 * Установить новый период для lv_timer
 * @param timer указатель на lv_timer
 * @param period новый период
 */
void lv_timer_set_period(lv_timer_t * timer, uint32_t period);

/**
 * Подготовьте lv_timer. Оно не будет ждать своего часа.
 * @param timer указатель на lv_timer.
 */
void lv_timer_ready(lv_timer_t * timer);

/**
 * Установите количество повторений таймера.
 * @param timer указатель на lv_timer.
 * @param repeat_count -1: бесконечность;  0: стоп;  n>0: остаточные времена
 */
void lv_timer_set_repeat_count(lv_timer_t * timer, int32_t repeat_count);

/**
 * Установите, будет лиlv_timerавтоматически удаляться при многократном вызове`repeat_count`.
 * @param timer указатель на lv_timer.
 * @param auto_delete true: автоматическое удаление; false: таймер будет поставлен на паузу при вызове`repeat_count`раз.
 */
void lv_timer_set_auto_delete(lv_timer_t * timer, bool auto_delete);

/**
 * Установите пользовательский параметр lv_timer.
 * @param timer указатель на lv_timer.
 * @param user_data специальный параметр
 */
void lv_timer_set_user_data(lv_timer_t * timer, void * user_data);

/**
 * Сбросьтеlv_timer.
 * Через несколько миллисекунд он будет называться ранее установленным периодом.
 * @param timer указатель на lv_timer.
 */
void lv_timer_reset(lv_timer_t * timer);

/**
 * Включите или отключите всю обработку lv_timer.
 * @param en true: обработкаlv_timerвыполняется, false: обработкаlv_timerприостановлена
 */
void lv_timer_enable(bool en);

/**
 * Получить процент простоя
 * @return простояlv_timerв процентах
 */
uint32_t lv_timer_get_idle(void);

/**
 * Получите время, оставшееся до запуска следующего таймера
 * @return оставшееся время в мс
 */
uint32_t lv_timer_get_time_until_next(void);

/**
 * Перебирать таймеры
 * @param timer NULL для начала итерации или предыдущее возвращаемое значение для получения следующего таймера
 * @return следующий таймер или NULL, если таймера больше нет
 */
lv_timer_t * lv_timer_get_next(lv_timer_t * timer);

/**
 * Получитьuser_data, передаваемый при создании таймера
 * @param timer указатель на lv_timer
 * @return указатель на user_data
 */
void * lv_timer_get_user_data(lv_timer_t * timer);

/**
 * Получить состояние паузы таймера
 * @param timer указатель на lv_timer
 * @return true: таймер приостановлен; ложь: таймер работает
 */
bool lv_timer_get_paused(lv_timer_t * timer);

#if LV_USE_EXT_DATA
/**
 * @brief Прикрепляет внешние пользовательские данные и обратный вызов деструктора к объекту таймера.
 *
 * Связывает пользовательские данные с таймером LVGL и определяет функцию деструктора.
 * который будет автоматически вызываться при удалении таймера для правильной очистки.
 * связанные ресурсы.
 *
 * @param timer      Указатель на объект таймера
 * @param data       Пользовательский указатель данных для связи с таймером
 * @param destructor Функция обратного вызова для очисткиext_dataпри удалении таймера.
 *                   Получаетext_dataв качестве параметра.  NULLозначает, что очистка не требуется.
 */
void lv_timer_set_external_data(lv_timer_t * timer, void * data, void (* free_cb)(void * data));
#endif

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
