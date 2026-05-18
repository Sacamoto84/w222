#if LV_BUILD_TEST == 1
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

static uint32_t observer_called = 0;

void setUp(void)
{
    observer_called = 0;
    /* Функция запускается перед каждым тестом */
}

void tearDown(void)
{
    /* Функция запускается после каждого теста */
    lv_obj_clean(lv_screen_active());
}

static int32_t prev_v;
static int32_t current_v;

static void observer_basic(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(observer);
    LV_UNUSED(subject);
    observer_called++;
}

static void observer_int(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(observer);
    prev_v = lv_subject_get_previous_int(subject);
    current_v = lv_subject_get_int(subject);
}

void test_observer_add_remove(void)
{
    static lv_subject_t subject;
    lv_subject_init_int(&subject, 5);

    lv_observer_t * observer =
        lv_subject_add_observer(&subject, observer_int, NULL);

    current_v = 0;
    lv_subject_set_int(&subject, 10);
    TEST_ASSERT_EQUAL(10, lv_subject_get_int(&subject));
    TEST_ASSERT_EQUAL(10, current_v);

    lv_observer_remove(observer);
    lv_subject_set_int(&subject, 15);
    TEST_ASSERT_EQUAL(15, lv_subject_get_int(&subject));
    TEST_ASSERT_EQUAL(10, current_v); /*Наблюдатель cb не вызывается*/

    static lv_subject_t uninitialized_subject;
    observer = lv_subject_add_observer(&uninitialized_subject, observer_int,
                                       NULL);
    TEST_ASSERT_EQUAL_PTR(NULL, observer); /*Наблюдатель должен быть NULL.*/
}

void test_object_observer_add_remove(void)
{

    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    static lv_subject_t subject;
    lv_subject_init_int(&subject, 1);

    lv_observer_t * observer = lv_obj_bind_flag_if_eq(obj, &subject, LV_OBJ_FLAG_HIDDEN, 5);

    TEST_ASSERT_EQUAL(false, lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN));
    lv_subject_set_int(&subject, 5);
    TEST_ASSERT_EQUAL(true, lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN));
    lv_observer_remove(observer);
    lv_subject_set_int(&subject, 1);

    /* Это не должно обновляться */
    TEST_ASSERT_EQUAL(true, lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN));
    lv_obj_delete(obj);
    /* Мы не должны здесь разбиться */
}

static lv_event_dsc_t * get_event_delete_from_obj(lv_obj_t * obj)
{

    /* Событие удаления — это обратный вызов события, использующий наблюдателя в качестве пользовательских данных. */
    uint32_t event_cnt = lv_event_get_count(&obj->spec_attr->event_list);
    for(uint32_t i = 0; i < event_cnt; i++) {
        lv_event_dsc_t * event = lv_obj_get_event_dsc(obj, i);
        TEST_ASSERT_NOT_NULL(event);
        if(event->filter == LV_EVENT_DELETE) {
            return event;
        }
    }
    return NULL;
}
void test_obj_remove_from_subject_removes_delete_event(void)
{

    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    static lv_subject_t subject;
    lv_subject_init_int(&subject, 1);
    (void)lv_subject_add_observer_obj(&subject, observer_basic, obj, NULL);

    {
        /*
         * Мы ожидаем, что удаление события будет добавлено к объекту, что позволит наблюдателю
         * удаляться при удалении объекта
         */
        TEST_ASSERT_NOT_NULL(obj->spec_attr);
        TEST_ASSERT_EQUAL(lv_event_get_count(&obj->spec_attr->event_list), 1);
        lv_event_dsc_t * delete_event = get_event_delete_from_obj(obj);
        TEST_ASSERT_NOT_NULL(delete_event);
    }
    {
        /* Удаление объекта из темы должно удалить запись о событии удаления. */
        lv_obj_remove_from_subject(obj, &subject);
        lv_event_dsc_t * delete_event  = get_event_delete_from_obj(obj);
        TEST_ASSERT_NULL(delete_event);
    }

}

void test_observer_remove_removes_obj_callback(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    static lv_subject_t subject;
    lv_subject_init_int(&subject, 1);
    lv_observer_t * observer = lv_subject_add_observer_obj(&subject, observer_basic, obj, NULL);

    {
        /*
         * Мы ожидаем, что удаление события будет добавлено к объекту, что позволит наблюдателю
         * удаляться при удалении объекта
         */
        TEST_ASSERT_NOT_NULL(obj->spec_attr);
        TEST_ASSERT_EQUAL(lv_event_get_count(&obj->spec_attr->event_list), 1);
        lv_event_dsc_t * delete_event = get_event_delete_from_obj(obj);
        TEST_ASSERT_NOT_NULL(delete_event);
    }
    {
        /* Удаление наблюдателя, связанного с объектом, должно удалить запись о событии удаления. */
        lv_observer_remove(observer);
        lv_event_dsc_t * delete_event  = get_event_delete_from_obj(obj);
        TEST_ASSERT_NULL(delete_event);
    }
}

