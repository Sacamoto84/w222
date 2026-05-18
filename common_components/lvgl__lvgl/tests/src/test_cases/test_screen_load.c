#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

void test_screen_load_no_crash(void)
{
    /*Удалить активный экран и загрузить новый экран не должно привести к сбою*/
    lv_obj_t * screen = lv_screen_active();
    lv_obj_delete(screen);
    screen = lv_obj_create(NULL);
    lv_screen_load(screen);

    /*Последовательная загрузка нескольких экранов с анимацией перехода не должна вызывать сбои.*/
    lv_obj_t * screen_with_anim_1 = lv_obj_create(NULL);
    lv_obj_t * screen_with_anim_2 = lv_obj_create(NULL);
    lv_screen_load_anim(screen_with_anim_1, LV_SCREEN_LOAD_ANIM_OVER_LEFT, 2000, 0, false);
    lv_screen_load_anim(screen_with_anim_2, LV_SCREEN_LOAD_ANIM_OVER_RIGHT, 1000, 500, false);
}

void test_screen_load_with_delete_no_crash(void)
{
    /*Удалить активный экран и загрузить новый экран не должно привести к сбою*/
    lv_obj_t * screen = lv_screen_active();
    lv_obj_delete(screen);
    screen = lv_obj_create(NULL);
    lv_screen_load(screen);

    /*Последовательная загрузка нескольких экранов (при удалении одного) с анимацией перехода не должна вызывать сбой.*/
    lv_obj_t * screen_with_anim_1 = lv_obj_create(NULL);
    lv_obj_t * screen_with_anim_2 = lv_obj_create(NULL);
    lv_obj_t * screen_with_anim_3 = lv_obj_create(NULL);

    lv_screen_load_anim(screen_with_anim_1, LV_SCREEN_LOAD_ANIM_OVER_LEFT, 0, 0, false);
    lv_screen_load_anim(screen_with_anim_2, LV_SCREEN_LOAD_ANIM_OVER_RIGHT, 1000, 0, true);

    /*Подождите, чтобы вызвать обратные вызовы начала анимации*/
    lv_test_wait(100);

    lv_screen_load_anim(screen_with_anim_3, LV_SCREEN_LOAD_ANIM_OVER_LEFT, 200, 0, true);

    /*Активный экран следует немедленно заменить.*/
    TEST_ASSERT_EQUAL(lv_screen_active(), screen_with_anim_2);

    lv_test_wait(400);

    /*Проверьте состояние экранов после перехода*/
    TEST_ASSERT_EQUAL(lv_obj_is_valid(screen_with_anim_1), false);
    TEST_ASSERT_EQUAL(lv_obj_is_valid(screen_with_anim_2), false);
    TEST_ASSERT_EQUAL(lv_obj_is_valid(screen_with_anim_3), true);
}

void test_screen_load_with_delete_no_crash2(void)
{
    /*Удалить активный экран и загрузить новый экран не должно привести к сбою*/
    lv_obj_t * screen = lv_screen_active();
    lv_obj_delete(screen);
    screen = lv_obj_create(NULL);
    lv_screen_load(screen);

    /*Последовательная загрузка нескольких экранов (при удалении одного) с анимацией перехода не должна вызывать сбой.*/
    lv_obj_t * screen_with_anim_1 = lv_obj_create(NULL);
    lv_obj_t * screen_with_anim_2 = lv_obj_create(NULL);
    lv_obj_t * screen_with_anim_3 = lv_obj_create(NULL);
    lv_obj_t * screen_with_anim_4 = lv_obj_create(NULL);

    lv_screen_load_anim(screen_with_anim_1, LV_SCREEN_LOAD_ANIM_OVER_LEFT, 0, 0, false);
    lv_screen_load_anim(screen_with_anim_2, LV_SCREEN_LOAD_ANIM_OVER_RIGHT, 1000, 0, true);
    lv_screen_load_anim(screen_with_anim_3, LV_SCREEN_LOAD_ANIM_OVER_LEFT, 0, 0, true);

    /*Подождите, чтобы вызвать обратные вызовы начала анимации*/
    lv_test_wait(100);

    lv_screen_load_anim(screen_with_anim_4, LV_SCREEN_LOAD_ANIM_OVER_LEFT, 200, 0, true);

    /*Активный экран следует немедленно заменить.*/
    TEST_ASSERT_EQUAL(lv_screen_active(), screen_with_anim_3);

    lv_test_wait(400);

    /*Проверьте состояние экранов после перехода*/
    TEST_ASSERT_EQUAL(lv_obj_is_valid(screen_with_anim_1), false);
    TEST_ASSERT_EQUAL(lv_obj_is_valid(screen_with_anim_2), false);
    TEST_ASSERT_EQUAL(lv_obj_is_valid(screen_with_anim_3), false);
    TEST_ASSERT_EQUAL(lv_obj_is_valid(screen_with_anim_4), true);
}

