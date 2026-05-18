/**
 * @file lv_libinput_private.h
 *
 */

#ifndef LV_LIBINPUT_PRIVATE_H
#define LV_LIBINPUT_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_libinput.h"

#if LV_USE_LIBINPUT

#if LV_LIBINPUT_XKB
#include "lv_xkb_private.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_libinput_event_t {
    lv_indev_state_t pressed;
    int key_val;
    lv_point_t point;
};

struct _lv_libinput_t {
    int fd;
    struct pollfd fds[1];

    /* Массив точек реализован в виде круговой очереди LIFO. */
    lv_libinput_event_t points[LV_LIBINPUT_MAX_EVENTS]; /* Буфер событий */
    lv_libinput_event_t slots[2]; /* Состояние до 2 пальцев в реальном времени для обработки мультитач */

    /* Указательные устройства работают в libinput несколько иначе, что требует от нас сохранения последнего известного состояния. */
    lv_point_t pointer_position;
    bool pointer_button_down;

    int start; /* Индекс начала очереди событий */
    int end; /* Индекс конца очереди*/
    lv_libinput_event_t last_event; /* Сообщать, когда нет новых событий
                                   * чтобы поддерживать постоянное состояние разработки
                                   */
    bool deinit; /* Скажите рабочему потоку выйти */
    pthread_mutex_t event_lock;
    pthread_t worker_thread;

    struct libinput * libinput_context;
    struct libinput_device * libinput_device;

#if LV_LIBINPUT_XKB
    lv_xkb_t xkb;
#endif /* LV_LIBINPUT_XKB */
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_LIBINPUT */

#ifdef __cplusplus
} /*внешний "С"*/
#endif

#endif /*LV_LIBINPUT_PRIVATE_H*/
