/**
 * @file lv_async.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_async.h"
#include "lv_timer_private.h"
#include "../stdlib/lv_mem.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct _lv_async_info_t {
    lv_async_cb_t cb;
    void * user_data;
} lv_async_info_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_async_timer_cb(lv_timer_t * timer);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_result_t lv_async_call(lv_async_cb_t async_xcb, void * user_data)
{
    /*Выделить информационную структуру*/
    lv_async_info_t * info = lv_malloc(sizeof(lv_async_info_t));

    if(info == NULL)
        return LV_RESULT_INVALID;

    /*Создать новый таймер*/
    lv_timer_t * timer = lv_timer_create(lv_async_timer_cb, 0, info);

    if(timer == NULL) {
        lv_free(info);
        return LV_RESULT_INVALID;
    }

    info->cb = async_xcb;
    info->user_data = user_data;

    lv_timer_set_repeat_count(timer, 1);
    return LV_RESULT_OK;
}

lv_result_t lv_async_call_cancel(lv_async_cb_t async_xcb, void * user_data)
{
    lv_timer_t * timer = lv_timer_get_next(NULL);
    lv_result_t res = LV_RESULT_INVALID;

    while(timer != NULL) {
        /*Найдите следующий узел таймера*/
        lv_timer_t * timer_next = lv_timer_get_next(timer);

        /*Найти обратный вызов асинхронного таймера*/
        if(timer->timer_cb == lv_async_timer_cb) {
            lv_async_info_t * info = (lv_async_info_t *)timer->user_data;

            /*Сопоставление обратного вызова пользовательской функции и пользовательских данных*/
            if(info->cb == async_xcb && info->user_data == user_data) {
                lv_timer_delete(timer);
                lv_free(info);
                res = LV_RESULT_OK;
            }
        }

        timer = timer_next;
    }

    return res;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_async_timer_cb(lv_timer_t * timer)
{
    /*Сохраните информацию, потому чтоlv_async_call_cancelможет удалить ее обратным вызовом.*/
    lv_async_info_t * info = (lv_async_info_t *)timer->user_data;
    lv_async_info_t info_save = *info;
    lv_timer_delete(timer);
    lv_free(info);

    info_save.cb(info_save.user_data);
}