static bool screen_1_unloaded_called = false;

static void screen_with_anim_1_unloaded_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    screen_1_unloaded_called = true;
}

void test_screen_load_with_delete_event(void)
{
    /*Удалить активный экран и загрузить новый экран не должно привести к сбою*/
    lv_obj_t * screen = lv_screen_active();
    lv_obj_delete(screen);
    screen = lv_obj_create(NULL);
    lv_screen_load(screen);

    /*Последовательная загрузка нескольких экранов (при удалении одного) с анимацией перехода не должна вызывать сбой.*/
    lv_obj_t * screen_with_anim_1 = lv_obj_create(NULL);
    lv_obj_t * screen_with_anim_2 = lv_obj_create(NULL);
    lv_obj_t * screen_with_anim_3 = lv_obj_create(NULL);
    lv_obj_t * screen_with_anim_4 = lv_obj_create(NULL);

    lv_screen_load_anim(screen_with_anim_1, LV_SCREEN_LOAD_ANIM_OVER_LEFT, 0, 0, false);
    lv_obj_add_event_cb(screen_with_anim_1, screen_with_anim_1_unloaded_cb, LV_EVENT_SCREEN_UNLOADED, NULL);
    lv_screen_load_anim(screen_with_anim_2, LV_SCREEN_LOAD_ANIM_OVER_RIGHT, 1000, 0, true);
    lv_screen_load_anim(screen_with_anim_3, LV_SCREEN_LOAD_ANIM_OVER_LEFT, 0, 0, true);

    /*Подождите, чтобы вызвать обратные вызовы начала анимации*/
    lv_test_wait(100);

    TEST_ASSERT_EQUAL(screen_1_unloaded_called, true);

    lv_screen_load_anim(screen_with_anim_4, LV_SCREEN_LOAD_ANIM_OVER_LEFT, 200, 0, true);

    /*Активный экран следует немедленно заменить.*/
    TEST_ASSERT_EQUAL(lv_screen_active(), screen_with_anim_3);

    lv_test_wait(400);

    /*Проверьте состояние экранов после перехода*/
    TEST_ASSERT_EQUAL(lv_obj_is_valid(screen_with_anim_1), false);
    TEST_ASSERT_EQUAL(lv_obj_is_valid(screen_with_anim_2), false);
    TEST_ASSERT_EQUAL(lv_obj_is_valid(screen_with_anim_3), false);
    TEST_ASSERT_EQUAL(lv_obj_is_valid(screen_with_anim_4), true);
}