void test_observer_int(void)
{
    static lv_subject_t subject;
    lv_subject_init_int(&subject, 5);
    lv_observer_t * basic_observer =
        lv_subject_add_observer(&subject, observer_basic, NULL);

    TEST_ASSERT_EQUAL(5, lv_subject_get_int(&subject));
    TEST_ASSERT_EQUAL(5, lv_subject_get_previous_int(&subject));
    TEST_ASSERT_EQUAL(1, observer_called);

    lv_subject_set_int(&subject, 10);
    TEST_ASSERT_EQUAL(10, lv_subject_get_int(&subject));
    TEST_ASSERT_EQUAL(5, lv_subject_get_previous_int(&subject));
    TEST_ASSERT_EQUAL(2, observer_called);

    lv_subject_set_int(&subject, 15);
    TEST_ASSERT_EQUAL(15, lv_subject_get_int(&subject));
    TEST_ASSERT_EQUAL(10, lv_subject_get_previous_int(&subject));
    TEST_ASSERT_EQUAL(3, observer_called);

    /* Наблюдатель не должен вызываться, если значение одинаковое */
    lv_subject_set_int(&subject, 15);
    TEST_ASSERT_EQUAL(15, lv_subject_get_int(&subject));
    TEST_ASSERT_EQUAL(15, lv_subject_get_previous_int(&subject));
    TEST_ASSERT_EQUAL(3, observer_called);

    /*Игнорировать неправильные типы*/
    lv_subject_set_pointer(&subject, NULL);
    TEST_ASSERT_EQUAL(15, lv_subject_get_int(&subject));
    TEST_ASSERT_EQUAL(15, lv_subject_get_previous_int(&subject));
    TEST_ASSERT_EQUAL(3, observer_called);

    lv_subject_set_color(&subject, lv_color_black());
    TEST_ASSERT_EQUAL(15, lv_subject_get_int(&subject));
    TEST_ASSERT_EQUAL(15, lv_subject_get_previous_int(&subject));
    TEST_ASSERT_EQUAL(3, observer_called);

    lv_subject_copy_string(&subject, "hello");
    TEST_ASSERT_EQUAL(15, lv_subject_get_int(&subject));
    TEST_ASSERT_EQUAL(15, lv_subject_get_previous_int(&subject));
    TEST_ASSERT_EQUAL(3, observer_called);

    lv_observer_remove(basic_observer);
}

void test_observer_float(void)
{
    static lv_subject_t subject;
    lv_subject_init_float(&subject, 5.25);
    lv_observer_t * basic_observer =
        lv_subject_add_observer(&subject, observer_basic, NULL);

    TEST_ASSERT_EQUAL_FLOAT(5.25, lv_subject_get_float(&subject));
    TEST_ASSERT_EQUAL_FLOAT(5.25, lv_subject_get_previous_float(&subject));
    TEST_ASSERT_EQUAL(1, observer_called);

    lv_subject_set_float(&subject, 10.5);
    TEST_ASSERT_EQUAL_FLOAT(10.5, lv_subject_get_float(&subject));
    TEST_ASSERT_EQUAL_FLOAT(5.25, lv_subject_get_previous_float(&subject));
    TEST_ASSERT_EQUAL(2, observer_called);

    lv_subject_set_float(&subject, 15.75);
    TEST_ASSERT_EQUAL_FLOAT(15.75, lv_subject_get_float(&subject));
    TEST_ASSERT_EQUAL_FLOAT(10.5, lv_subject_get_previous_float(&subject));
    TEST_ASSERT_EQUAL(3, observer_called);

    /* Наблюдатель не должен вызываться, если значение одинаковое */
    lv_subject_set_float(&subject, 15.75);
    TEST_ASSERT_EQUAL_FLOAT(15.75, lv_subject_get_float(&subject));
    TEST_ASSERT_EQUAL_FLOAT(15.75, lv_subject_get_previous_float(&subject));
    TEST_ASSERT_EQUAL(3, observer_called);

    /*Игнорировать неправильные типы*/
    lv_subject_set_pointer(&subject, NULL);
    TEST_ASSERT_EQUAL_FLOAT(15.75, lv_subject_get_float(&subject));
    TEST_ASSERT_EQUAL_FLOAT(15.75, lv_subject_get_previous_float(&subject));
    TEST_ASSERT_EQUAL(3, observer_called);

    lv_subject_set_color(&subject, lv_color_black());
    TEST_ASSERT_EQUAL_FLOAT(15.75, lv_subject_get_float(&subject));
    TEST_ASSERT_EQUAL_FLOAT(15.75, lv_subject_get_previous_float(&subject));
    TEST_ASSERT_EQUAL(3, observer_called);

    lv_subject_copy_string(&subject, "hello");
    TEST_ASSERT_EQUAL_FLOAT(15.75, lv_subject_get_float(&subject));
    TEST_ASSERT_EQUAL_FLOAT(15.75, lv_subject_get_previous_float(&subject));
    TEST_ASSERT_EQUAL(3, observer_called);

    lv_observer_remove(basic_observer);
}

