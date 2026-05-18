#if LV_BUILD_TEST
#  include "../../lvgl_private.h"
#  include "../lvgl.h"

#  include "unity/unity.h"

static lv_obj_t * active_screen = NULL;

void setUp(void)
{
    active_screen = lv_screen_active();
}

void tearDown(void)
{
    lv_obj_clean(active_screen);
}

static void simple_style(lv_obj_t * obj)
{
    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_margin_all(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_outline_pad(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_outline_width(obj, 0, LV_PART_MAIN);
}

static lv_obj_t * create_row(lv_flex_align_t main_place, int n_children)
{
    lv_obj_t * row = lv_obj_create(lv_screen_active());
    lv_obj_set_size(row, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, main_place, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    simple_style(row);
    for(int i = 0; i < n_children; i++) {
        lv_obj_t * child = lv_obj_create(row);
        lv_obj_set_size(child, 40, 40);
        lv_obj_set_style_bg_color(child, lv_palette_main(LV_PALETTE_BLUE), 0);
        simple_style(child);
    }
    return row;
}

void test_align(void)
{
    create_row(LV_FLEX_ALIGN_CENTER, 1);
    TEST_ASSERT_EQUAL_SCREENSHOT("align_flex_center1.png");
    lv_obj_clean(active_screen);

    create_row(LV_FLEX_ALIGN_CENTER, 2);
    TEST_ASSERT_EQUAL_SCREENSHOT("align_flex_center2.png");
    lv_obj_clean(active_screen);

    create_row(LV_FLEX_ALIGN_SPACE_EVENLY, 1);
    TEST_ASSERT_EQUAL_SCREENSHOT("align_flex_space_evenly1.png");
    lv_obj_clean(active_screen);

    create_row(LV_FLEX_ALIGN_SPACE_EVENLY, 2);
    TEST_ASSERT_EQUAL_SCREENSHOT("align_flex_space_evenly2.png");
    lv_obj_clean(active_screen);

    create_row(LV_FLEX_ALIGN_SPACE_AROUND, 1);
    TEST_ASSERT_EQUAL_SCREENSHOT("align_flex_space_around1.png");
    lv_obj_clean(active_screen);

    create_row(LV_FLEX_ALIGN_SPACE_AROUND, 2);
    TEST_ASSERT_EQUAL_SCREENSHOT("align_flex_space_around2.png");
    lv_obj_clean(active_screen);

    create_row(LV_FLEX_ALIGN_SPACE_BETWEEN, 1);
    TEST_ASSERT_EQUAL_SCREENSHOT("align_flex_space_between1.png");
    lv_obj_clean(active_screen);

    create_row(LV_FLEX_ALIGN_SPACE_BETWEEN, 2);
    TEST_ASSERT_EQUAL_SCREENSHOT("align_flex_space_between2.png");
    lv_obj_clean(active_screen);
}

/*См. https://github.com/lvgl/lvgl/issues/7035.*/
void test_wrap_grow_min_width(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_SPACE_EVENLY);

    for(int i = 0; i < 14; i++) {
        lv_obj_t * obj = lv_obj_create(lv_screen_active());
        lv_obj_set_style_min_width(obj, 100, 0);
        lv_obj_set_style_pad_all(obj, 0, 0);
        lv_obj_set_height(obj, 70 + (i % 3) * 20);

        uint8_t grow = (i % 3);
        lv_obj_set_flex_grow(obj, grow);

        if(i == 9) {
            lv_obj_add_flag(obj, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
        }

        lv_obj_t * label = lv_label_create(obj);
        lv_label_set_text_fmt(label, "grow:%d%s", grow, i == 9 ? "\nnew track" : "");
        lv_obj_center(label);
    }
    TEST_ASSERT_EQUAL_SCREENSHOT("flex_wrap_grow_min_width.png");
}

void test_wrap_grow_min_width_content(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_SPACE_EVENLY);

    for(int i = 0; i < 17; i++) {
        lv_obj_t * obj = lv_obj_create(lv_screen_active());
        lv_obj_set_style_min_width(obj, LV_SIZE_CONTENT, 0);
        lv_obj_set_style_pad_all(obj, 0, 0);
        lv_obj_set_height(obj, 70 + (i % 3) * 20);
        lv_obj_set_width(obj, 120); // эту ширину следует игнорировать, если растет != 0

        uint8_t grow = (i % 3);
        lv_obj_set_flex_grow(obj, grow);

        if(i == 7) {
            lv_obj_set_style_min_width(obj, 0, 0);
        }

        if(i == 12) {
            lv_obj_add_flag(obj, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
        }

        lv_obj_t * label = lv_label_create(obj);
        lv_label_set_text_fmt(label, "grow:%d%s", grow, i == 12 ? "\nnew track" : "");
        lv_obj_center(label);
    }
    TEST_ASSERT_EQUAL_SCREENSHOT("flex_wrap_grow_min_width_content.png");
}

void test_wrap_grow_min_width_pct(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_SPACE_EVENLY);

    for(int i = 0; i < 14; i++) {
        lv_obj_t * obj = lv_obj_create(lv_screen_active());
        lv_obj_set_style_min_width(obj, LV_PCT(20), 0);
        lv_obj_set_style_pad_all(obj, 0, 0);
        lv_obj_set_height(obj, 70 + (i % 3) * 20);

        uint8_t grow = (i % 3);
        lv_obj_set_flex_grow(obj, grow);

        if(i == 9) {
            lv_obj_add_flag(obj, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
        }

        lv_obj_t * label = lv_label_create(obj);
        lv_label_set_text_fmt(label, "grow:%d%s", grow, i == 9 ? "\nnew track" : "");
        lv_obj_center(label);
    }
    TEST_ASSERT_EQUAL_SCREENSHOT("flex_wrap_grow_min_width_pct.png");
}

void test_wrap_grow_max_width_content(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_SPACE_EVENLY);

    for(int i = 0; i < 14; i++) {
        lv_obj_t * obj = lv_obj_create(lv_screen_active());
        lv_obj_set_style_max_width(obj, LV_SIZE_CONTENT, 0);
        lv_obj_set_style_pad_all(obj, 0, 0);
        lv_obj_set_height(obj, 70 + (i % 3) * 20);

        uint8_t grow = (i % 3);
        lv_obj_set_flex_grow(obj, grow);

        if(i == 9) {
            lv_obj_add_flag(obj, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
        }

        lv_obj_t * label = lv_label_create(obj);
        lv_label_set_text_fmt(label, "grow:%d%s", grow, i == 9 ? "\nnew track" : "");
        lv_obj_center(label);
    }
    TEST_ASSERT_EQUAL_SCREENSHOT("flex_wrap_grow_max_width_content.png");
}

void test_wrap_grow_max_width_pct(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_SPACE_EVENLY);

    for(int i = 0; i < 14; i++) {
        lv_obj_t * obj = lv_obj_create(lv_screen_active());
        lv_obj_set_style_max_width(obj, LV_PCT(20), 0);
        lv_obj_set_style_pad_all(obj, 0, 0);
        lv_obj_set_height(obj, 70 + (i % 3) * 20);

        uint8_t grow = (i % 3);
        lv_obj_set_flex_grow(obj, grow);

        if(i == 9) {
            lv_obj_add_flag(obj, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
        }

        lv_obj_t * label = lv_label_create(obj);
        lv_label_set_text_fmt(label, "grow:%d%s", grow, i == 9 ? "\nnew track" : "");
        lv_obj_center(label);
    }
    TEST_ASSERT_EQUAL_SCREENSHOT("flex_wrap_grow_max_width_pct.png");
}
static lv_obj_t * create_col_wrap(size_t count)
{
    lv_obj_t * col = lv_obj_create(lv_screen_active());
    lv_obj_set_flex_flow(col, LV_FLEX_FLOW_COLUMN_WRAP);
    lv_obj_set_flex_align(col, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_SPACE_EVENLY);

    for(size_t i = 0; i < count; i++) {
        lv_obj_t * obj = lv_obj_create(col);
        lv_obj_set_style_pad_all(obj, 0, 0);
        lv_obj_set_height(obj, 70 + (i % 3) * 20);
        lv_obj_set_width(obj, 100);
        lv_obj_set_style_bg_color(obj, lv_palette_main(LV_PALETTE_BLUE), 0);

        lv_obj_t * label = lv_label_create(obj);
        lv_label_set_text_fmt(label, "%lu", (unsigned long)i);
        lv_obj_center(label);
    }

    return col;
}

void test_col_wrap_grow_size_content(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_SPACE_EVENLY);

    lv_obj_t * obj = create_col_wrap(12);
    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_PCT(100));

    obj = create_col_wrap(12);
    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_PCT(100)); // `LV_SIZE_CONTENT` следует игнорировать, поскольку установлен гибкий рост.
    lv_obj_set_flex_grow(obj, 1);

    TEST_ASSERT_EQUAL_SCREENSHOT("flex_col_wrap_grow_size.png");
}