static size_t display_screen_load_start = 0;
static size_t display_screen_loaded = 0;
static size_t display_screen_unload_start = 0;
static size_t display_screen_unloaded = 0;
static void count_display_screen_load_events_cb(lv_event_t * e)
{
    lv_obj_t * screen = lv_event_get_param(e);
    TEST_ASSERT_NOT_NULL(screen);

    lv_display_t * screen_display = lv_obj_get_display(screen);
    TEST_ASSERT_NOT_NULL(screen_display);
    TEST_ASSERT_EQUAL(screen_display, lv_event_get_target(e));

    switch(lv_event_get_code(e)) {
        case LV_EVENT_SCREEN_UNLOADED:
            display_screen_unloaded++;
            break;
        case LV_EVENT_SCREEN_UNLOAD_START:
            display_screen_unload_start++;
            break;
        case LV_EVENT_SCREEN_LOADED:
            display_screen_loaded++;
            break;
        case LV_EVENT_SCREEN_LOAD_START:
            display_screen_load_start++;
            break;
        default:
            break;
    }
}

void test_display_receives_screen_load_events(void)
{
    display_screen_load_start = 0;
    display_screen_loaded = 0;
    display_screen_unload_start = 0;
    display_screen_unloaded = 0;

    lv_display_t * display = lv_display_create(100, 100);
    lv_display_set_default(display);

    lv_obj_t * screen1 = lv_obj_create(NULL);

    lv_display_add_event_cb(display, count_display_screen_load_events_cb, LV_EVENT_SCREEN_UNLOAD_START, NULL);
    lv_display_add_event_cb(display, count_display_screen_load_events_cb, LV_EVENT_SCREEN_UNLOADED, NULL);
    lv_display_add_event_cb(display, count_display_screen_load_events_cb, LV_EVENT_SCREEN_LOAD_START, NULL);
    lv_display_add_event_cb(display, count_display_screen_load_events_cb, LV_EVENT_SCREEN_LOADED, NULL);

    lv_screen_load(screen1);
    TEST_ASSERT_EQUAL(1, display_screen_unloaded);
    TEST_ASSERT_EQUAL(1, display_screen_unload_start);
    TEST_ASSERT_EQUAL(1, display_screen_loaded);
    TEST_ASSERT_EQUAL(1, display_screen_load_start);

    /* Загрузка того же экрана ничего не дает*/
    lv_screen_load(screen1);
    TEST_ASSERT_EQUAL(1, display_screen_unloaded);
    TEST_ASSERT_EQUAL(1, display_screen_unload_start);
    TEST_ASSERT_EQUAL(1, display_screen_loaded);
    TEST_ASSERT_EQUAL(1, display_screen_load_start);

    lv_display_delete(display);
}

static size_t screen_event_count = 0;
static void screen_event_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    screen_event_count++;
}

static size_t display_event_count = 0;
/* Этот обработчик событий удаляет отображение во время событий загрузки экрана.*/
static void display_event_delete_cb(lv_event_t * e)
{
    lv_obj_t * screen = lv_event_get_param(e);
    TEST_ASSERT_NOT_NULL(screen);

    lv_display_t * screen_display = lv_obj_get_display(screen);
    TEST_ASSERT_NOT_NULL(screen_display);
    lv_display_t * event_display = lv_event_get_target(e);
    TEST_ASSERT_NOT_NULL(event_display);

    /* Отображение экрана и отображение событий должны совпадать.*/
    TEST_ASSERT_EQUAL(screen_display, event_display);
    lv_display_delete(event_display);
    display_event_count++;
}



