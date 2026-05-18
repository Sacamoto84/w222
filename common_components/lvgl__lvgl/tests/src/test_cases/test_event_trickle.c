#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

static uint32_t event_count = 0;
static lv_obj_t * last_target = NULL;

static void test_event_cb(lv_event_t * e)
{
    event_count++;
    last_target = lv_event_get_current_target_obj(e);
}

void test_event_trickle_basic(void)
{
    /*Создать родительский контейнер*/
    lv_obj_t * parent = lv_obj_create(lv_screen_active());
    lv_obj_add_flag(parent, LV_OBJ_FLAG_EVENT_TRICKLE);
    lv_obj_add_event_cb(parent, test_event_cb, LV_EVENT_CLICKED, NULL);

    /*Создание детей*/
    lv_obj_t * child1 = lv_obj_create(parent);
    lv_obj_add_event_cb(child1, test_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * child2 = lv_obj_create(parent);
    lv_obj_add_event_cb(child2, test_event_cb, LV_EVENT_CLICKED, NULL);

    /*Сбросить счетчики*/
    event_count = 0;
    last_target = NULL;

    /*Отправить событие родителю - должно дойти до детей*/
    lv_obj_send_event(parent, LV_EVENT_CLICKED, NULL);

    /*Должно было получить 3 события: родительский + 2 дочерних.*/
    TEST_ASSERT_EQUAL(3, event_count);

    /*Очистить*/
    lv_obj_delete(parent);
}

void test_event_trickle_stop(void)
{
    /*Создать родительский контейнер*/
    lv_obj_t * parent = lv_obj_create(lv_screen_active());
    lv_obj_add_flag(parent, LV_OBJ_FLAG_EVENT_TRICKLE);

    /*Добавьте обработчик событий, который останавливает просачивание вниз*/
    lv_obj_add_event_cb(parent, test_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(parent, (lv_event_cb_t)lv_event_stop_trickling, LV_EVENT_CLICKED, NULL);

    /*Создание детей*/
    lv_obj_t * child1 = lv_obj_create(parent);
    lv_obj_add_event_cb(child1, test_event_cb, LV_EVENT_CLICKED, NULL);

    /*Сбросить счетчики*/
    event_count = 0;
    last_target = NULL;

    /*Отправить событие родителю — если NOT уменьшится из-за остановки*/
    lv_obj_send_event(parent, LV_EVENT_CLICKED, NULL);

    /*Должен был получить только 1 событие: только родительский*/
    TEST_ASSERT_EQUAL(1, event_count); /* обработчик родительского события + обработчик остановки */

    /*Очистить*/
    lv_obj_delete(parent);
}

void test_event_trickle_disabled(void)
{
    /*Создайте флаг просачивания родительского контейнера WITHOUT.*/
    lv_obj_t * parent = lv_obj_create(lv_screen_active());
    lv_obj_add_event_cb(parent, test_event_cb, LV_EVENT_CLICKED, NULL);

    /*Создание детей*/
    lv_obj_t * child1 = lv_obj_create(parent);
    lv_obj_add_event_cb(child1, test_event_cb, LV_EVENT_CLICKED, NULL);

    /*Сбросить счетчики*/
    event_count = 0;
    last_target = NULL;

    /*Отправить событие родителю - если NOT просачивается вниз*/
    lv_obj_send_event(parent, LV_EVENT_CLICKED, NULL);

    /*Должен был получить только 1 событие: только родительский*/
    TEST_ASSERT_EQUAL(1, event_count);

    /*Очистить*/
    lv_obj_delete(parent);
}

#endif
