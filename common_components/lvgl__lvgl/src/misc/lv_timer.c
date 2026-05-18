/**
 * @file lv_timer.c
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_timer_private.h"
#include "../core/lv_global.h"
#include "../tick/lv_tick.h"
#include "../stdlib/lv_mem.h"
#include "../stdlib/lv_sprintf.h"
#include "lv_assert.h"
#include "lv_ll.h"
#include "lv_profiler.h"

/*********************
 *      DEFINES
 *********************/

#define IDLE_MEAS_PERIOD 500 /*[ms]*/
#define DEF_PERIOD 500

#define state LV_GLOBAL_DEFAULT()->timer_state
#define timer_ll_p &(state.timer_ll)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_timer_exec(lv_timer_t * timer);
static uint32_t lv_timer_time_remaining(lv_timer_t * timer);
static void lv_timer_handler_resume(void);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/
#if LV_USE_LOG && LV_LOG_TRACE_TIMER
    #define LV_TRACE_TIMER(...) LV_LOG_TRACE(__VA_ARGS__)
#else
    #define LV_TRACE_TIMER(...)
#endif

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_timer_core_init(void)
{
    lv_ll_init(timer_ll_p, sizeof(lv_timer_t));

    /*обработка обработки грузовlv_timer.*/
    lv_timer_enable(true);
}

LV_ATTRIBUTE_TIMER_HANDLER uint32_t lv_timer_handler(void)
{
    LV_TRACE_TIMER("begin");

    lv_timer_state_t * state_p = &state;
    /*Избегайте одновременного запуска обработчика таймера*/
    if(state_p->already_running) {
        LV_TRACE_TIMER("already running, concurrent calls are not allow, returning");
        return 1;
    }
    state_p->already_running = true;

    if(state_p->lv_timer_run == false) {
        state_p->already_running = false; /*Освободить мьютекс*/
        return 1;
    }

    LV_PROFILER_TIMER_BEGIN;
    lv_lock();

    uint32_t handler_start = lv_tick_get();

    if(handler_start == 0) {
        state.run_cnt++;
        if(state.run_cnt > 100) {
            state.run_cnt = 0;
            LV_LOG_WARN("It seems lv_tick_inc() is not called.");
        }
    }

    /*Запустить все таймеры из списка*/
    lv_timer_t * next;
    lv_timer_t * timer_active;
    lv_ll_t * timer_head = timer_ll_p;
    do {
        state_p->timer_deleted             = false;
        state_p->timer_created             = false;

        timer_active = lv_ll_get_head(timer_head);
        while(timer_active) {
            /*Таймер может быть удален, если он запускается только один раз ('repeat_count = 1').
             *Итак, получайте следующий элемент, пока текущий не станет действительным.*/
            next = lv_ll_get_next(timer_head, timer_active);

            if(lv_timer_exec(timer_active)) {
                /*Если таймер был создан или удален, возможно, этот или следующий элемент поврежден.*/
                if(state_p->timer_created || state_p->timer_deleted) {
                    LV_TRACE_TIMER("Start from the first timer again because a timer was created or deleted");
                    break;
                }
            }

            timer_active = next; /*Загрузите следующий таймер*/
        }
    } while(timer_active);

    uint32_t time_until_next = LV_NO_TIMER_READY;
    next = lv_ll_get_head(timer_head);
    while(next) {
        if(!next->paused) {
            uint32_t delay = lv_timer_time_remaining(next);
            if(delay < time_until_next)
                time_until_next = delay;
        }

        next = lv_ll_get_next(timer_head, next); /*Найти следующий таймер*/
    }

    state_p->busy_time += lv_tick_elaps(handler_start);
    uint32_t idle_period_time = lv_tick_elaps(state_p->idle_period_start);
    if(idle_period_time >= IDLE_MEAS_PERIOD) {
        state_p->idle_last         = (state_p->busy_time * 100) / idle_period_time;  /*Посчитайте процент занятости*/
        state_p->idle_last         = state_p->idle_last > 100 ? 0 : 100 - state_p->idle_last; /*Но нам нужно время простоя*/
        state_p->busy_time         = 0;
        state_p->idle_period_start = lv_tick_get();
    }

    state_p->timer_time_until_next = time_until_next;
    state_p->already_running = false; /*Освободите мьютекс*/

    LV_TRACE_TIMER("finished (%" LV_PRIu32 " ms until the next timer call)", time_until_next);
    lv_unlock();

    LV_PROFILER_TIMER_END;
    return time_until_next;
}