void test_col_wrap_grow_size_content_2(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_SPACE_EVENLY);

    lv_obj_t * obj = create_col_wrap(12);
    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(50));

    obj = create_col_wrap(12);
    lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT); // `LV_SIZE_CONTENT` следует игнорировать, поскольку установлен гибкий рост.
    lv_obj_set_flex_grow(obj, 1);

    TEST_ASSERT_EQUAL_SCREENSHOT("flex_col_wrap_grow_size_2.png");
}

static lv_obj_t * create_row_wrap(size_t count)
{
    lv_obj_t * row = lv_obj_create(lv_screen_active());
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_SPACE_EVENLY);

    for(size_t i = 0; i < count; i++) {
        lv_obj_t * obj = lv_obj_create(row);
        lv_obj_set_style_pad_all(obj, 0, 0);
        lv_obj_set_height(obj, 50 + (i % 3) * 20);
        lv_obj_set_width(obj, 100);
        lv_obj_set_style_bg_color(obj, lv_palette_main(LV_PALETTE_BLUE), 0);

        lv_obj_t * label = lv_label_create(obj);
        lv_label_set_text_fmt(label, "%lu", (unsigned long)i);
        lv_obj_center(label);
    }

    return row;
}

void test_row_wrap_grow_size_content(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_SPACE_EVENLY);

    lv_obj_t * obj = create_row_wrap(12);
    lv_obj_set_size(obj, LV_PCT(50), LV_PCT(100));

    obj = create_row_wrap(12);
    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_PCT(100)); // `LV_SIZE_CONTENT` следует игнорировать, поскольку установлен гибкий рост.
    lv_obj_set_flex_grow(obj, 1);

    TEST_ASSERT_EQUAL_SCREENSHOT("flex_row_wrap_grow_size.png");
}

