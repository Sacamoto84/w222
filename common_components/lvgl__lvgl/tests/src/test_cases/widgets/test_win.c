#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"
#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;
static lv_obj_t * win = NULL;
static lv_obj_t * header = NULL;
static lv_obj_t * content = NULL;

const char * dummy_text = "Hello LVGL!";

void setUp(void)
{
    active_screen = lv_screen_active();
}

void tearDown(void)
{
    lv_obj_clean(active_screen);
}

void test_win_should_have_valid_documented_default_values(void)
{
    // Создайте объект выигрыша и обновите макет.
    win = lv_win_create(active_screen);
    lv_obj_update_layout(win);

    // Убедитесь, что окно создано
    TEST_ASSERT_NOT_NULL(win);

    // Убедитесь, что были созданы правильные дочерние элементы
    TEST_ASSERT_EQUAL(2, lv_obj_get_child_count(win));

    header = lv_win_get_header(win);
    content = lv_win_get_content(win);

    TEST_ASSERT_EQUAL(header, lv_obj_get_child(win, 0));
    TEST_ASSERT_EQUAL(content, lv_obj_get_child(win, 1));

    // Убедитесь, что заголовок имеет правильный размер и пуст.
    TEST_ASSERT_EQUAL(lv_display_get_dpi(lv_obj_get_display(win)) / 2, lv_obj_get_height(header));
    TEST_ASSERT_EQUAL(0, lv_obj_get_child_count(header));

    // Убедитесь, что содержимое пусто
    TEST_ASSERT_EQUAL(0, lv_obj_get_child_count(content));
}

void test_win_add_title_single(void)
{
    // Создайте объект win, получите заголовок и обновите макет.
    win = lv_win_create(active_screen);
    header = lv_win_get_header(win);
    lv_obj_update_layout(win);

    // Добавьте заголовок в окно
    lv_win_add_title(win, dummy_text);

    // Убедитесь, что под командой win не было создано никаких дополнительных дочерних элементов.
    // Вместо этого дочерний элемент должен быть создан под заголовком
    TEST_ASSERT_EQUAL(2, lv_obj_get_child_count(win));
    TEST_ASSERT_EQUAL(1, lv_obj_get_child_count(header));

    // Убедитесь, что заголовок является меткой и создан правильно.
    lv_obj_t * title = lv_obj_get_child(header, 0);
    TEST_ASSERT_EQUAL_STRING(dummy_text, lv_label_get_text(title));
    TEST_ASSERT_EQUAL(1, lv_label_get_long_mode(title));
}

void test_win_add_title_multiple(void)
{
    // Создайте объект win, получите заголовок и обновите макет.
    win = lv_win_create(active_screen);
    header = lv_win_get_header(win);
    lv_obj_update_layout(win);

    // Добавьте два заголовка в окно
    lv_win_add_title(win, dummy_text);
    lv_win_add_title(win, dummy_text);

    // Убедитесь, что под командой win не было создано никаких дополнительных дочерних элементов.
    // Вместо этого дочерний элемент должен быть создан под заголовком
    TEST_ASSERT_EQUAL(2, lv_obj_get_child_count(win));
    TEST_ASSERT_EQUAL(2, lv_obj_get_child_count(header));
}

void test_win_add_button(void)
{
    int win_button_width = 50;

    // Создайте объект win, получите заголовок и обновите макет.
    win = lv_win_create(active_screen);
    header = lv_win_get_header(win);
    lv_obj_update_layout(win);

    // Добавляем кнопку в заголовок окна
    lv_win_add_button(win, LV_SYMBOL_OK, win_button_width);
    lv_obj_update_layout(win);

    // Убедитесь, что под командой win не было создано никаких дополнительных дочерних элементов.
    // Вместо этого дочерний элемент должен быть создан под заголовком
    TEST_ASSERT_EQUAL(2, lv_obj_get_child_count(win));
    TEST_ASSERT_EQUAL(1, lv_obj_get_child_count(header));

    // Убедитесь, что кнопка создана правильно
    lv_obj_t * btn = lv_obj_get_child(header, 0);
    TEST_ASSERT_EQUAL(1, lv_obj_get_child_count(btn));
    TEST_ASSERT_EQUAL(win_button_width, lv_obj_get_width(btn));

    // Убедитесь, что вывод остается визуально последовательным
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/win_01.png");
}

void test_win_add_multiple_elements(void)
{
    lv_obj_t * btn;
    lv_obj_t * title;

    int win_button_width = 50;
    int win_button_close_width = 60;

    // Создайте объект win, получите заголовок и обновите макет.
    win = lv_win_create(active_screen);
    lv_win_add_button(win, LV_SYMBOL_LEFT, win_button_width);
    lv_win_add_title(win, dummy_text);
    lv_win_add_button(win, LV_SYMBOL_RIGHT, win_button_width);
    lv_win_add_button(win, LV_SYMBOL_CLOSE, win_button_close_width);

    header = lv_win_get_header(win);
    lv_obj_update_layout(win);

    // Убедитесь, что под командой win не было создано никаких дополнительных дочерних элементов.
    // Вместо этого дочерний элемент должен быть создан под заголовком
    TEST_ASSERT_EQUAL(2, lv_obj_get_child_count(win));
    TEST_ASSERT_EQUAL(4, lv_obj_get_child_count(header));

    // Убедитесь, что левая кнопка создана правильно.
    btn = lv_obj_get_child(header, 0);
    TEST_ASSERT_EQUAL(1, lv_obj_get_child_count(btn));
    TEST_ASSERT_EQUAL(win_button_width, lv_obj_get_width(btn));

    // Убедитесь, что заголовок является меткой и создан правильно.
    title = lv_obj_get_child(header, 1);
    TEST_ASSERT_EQUAL_STRING(dummy_text, lv_label_get_text(title));
    TEST_ASSERT_EQUAL(1, lv_label_get_long_mode(title));

    // Убедитесь, что правая кнопка создана правильно.
    btn = lv_obj_get_child(header, 2);
    TEST_ASSERT_EQUAL(1, lv_obj_get_child_count(btn));
    TEST_ASSERT_EQUAL(win_button_width, lv_obj_get_width(btn));

    // Убедитесь, что кнопка закрытия создана правильно.
    btn = lv_obj_get_child(header, 3);
    TEST_ASSERT_EQUAL(1, lv_obj_get_child_count(btn));
    TEST_ASSERT_EQUAL(win_button_close_width, lv_obj_get_width(btn));

    // Убедитесь, что вывод остается визуально последовательным
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/win_02.png");
}

#endif