LV_ATTRIBUTE_TIMER_HANDLER void lv_timer_periodic_handler(void)
{
    lv_timer_state_t * state_p = &state;
    if(lv_tick_elaps(state_p->periodic_last_tick) >= state_p->timer_time_until_next) {
        LV_TRACE_TIMER("calling lv_timer_handler()");
        lv_timer_handler();
        state_p->periodic_last_tick = lv_tick_get();
    }
}

lv_timer_t * lv_timer_create_basic(void)
{
    return lv_timer_create(NULL, DEF_PERIOD, NULL);
}

lv_timer_t * lv_timer_create(lv_timer_cb_t timer_xcb, uint32_t period, void * user_data)
{
    lv_timer_t * new_timer = NULL;

    new_timer = lv_ll_ins_head(timer_ll_p);
    LV_ASSERT_MALLOC(new_timer);
    if(new_timer == NULL) return NULL;

    new_timer->period = period;
    new_timer->timer_cb = timer_xcb;
    new_timer->repeat_count = -1;
    new_timer->paused = 0;
    new_timer->last_run = lv_tick_get();
    new_timer->user_data = user_data;
    new_timer->auto_delete = true;
#if LV_USE_EXT_DATA
    new_timer->ext_data.free_cb = NULL;
    new_timer->ext_data.data = NULL;
#endif

    state.timer_created = true;

    lv_timer_handler_resume();

    return new_timer;
}

void lv_timer_set_cb(lv_timer_t * timer, lv_timer_cb_t timer_cb)
{
    LV_ASSERT_NULL(timer);
    timer->timer_cb = timer_cb;
}

void lv_timer_delete(lv_timer_t * timer)
{
    lv_ll_remove(timer_ll_p, timer);
    state.timer_deleted = true;

#if LV_USE_EXT_DATA
    if(timer->ext_data.free_cb) {
        timer->ext_data.free_cb(timer->ext_data.data);
        timer->ext_data.data = NULL;
    }
#endif

    lv_free(timer);
}

void lv_timer_pause(lv_timer_t * timer)
{
    LV_ASSERT_NULL(timer);
    timer->paused = true;
}

void lv_timer_resume(lv_timer_t * timer)
{
    LV_ASSERT_NULL(timer);
    timer->paused = false;
    lv_timer_handler_resume();
}

void lv_timer_set_period(lv_timer_t * timer, uint32_t period)
{
    LV_ASSERT_NULL(timer);
    timer->period = period;
    lv_timer_handler_resume();
}

void lv_timer_ready(lv_timer_t * timer)
{
    LV_ASSERT_NULL(timer);
    timer->last_run = lv_tick_get() - timer->period - 1;
    lv_timer_handler_resume();
}

void lv_timer_set_repeat_count(lv_timer_t * timer, int32_t repeat_count)
{
    LV_ASSERT_NULL(timer);
    timer->repeat_count = repeat_count;
}

void lv_timer_set_auto_delete(lv_timer_t * timer, bool auto_delete)
{
    LV_ASSERT_NULL(timer);
    timer->auto_delete = auto_delete;
}

void lv_timer_set_user_data(lv_timer_t * timer, void * user_data)
{
    LV_ASSERT_NULL(timer);
    timer->user_data = user_data;
}

void lv_timer_reset(lv_timer_t * timer)
{
    LV_ASSERT_NULL(timer);
    timer->last_run = lv_tick_get();
    lv_timer_handler_resume();
}

void lv_timer_enable(bool en)
{
    state.lv_timer_run = en;
    if(en) lv_timer_handler_resume();
}

void lv_timer_core_deinit(void)
{
    lv_timer_enable(false);

    lv_ll_clear(timer_ll_p);
}

uint32_t lv_timer_get_idle(void)
{
    return state.idle_last;
}

uint32_t lv_timer_get_time_until_next(void)
{
    return state.timer_time_until_next;
}