void test_display_delete_when_screen_is_loaded(void)
{
    /* Убедитесь, что LVGL правильно обрабатывает удаление отображения во время событий загрузки экрана.*/
    {
        lv_display_t * display = lv_display_create(100, 100);
        lv_display_set_default(display);
        lv_obj_t * screen = lv_obj_create(NULL);
        lv_display_add_event_cb(display, display_event_delete_cb, LV_EVENT_SCREEN_UNLOAD_START, NULL);

        lv_obj_add_event_cb(lv_screen_active(), screen_event_cb, LV_EVENT_SCREEN_UNLOAD_START, NULL);
        lv_obj_add_event_cb(lv_screen_active(), screen_event_cb, LV_EVENT_SCREEN_UNLOADED, NULL);

        lv_obj_add_event_cb(screen, screen_event_cb, LV_EVENT_SCREEN_LOAD_START, NULL);
        lv_obj_add_event_cb(screen, screen_event_cb, LV_EVENT_SCREEN_LOADED, NULL);
        lv_screen_load(screen);

        /* Событие экрана дисплея вызывалось только один раз*/
        TEST_ASSERT_EQUAL(display_event_count, 1);
        /* Нет события выгрузки для предыдущего экрана, поскольку дисплей был удален
         * Нет события загрузки для нового экрана, поскольку дисплей был удален */
        TEST_ASSERT_EQUAL(screen_event_count, 0);

        display_event_count = screen_event_count = 0;
    }

    {
        lv_display_t * display = lv_display_create(100, 100);
        lv_display_set_default(display);
        lv_obj_t * screen = lv_obj_create(NULL);
        lv_display_add_event_cb(display, display_event_delete_cb, LV_EVENT_SCREEN_LOAD_START, NULL);
        lv_obj_add_event_cb(lv_screen_active(), screen_event_cb, LV_EVENT_SCREEN_UNLOAD_START, NULL);
        lv_obj_add_event_cb(lv_screen_active(), screen_event_cb, LV_EVENT_SCREEN_UNLOADED, NULL);

        lv_obj_add_event_cb(screen, screen_event_cb, LV_EVENT_SCREEN_LOAD_START, NULL);
        lv_obj_add_event_cb(screen, screen_event_cb, LV_EVENT_SCREEN_LOADED, NULL);
        lv_screen_load(screen);

        /* Событие экрана дисплея вызывалось только один раз*/
        TEST_ASSERT_EQUAL(display_event_count, 1);
        /* Мы должны были получить событие начала выгрузки с предыдущего экрана до того, как дисплей был удален.
         * Нет события загрузки для нового экрана, поскольку дисплей был удален */
        TEST_ASSERT_EQUAL(screen_event_count, 1);

        display_event_count = screen_event_count = 0;
    }

    {
        lv_display_t * display = lv_display_create(100, 100);
        lv_display_set_default(display);
        lv_obj_t * screen = lv_obj_create(NULL);
        lv_display_add_event_cb(display, display_event_delete_cb, LV_EVENT_SCREEN_LOADED, NULL);

        lv_obj_add_event_cb(lv_screen_active(), screen_event_cb, LV_EVENT_SCREEN_UNLOAD_START, NULL);
        lv_obj_add_event_cb(lv_screen_active(), screen_event_cb, LV_EVENT_SCREEN_UNLOADED, NULL);
        lv_obj_add_event_cb(screen, screen_event_cb, LV_EVENT_SCREEN_LOAD_START, NULL);
        lv_obj_add_event_cb(screen, screen_event_cb, LV_EVENT_SCREEN_LOADED, NULL);
        lv_screen_load(screen);

        /* Событие экрана дисплея вызывалось только один раз*/
        TEST_ASSERT_EQUAL(display_event_count, 1);
        /* Мы должны были получить событие начала выгрузки с предыдущего экрана до того, как дисплей был удален.
         * Мы должны были увидеть событиеload_startдля нового экрана до того, как дисплей был удален. */
        TEST_ASSERT_EQUAL(screen_event_count, 2);

        display_event_count = screen_event_count = 0;
    }
    {
        lv_display_t * display = lv_display_create(100, 100);
        lv_display_set_default(display);
        lv_obj_t * screen = lv_obj_create(NULL);
        lv_display_add_event_cb(display, display_event_delete_cb, LV_EVENT_SCREEN_UNLOADED, NULL);
        lv_obj_add_event_cb(lv_screen_active(), screen_event_cb, LV_EVENT_SCREEN_UNLOAD_START, NULL);
        lv_obj_add_event_cb(lv_screen_active(), screen_event_cb, LV_EVENT_SCREEN_UNLOADED, NULL);

        lv_obj_add_event_cb(screen, screen_event_cb, LV_EVENT_SCREEN_LOAD_START, NULL);
        lv_obj_add_event_cb(screen, screen_event_cb, LV_EVENT_SCREEN_LOADED, NULL);
        lv_screen_load(screen);

        /* Событие экрана дисплея вызывалось только один раз*/
        TEST_ASSERT_EQUAL(display_event_count, 1);
        /* Мы должны были получить событие начала выгрузки с предыдущего экрана до того, как дисплей был удален.
         * Мы должны были получитьload_startи загрузить события для нового экрана до того, как индикатор был удален. */
        TEST_ASSERT_EQUAL(screen_event_count, 3);
        display_event_count = screen_event_count = 0;
    }
}