void test_observer_string(void)
{
    char buf_current[32];
    char buf_previous[32];
    lv_subject_t subject;
    lv_subject_init_string(&subject, buf_current, buf_previous,
                           sizeof(buf_current), "hello");

    lv_observer_t * basic_observer =
        lv_subject_add_observer(&subject, observer_basic, NULL);

    TEST_ASSERT_EQUAL_STRING("hello", lv_subject_get_string(&subject));
    TEST_ASSERT_EQUAL_STRING("hello",
                             lv_subject_get_previous_string(&subject));
    TEST_ASSERT_EQUAL(1, observer_called);

    lv_subject_copy_string(&subject, "my name is John");
    TEST_ASSERT_EQUAL_STRING("my name is John",
                             lv_subject_get_string(&subject));
    TEST_ASSERT_EQUAL_STRING("hello",
                             lv_subject_get_previous_string(&subject));
    TEST_ASSERT_EQUAL(2, observer_called);

    lv_subject_copy_string(&subject, "how are you?");
    TEST_ASSERT_EQUAL_STRING("how are you?",
                             lv_subject_get_string(&subject));
    TEST_ASSERT_EQUAL_STRING("my name is John",
                             lv_subject_get_previous_string(&subject));
    TEST_ASSERT_EQUAL(3, observer_called);

    /* Наблюдатель не должен вызываться с тем же значением */
    lv_subject_copy_string(&subject, "how are you?");
    TEST_ASSERT_EQUAL_STRING("how are you?",
                             lv_subject_get_string(&subject));
    TEST_ASSERT_EQUAL_STRING("how are you?",
                             lv_subject_get_previous_string(&subject));
    TEST_ASSERT_EQUAL(3, observer_called);

    lv_subject_snprintf(&subject, "I ate %d pizzas", 10);
    TEST_ASSERT_EQUAL_STRING("I ate 10 pizzas",
                             lv_subject_get_string(&subject));
    TEST_ASSERT_EQUAL_STRING("how are you?",
                             lv_subject_get_previous_string(&subject));
    TEST_ASSERT_EQUAL(4, observer_called);

    lv_subject_snprintf(&subject, "%d: %s", 1, "Coding is fun !");
    TEST_ASSERT_EQUAL_STRING("1: Coding is fun !",
                             lv_subject_get_string(&subject));
    TEST_ASSERT_EQUAL_STRING("I ate 10 pizzas",
                             lv_subject_get_previous_string(&subject));
    TEST_ASSERT_EQUAL(5, observer_called);

    /* Наблюдатель не должен вызываться с тем же значением */
    lv_subject_snprintf(&subject, "%d: %s", 1, "Coding is fun !");
    TEST_ASSERT_EQUAL_STRING("1: Coding is fun !",
                             lv_subject_get_string(&subject));
    TEST_ASSERT_EQUAL_STRING("1: Coding is fun !",
                             lv_subject_get_previous_string(&subject));
    TEST_ASSERT_EQUAL(5, observer_called);

    /*Вырезать длинный текст*/
    lv_subject_copy_string(
        &subject,
        "text to be clipped to 32 chars.this should be clipped");
    TEST_ASSERT_EQUAL_STRING("text to be clipped to 32 chars.",
                             lv_subject_get_string(&subject));
    TEST_ASSERT_EQUAL_STRING("1: Coding is fun !",
                             lv_subject_get_previous_string(&subject));
    TEST_ASSERT_EQUAL(6, observer_called);

    /*Проверьте, правильно ли обрезана предыдущая строка*/
    lv_subject_copy_string(&subject, "a");
    TEST_ASSERT_EQUAL_STRING("a", lv_subject_get_string(&subject));
    TEST_ASSERT_EQUAL_STRING("text to be clipped to 32 chars.",
                             lv_subject_get_previous_string(&subject));
    TEST_ASSERT_EQUAL(7, observer_called);

    /*Игнорировать неправильные типы*/
    lv_subject_set_pointer(&subject, NULL);
    TEST_ASSERT_EQUAL_STRING("a", lv_subject_get_string(&subject));
    TEST_ASSERT_EQUAL_STRING("text to be clipped to 32 chars.",
                             lv_subject_get_previous_string(&subject));
    TEST_ASSERT_EQUAL(7, observer_called);

    lv_subject_set_color(&subject, lv_color_black());
    TEST_ASSERT_EQUAL_STRING("a", lv_subject_get_string(&subject));
    TEST_ASSERT_EQUAL_STRING("text to be clipped to 32 chars.",
                             lv_subject_get_previous_string(&subject));
    TEST_ASSERT_EQUAL(7, observer_called);

    lv_subject_set_int(&subject, 10);
    TEST_ASSERT_EQUAL_STRING("a", lv_subject_get_string(&subject));
    TEST_ASSERT_EQUAL_STRING("text to be clipped to 32 chars.",
                             lv_subject_get_previous_string(&subject));
    TEST_ASSERT_EQUAL(7, observer_called);
    lv_observer_remove(basic_observer);
}

void test_observer_pointer(void)
{
    static int32_t a[3] = { 0 };
    static lv_subject_t subject;
    lv_subject_init_pointer(&subject, &a[0]);

    lv_observer_t * basic_observer =
        lv_subject_add_observer(&subject, observer_basic, NULL);

    TEST_ASSERT_EQUAL(1, observer_called);

    TEST_ASSERT_EQUAL_PTR(&a[0], lv_subject_get_pointer(&subject));
    TEST_ASSERT_EQUAL_PTR(&a[0], lv_subject_get_previous_pointer(&subject));

    lv_subject_set_pointer(&subject, &a[1]);
    TEST_ASSERT_EQUAL_PTR(&a[1], lv_subject_get_pointer(&subject));
    TEST_ASSERT_EQUAL_PTR(&a[0], lv_subject_get_previous_pointer(&subject));
    TEST_ASSERT_EQUAL(2, observer_called);

    lv_subject_set_pointer(&subject, &a[2]);
    TEST_ASSERT_EQUAL_PTR(&a[2], lv_subject_get_pointer(&subject));
    TEST_ASSERT_EQUAL_PTR(&a[1], lv_subject_get_previous_pointer(&subject));
    TEST_ASSERT_EQUAL(3, observer_called);

    /*
     * Даже если указатель тот же, наблюдатель все равно должен вызываться, поскольку мы не должны предполагать, что
     * что указывает указатель
     */
    lv_subject_set_pointer(&subject, &a[2]);
    TEST_ASSERT_EQUAL_PTR(&a[2], lv_subject_get_pointer(&subject));
    TEST_ASSERT_EQUAL_PTR(&a[2], lv_subject_get_previous_pointer(&subject));
    TEST_ASSERT_EQUAL(4, observer_called);

    /*Игнорировать неправильные типы*/
    lv_subject_set_int(&subject, 10);
    TEST_ASSERT_EQUAL_PTR(&a[2], lv_subject_get_pointer(&subject));
    TEST_ASSERT_EQUAL_PTR(&a[2], lv_subject_get_previous_pointer(&subject));
    TEST_ASSERT_EQUAL(4, observer_called);

    lv_subject_set_color(&subject, lv_color_black());
    TEST_ASSERT_EQUAL_PTR(&a[2], lv_subject_get_pointer(&subject));
    TEST_ASSERT_EQUAL_PTR(&a[2], lv_subject_get_previous_pointer(&subject));
    TEST_ASSERT_EQUAL(4, observer_called);

    lv_subject_copy_string(&subject, "hello");
    TEST_ASSERT_EQUAL_PTR(&a[2], lv_subject_get_pointer(&subject));
    TEST_ASSERT_EQUAL_PTR(&a[2], lv_subject_get_previous_pointer(&subject));
    TEST_ASSERT_EQUAL(4, observer_called);
    lv_observer_remove(basic_observer);
}

