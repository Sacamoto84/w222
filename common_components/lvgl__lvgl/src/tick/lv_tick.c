/**
 * @file lv_tick.c
 * Обеспечить доступ к системному тику с разрешением 1 миллисекунду
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_tick_private.h"
#include "../misc/lv_types.h"
#include "../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/
#define state LV_GLOBAL_DEFAULT()->tick_state

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

LV_ATTRIBUTE_TICK_INC void lv_tick_inc(uint32_t tick_period)
{
    lv_tick_state_t * state_p = &state;

    state_p->sys_irq_flag = 0;
    state_p->sys_time += tick_period;
}

uint32_t lv_tick_get(void)
{
    lv_tick_state_t * state_p = &state;

    if(state_p->tick_get_cb)
        return state_p->tick_get_cb();

    /*Если`lv_tick_inc`возникает из-за прерывания во время чтения `sys_time`
     *результат может быть испорчен.
     *Этот цикл был вызван`lv_tick_inc`во время чтения `sys_time`.
     *Если`tick_irq_flag`был очищен в`lv_tick_inc`, форма была прочтена еще раз.
     *пока`tick_irq_flag`не останется`1`.*/
    uint32_t result;
    do {
        state_p->sys_irq_flag = 1;
        result        = state_p->sys_time;
    } while(!state_p->sys_irq_flag); /*Продолжайте, пока не увидите непрерывный цикл.*/

    return result;
}

uint32_t lv_tick_elaps(uint32_t prev_tick)
{
    return lv_tick_diff(lv_tick_get(), prev_tick);
}

uint32_t lv_tick_diff(uint32_t tick, uint32_t prev_tick)
{
    /*Беззнаковое переполнение четко определено и работает для одного переноса.*/
    return tick - prev_tick;
}

void lv_delay_ms(uint32_t ms)
{
    if(state.delay_cb) {
        state.delay_cb(ms);
    }
    else {
        uint32_t t = lv_tick_get();
        while(lv_tick_elaps(t) < ms) {
            /*Сделайте что-нибудь, чтобы не открывать`lv_tick_elaps`слишком часто, так как это может спровоцировать прерывания.*/
            volatile uint32_t i;
            volatile uint32_t x = ms;
            for(i = 0; i < 100; i++) {
                x = x * 3;
            }
        }
    }
}

void lv_tick_set_cb(lv_tick_get_cb_t cb)
{
    state.tick_get_cb = cb;
}

lv_tick_get_cb_t lv_tick_get_cb(void)
{
    return state.tick_get_cb;
}

void lv_delay_set_cb(lv_delay_cb_t cb)
{
    state.delay_cb = cb;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
