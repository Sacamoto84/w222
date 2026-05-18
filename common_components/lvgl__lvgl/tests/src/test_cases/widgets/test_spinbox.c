#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;
static lv_obj_t * spinbox_negative_min_range = NULL;
static lv_obj_t * spinbox_zero_min_range = NULL;
static lv_obj_t * spinbox_events = NULL;
static lv_group_t * g = NULL;

static const int32_t SPINBOX_NEGATIVE_MIN_RANGE_VALUE = -11;
static const int32_t SPINBOX_ZERO_MIN_RANGE_VALUE = 0;
static const int32_t SPINBOX_NEGATIVE_MAX_RANGE_VALUE = 12;
static const uint8_t SPINBOX_DECIMAL_POSITION = 1U;

void setUp(void)
{
    active_screen = lv_screen_active();
    spinbox_negative_min_range = lv_spinbox_create(active_screen);
    spinbox_zero_min_range = lv_spinbox_create(active_screen);
    spinbox_events = lv_spinbox_create(active_screen);

    lv_spinbox_set_range(spinbox_negative_min_range, SPINBOX_NEGATIVE_MIN_RANGE_VALUE, SPINBOX_NEGATIVE_MAX_RANGE_VALUE);
    lv_spinbox_set_range(spinbox_zero_min_range, SPINBOX_ZERO_MIN_RANGE_VALUE, SPINBOX_NEGATIVE_MAX_RANGE_VALUE);

    g = lv_group_create();
    lv_indev_set_group(lv_test_indev_get_indev(LV_INDEV_TYPE_ENCODER), g);
}

void tearDown(void)
{
    lv_group_remove_obj(spinbox_events);

    lv_obj_delete(spinbox_negative_min_range);
    lv_obj_delete(spinbox_zero_min_range);
    lv_obj_delete(spinbox_events);

    lv_obj_clean(active_screen);
}

/* Дополнительную информацию см. в выпуске № 3559. */
void test_spinbox_decrement_when_min_range_is_negative(void)
{
    /* Текущее значение счетчика равно 2. */
    const int32_t expected_value = -11;
    lv_spinbox_set_value(spinbox_negative_min_range, 2);

    /* Измените положение курсора счетчика на 10. */
    lv_spinbox_set_cursor_pos(spinbox_negative_min_range, SPINBOX_DECIMAL_POSITION);
    lv_spinbox_decrement(spinbox_negative_min_range);
    lv_spinbox_decrement(spinbox_negative_min_range);

    /* Мы ожидаем, что значение теперь будет -11. */
    int32_t actual_value = lv_spinbox_get_value(spinbox_negative_min_range);

    TEST_ASSERT_EQUAL_INT32(expected_value, actual_value);
}

void test_spinbox_decrement_when_min_range_is_zero(void)
{
    /* Текущее значение счетчика равно 2. */
    const int32_t expected_value = 0;
    lv_spinbox_set_value(spinbox_zero_min_range, 2);

    /* Измените положение курсора счетчика на 10. */
    lv_spinbox_set_cursor_pos(spinbox_zero_min_range, SPINBOX_DECIMAL_POSITION);
    lv_spinbox_decrement(spinbox_zero_min_range);
    lv_spinbox_decrement(spinbox_zero_min_range);

    /* Мы ожидаем, что значение теперь равно 0 */
    int32_t actual_value = lv_spinbox_get_value(spinbox_zero_min_range);

    TEST_ASSERT_EQUAL_INT32(expected_value, actual_value);
}

void test_spinbox_position_selection(void)
{
    /* Шаг подтверждения равен 1 при выборе минимально возможной позиции. */
    lv_spinbox_set_cursor_pos(spinbox_zero_min_range, 0);
    TEST_ASSERT_EQUAL(1, lv_spinbox_get_step(spinbox_zero_min_range));

    /* Другая ветвь в if */
    lv_spinbox_set_cursor_pos(spinbox_zero_min_range, 1);
    TEST_ASSERT_EQUAL(10, lv_spinbox_get_step(spinbox_zero_min_range));

    /* Когда невозможно выбрать указанную позицию */
    lv_obj_t * tmp;
    tmp = lv_spinbox_create(active_screen);
    lv_spinbox_set_range(tmp, 0, 10);
    lv_spinbox_set_cursor_pos(tmp, 2);
    TEST_ASSERT_EQUAL(1, lv_spinbox_get_step(tmp));
    lv_obj_clean(tmp);
}