void test_observer_color(void)
{
    static lv_subject_t subject;

    lv_subject_init_color(&subject, lv_color_hex3(0x123));
    lv_observer_t * basic_observer =
        lv_subject_add_observer(&subject, observer_basic, NULL);
    TEST_ASSERT_EQUAL_COLOR(lv_color_hex3(0x123),
                            lv_subject_get_color(&subject));
    TEST_ASSERT_EQUAL_COLOR(lv_color_hex3(0x123),
                            lv_subject_get_previous_color(&subject));
    TEST_ASSERT_EQUAL(1, observer_called);

    lv_subject_set_color(&subject, lv_color_hex3(0x456));
    TEST_ASSERT_EQUAL_COLOR(lv_color_hex3(0x456),
                            lv_subject_get_color(&subject));
    TEST_ASSERT_EQUAL_COLOR(lv_color_hex3(0x123),
                            lv_subject_get_previous_color(&subject));
    TEST_ASSERT_EQUAL(2, observer_called);

    lv_subject_set_color(&subject, lv_color_hex3(0xabc));
    TEST_ASSERT_EQUAL_COLOR(lv_color_hex3(0xabc),
                            lv_subject_get_color(&subject));
    TEST_ASSERT_EQUAL_COLOR(lv_color_hex3(0x456),
                            lv_subject_get_previous_color(&subject));
    TEST_ASSERT_EQUAL(3, observer_called);

    /* Наблюдатель не должен вызываться, если значение одинаковое */
    lv_subject_set_color(&subject, lv_color_hex3(0xabc));
    TEST_ASSERT_EQUAL_COLOR(lv_color_hex3(0xabc),
                            lv_subject_get_color(&subject));
    TEST_ASSERT_EQUAL_COLOR(lv_color_hex3(0xabc),
                            lv_subject_get_previous_color(&subject));
    TEST_ASSERT_EQUAL(3, observer_called);

    /*Игнорировать неправильные типы*/
    lv_subject_set_pointer(&subject, NULL);
    TEST_ASSERT_EQUAL_COLOR(lv_color_hex3(0xabc),
                            lv_subject_get_color(&subject));
    TEST_ASSERT_EQUAL_COLOR(lv_color_hex3(0xabc),
                            lv_subject_get_previous_color(&subject));

    lv_subject_set_int(&subject, 10);
    TEST_ASSERT_EQUAL_COLOR(lv_color_hex3(0xabc),
                            lv_subject_get_color(&subject));
    TEST_ASSERT_EQUAL_COLOR(lv_color_hex3(0xabc),
                            lv_subject_get_previous_color(&subject));

    lv_subject_copy_string(&subject, "hello");
    TEST_ASSERT_EQUAL_COLOR(lv_color_hex3(0xabc),
                            lv_subject_get_color(&subject));
    TEST_ASSERT_EQUAL_COLOR(lv_color_hex3(0xabc),
                            lv_subject_get_previous_color(&subject));
    lv_observer_remove(basic_observer);
}

static int32_t group_observer_called;

static void group_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(observer);
    LV_UNUSED(subject);
    group_observer_called++;
}

void test_observer_group(void)
{
    static lv_subject_t subject_main;
    static lv_subject_t subject_sub1;
    static lv_subject_t subject_sub2;
    static lv_subject_t * subject_list[2] = { &subject_sub1, &subject_sub2 };

    lv_subject_init_int(&subject_sub1, 1);
    lv_subject_init_int(&subject_sub2, 2);

    lv_subject_init_group(&subject_main, subject_list, 2);
    TEST_ASSERT_EQUAL_PTR(&subject_sub1,
                          lv_subject_get_group_element(&subject_main, 0));
    TEST_ASSERT_EQUAL_PTR(&subject_sub2,
                          lv_subject_get_group_element(&subject_main, 1));
    TEST_ASSERT_EQUAL_PTR(NULL,
                          lv_subject_get_group_element(&subject_main, 2));
    TEST_ASSERT_EQUAL_PTR(NULL, lv_subject_get_group_element(&subject_main,
                                                             1000));

    group_observer_called = 0;
    lv_subject_add_observer(&subject_main, group_observer_cb, NULL);
    TEST_ASSERT_EQUAL(1, group_observer_called);

    lv_subject_set_int(&subject_sub1, 10);
    TEST_ASSERT_EQUAL(2, group_observer_called);

    lv_subject_set_int(&subject_sub2, 20);
    TEST_ASSERT_EQUAL(3, group_observer_called);
}