void test_row_wrap_grow_size_content_2(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_SPACE_EVENLY);

    lv_obj_t * obj = create_row_wrap(12);
    lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);

    obj = create_row_wrap(12);
    lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT); // `LV_SIZE_CONTENT` следует игнорировать, поскольку установлен гибкий рост.
    lv_obj_set_flex_grow(obj, 1);

    TEST_ASSERT_EQUAL_SCREENSHOT("flex_row_wrap_grow_size_2.png");
}
/**
 * Когда для контейнера установлено значение LV_SIZE_CONTENT в гибком направлении, а элемент настроен на рост,
 * размер выращенного предмета должен быть равен минимальному размеру предмета
 */
void test_col_grow_size_content(void)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_name(cont, "cont");
    lv_obj_set_size(cont, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_bg_color(cont, lv_color_hex(0xff0000), 0);
    lv_obj_set_style_bg_opa(cont, LV_OPA_COVER, 0);

    const int32_t pad_all = lv_obj_get_style_pad_top(cont, 0);
    const int32_t pad_gap = lv_obj_get_style_pad_row(cont, 0);
    const int32_t space_start = lv_obj_get_style_space_top(cont, LV_PART_MAIN);
    const int32_t space_end = lv_obj_get_style_space_bottom(cont, LV_PART_MAIN);

    /* Избыточно, но гарантирует, что все отступы будут такими, как ожидалось, если тема устанавливает разные верхние и нижние отступы. */
    lv_obj_set_style_pad_all(cont, pad_all, 0);
    lv_obj_set_style_pad_gap(cont, pad_gap, 0);

    lv_obj_t * header = lv_label_create(cont);
    lv_obj_set_name(header, "header");
    lv_label_set_text(header, "header");

    lv_obj_t * item = lv_obj_create(cont);
    lv_obj_set_name(item, "item");
    lv_obj_set_width(item, LV_PCT(100));
    lv_obj_set_flex_grow(item, 1);
    lv_obj_set_style_bg_color(item, lv_color_hex(0x00ff00), 0);
    lv_obj_set_style_bg_opa(item, LV_OPA_COVER, 0);

    lv_obj_t * footer = lv_label_create(cont);
    lv_obj_set_name(footer, "footer");
    lv_label_set_text(footer, "footer");

    lv_obj_update_layout(cont);
    const int32_t header_height = lv_obj_get_height(header);
    const int32_t footer_height = lv_obj_get_height(footer);

    int32_t item_min_height = 0;
    int32_t cont_min_height = 0;
    int32_t item_max_height = LV_COORD_MAX;
    int32_t cont_max_height = LV_COORD_MAX;

    /* 2 * pad_gap, потому что в продолжении 3 объекта */
    const int32_t fixed_size = space_start + header_height + 2 * pad_gap + footer_height + space_end;

    TEST_ASSERT_GREATER_THAN_INT32(0, fixed_size);
    TEST_ASSERT_EQUAL(fixed_size, lv_obj_get_height(cont));
    TEST_ASSERT_EQUAL(0, lv_obj_get_height(item));

    item_min_height = 200;
    lv_obj_set_style_min_height(item, item_min_height, LV_PART_MAIN);

    lv_obj_update_layout(cont);
    TEST_ASSERT_EQUAL(item_min_height + fixed_size, lv_obj_get_height(cont));
    TEST_ASSERT_EQUAL(item_min_height, lv_obj_get_height(item));

    /* Минимальный размер объекта должен «переопределять» высоту `LV_SIZE_CONTENT`, чтобы элемент был виден и увеличивался до
     * заполнить пространство */
    item_min_height = 0;
    cont_min_height = 400;
    lv_obj_set_style_min_height(cont, cont_min_height, LV_PART_MAIN);
    lv_obj_set_style_min_height(item, item_min_height, LV_PART_MAIN);

    lv_obj_update_layout(cont);
    TEST_ASSERT_EQUAL(cont_min_height, lv_obj_get_height(cont));
    TEST_ASSERT_EQUAL(cont_min_height - fixed_size, lv_obj_get_height(item));

    /* Минимальный размер объекта должен «переопределять» высоту `LV_SIZE_CONTENT`, чтобы элемент был виден и увеличивался до
     * заполнить пространство до максимального размера */
    item_max_height = 200;
    lv_obj_set_style_max_height(item, item_max_height, LV_PART_MAIN);

    lv_obj_update_layout(cont);
    TEST_ASSERT_LESS_THAN_INT32_MESSAGE(
        cont_min_height - fixed_size,
        item_max_height,
        "For this test to function, `item_max_height` must be reduced otherwise the item size will not be clamped");
    TEST_ASSERT_EQUAL(cont_min_height, lv_obj_get_height(cont));
    TEST_ASSERT_EQUAL(item_max_height, lv_obj_get_height(item));

    /* высота элемента должна быть равна 0, поскольку минимальный размер не установлен для элемента или продолжения, cont не должен быть максимальным размером, поскольку максимальный размер равен
     * больше, чем содержание */
    cont_min_height = 0;
    cont_max_height = 500;
    item_max_height = 0;
    TEST_ASSERT_GREATER_THAN_INT32_MESSAGE(
        fixed_size,
        cont_max_height,
        "For this test to function, `cont_max_height` must be increased otherwise the cont size will be clamped");
    lv_obj_set_style_min_height(cont, cont_min_height, LV_PART_MAIN);
    lv_obj_set_style_max_height(item, item_max_height, LV_PART_MAIN);
    lv_obj_set_style_max_height(cont, cont_max_height, LV_PART_MAIN);

    lv_obj_update_layout(cont);
    TEST_ASSERT_EQUAL(0, lv_obj_get_height(item));
    TEST_ASSERT_EQUAL(fixed_size, lv_obj_get_height(cont));

    /* высота элемента должна быть равна 0, поскольку минимальный размер не установлен для элемента или продолжения, cont должен быть максимальным размером, поскольку максимальный размер равен
     * меньше, чем содержание */
    item_max_height = 0;
    cont_max_height = 3 * fixed_size / 4;
    lv_obj_set_style_max_height(item, item_max_height, LV_PART_MAIN);
    lv_obj_set_style_max_height(cont, cont_max_height, LV_PART_MAIN);

    lv_obj_update_layout(cont);
    TEST_ASSERT_EQUAL(0, lv_obj_get_height(item));
    TEST_ASSERT_EQUAL(cont_max_height, lv_obj_get_height(cont));

    /* элемент должен иметь минимальную высоту (50), а cont будет максимальной высоты, поэтому cont должен быть прокручиваемым */
    item_min_height = 50;
    lv_obj_set_style_min_height(item, item_min_height, LV_PART_MAIN);

    lv_obj_update_layout(cont);
    TEST_ASSERT_EQUAL(item_min_height, lv_obj_get_height(item));
    TEST_ASSERT_EQUAL(cont_max_height, lv_obj_get_height(cont));

    /* здесь следует игнорировать максимальную высоту cont */
    cont_max_height = LV_PCT(70);
    TEST_ASSERT_GREATER_THAN_INT32_MESSAGE(fixed_size + item_min_height,
                                           cont_max_height,
                                           "For this test to function, `cont_max_height` must be increased otherwise "
                                           "the item size will be unintentionally clamped");
    lv_obj_set_style_max_height(cont, cont_max_height, LV_PART_MAIN);

    lv_obj_update_layout(cont);
    TEST_ASSERT_EQUAL(item_min_height, lv_obj_get_height(item));
    TEST_ASSERT_EQUAL(fixed_size + item_min_height, lv_obj_get_height(cont));
}