void test_spinbox_set_range(void)
{
    int32_t range_max = 40;
    int32_t range_min = 20;

    lv_obj_t * tmp;
    tmp = lv_spinbox_create(active_screen);
    lv_spinbox_set_range(tmp, 0, 100);
    lv_spinbox_set_value(tmp, 50);

    /* Значение проверки обновляется, когдаrange_maxменьше. */
    lv_spinbox_set_range(tmp, 0, range_max);

    TEST_ASSERT_EQUAL(range_max, lv_spinbox_get_value(tmp));

    /* Значение проверки обновляется, когдаrange_minбольше. */
    lv_spinbox_set_value(tmp, 5);
    lv_spinbox_set_range(tmp, range_min, range_max);

    TEST_ASSERT_EQUAL(range_min, lv_spinbox_get_value(tmp));

    lv_obj_clean(tmp);
}

void test_spinbox_step_prev(void)
{
    lv_obj_t * tmp = lv_spinbox_create(active_screen);

    /* Когда следующий шаг превышает самый большой диапазон */
    lv_spinbox_set_range(tmp, 0, 5);
    lv_spinbox_step_prev(tmp);
    TEST_ASSERT_EQUAL(1, lv_spinbox_get_step(tmp));

    lv_spinbox_step_next(tmp);
    /* Когда следующий шаг меньше range_max */
    lv_spinbox_set_range(tmp, 0, 20);
    lv_spinbox_step_prev(tmp);
    TEST_ASSERT_EQUAL(10, lv_spinbox_get_step(tmp));

    lv_spinbox_step_next(tmp);
    /* Когда следующий шаг меньше абсурда (range_min) */
    lv_spinbox_set_range(tmp, -25, 5);
    lv_spinbox_step_prev(tmp);
    TEST_ASSERT_EQUAL(10, lv_spinbox_get_step(tmp));

    lv_obj_clean(tmp);
}

void test_spinbox_rollover(void)
{
    lv_obj_t * tmp = lv_spinbox_create(active_screen);

    lv_spinbox_set_rollover(tmp, true);
    TEST_ASSERT_TRUE(lv_spinbox_get_rollover(tmp));

    lv_spinbox_set_rollover(tmp, false);
    TEST_ASSERT_FALSE(lv_spinbox_get_rollover(tmp));

    lv_obj_clean(tmp);
}

void test_spinbox_event_key(void)
{
    /* Spinbox должен увеличить свое значение на единицу после получения события LV_KEY_UP. */
    lv_spinbox_set_value(spinbox_events, 0);
    uint32_t key = LV_KEY_UP;
    lv_obj_send_event(spinbox_events, LV_EVENT_KEY, (void *) &key);

    TEST_ASSERT_EQUAL(1, lv_spinbox_get_value(spinbox_events));

    /* Spinbox должен уменьшить значение единицы после получения события LV_KEY_DOWN. */
    key = LV_KEY_DOWN;
    lv_obj_send_event(spinbox_events, LV_EVENT_KEY, (void *) &key);

    TEST_ASSERT_EQUAL(0, lv_spinbox_get_value(spinbox_events));

    /* Spinbox должен умножить значение шага на 10 после получения события LV_KEY_LEFT. */
    int32_t step = lv_spinbox_get_step(spinbox_events);
    key = LV_KEY_LEFT;
    lv_obj_send_event(spinbox_events, LV_EVENT_KEY, (void *) &key);

    TEST_ASSERT_EQUAL(step * 10, lv_spinbox_get_step(spinbox_events));

    /* Spinbox должен учитывать значение шага 10 после получения события LV_KEY_RIGHT. */
    step = lv_spinbox_get_step(spinbox_events);
    key = LV_KEY_RIGHT;
    lv_obj_send_event(spinbox_events, LV_EVENT_KEY, (void *) &key);

    TEST_ASSERT_EQUAL(step / 10, lv_spinbox_get_step(spinbox_events));
}