void test_observer_obj_flag_invalid_subject(void)
{
    typedef lv_observer_t * (*lv_obj_bind_flag_fn)(
        lv_obj_t *, lv_subject_t *, lv_obj_flag_t, int32_t);
    static const lv_obj_bind_flag_fn fns[] = {
        lv_obj_bind_flag_if_eq, lv_obj_bind_flag_if_not_eq,
        lv_obj_bind_flag_if_ge, lv_obj_bind_flag_if_gt,
        lv_obj_bind_flag_if_lt, lv_obj_bind_flag_if_le,
    };
    static lv_subject_t invalid_subjects[4];

    static char buf1[30];
    static char buf2[30];
    lv_obj_t * obj = lv_obj_create(lv_screen_active());

    const size_t fns_size = sizeof(fns) / sizeof(fns[0]);
    const size_t subjects_size =
        sizeof(invalid_subjects) / sizeof(invalid_subjects[0]);

    /* Можно привязать только к int */
    lv_subject_init_pointer(&invalid_subjects[0], NULL);
    lv_subject_init_string(&invalid_subjects[1], buf1, buf2, 30, "test");
    lv_subject_init_color(&invalid_subjects[2], (lv_color_t) {
        0, 0, 0
    });
    lv_subject_init_group(&invalid_subjects[3], NULL, 0);

    for(size_t i = 0; i < fns_size; ++i) {
        for(size_t j = 0; j < subjects_size; ++j) {
            TEST_ASSERT_EQUAL_PTR(NULL,
                                  fns[i](obj, &invalid_subjects[j],
                                         LV_OBJ_FLAG_HIDDEN, 5));
        }
    }
}
void test_observer_obj_flag_eq(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    static lv_subject_t subject;
    lv_subject_init_int(&subject, 1);

    lv_obj_bind_flag_if_eq(obj, &subject, LV_OBJ_FLAG_HIDDEN, 5);
    /*Следует применять немедленно*/
    TEST_ASSERT_EQUAL(false, lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN));

    lv_obj_bind_flag_if_not_eq(obj, &subject, LV_OBJ_FLAG_CHECKABLE, 10);
    /*Следует применять немедленно*/
    TEST_ASSERT_EQUAL(false, lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN));
    TEST_ASSERT_EQUAL(true, lv_obj_has_flag(obj, LV_OBJ_FLAG_CHECKABLE));

    lv_subject_set_int(&subject, 5);
    TEST_ASSERT_EQUAL(true, lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN));
    TEST_ASSERT_EQUAL(true, lv_obj_has_flag(obj, LV_OBJ_FLAG_CHECKABLE));

    lv_subject_set_int(&subject, 10);
    TEST_ASSERT_EQUAL(false, lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN));
    TEST_ASSERT_EQUAL(false, lv_obj_has_flag(obj, LV_OBJ_FLAG_CHECKABLE));
}

void test_observer_obj_flag_ge(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    static lv_subject_t subject;
    lv_subject_init_int(&subject, 1);

    lv_obj_bind_flag_if_ge(obj, &subject, LV_OBJ_FLAG_HIDDEN, 5);
    /*Следует применять немедленно*/
    TEST_ASSERT_EQUAL(false, lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN));

    lv_subject_set_int(&subject, 5);
    TEST_ASSERT_EQUAL(true, lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN));

    lv_subject_set_int(&subject, 4);
    TEST_ASSERT_EQUAL(false, lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN));

    lv_subject_set_int(&subject, 6);
    TEST_ASSERT_EQUAL(true, lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN));
}

void test_observer_obj_flag_gt(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    static lv_subject_t subject;
    lv_subject_init_int(&subject, 1);

    lv_obj_bind_flag_if_gt(obj, &subject, LV_OBJ_FLAG_HIDDEN, 5);
    /*Следует применять немедленно*/
    TEST_ASSERT_EQUAL(false, lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN));

    lv_subject_set_int(&subject, 5);
    TEST_ASSERT_EQUAL(false, lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN));

    lv_subject_set_int(&subject, 6);
    TEST_ASSERT_EQUAL(true, lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN));

    lv_subject_set_int(&subject, 4);
    TEST_ASSERT_EQUAL(false, lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN));
}

void test_observer_obj_flag_le(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    static lv_subject_t subject;
    lv_subject_init_int(&subject, 7);

    lv_obj_bind_flag_if_le(obj, &subject, LV_OBJ_FLAG_HIDDEN, 5);
    /*Следует применять немедленно*/
    TEST_ASSERT_EQUAL(false, lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN));

    lv_subject_set_int(&subject, 5);
    TEST_ASSERT_EQUAL(true, lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN));

    lv_subject_set_int(&subject, 6);
    TEST_ASSERT_EQUAL(false, lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN));

    lv_subject_set_int(&subject, 4);
    TEST_ASSERT_EQUAL(true, lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN));
}

void test_observer_obj_flag_lt(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    static lv_subject_t subject;
    lv_subject_init_int(&subject, 7);

    lv_obj_bind_flag_if_lt(obj, &subject, LV_OBJ_FLAG_HIDDEN, 5);
    /*Следует применять немедленно*/
    TEST_ASSERT_EQUAL(false, lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN));

    lv_subject_set_int(&subject, 4);
    TEST_ASSERT_EQUAL(true, lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN));

    lv_subject_set_int(&subject, 5);
    TEST_ASSERT_EQUAL(false, lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN));

    lv_subject_set_int(&subject, 3);
    TEST_ASSERT_EQUAL(true, lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN));
}