lv_timer_t * lv_timer_get_next(lv_timer_t * timer)
{
    if(timer == NULL) return lv_ll_get_head(timer_ll_p);
    else return lv_ll_get_next(timer_ll_p, timer);
}

LV_ATTRIBUTE_TIMER_HANDLER uint32_t lv_timer_handler_run_in_period(uint32_t period)
{
    static uint32_t last_tick = 0;

    if(lv_tick_elaps(last_tick) >= period) {
        last_tick = lv_tick_get();
        return lv_timer_handler();
    }
    return 1;
}

void * lv_timer_get_user_data(lv_timer_t * timer)
{
    return timer->user_data;
}

bool lv_timer_get_paused(lv_timer_t * timer)
{
    return timer->paused;
}

#if LV_USE_EXT_DATA
void lv_timer_set_external_data(lv_timer_t * timer, void * data, void (* free_cb)(void * data))
{
    if(!timer) {
        LV_LOG_WARN("Can't attach external user data and destructor callback to a NULL timer");
        return;
    }

    timer->ext_data.data = data;
    timer->ext_data.free_cb = free_cb;
}
#endif

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Выполнить таймер, если его оставшееся время равно нулю
 * @param timer указатель на lv_timer
 * @return true: выполнить, false: не выполнить
 */
static bool lv_timer_exec(lv_timer_t * timer)
{
    if(timer->paused) return false;

    bool exec = false;
    if(lv_timer_time_remaining(timer) == 0) {
        /* Уменьшите количество повторений перед выполнением timer_cb.
         * Если какой-либо таймер удален,`if(timer->repeat_count == 0)`не достигнет результата ниже.
         * но по крайней мере счетчик повторов равен нулю и таймер можно удалить в следующем раунде*/
        int32_t original_repeat_count = timer->repeat_count;
        if(timer->repeat_count > 0) timer->repeat_count--;
        timer->last_run = lv_tick_get();
        LV_TRACE_TIMER("calling timer callback: %p", *((void **)&timer->timer_cb));

        if(timer->timer_cb && original_repeat_count != 0) {
            LV_PROFILER_TIMER_BEGIN_TAG("timer_cb");
            timer->timer_cb(timer);
            LV_PROFILER_TIMER_END_TAG("timer_cb");
        }

        if(!state.timer_deleted) {
            LV_TRACE_TIMER("timer callback %p finished", *((void **)&timer->timer_cb));
        }
        else {
            LV_TRACE_TIMER("timer callback finished");
        }

        LV_ASSERT_MEM_INTEGRITY();
        exec = true;
    }

    if(state.timer_deleted == false) { /*Таймер также может быть удален сам по себе.*/
        if(timer->repeat_count == 0) { /*Счетчик повторений закончился, удалите таймер*/
            if(timer->auto_delete) {
                LV_TRACE_TIMER("deleting timer with %p callback because the repeat count is over", *((void **)&timer->timer_cb));
                lv_timer_delete(timer);
            }
            else {
                LV_TRACE_TIMER("pausing timer with %p callback because the repeat count is over", *((void **)&timer->timer_cb));
                lv_timer_pause(timer);
            }
        }
    }

    return exec;
}

/**
 * Узнайте, сколько времени осталось до запуска таймера.
 * @param timer указатель на lv_timer
 * @return оставшееся время или 0, если его нужно запустить снова
 */
static uint32_t lv_timer_time_remaining(lv_timer_t * timer)
{
    /*Проверьте, прошло ли хотя бы время «периода»*/
    uint32_t elp = lv_tick_elaps(timer->last_run);
    if(elp >= timer->period)
        return 0;
    return timer->period - elp;
}

/**
 * Вызов готового lv_timer
 */
static void lv_timer_handler_resume(void)
{
    /*Если есть таймер, который готов к работе, возобновите цикл таймера.*/
    state.timer_time_until_next = 0;
    if(state.resume_cb) {
        state.resume_cb(state.resume_data);
    }
}

void lv_timer_handler_set_resume_cb(lv_timer_handler_resume_cb_t cb, void * data)
{
    state.resume_cb = cb;
    state.resume_data = data;
}