/**
 * Когда для контейнера установлено значение LV_SIZE_CONTENT в гибком направлении, а элемент настроен на рост,
 * размер выращенного предмета должен быть равен минимальному размеру предмета
 */
void test_row_grow_size_content(void)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_name(cont, "cont");
    lv_obj_set_size(cont, LV_SIZE_CONTENT, LV_PCT(100));
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_bg_color(cont, lv_color_hex(0xff0000), 0);
    lv_obj_set_style_bg_opa(cont, LV_OPA_COVER, 0);

    const int32_t pad_all = lv_obj_get_style_pad_left(cont, 0);
    const int32_t pad_gap = lv_obj_get_style_pad_row(cont, 0);
    const int32_t space_start = lv_obj_get_style_space_left(cont, LV_PART_MAIN);
    const int32_t space_end = lv_obj_get_style_space_right(cont, LV_PART_MAIN);

    /* Избыточно, но гарантирует, что все отступы будут такими, как ожидалось, если тема устанавливает разные верхние и нижние отступы. */
    lv_obj_set_style_pad_all(cont, pad_all, 0);
    lv_obj_set_style_pad_gap(cont, pad_gap, 0);

    lv_obj_t * left = lv_label_create(cont);
    lv_obj_set_name(left, "left");
    lv_label_set_text(left, "left");

    lv_obj_t * item = lv_obj_create(cont);
    lv_obj_set_name(item, "item");
    lv_obj_set_height(item, LV_PCT(100));
    lv_obj_set_flex_grow(item, 1);
    lv_obj_set_style_bg_color(item, lv_color_hex(0x00ff00), 0);
    lv_obj_set_style_bg_opa(item, LV_OPA_COVER, 0);

    lv_obj_t * right = lv_label_create(cont);
    lv_obj_set_name(right, "right");
    lv_label_set_text(right, "right");

    lv_obj_update_layout(cont);
    const int32_t left_width = lv_obj_get_width(left);
    const int32_t right_width = lv_obj_get_width(right);

    int32_t item_min_width = 0;
    int32_t cont_min_width = 0;
    int32_t item_max_width = LV_COORD_MAX;
    int32_t cont_max_width = LV_COORD_MAX;

    /* 2 * pad_gap, потому что в продолжении 3 объекта */
    const int32_t fixed_size = space_start + left_width + 2 * pad_gap + right_width + space_end;

    TEST_ASSERT_GREATER_THAN_INT32(0, fixed_size);
    TEST_ASSERT_EQUAL(fixed_size, lv_obj_get_width(cont));
    TEST_ASSERT_EQUAL(0, lv_obj_get_width(item));

    item_min_width = 200;
    lv_obj_set_style_min_width(item, item_min_width, LV_PART_MAIN);

    lv_obj_update_layout(cont);
    TEST_ASSERT_EQUAL(item_min_width + fixed_size, lv_obj_get_width(cont));
    TEST_ASSERT_EQUAL(item_min_width, lv_obj_get_width(item));

    /* Минимальный размер объекта должен «переопределять» ширину `LV_SIZE_CONTENT`, чтобы элемент был виден и увеличивался до
     * заполнить пространство */
    item_min_width = 0;
    cont_min_width = 400;
    lv_obj_set_style_min_width(item, item_min_width, LV_PART_MAIN);
    lv_obj_set_style_min_width(cont, cont_min_width, LV_PART_MAIN);

    lv_obj_update_layout(cont);
    TEST_ASSERT_EQUAL(cont_min_width, lv_obj_get_width(cont));
    TEST_ASSERT_EQUAL(cont_min_width - fixed_size, lv_obj_get_width(item));

    /* Минимальный размер объекта должен «переопределять» ширину `LV_SIZE_CONTENT`, чтобы элемент был виден и увеличивался до
     * заполнить пространство до максимального размера */
    item_max_width = 200;
    lv_obj_set_style_max_width(item, item_max_width, LV_PART_MAIN);

    lv_obj_update_layout(cont);
    TEST_ASSERT_LESS_THAN_INT32_MESSAGE(
        cont_min_width - fixed_size,
        item_max_width,
        "For this test to function, `item_max_width` must be reduced otherwise the item size will not be clamped");
    TEST_ASSERT_EQUAL(cont_min_width, lv_obj_get_width(cont));
    TEST_ASSERT_EQUAL(item_max_width, lv_obj_get_width(item));

    /* Ширина элемента должна быть равна 0, поскольку минимальный размер не установлен для элемента или продолжения, cont не должен быть максимальным размером, поскольку максимальный размер равен
     * больше, чем содержание */
    cont_min_width = 0;
    cont_max_width = 500;
    item_max_width = 0;
    TEST_ASSERT_GREATER_THAN_INT32_MESSAGE(
        fixed_size,
        cont_max_width,
        "For this test to function, `cont_max_width` must be increased otherwise the cont size will be clamped");
    lv_obj_set_style_min_width(cont, cont_min_width, LV_PART_MAIN);
    lv_obj_set_style_max_width(item, item_max_width, LV_PART_MAIN);
    lv_obj_set_style_max_width(cont, cont_max_width, LV_PART_MAIN);

    lv_obj_update_layout(cont);
    TEST_ASSERT_EQUAL(0, lv_obj_get_width(item));
    TEST_ASSERT_EQUAL(fixed_size, lv_obj_get_width(cont));

    /* Ширина элемента должна быть равна 0, поскольку минимальный размер не установлен для элемента или продолжения, cont должен быть максимальным размером, поскольку максимальный размер равен
     * меньше, чем содержание */
    item_max_width = 0;
    cont_max_width = 3 * fixed_size / 4;
    lv_obj_set_style_max_width(item, item_max_width, LV_PART_MAIN);
    lv_obj_set_style_max_width(cont, cont_max_width, LV_PART_MAIN);

    lv_obj_update_layout(cont);
    TEST_ASSERT_EQUAL(0, lv_obj_get_width(item));
    TEST_ASSERT_EQUAL(cont_max_width, lv_obj_get_width(cont));

    /* элемент должен иметь минимальную ширину (90), а cont - максимальную ширину (70), поэтому cont должен быть прокручиваемым. */
    item_min_width = 50;
    lv_obj_set_style_min_width(item, item_min_width, LV_PART_MAIN);

    lv_obj_update_layout(cont);
    TEST_ASSERT_EQUAL(item_min_width, lv_obj_get_width(item));
    TEST_ASSERT_EQUAL(cont_max_width, lv_obj_get_width(cont));

    /* здесь следует игнорировать максимальную ширину cont */
    cont_max_width = LV_PCT(70);
    TEST_ASSERT_GREATER_THAN_INT32_MESSAGE(fixed_size + item_min_width,
                                           cont_max_width,
                                           "For this test to function, `cont_max_width` must be increased otherwise "
                                           "the item size will be unintentionally clamped");
    lv_obj_set_style_max_width(cont, cont_max_width, LV_PART_MAIN);

    lv_obj_update_layout(cont);
    TEST_ASSERT_EQUAL(item_min_width, lv_obj_get_width(item));
    TEST_ASSERT_EQUAL(fixed_size + item_min_width, lv_obj_get_width(cont));
}