static size_t screen_delete_event_count = 0;
/* Этот обработчик событий удаляет целевой объект во время событий загрузки экрана.*/
static void screen_delete_event_cb(lv_event_t * e)
{
    lv_obj_delete(lv_event_get_target_obj(e));
    screen_delete_event_count++;
}

void test_new_screen_delete_when_screen_is_loaded(void)
{
    /* Убедитесь, что LVGL правильно обрабатывает удаление нового экрана во время событий загрузки экрана.*/
    {
        lv_obj_t * screen = lv_obj_create(NULL);
        lv_obj_add_event_cb(screen, screen_delete_event_cb, LV_EVENT_SCREEN_LOAD_START, NULL);
        lv_screen_load(screen);

        /* Новый экран удален во время загрузки экрана*/
        TEST_ASSERT_EQUAL(screen_delete_event_count, 1);
        TEST_ASSERT_NULL(lv_display_get_screen_active(lv_display_get_default()));
        screen_delete_event_count = 0;
    }

    {
        lv_obj_t * screen = lv_obj_create(NULL);
        lv_obj_add_event_cb(screen, screen_delete_event_cb, LV_EVENT_SCREEN_LOADED, NULL);
        lv_screen_load(screen);

        /* Новый экран удален во время загрузки экрана*/
        TEST_ASSERT_EQUAL(screen_delete_event_count, 1);
        TEST_ASSERT_NULL(lv_display_get_screen_active(lv_display_get_default()));
        screen_delete_event_count = 0;
    }
}
void test_old_screen_delete_when_screen_is_loaded(void)
{
    /* Убедитесь, что LVGL правильно обрабатывает удаление старого экрана во время событий загрузки экрана.*/
    lv_obj_t * default_screen = lv_obj_create(NULL);
    lv_screen_load(default_screen);
    {
        lv_obj_t * screen = lv_obj_create(NULL);
        lv_obj_add_event_cb(lv_screen_active(), screen_delete_event_cb, LV_EVENT_SCREEN_UNLOAD_START, NULL);
        lv_screen_load(screen);

        /* Старый экран удален во время загрузки экрана*/
        TEST_ASSERT_EQUAL(screen_delete_event_count, 1);
        TEST_ASSERT_EQUAL(screen, lv_display_get_screen_active(lv_display_get_default()));
        screen_delete_event_count = 0;
    }
    {
        lv_obj_t * screen = lv_obj_create(NULL);
        lv_obj_add_event_cb(lv_screen_active(), screen_delete_event_cb, LV_EVENT_SCREEN_UNLOADED, NULL);
        lv_screen_load(screen);

        /* Старый экран удален во время загрузки экрана*/
        TEST_ASSERT_EQUAL(screen_delete_event_count, 1);
        TEST_ASSERT_EQUAL(screen, lv_display_get_screen_active(lv_display_get_default()));
        screen_delete_event_count = 0;
    }
    lv_obj_t * active_screen = lv_screen_active();
    TEST_ASSERT_NOT_NULL(active_screen);
    lv_obj_delete(active_screen);
}