void test_observer_obj_state_invalid_subject(void)
{
    typedef lv_observer_t * (*lv_obj_bind_state_fn)(
        lv_obj_t *, lv_subject_t *, lv_state_t, int32_t);

    static const lv_obj_bind_state_fn fns[] = {
        lv_obj_bind_state_if_eq, lv_obj_bind_state_if_not_eq,
        lv_obj_bind_state_if_ge, lv_obj_bind_state_if_gt,
        lv_obj_bind_state_if_lt, lv_obj_bind_state_if_le,
    };
    static lv_subject_t invalid_subjects[4];

    static char buf1[30];
    static char buf2[30];
    lv_obj_t * obj = lv_obj_create(lv_screen_active());

    const size_t fns_size = sizeof(fns) / sizeof(fns[0]);
    const size_t subjects_size =
        sizeof(invalid_subjects) / sizeof(invalid_subjects[0]);

    /* Можно привязать только к int */
    lv_subject_init_pointer(&invalid_subjects[0], NULL);
    lv_subject_init_string(&invalid_subjects[1], buf1, buf2, 30, "test");
    lv_subject_init_color(&invalid_subjects[2], (lv_color_t) {
        0, 0, 0
    });
    lv_subject_init_group(&invalid_subjects[3],
                          NULL, 0);

    for(size_t i = 0; i < fns_size; ++i) {
        for(size_t j = 0; j < subjects_size; ++j) {
            TEST_ASSERT_EQUAL_PTR(
                NULL, fns[i](obj, &invalid_subjects[j], 0, 5));
        }
    }
}

void test_observer_obj_state_eq(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    static lv_subject_t subject;
    lv_subject_init_int(&subject, 1);

    lv_obj_bind_state_if_eq(obj, &subject, LV_STATE_CHECKED, 5);
    /*Следует применять немедленно*/
    TEST_ASSERT_EQUAL(false, lv_obj_has_state(obj, LV_STATE_CHECKED));

    lv_obj_bind_state_if_not_eq(obj, &subject, LV_STATE_DISABLED, 10);
    /*Следует применять немедленно*/
    TEST_ASSERT_EQUAL(false, lv_obj_has_state(obj, LV_STATE_CHECKED));
    TEST_ASSERT_EQUAL(true, lv_obj_has_state(obj, LV_STATE_DISABLED));

    lv_subject_set_int(&subject, 5);
    TEST_ASSERT_EQUAL(true, lv_obj_has_state(obj, LV_STATE_CHECKED));
    TEST_ASSERT_EQUAL(true, lv_obj_has_state(obj, LV_STATE_DISABLED));

    lv_subject_set_int(&subject, 10);
    TEST_ASSERT_EQUAL(false, lv_obj_has_state(obj, LV_STATE_CHECKED));
    TEST_ASSERT_EQUAL(false, lv_obj_has_state(obj, LV_STATE_DISABLED));
}

void test_observer_obj_state_gt(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    static lv_subject_t subject;
    lv_subject_init_int(&subject, 1);

    lv_obj_bind_state_if_gt(obj, &subject, LV_STATE_CHECKED, 5);
    /*Следует применять немедленно*/
    TEST_ASSERT_EQUAL(false, lv_obj_has_state(obj, LV_STATE_CHECKED));

    lv_subject_set_int(&subject, 6);
    TEST_ASSERT_EQUAL(true, lv_obj_has_state(obj, LV_STATE_CHECKED));

    lv_subject_set_int(&subject, 5);
    TEST_ASSERT_EQUAL(false, lv_obj_has_state(obj, LV_STATE_CHECKED));

    lv_subject_set_int(&subject, 7);
    TEST_ASSERT_EQUAL(true, lv_obj_has_state(obj, LV_STATE_CHECKED));
}

void test_observer_obj_state_ge(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    static lv_subject_t subject;
    lv_subject_init_int(&subject, 1);

    lv_obj_bind_state_if_ge(obj, &subject, LV_STATE_CHECKED, 5);
    /*Следует применять немедленно*/
    TEST_ASSERT_EQUAL(false, lv_obj_has_state(obj, LV_STATE_CHECKED));

    lv_subject_set_int(&subject, 6);
    TEST_ASSERT_EQUAL(true, lv_obj_has_state(obj, LV_STATE_CHECKED));

    lv_subject_set_int(&subject, 4);
    TEST_ASSERT_EQUAL(false, lv_obj_has_state(obj, LV_STATE_CHECKED));

    lv_subject_set_int(&subject, 5);
    TEST_ASSERT_EQUAL(true, lv_obj_has_state(obj, LV_STATE_CHECKED));
}

void test_observer_obj_state_le(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    static lv_subject_t subject;
    lv_subject_init_int(&subject, 1);

    lv_obj_bind_state_if_le(obj, &subject, LV_STATE_CHECKED, 5);
    /*Следует применять немедленно*/
    TEST_ASSERT_EQUAL(true, lv_obj_has_state(obj, LV_STATE_CHECKED));

    lv_subject_set_int(&subject, 6);
    TEST_ASSERT_EQUAL(false, lv_obj_has_state(obj, LV_STATE_CHECKED));

    lv_subject_set_int(&subject, 4);
    TEST_ASSERT_EQUAL(true, lv_obj_has_state(obj, LV_STATE_CHECKED));
}

void test_observer_obj_state_lt(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    static lv_subject_t subject;
    lv_subject_init_int(&subject, 1);

    lv_obj_bind_state_if_lt(obj, &subject, LV_STATE_CHECKED, 5);
    /*Следует применять немедленно*/
    TEST_ASSERT_EQUAL(true, lv_obj_has_state(obj, LV_STATE_CHECKED));

    lv_subject_set_int(&subject, 5);
    TEST_ASSERT_EQUAL(false, lv_obj_has_state(obj, LV_STATE_CHECKED));

    lv_subject_set_int(&subject, 4);
    TEST_ASSERT_EQUAL(true, lv_obj_has_state(obj, LV_STATE_CHECKED));
}