/* Проверьте, работает ли гибкий рост с минимальным размером = LV_SIZE_CONTENT. */
void test_nested_flex_grow_size_content(void)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_name(cont, "cont");
    lv_obj_set_style_min_width(cont, LV_SIZE_CONTENT, LV_PART_MAIN);
    lv_obj_set_size(cont, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_bg_color(cont, lv_color_hex(0xff0000), 0);
    lv_obj_set_style_bg_opa(cont, LV_OPA_COVER, 0);

    const int32_t pad_all = lv_obj_get_style_pad_left(cont, 0);
    const int32_t pad_gap = lv_obj_get_style_pad_row(cont, 0);
    const int32_t space_start = lv_obj_get_style_space_left(cont, LV_PART_MAIN);
    const int32_t space_end = lv_obj_get_style_space_right(cont, LV_PART_MAIN);

    /* Избыточно, но гарантирует, что все отступы будут такими, как ожидалось, если тема устанавливает разные верхние и нижние отступы. */
    lv_obj_set_style_pad_all(cont, pad_all, 0);
    lv_obj_set_style_pad_gap(cont, pad_gap, 0);

    lv_obj_t * label = lv_label_create(cont);
    lv_obj_set_name(label, "label");
    lv_label_set_text(label, "label");

    lv_obj_t * sub_cont = lv_obj_create(cont);
    lv_obj_set_name(sub_cont, "sub_cont");
    lv_obj_set_height(sub_cont, LV_SIZE_CONTENT);
    lv_obj_set_flex_grow(sub_cont, 1);
    lv_obj_set_flex_flow(sub_cont, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_bg_color(sub_cont, lv_color_hex(0x00ff00), 0);
    lv_obj_set_style_bg_opa(sub_cont, LV_OPA_COVER, 0);

    /* Гарантирует, что все отступы соответствуют ожиданиям, если в теме установлены разные верхние и нижние отступы. */
    lv_obj_set_style_pad_all(sub_cont, pad_all, 0);
    lv_obj_set_style_pad_gap(sub_cont, pad_gap, 0);

    lv_obj_update_layout(cont);
    const int32_t label_width = lv_obj_get_width(label);
    const int32_t cont_width = lv_obj_get_width(cont); /* LV_PCT(100) */
    const int32_t fixed_size = space_start + label_width + pad_gap + space_end;

    /* Проверьте, растет ли подконтент, чтобы заполнить ожидаемое пространство */
    TEST_ASSERT_EQUAL(cont_width - fixed_size, lv_obj_get_width(sub_cont));
    TEST_ASSERT_EQUAL(0,
                      lv_obj_get_height(sub_cont) - (lv_obj_get_style_space_top(sub_cont, LV_PART_MAIN) +
                                                     lv_obj_get_style_space_bottom(sub_cont, LV_PART_MAIN)));

    const size_t items = 5;
    size_t total_item_width = 0;
    for(size_t i = 0; i < items; i++) {
        lv_obj_t * item = lv_label_create(sub_cont);
        lv_obj_set_name(item, "item_#");
        lv_label_set_text_fmt(item, "item_%d", (int)i);
        lv_obj_set_style_text_color(item, lv_color_black(), 0);

        lv_obj_update_layout(item);
        total_item_width += lv_obj_get_width(item);
    }

    lv_obj_set_style_max_width(sub_cont, LV_SIZE_CONTENT, LV_PART_MAIN);

    lv_obj_update_layout(cont);

    /* Вручную проверьте, что дополнительный контейнер закреплен в соответствии с размером содержимого. */
    int32_t content_size = lv_obj_get_style_space_left(sub_cont, LV_PART_MAIN) + total_item_width +
                           (items - 1) * pad_gap + lv_obj_get_style_space_right(sub_cont, LV_PART_MAIN);
    TEST_ASSERT_LESS_THAN_INT32_MESSAGE(cont_width - fixed_size,
                                        content_size,
                                        "For this test to work the content size must be less than the available grow "
                                        "space, reduce the number of items or length of text in the items");
    TEST_ASSERT_EQUAL(content_size, lv_obj_get_width(sub_cont));

    /* Добавьте больше элементов, чтобы убедиться, что при расчете размера контента (который не учитывает перенос, поэтому максимальная ширина
     * должна быть больше, чем доступное пространство для выращивания), ширина не ограничивается */

    const size_t new_items = 20;
    for(size_t i = items; i < new_items; i++) {
        lv_obj_t * item = lv_label_create(sub_cont);
        lv_obj_set_name(item, "item_#");
        lv_label_set_text_fmt(item, "item_%d", (int)i);
        lv_obj_set_style_text_color(item, lv_color_black(), 0);

        lv_obj_update_layout(item);
        total_item_width += lv_obj_get_width(item);
    }

    lv_obj_update_layout(sub_cont);
    int32_t new_content_size = lv_obj_get_style_space_left(sub_cont, LV_PART_MAIN) + total_item_width +
                               (new_items - 1) * pad_gap + lv_obj_get_style_space_right(sub_cont, LV_PART_MAIN);

    TEST_ASSERT_GREATER_THAN_INT32_MESSAGE(
        cont_width - fixed_size,
        new_content_size,
        "For this test to work the content size must be greater than the available grow "
        "space, increase the number of items or length of text in the items");

    TEST_ASSERT_EQUAL(cont_width - fixed_size, lv_obj_get_width(sub_cont));

    /* Ранее это взаимодействие было ошибочным, поскольку размер субконтента оставался прежним.
     * были добавлены новые предметы. Раньше вам приходилось вручную переопределять максимальную ширину, чтобы сбросить ее. */
    lv_obj_set_style_max_width(sub_cont, LV_COORD_MAX, LV_PART_MAIN);
    lv_obj_update_layout(sub_cont);
    lv_obj_set_style_max_width(sub_cont, LV_SIZE_CONTENT, LV_PART_MAIN);

    TEST_ASSERT_EQUAL(cont_width - fixed_size, lv_obj_get_width(sub_cont));
}

static lv_obj_t * cont_row_5_create(void)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_center(cont);

    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);

    for(int i = 0; i < 5; i++) {
        lv_obj_t * btn = lv_button_create(cont);
        lv_obj_set_size(btn, 40, 60);

        lv_obj_t * label = lv_label_create(btn);
        lv_label_set_text_fmt(label, "%d", i);
        lv_obj_center(label);
    }

    return cont;
}