void test_spinbox_event_key_encoder_indev_turn_right(void)
{
    /* Группа настройки и инкодер */
    lv_group_add_obj(g, spinbox_events);

    /* Spinbox должен увеличить свое значение на один шаг после получения события LV_KEY_UP. */
    lv_spinbox_set_value(spinbox_events, 0);

    lv_test_encoder_click();
    lv_test_encoder_turn(1);

    TEST_ASSERT_EQUAL(1, lv_spinbox_get_value(spinbox_events));
}

void test_spinbox_event_key_encoder_indev_turn_left(void)
{
    int32_t value = 10;
    /* Группа настройки и инкодер */
    lv_group_add_obj(g, spinbox_events);

    /* Spinbox должен уменьшить свое значение на один шаг после получения события LV_KEY_UP. */
    lv_spinbox_set_value(spinbox_events, value);
    lv_spinbox_set_cursor_pos(spinbox_events, 0);

    lv_test_encoder_click();
    lv_test_encoder_turn(-1);
    TEST_ASSERT_EQUAL(value - 1, lv_spinbox_get_value(spinbox_events));
}

void test_spinbox_event_key_encoder_indev_editing_group(void)
{
    int32_t value = 10;
    /* Группа настройки и инкодер */
    lv_spinbox_set_range(spinbox_events, 0, 20);
    lv_group_add_obj(g, spinbox_events);
    lv_group_set_editing(g, true);

    lv_spinbox_set_value(spinbox_events, value);
    lv_spinbox_set_cursor_pos(spinbox_events, 0);

    lv_test_encoder_click();
    lv_test_encoder_turn(-1);
    TEST_ASSERT_EQUAL(0, lv_spinbox_get_value(spinbox_events));
    /* digit_count равен 5, поэтому мы ожидаем, что он будет находиться в позиции цифрыMSB. */
    TEST_ASSERT_EQUAL(1000, lv_spinbox_get_step(spinbox_events));

    /* Тест сdigit_count== 1 */
    lv_spinbox_set_digit_format(spinbox_events, 1, 2);
    lv_spinbox_set_cursor_pos(spinbox_events, 0);

    lv_test_encoder_click();
    lv_test_encoder_turn(-1);
    TEST_ASSERT_EQUAL(0, lv_spinbox_get_value(spinbox_events));
    /* digit_count равен 1, поэтому мы ожидаем, что окажемся в той же позиции. */
    TEST_ASSERT_EQUAL(1, lv_spinbox_get_step(spinbox_events));
}

void test_spinbox_event_key_encoder_indev_editing_group_left_step_direction(void)
{
    int32_t value = 10;
    /* Группа настройки и инкодер */
    lv_spinbox_set_digit_step_direction(spinbox_events, LV_DIR_LEFT);
    lv_spinbox_set_range(spinbox_events, 0, 20);
    lv_group_add_obj(g, spinbox_events);
    lv_group_set_editing(g, true);

    lv_spinbox_set_value(spinbox_events, value);
    lv_spinbox_set_cursor_pos(spinbox_events, 0);

    lv_test_encoder_click();
    lv_test_encoder_turn(-1);
    TEST_ASSERT_EQUAL(0, lv_spinbox_get_value(spinbox_events));
    /* digit_count равен 5, мы ожидаем, что окажемся в позиции слева */
    TEST_ASSERT_EQUAL(10, lv_spinbox_get_step(spinbox_events));

    /* Тест сdigit_count== 1 */
    lv_spinbox_set_digit_format(spinbox_events, 2, 2);
    lv_spinbox_set_cursor_pos(spinbox_events, 1);

    lv_test_encoder_click();
    lv_test_encoder_turn(-1);
    TEST_ASSERT_EQUAL(0, lv_spinbox_get_value(spinbox_events));
    /* digit_count равен 1, поэтому мы ожидаем, что окажемся в той же позиции. */
    TEST_ASSERT_EQUAL(1, lv_spinbox_get_step(spinbox_events));
}