void test_observer_button_checked(void)
{
    lv_obj_t * obj = lv_button_create(lv_screen_active());
    lv_obj_set_size(obj, 100, 100);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_update_layout(obj);

    /*Может привязываться только к int*/
    static lv_subject_t subject_wrong;
    lv_subject_init_pointer(&subject_wrong, NULL);
    lv_observer_t * observer = lv_obj_bind_checked(obj, &subject_wrong);
    TEST_ASSERT_EQUAL_PTR(NULL, observer);

    static lv_subject_t subject;
    lv_subject_init_int(&subject, 1);
    lv_obj_bind_checked(obj, &subject);

    TEST_ASSERT_EQUAL(true, lv_obj_has_state(obj, LV_STATE_CHECKED));

    lv_subject_set_int(&subject, 0);
    TEST_ASSERT_EQUAL(false, lv_obj_has_state(obj, LV_STATE_CHECKED));

    lv_test_mouse_click_at(10, 10);
    TEST_ASSERT_EQUAL(true, lv_obj_has_state(obj, LV_STATE_CHECKED));
    TEST_ASSERT_EQUAL(1, lv_subject_get_int(&subject));
}

void test_observer_label_text_normal(void)
{
    lv_obj_t * obj = lv_label_create(lv_screen_active());

    lv_observer_t * observer;

    /*Не могу привязать цвет*/
    static lv_subject_t subject_color;
    lv_subject_init_color(&subject_color, lv_color_black());
    observer = lv_label_bind_text(obj, &subject_color, NULL);
    TEST_ASSERT_EQUAL_PTR(NULL, observer);

    /*Свяжите его с помощью "%d", если передан NULL.*/
    static lv_subject_t subject_int;
    lv_subject_init_int(&subject_int, 10);
    observer = lv_label_bind_text(obj, &subject_int, NULL);
    TEST_ASSERT_EQUAL_STRING("10", lv_label_get_text(obj));

    /*Свяжите его с помощью «%0.1f», если передан NULL.*/
    static lv_subject_t subject_float;
    lv_subject_init_float(&subject_float, 10.5);
    observer = lv_label_bind_text(obj, &subject_float, NULL);
    TEST_ASSERT_EQUAL_STRING("10.5", lv_label_get_text(obj));

    /*Свяжите его с помощью «%0.1f», если передан NULL.*/
    lv_subject_set_float(&subject_float, 81.5);
    observer = lv_label_bind_text(obj, &subject_float, "Value: %0.2f");
    TEST_ASSERT_EQUAL_STRING("Value: 81.50", lv_label_get_text(obj));

    /*Привязать к строке*/
    static char buf[32];
    static lv_subject_t subject_string;
    lv_subject_init_string(&subject_string, buf, NULL, 32, "hello");
    lv_label_bind_text(obj, &subject_string, NULL);
    TEST_ASSERT_EQUAL_STRING("hello", lv_label_get_text(obj));

    lv_subject_copy_string(&subject_string, "world");
    TEST_ASSERT_EQUAL_STRING("world", lv_label_get_text(obj));

    /*Удалить ярлык с темы*/
    lv_obj_remove_from_subject(obj, &subject_string);
    lv_subject_copy_string(&subject_string, "nothing");
    TEST_ASSERT_EQUAL_STRING("world", lv_label_get_text(obj));

    /*Привязка к указателю*/
    static lv_subject_t subject_pointer;
    lv_subject_init_pointer(&subject_pointer, "HELLO");
    lv_label_bind_text(obj, &subject_pointer, NULL);
    TEST_ASSERT_EQUAL_STRING("HELLO", lv_label_get_text(obj));

    lv_subject_set_pointer(&subject_pointer, "WORLD");
    TEST_ASSERT_EQUAL_STRING("WORLD", lv_label_get_text(obj));

    /*Удалить ярлык с темы*/
    lv_obj_remove_from_subject(obj, &subject_pointer);
    lv_subject_copy_string(&subject_pointer, "NOTHING");
    TEST_ASSERT_EQUAL_STRING("WORLD", lv_label_get_text(obj));
}