static void hide(lv_obj_t * cont, uint32_t idx)
{
    lv_obj_add_flag(lv_obj_get_child(cont, idx), LV_OBJ_FLAG_HIDDEN);
}

void test_flex_hide_items(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_COLUMN_WRAP);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_SPACE_EVENLY);

    lv_obj_t * cont;
    cont = cont_row_5_create();

    cont = cont_row_5_create();
    hide(cont, 0);

    cont = cont_row_5_create();
    hide(cont, 0);
    hide(cont, 1);

    cont = cont_row_5_create();
    hide(cont, 4);

    cont = cont_row_5_create();
    hide(cont, 3);
    hide(cont, 4);

    cont = cont_row_5_create();
    hide(cont, 0);
    hide(cont, 1);
    hide(cont, 3);
    hide(cont, 4);

    cont = cont_row_5_create();
    hide(cont, 1);
    hide(cont, 3);

    cont = cont_row_5_create();
    hide(cont, 0);
    hide(cont, 4);

    TEST_ASSERT_EQUAL_SCREENSHOT("flex_hide_items.png");
}

void test_flex_use_rtl(void)
{
    lv_obj_t * test = lv_obj_create(lv_screen_active());
    lv_obj_center(test);
    lv_obj_set_width(test, LV_PCT(40));
    lv_obj_set_height(test, LV_SIZE_CONTENT);
    lv_obj_set_style_border_width(test, 2, 0);
    lv_obj_set_style_base_dir(test, LV_BASE_DIR_RTL, 0);

    lv_obj_set_flex_flow(test, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(test, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);

    for(int i = 0; i < 8; i++) {
        lv_obj_t * obj = lv_obj_create(test);
        lv_obj_set_size(obj, 70, 70);
        lv_obj_set_style_border_width(obj, 2, 0);
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("flex_use_rtl.png");
}


void test_flex_grow_on_screen_should_not_crash(void)
{
    lv_obj_set_flex_grow(lv_screen_active(), 1);
    lv_refr_now(NULL);
}

#endif