static size_t display_screen_delete_screen_event_count = 0;
/* Удаляет дисплей, переданный как параметр в событии загрузки экрана дисплея.*/
static void display_delete_screen_event_cb(lv_event_t * e)
{
    lv_obj_t * screen = lv_event_get_param(e);
    TEST_ASSERT_NOT_NULL(screen);
    lv_obj_delete(screen);
    display_screen_delete_screen_event_count++;
}

void test_screen_is_deleted_when_loaded_in_display_event(void)
{
    /* Убедитесь, что LVGL правильно обрабатывает удаление экранов во время событий загрузки экрана дисплея.*/
    {
        lv_display_t * display = lv_display_create(100, 100);
        lv_display_set_default(display);
        lv_obj_t * screen = lv_obj_create(NULL);
        lv_display_add_event_cb(display, display_delete_screen_event_cb, LV_EVENT_SCREEN_UNLOAD_START, NULL);

        lv_obj_add_event_cb(lv_screen_active(), screen_event_cb, LV_EVENT_SCREEN_UNLOAD_START, NULL);
        lv_obj_add_event_cb(lv_screen_active(), screen_event_cb, LV_EVENT_SCREEN_UNLOADED, NULL);

        lv_obj_add_event_cb(screen, screen_event_cb, LV_EVENT_SCREEN_LOAD_START, NULL);
        lv_obj_add_event_cb(screen, screen_event_cb, LV_EVENT_SCREEN_LOADED, NULL);
        lv_screen_load(screen);

        /* Событие экрана дисплея вызывалось только один раз*/
        TEST_ASSERT_EQUAL(display_screen_delete_screen_event_count, 1);
        /* События выгрузки не отправляются на предыдущий экран, поскольку старый экран был удален во время события отображения.
         * Оба события загрузки для нового экрана отправляются */
        TEST_ASSERT_EQUAL(screen_event_count, 2);

        TEST_ASSERT_EQUAL(screen, lv_screen_active());

        display_screen_delete_screen_event_count = screen_event_count = 0;
        lv_display_delete(display);
    }

    {
        lv_display_t * display = lv_display_create(100, 100);
        lv_display_set_default(display);
        lv_obj_t * screen = lv_obj_create(NULL);
        lv_display_add_event_cb(display, display_delete_screen_event_cb, LV_EVENT_SCREEN_LOAD_START, NULL);
        lv_obj_add_event_cb(lv_screen_active(), screen_event_cb, LV_EVENT_SCREEN_UNLOAD_START, NULL);
        lv_obj_add_event_cb(lv_screen_active(), screen_event_cb, LV_EVENT_SCREEN_UNLOADED, NULL);

        lv_obj_add_event_cb(screen, screen_event_cb, LV_EVENT_SCREEN_LOAD_START, NULL);
        lv_obj_add_event_cb(screen, screen_event_cb, LV_EVENT_SCREEN_LOADED, NULL);
        lv_screen_load(screen);

        /* Событие экрана дисплея вызывалось только один раз*/
        TEST_ASSERT_EQUAL(display_screen_delete_screen_event_count, 1);
        /* Оба события выгрузки отправляются на предыдущий экран.
         * Нет события загрузки для нового экрана, поскольку экран был удален во время события отображения. */
        TEST_ASSERT_EQUAL(screen_event_count, 2);

        TEST_ASSERT_NULL(lv_screen_active());

        display_screen_delete_screen_event_count = screen_event_count = 0;
        lv_display_delete(display);
    }

    {
        lv_display_t * display = lv_display_create(100, 100);
        lv_display_set_default(display);
        lv_obj_t * screen = lv_obj_create(NULL);
        lv_display_add_event_cb(display, display_delete_screen_event_cb, LV_EVENT_SCREEN_LOADED, NULL);

        lv_obj_add_event_cb(lv_screen_active(), screen_event_cb, LV_EVENT_SCREEN_UNLOAD_START, NULL);
        lv_obj_add_event_cb(lv_screen_active(), screen_event_cb, LV_EVENT_SCREEN_UNLOADED, NULL);
        lv_obj_add_event_cb(screen, screen_event_cb, LV_EVENT_SCREEN_LOAD_START, NULL);
        lv_obj_add_event_cb(screen, screen_event_cb, LV_EVENT_SCREEN_LOADED, NULL);
        lv_screen_load(screen);

        /* Событие экрана дисплея вызывалось только один раз*/
        TEST_ASSERT_EQUAL(display_screen_delete_screen_event_count, 1);
        /* Оба события выгрузки отправляются на предыдущий экран.
         * загруженное событие не отправляется на новый экран, поскольку экран был удален во время события отображения */
        TEST_ASSERT_EQUAL(screen_event_count, 3);

        TEST_ASSERT_NULL(lv_screen_active());

        display_screen_delete_screen_event_count = screen_event_count = 0;
        lv_display_delete(display);
    }
    {
        lv_display_t * display = lv_display_create(100, 100);
        lv_display_set_default(display);
        lv_obj_t * screen = lv_obj_create(NULL);
        lv_display_add_event_cb(display, display_delete_screen_event_cb, LV_EVENT_SCREEN_UNLOADED, NULL);
        lv_obj_add_event_cb(lv_screen_active(), screen_event_cb, LV_EVENT_SCREEN_UNLOAD_START, NULL);
        lv_obj_add_event_cb(lv_screen_active(), screen_event_cb, LV_EVENT_SCREEN_UNLOADED, NULL);

        lv_obj_add_event_cb(screen, screen_event_cb, LV_EVENT_SCREEN_LOAD_START, NULL);
        lv_obj_add_event_cb(screen, screen_event_cb, LV_EVENT_SCREEN_LOADED, NULL);
        lv_screen_load(screen);

        /* Событие экрана дисплея вызывалось только один раз*/
        TEST_ASSERT_EQUAL(display_screen_delete_screen_event_count, 1);
        /* выгруженное событие не отправляется на предыдущий экран, поскольку экран удаляется во время события отображения
         * Оба события загрузки для нового экрана отправляются */
        TEST_ASSERT_EQUAL(screen_event_count, 3);

        TEST_ASSERT_EQUAL(screen, lv_screen_active());

        display_screen_delete_screen_event_count = screen_event_count = 0;
        lv_display_delete(display);
    }

}

