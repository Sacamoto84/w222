#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"


void setUp(void)
{
    /* Функция запускается перед каждым тестом */
}

void tearDown(void)
{
    /* Функция запускается после каждого теста */
    lv_obj_clean(lv_screen_active());
    lv_anim_delete_all();
    lv_anim_enable_vsync_mode(false);
}

static void start_cb(lv_anim_t * anim)
{
    (*(int *)lv_anim_get_user_data(anim))++;
}

static void exec_cb(void * var, int32_t v)
{
    int32_t * var_i32 = var;
    *var_i32 = v;
}

static void custom_exec_cb(lv_anim_t * a, int32_t v)
{
    int32_t * var_i32 = a->var;
    *var_i32 = v;
}

void test_anim_delete(void)
{
    int32_t var;

    /*Запустить анимацию*/
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, &var);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_exec_cb(&a, exec_cb);
    lv_anim_set_duration(&a, 100);
    lv_anim_start(&a);

    lv_test_wait(20);
    TEST_ASSERT_EQUAL(19, var);

    bool deleted;
    /*Неправильная переменная, ничего не должно произойти*/
    deleted = lv_anim_delete(&a, exec_cb);
    TEST_ASSERT_FALSE(deleted);

    lv_test_wait(20);
    TEST_ASSERT_EQUAL(39, var);

    /*Значение не должно меняться после удаления*/
    deleted = lv_anim_delete(&var, exec_cb);
    TEST_ASSERT_TRUE(deleted);

    lv_test_wait(20);
    TEST_ASSERT_EQUAL(39, var);
}

void test_anim_delete_custom(void)
{
    int32_t var;

    /*Запустить анимацию*/
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, &var);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_custom_exec_cb(&a, custom_exec_cb);
    lv_anim_set_duration(&a, 100);
    lv_anim_start(&a);

    lv_test_wait(20);
    TEST_ASSERT_EQUAL(19, var);

    bool deleted;
    /*Неправильный обратный вызов, ничего не должно произойти*/
    deleted = lv_anim_delete(&var, exec_cb);
    TEST_ASSERT_FALSE(deleted);

    lv_test_wait(20);
    TEST_ASSERT_EQUAL(39, var);

    /*Значение не должно меняться после удаления*/
    deleted = lv_anim_delete(&var, NULL);
    TEST_ASSERT_TRUE(deleted);

    lv_test_wait(20);
    TEST_ASSERT_EQUAL(39, var);
}
void test_anim_pause(void)
{
    int32_t var;

    /*Запустить анимацию*/
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, &var);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_exec_cb(&a, exec_cb);
    lv_anim_set_duration(&a, 100);
    lv_anim_t * animation = lv_anim_start(&a);

    lv_test_wait(40);
    TEST_ASSERT_EQUAL(39, var);

    lv_anim_pause(animation);

    lv_test_wait(40);
    TEST_ASSERT_EQUAL(39, var);

    lv_anim_resume(animation);

    lv_test_wait(20);
    TEST_ASSERT_EQUAL(59, var);

    lv_test_wait(41);
    TEST_ASSERT_EQUAL(100, var);
}

void test_anim_pause_for(void)
{
    int32_t var;

    /*Запустить анимацию*/
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, &var);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_exec_cb(&a, exec_cb);
    lv_anim_set_duration(&a, 100);
    lv_anim_t * animation = lv_anim_start(&a);

    lv_anim_pause_for(animation, 20);
    lv_test_wait(40);

    TEST_ASSERT_EQUAL(19, var);

    lv_anim_pause_for(animation, 20);

    lv_test_wait(40);
    lv_test_wait(40);

    TEST_ASSERT_EQUAL(79, var);

    /*Удалите анимацию, чтобы избежать доступа к ней после возврата.*/
    lv_anim_delete(&var, exec_cb);
}

void test_anim_pause_for_resume(void)
{
    int32_t var;

    /*Запустить анимацию*/
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, &var);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_exec_cb(&a, exec_cb);
    lv_anim_set_duration(&a, 100);
    lv_anim_t * animation = lv_anim_start(&a);

    lv_anim_pause_for(animation, 40);

    lv_test_wait(20);
    lv_anim_resume(animation);

    lv_test_wait(20);
    TEST_ASSERT_EQUAL(19, var);
}

static void event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target_obj(e);
    int * var = lv_event_get_user_data(e);
    lv_anim_delete(obj, NULL);
    *var += 1;
}

void test_scroll_anim_delete(void)
{
    int var = 0;
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_add_event_cb(obj, event_cb, LV_EVENT_SCROLL_END, &var);
    lv_obj_scroll_by(obj, 0, 100, LV_ANIM_ON);
    lv_test_wait(20);
    lv_obj_scroll_by(obj, 0, 100, LV_ANIM_ON);

    TEST_ASSERT_EQUAL(1, var);
}
void test_anim_start_cb_is_called(void)
{
    int32_t var;
    int start_cb_call_count = 0;
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, &var);
    lv_anim_set_user_data(&a, (void *)&start_cb_call_count);
    lv_anim_set_start_cb(&a, start_cb);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_exec_cb(&a, exec_cb);
    lv_anim_set_duration(&a, 100);
    lv_anim_set_repeat_count(&a, 2);
    lv_anim_start(&a);
    lv_test_wait(50);
    TEST_ASSERT_EQUAL(1, start_cb_call_count);
    lv_test_wait(100);
    TEST_ASSERT_EQUAL(2, start_cb_call_count);
    lv_test_wait(50);
    /*Удалите анимацию, чтобы избежать доступа к ней после возврата.*/
    lv_anim_delete(&var, exec_cb);
}

void test_anim_vsync_mode(void)
{
    lv_anim_enable_vsync_mode(true);

    /* таймер анимации должен быть NULL, когда включен режим vsync. */
    TEST_ASSERT_NULL(lv_anim_get_timer());

    int32_t var;

    /*Запустить анимацию*/
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, &var);
    lv_anim_set_values(&a, 0, 1000);
    lv_anim_set_exec_cb(&a, exec_cb);
    lv_anim_set_duration(&a, 1000);
    lv_anim_start(&a);

    /*Используйте события vsync для уведомления об обновлениях анимации.*/
    lv_tick_inc(10);
    lv_display_send_vsync_event(NULL, NULL);
    TEST_ASSERT_EQUAL(9, var);

    lv_tick_inc(10);
    lv_display_send_vsync_event(NULL, NULL);
    TEST_ASSERT_EQUAL(19, var);

    lv_anim_enable_vsync_mode(false);
    TEST_ASSERT_NOT_NULL(lv_anim_get_timer());

    /* Не следует обновлять анимацию с помощью событий vsync, когда режим vsync отключен. */
    lv_tick_inc(20);
    lv_display_send_vsync_event(NULL, NULL);
    TEST_ASSERT_EQUAL(19, var);

    /* Проверьте нормальный режим таймера */
    lv_test_wait(20);
    TEST_ASSERT_EQUAL(59, var);

    lv_anim_delete(&var, exec_cb);
}

#endif