void test_observer_label_text_formatted(void)
{
    lv_obj_t * obj = lv_label_create(lv_screen_active());

    lv_observer_t * observer;

    /*Не могу привязать цвет*/
    static lv_subject_t subject_color;
    lv_subject_init_color(&subject_color, lv_color_black());
    observer = lv_label_bind_text(obj, &subject_color, NULL);
    TEST_ASSERT_EQUAL_PTR(NULL, observer);

    /*Привязка к int*/
    static lv_subject_t subject_int;
    lv_subject_init_int(&subject_int, 10);
    lv_label_bind_text(obj, &subject_int, "value: %d");
    TEST_ASSERT_EQUAL_STRING("value: 10", lv_label_get_text(obj));

    lv_subject_set_int(&subject_int, -20);
    TEST_ASSERT_EQUAL_STRING("value: -20", lv_label_get_text(obj));

    /*Удалить ярлык с темы*/
    lv_obj_remove_from_subject(obj, &subject_int);
    lv_subject_set_int(&subject_int, 100);
    TEST_ASSERT_EQUAL_STRING("value: -20", lv_label_get_text(obj));

    /*Привязать к строке*/
    static char buf[32];
    static lv_subject_t subject_string;
    lv_subject_init_string(&subject_string, buf, NULL, 32, "hello");
    lv_label_bind_text(obj, &subject_string, "text: %s");
    TEST_ASSERT_EQUAL_STRING("text: hello", lv_label_get_text(obj));

    lv_subject_copy_string(&subject_string, "world");
    TEST_ASSERT_EQUAL_STRING("text: world", lv_label_get_text(obj));

    /*Удалить ярлык с темы*/
    lv_obj_remove_from_subject(obj, &subject_string);
    lv_subject_copy_string(&subject_string, "nothing");
    TEST_ASSERT_EQUAL_STRING("text: world", lv_label_get_text(obj));

    /*Привязка к указателю*/
    static lv_subject_t subject_pointer;
    lv_subject_init_pointer(&subject_pointer, "HELLO");
    lv_label_bind_text(obj, &subject_pointer, "pointer: %s");
    TEST_ASSERT_EQUAL_STRING("pointer: HELLO", lv_label_get_text(obj));

    lv_subject_set_pointer(&subject_pointer, "WORLD");
    TEST_ASSERT_EQUAL_STRING("pointer: WORLD", lv_label_get_text(obj));

    /*Удалить ярлык с темы*/
    lv_obj_remove_from_subject(obj, &subject_pointer);
    lv_subject_copy_string(&subject_pointer, "NOTHING");
    TEST_ASSERT_EQUAL_STRING("pointer: WORLD", lv_label_get_text(obj));
}

void test_observer_arc_value(void)
{
    lv_obj_t * obj = lv_arc_create(lv_screen_active());

    static lv_subject_t subject;
    lv_subject_init_int(&subject, 30);
    lv_arc_bind_value(obj, &subject);

    TEST_ASSERT_EQUAL(30, lv_arc_get_value(obj));

    lv_subject_set_int(&subject, 40);
    TEST_ASSERT_EQUAL(40, lv_arc_get_value(obj));

    lv_obj_update_layout(obj);
    lv_test_mouse_release();
    lv_test_wait(100);

    lv_test_mouse_move_to(65, 10);
    lv_test_mouse_press();
    lv_test_wait(100);
    lv_test_mouse_release();

    TEST_ASSERT_EQUAL(50, lv_arc_get_value(obj));
    TEST_ASSERT_EQUAL(50, lv_subject_get_int(&subject));
}

void test_observer_slider_value(void)
{
    lv_obj_t * obj = lv_slider_create(lv_screen_active());

    static lv_subject_t subject;
    lv_subject_init_int(&subject, 30);
    lv_slider_bind_value(obj, &subject);

    TEST_ASSERT_EQUAL(30, lv_slider_get_value(obj));

    lv_subject_set_int(&subject, 40);
    TEST_ASSERT_EQUAL(40, lv_slider_get_value(obj));

    lv_obj_update_layout(obj);
    lv_test_mouse_release();
    lv_test_wait(100);

    lv_test_mouse_move_to(65, 10);
    lv_test_mouse_press();
    lv_test_wait(100);

    lv_test_mouse_move_to(75, 10);
    lv_test_mouse_press();
    lv_test_wait(100);
    lv_test_mouse_release();

    TEST_ASSERT_EQUAL(29, lv_slider_get_value(obj));
    TEST_ASSERT_EQUAL(29, lv_subject_get_int(&subject));
}

void test_observer_roller_value(void)
{
    lv_obj_t * obj = lv_roller_create(lv_screen_active());

    static lv_subject_t subject;
    lv_subject_init_int(&subject, 1);
    lv_roller_bind_value(obj, &subject);

    TEST_ASSERT_EQUAL(1, lv_roller_get_selected(obj));

    lv_subject_set_int(&subject, 2);
    TEST_ASSERT_EQUAL(2, lv_roller_get_selected(obj));

    lv_obj_update_layout(obj);
    lv_test_mouse_click_at(30, 10);

    TEST_ASSERT_EQUAL(1, lv_roller_get_selected(obj));
    TEST_ASSERT_EQUAL(1, lv_subject_get_int(&subject));
}

void test_observer_dropdown_value(void)
{
    lv_obj_t * obj = lv_dropdown_create(lv_screen_active());

    static lv_subject_t subject;
    lv_subject_init_int(&subject, 1);
    lv_dropdown_bind_value(obj, &subject);

    TEST_ASSERT_EQUAL(1, lv_dropdown_get_selected(obj));

    lv_subject_set_int(&subject, 2);
    TEST_ASSERT_EQUAL(2, lv_dropdown_get_selected(obj));

    lv_obj_update_layout(obj);
    lv_test_mouse_click_at(30, 10);
    lv_test_mouse_click_at(30, 60);

    TEST_ASSERT_EQUAL(0, lv_dropdown_get_selected(obj));
    TEST_ASSERT_EQUAL(0, lv_subject_get_int(&subject));
}

void test_observer_deinit(void)
{
    static lv_subject_t subject;

    uint32_t mem = lv_test_get_free_mem();
    uint32_t i;
    for(i = 0; i < 64; i++) {
        lv_obj_t * obj1 = lv_slider_create(lv_screen_active());
        lv_obj_t * obj2 = lv_slider_create(lv_screen_active());

        lv_subject_init_int(&subject, 30);
        lv_slider_bind_value(obj1, &subject);
        lv_slider_bind_value(obj2, &subject);
        lv_subject_add_observer(&subject, observer_int, NULL);
        lv_obj_delete(obj1);
        lv_subject_deinit(&subject);
        lv_obj_delete(obj2);
    }

    TEST_ASSERT_MEM_LEAK_LESS_THAN(mem, 32);
}

#endif