static void unloaded_event_cb(lv_event_t * e)
{
    lv_obj_delete(lv_event_get_target_obj(e));
}

static lv_obj_t * screen_create(void)
{
    lv_obj_t * screen = lv_obj_create(NULL);
    return screen;
}

void test_screen_mix_event_and_manual_creation(void)
{
    size_t free_mem = lv_test_get_free_mem();

    lv_obj_t * screen = lv_obj_create(NULL);
    lv_obj_t * screen_create_trigger = lv_obj_create(screen);
    lv_obj_add_event_cb(screen, unloaded_event_cb, LV_EVENT_SCREEN_UNLOADED, NULL);
    lv_obj_add_screen_create_event(screen_create_trigger, LV_EVENT_CLICKED, screen_create, LV_SCREEN_LOAD_ANIM_NONE, 0, 0);

    /* Загрузите новый экран, нажав кнопку «Создать». Текущий экран удаляется в нашем выгруженном событии cb*/
    lv_obj_send_event(screen_create_trigger, LV_EVENT_CLICKED, NULL);

    /* Загрузка экрана вручную с автоматическим удалением, установленным на`true`, не должна приводить к двойному освобождению экрана. */
    lv_screen_load_anim(screen, LV_SCREEN_LOAD_ANIM_NONE, 0, 0, true);
    lv_obj_delete(lv_screen_active());

    TEST_ASSERT_MEM_LEAK_LESS_THAN(free_mem, 32);
}

#endif
