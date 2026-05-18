#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

static uint32_t event_cnt_pressed;
static uint32_t event_cnt_pressing;
static uint32_t event_cnt_released;
static uint32_t event_cnt_long_pressed;
static uint32_t event_cnt_long_pressed_repeat;
static uint32_t event_cnt_key;
static lv_key_t last_key;

void setUp(void)
{
    /* Функция запускается перед каждым тестом */
    event_cnt_pressed = 0;
    event_cnt_pressing = 0;
    event_cnt_released = 0;
    event_cnt_long_pressed = 0;
    event_cnt_long_pressed_repeat = 0;
    event_cnt_key = 0;
    last_key = 0;
}

void tearDown(void)
{
    /* Функция запускается после каждого теста */
}

static void keypad_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    switch(code) {
        case LV_EVENT_PRESSED:
            event_cnt_pressed++;
            break;
        case LV_EVENT_PRESSING:
            event_cnt_pressing++;
            break;
        case LV_EVENT_RELEASED:
            event_cnt_released++;
            break;
        case LV_EVENT_LONG_PRESSED:
            event_cnt_long_pressed++;
            break;
        case LV_EVENT_LONG_PRESSED_REPEAT:
            event_cnt_long_pressed_repeat++;
            break;
        case LV_EVENT_KEY:
            event_cnt_key++;
            last_key = lv_indev_get_key(lv_indev_active());
            break;
        default:
            break;
    }
}

void test_indev_keypad_no_group_key_event(void)
{
    lv_indev_t * indev = lv_test_indev_get_indev(LV_INDEV_TYPE_KEYPAD);
    TEST_ASSERT_NOT_NULL(indev);
    lv_indev_set_group(indev, NULL);

    /* Добавьте события обратного вызова в indev */
    lv_indev_add_event_cb(indev, keypad_event_cb, LV_EVENT_KEY, NULL);

    /* Нажмите клавишу */
    lv_test_key_hit('x');

    /* Должно быть создано событие KEY. */
    TEST_ASSERT_GREATER_THAN_UINT32(0, event_cnt_key);
    TEST_ASSERT_EQUAL_UINT32('x', last_key);

    /* Очистка */
    lv_indev_remove_event_cb_with_user_data(indev, keypad_event_cb, NULL);
}

void test_indev_keypad_no_group_press_release(void)
{
    lv_indev_t * indev = lv_test_indev_get_indev(LV_INDEV_TYPE_KEYPAD);
    TEST_ASSERT_NOT_NULL(indev);
    lv_indev_set_group(indev, NULL);

    /* Добавить обратные вызовы событий в indev */
    lv_indev_add_event_cb(indev, keypad_event_cb, LV_EVENT_PRESSED, NULL);
    lv_indev_add_event_cb(indev, keypad_event_cb, LV_EVENT_RELEASED, NULL);

    /* Нажмите клавишу (нажмите и отпустите) */
    lv_test_key_hit('a');

    /* Должны генерироваться события PRESSED и RELEASED. */
    TEST_ASSERT_EQUAL_UINT32(1, event_cnt_pressed);
    TEST_ASSERT_EQUAL_UINT32(1, event_cnt_released);

    /* Очистка */
    lv_indev_remove_event_cb_with_user_data(indev, keypad_event_cb, NULL);
}

void test_indev_keypad_no_group_long_press(void)
{
    lv_indev_t * indev = lv_test_indev_get_indev(LV_INDEV_TYPE_KEYPAD);
    TEST_ASSERT_NOT_NULL(indev);
    lv_indev_set_group(indev, NULL);

    /* Добавить обратные вызовы событий в indev */
    lv_indev_add_event_cb(indev, keypad_event_cb, LV_EVENT_PRESSED, NULL);
    lv_indev_add_event_cb(indev, keypad_event_cb, LV_EVENT_LONG_PRESSED, NULL);
    lv_indev_add_event_cb(indev, keypad_event_cb, LV_EVENT_LONG_PRESSED_REPEAT, NULL);
    lv_indev_add_event_cb(indev, keypad_event_cb, LV_EVENT_RELEASED, NULL);

    /* Убедитесь, что мы начинаем в выпущенном состоянии */
    lv_test_key_release();
    lv_test_wait(50);

    /* Нажмите кнопку и удерживайте ее в течение длительного времени (по умолчанию 400 мс). */
    lv_test_key_press('c');

    /* Удерживайте клавишу немного (дольше, чем LV_INDEV_DEF_LONG_PRESS_TIME ) */
    lv_test_wait(450);

    TEST_ASSERT_EQUAL_UINT32(1, event_cnt_pressed);
    TEST_ASSERT_EQUAL_UINT32(1, event_cnt_long_pressed);

    /* Продолжайте удерживать для повтора */
    lv_test_wait(200);

    TEST_ASSERT_GREATER_THAN_UINT32(0, event_cnt_long_pressed_repeat);

    /* Ключ отпускания */
    lv_test_key_release();
    lv_test_wait(50);

    TEST_ASSERT_EQUAL_UINT32(1, event_cnt_released);

    /* Очистка */
    lv_indev_remove_event_cb_with_user_data(indev, keypad_event_cb, NULL);
}

#endif