void test_spinbox_event_release(void)
{
    lv_spinbox_set_value(spinbox_events, 0);
    lv_spinbox_set_digit_format(spinbox_events, 5, 2);

    /* Установить курсор в наименее значащую десятичную цифру */
    lv_spinbox_set_cursor_pos(spinbox_events, 0);
    lv_obj_send_event(spinbox_events, LV_EVENT_RELEASED, NULL);

    TEST_ASSERT_EQUAL(1, lv_spinbox_get_step(spinbox_events));
}

void test_spinbox_zero_crossing(void)
{
    int32_t value = -13;
    /* Группа настройки и инкодер */
    lv_spinbox_set_digit_step_direction(spinbox_events, LV_DIR_LEFT);
    lv_spinbox_set_range(spinbox_events, -20, 20);
    lv_group_add_obj(g, spinbox_events);

    lv_spinbox_set_value(spinbox_events, value);
    lv_spinbox_set_cursor_pos(spinbox_events, 1);

    lv_test_encoder_click();
    lv_test_encoder_turn(1);
    TEST_ASSERT_EQUAL(-3, lv_spinbox_get_value(spinbox_events));

    lv_test_encoder_turn(1);
    TEST_ASSERT_EQUAL(3, lv_spinbox_get_value(spinbox_events));

    lv_test_encoder_turn(1);
    TEST_ASSERT_EQUAL(13, lv_spinbox_get_value(spinbox_events));
}

void test_spinbox_few_digits(void)
{
    lv_spinbox_set_range(spinbox_events, -20000, 20000);
    lv_spinbox_set_value(spinbox_events, 19000);
    lv_spinbox_set_digit_count(spinbox_events, 3);
    lv_obj_t * label = lv_obj_get_child(spinbox_events, 0);
    TEST_ASSERT_EQUAL_STRING("+190", lv_label_get_text(label));
}

void test_spinbox_properties(void)
{
#if LV_USE_OBJ_PROPERTY
    lv_obj_t * spinbox = lv_spinbox_create(lv_screen_active());

    lv_spinbox_set_range(spinbox, -100, 200);
    lv_spinbox_set_value(spinbox, 50);
    lv_spinbox_set_rollover(spinbox, true);
    lv_spinbox_set_digit_count(spinbox, 4);
    lv_spinbox_set_dec_point_pos(spinbox, 1);
    lv_spinbox_set_step(spinbox, 10);
    lv_spinbox_set_digit_step_direction(spinbox, LV_DIR_LEFT);

    lv_property_t prop = { };

    prop.id = LV_PROPERTY_SPINBOX_VALUE;
    TEST_ASSERT_EQUAL_INT(50, lv_obj_get_property(spinbox, prop.id).num);

    prop.id = LV_PROPERTY_SPINBOX_ROLLOVER;
    TEST_ASSERT_TRUE(lv_obj_get_property(spinbox, prop.id).enable);

    prop.id = LV_PROPERTY_SPINBOX_DIGIT_COUNT;
    TEST_ASSERT_EQUAL_INT(4, lv_obj_get_property(spinbox, prop.id).num);

    prop.id = LV_PROPERTY_SPINBOX_DEC_POINT_POS;
    TEST_ASSERT_EQUAL_INT(1, lv_obj_get_property(spinbox, prop.id).num);

    prop.id = LV_PROPERTY_SPINBOX_STEP;
    TEST_ASSERT_EQUAL_INT(10, lv_obj_get_property(spinbox, prop.id).num);

    prop.id = LV_PROPERTY_SPINBOX_MIN_VALUE;
    TEST_ASSERT_EQUAL_INT(-100, lv_obj_get_property(spinbox, prop.id).num);

    prop.id = LV_PROPERTY_SPINBOX_MAX_VALUE;
    TEST_ASSERT_EQUAL_INT(200, lv_obj_get_property(spinbox, prop.id).num);

    prop.id = LV_PROPERTY_SPINBOX_DIGIT_STEP_DIRECTION;
    TEST_ASSERT_EQUAL_INT(LV_DIR_LEFT, lv_obj_get_property(spinbox, prop.id).num);

    /* Наладчик тестов */
    prop.id = LV_PROPERTY_SPINBOX_VALUE;
    prop.num = 75;
    TEST_ASSERT_TRUE(lv_obj_set_property(spinbox, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(75, lv_spinbox_get_value(spinbox));

    lv_obj_delete(spinbox);
#endif
}

#endif

