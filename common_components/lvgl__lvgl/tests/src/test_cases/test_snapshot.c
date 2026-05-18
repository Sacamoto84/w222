#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../lvgl_private.h"

#include "unity/unity.h"

#define NUM_SNAPSHOTS 10

/* Общие цветовые форматы, используемые в нескольких тестах */
static const lv_color_format_t supported_color_formats[] = {
    LV_COLOR_FORMAT_A8,
    LV_COLOR_FORMAT_L8,
    LV_COLOR_FORMAT_RGB565,
    LV_COLOR_FORMAT_ARGB8565,
    LV_COLOR_FORMAT_RGB888,
    LV_COLOR_FORMAT_XRGB8888,
    LV_COLOR_FORMAT_ARGB8888,
    LV_COLOR_FORMAT_ARGB2222,
    LV_COLOR_FORMAT_ARGB4444,
    LV_COLOR_FORMAT_ARGB1555
};

#define NUM_SUPPORTED_FORMATS (sizeof(supported_color_formats) / sizeof(supported_color_formats[0]))

void setUp(void)
{
    /* Функция запускается перед каждым тестом */
}

void tearDown(void)
{
    /* Функция запускается после каждого теста */
    lv_obj_clean(lv_screen_active());
}

void test_snapshot_should_not_leak_memory(void)
{
    uint32_t idx = 0;
    size_t initial_available_memory = 0;
    size_t final_available_memory = 0;
    lv_mem_monitor_t monitor;

    lv_draw_buf_t * snapshots[NUM_SNAPSHOTS] = {NULL};

    lv_obj_clean(lv_screen_active());
    lv_obj_clean(lv_layer_top());
    lv_obj_clean(lv_layer_sys());
    lv_obj_clean(lv_layer_bottom());

    lv_mem_monitor(&monitor);
    initial_available_memory = monitor.free_size;

    for(idx = 0; idx < NUM_SNAPSHOTS; idx++) {
        snapshots[idx] = lv_snapshot_take(lv_screen_active(), LV_COLOR_FORMAT_NATIVE_WITH_ALPHA);
        TEST_ASSERT_NOT_NULL(snapshots[idx]);
    }

    for(idx = 0; idx < NUM_SNAPSHOTS; idx++) {
        lv_draw_buf_destroy(snapshots[idx]);
    }

    lv_mem_monitor(&monitor);
    final_available_memory = monitor.free_size;

    TEST_ASSERT_EQUAL(initial_available_memory, final_available_memory);
}

void test_snapshot_with_transform_should_not_leak_memory(void)
{
    uint32_t idx = 0;
    size_t initial_available_memory = 0;
    size_t final_available_memory = 0;
    lv_mem_monitor_t monitor;

    lv_draw_buf_t * snapshots[NUM_SNAPSHOTS] = {NULL};
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_obj_center(label);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_28, 0);
    lv_label_set_text(label, "Wubba lubba dub dub!");
    lv_obj_set_style_transform_rotation(label, 450, 0);
    lv_obj_update_layout(label);

    lv_mem_monitor(&monitor);
    initial_available_memory = monitor.free_size;

    for(idx = 0; idx < NUM_SNAPSHOTS; idx++) {
        snapshots[idx] = lv_snapshot_take(lv_screen_active(), LV_COLOR_FORMAT_NATIVE_WITH_ALPHA);
        TEST_ASSERT_NOT_NULL(snapshots[idx]);
    }

    for(idx = 0; idx < NUM_SNAPSHOTS; idx++) {
        lv_draw_buf_destroy(snapshots[idx]);
    }

    lv_mem_monitor(&monitor);
    final_available_memory = monitor.free_size;
    lv_obj_delete(label);

    TEST_ASSERT_EQUAL(initial_available_memory, final_available_memory);
}

void test_snapshot_take_snapshot_immediately_after_obj_create(void)
{
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_font(label, &lv_font_montserrat_28, 0);
    lv_label_set_text(label, "Wubba lubba dub dub!");

    lv_draw_buf_t * draw_dsc = lv_snapshot_take(label, LV_COLOR_FORMAT_ARGB8888);
    lv_obj_t * img_obj = lv_image_create(lv_screen_active());
    lv_image_set_src(img_obj, draw_dsc);

    lv_obj_delete(label);

    TEST_ASSERT_EQUAL_SCREENSHOT("snapshot_0.png");

    lv_obj_center(img_obj);
    lv_image_set_inner_align(img_obj, LV_IMAGE_ALIGN_CENTER);
    lv_image_set_rotation(img_obj, 450);

    TEST_ASSERT_EQUAL_SCREENSHOT("snapshot_1.png");

    lv_obj_delete(img_obj);
    lv_draw_buf_destroy(draw_dsc);
}

void test_snapshot_take_snapshot_with_transform(void)
{
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_font(label, &lv_font_montserrat_28, 0);
    lv_label_set_text(label, "Wubba lubba dub dub!");
    lv_obj_set_style_transform_rotation(label, 450, 0);

    lv_draw_buf_t * draw_dsc = lv_snapshot_take(lv_screen_active(), LV_COLOR_FORMAT_ARGB8888);

    lv_obj_delete(label);

    lv_obj_t * img_obj = lv_image_create(lv_screen_active());
    lv_image_set_src(img_obj, draw_dsc);

    TEST_ASSERT_EQUAL_SCREENSHOT("snapshot_2.png");

    lv_obj_delete(img_obj);
    lv_draw_buf_destroy(draw_dsc);
}

void test_snapshot_take_snapshot_multiple_widgets(void)
{
    lv_obj_t * obj_1 = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj_1, 200, 200);
    lv_obj_set_style_border_width(obj_1, 0, 0);
    lv_obj_set_style_bg_color(obj_1, lv_color_hex3(0xff0), 0);
    lv_obj_center(obj_1);

    lv_obj_t * top_obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(top_obj, lv_pct(100), lv_pct(100));
    lv_obj_set_style_border_width(top_obj, 0, 0);
    lv_obj_set_style_radius(top_obj, 0, 0);
    lv_obj_set_style_bg_opa(top_obj, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(top_obj, 0, 0);
    lv_obj_center(top_obj);

    lv_obj_t * obj_2 = lv_obj_create(top_obj);
    lv_obj_set_size(obj_2, 100, 100);
    lv_obj_set_style_border_width(obj_2, 0, 0);
    lv_obj_set_style_bg_color(obj_2, lv_color_hex3(0xf00), 0);
    lv_obj_center(obj_2);

    lv_obj_t * obj_3 = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj_3, 100, 100);
    lv_obj_set_style_border_width(obj_3, 0, 0);
    lv_obj_set_style_bg_color(obj_3, lv_color_hex3(0x00f), 0);
    lv_obj_align(obj_3, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t * obj_4 = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj_4, 100, 100);
    lv_obj_set_style_border_width(obj_4, 0, 0);
    lv_obj_set_style_bg_color(obj_4, lv_color_hex3(0x0ff), 0);
    lv_obj_align(obj_4, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_flag(obj_4, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t * obj_5 = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj_5, 100, 100);
    lv_obj_set_style_border_width(obj_5, 0, 0);
    lv_obj_set_style_bg_color(obj_5, lv_color_hex3(0xff0), 0);
    lv_obj_align(obj_5, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_opa(obj_5, LV_OPA_TRANSP, 0);

    lv_obj_t * obj_6 = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj_6, 100, 100);
    lv_obj_set_style_border_width(obj_6, 0, 0);
    lv_obj_set_style_bg_color(obj_6, lv_color_hex3(0xf0f), 0);
    lv_obj_align(obj_6, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_opa_layered(obj_6, LV_OPA_TRANSP, 0);

    lv_draw_buf_t * draw_buf = lv_snapshot_take(lv_screen_active(), LV_COLOR_FORMAT_ARGB8888);
    lv_obj_t * img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, draw_buf);
    lv_obj_center(img);

    TEST_ASSERT_EQUAL_SCREENSHOT("snapshot_3.png");

    lv_obj_clean(lv_screen_active());
    lv_draw_buf_destroy(draw_buf);
}

void test_snapshot_create_draw_buf_with_extended_size(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, 50, 40);

    /* Установить расширенный эффект рисования (тень) */
    lv_obj_set_style_shadow_width(obj, 5, 0);
    lv_obj_set_style_shadow_spread(obj, 3, 0);

    lv_draw_buf_t * draw_buf = lv_snapshot_create_draw_buf(obj, LV_COLOR_FORMAT_ARGB8888);
    TEST_ASSERT_NOT_NULL(draw_buf);

    /* Получите расширенный размер перед удалением объекта */
    int32_t ext_size = lv_obj_get_ext_draw_size(obj);
    int32_t expected_w = 50 + ext_size * 2;
    int32_t expected_h = 40 + ext_size * 2;

    /* Убедитесь, что размер буфера прорисовки соответствует размеру объекта, включая расширенную область. */
    TEST_ASSERT_EQUAL_UINT32(expected_w, draw_buf->header.w);
    TEST_ASSERT_EQUAL_UINT32(expected_h, draw_buf->header.h);

    lv_draw_buf_destroy(draw_buf);
}

void test_snapshot_create_draw_buf_zero_size_object(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, 0, 0);
    lv_draw_buf_t * draw_buf = lv_snapshot_create_draw_buf(obj, LV_COLOR_FORMAT_ARGB8888);
    TEST_ASSERT_NULL(draw_buf);

    /* Ширина 0 */
    lv_obj_t * obj1 = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj1, 0, 50);
    lv_draw_buf_t * buf1 = lv_snapshot_create_draw_buf(obj1, LV_COLOR_FORMAT_ARGB8888);
    TEST_ASSERT_NULL(buf1);
    lv_obj_delete(obj1);

    /* Высота 0 */
    lv_obj_t * obj2 = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj2, 50, 0);
    lv_draw_buf_t * buf2 = lv_snapshot_create_draw_buf(obj2, LV_COLOR_FORMAT_ARGB8888);
    TEST_ASSERT_NULL(buf2);
    lv_obj_delete(obj2);
}

void test_snapshot_reshape_draw_buf_normal_object(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, 100, 80);

    /* Создать буфер рисования того же размера, что и объект. */
    lv_draw_buf_t * draw_buf = lv_draw_buf_create(100, 80, LV_COLOR_FORMAT_ARGB8888, LV_STRIDE_AUTO);
    TEST_ASSERT_NOT_NULL(draw_buf);

    /* Тестовая операция изменения формы (должна пройти успешно, поскольку размеры совпадают) */
    lv_result_t result = lv_snapshot_reshape_draw_buf(obj, draw_buf);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, result);

    /* Убедитесь, что размер буфера не изменился */
    TEST_ASSERT_EQUAL_UINT32(100, draw_buf->header.w);
    TEST_ASSERT_EQUAL_UINT32(80, draw_buf->header.h);

    lv_draw_buf_destroy(draw_buf);
}

void test_snapshot_reshape_draw_buf_with_extended_size(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, 50, 40);

    /* Установить расширенный эффект рисования (тень) */
    lv_obj_set_style_shadow_width(obj, 5, 0);
    lv_obj_set_style_shadow_spread(obj, 3, 0);

    /* Рассчитать размер, включая расширенную площадь */
    int32_t ext_size = lv_obj_get_ext_draw_size(obj);
    int32_t total_w = 50 + ext_size * 2;
    int32_t total_h = 40 + ext_size * 2;

    /* Создать буфер прорисовки расширенного размера. */
    lv_draw_buf_t * draw_buf = lv_draw_buf_create(total_w, total_h, LV_COLOR_FORMAT_ARGB8888, LV_STRIDE_AUTO);
    TEST_ASSERT_NOT_NULL(draw_buf);

    /* Тестовая операция изменения формы (должна пройти успешно, поскольку размеры совпадают) */
    lv_result_t result = lv_snapshot_reshape_draw_buf(obj, draw_buf);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, result);

    /* Убедитесь, что размер буфера не изменился */
    TEST_ASSERT_EQUAL_UINT32(total_w, draw_buf->header.w);
    TEST_ASSERT_EQUAL_UINT32(total_h, draw_buf->header.h);

    lv_draw_buf_destroy(draw_buf);
}

void test_snapshot_reshape_draw_buf_zero_size_object(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, 0, 0);

    /* Создать начальный буфер отрисовки */
    lv_draw_buf_t * draw_buf = lv_draw_buf_create(50, 40, LV_COLOR_FORMAT_ARGB8888, LV_STRIDE_AUTO);
    TEST_ASSERT_NOT_NULL(draw_buf);

    /* Тестовая операция изменения формы должна завершиться неудачей */
    lv_result_t result = lv_snapshot_reshape_draw_buf(obj, draw_buf);
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, result);

    lv_draw_buf_destroy(draw_buf);
}

void test_snapshot_reshape_draw_buf_invalid_params(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, 50, 40);

    /* Создать начальный буфер отрисовки */
    lv_draw_buf_t * draw_buf = lv_draw_buf_create(50, 40, LV_COLOR_FORMAT_ARGB8888, LV_STRIDE_AUTO);
    TEST_ASSERT_NOT_NULL(draw_buf);

    /* Проверяйте только буферNULL, а не объектNULL(функция утверждает obj !=NULLвнутри) */
    lv_result_t result = lv_snapshot_reshape_draw_buf(obj, NULL);
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, result);

    lv_draw_buf_destroy(draw_buf);
}

void test_snapshot_reshape_draw_buf_reshape_failure(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, 100, 80);

    /* Создайте недопустимый буфер отрисовки (например, размера 0). */
    lv_draw_buf_t * draw_buf = lv_draw_buf_create(0, 0, LV_COLOR_FORMAT_ARGB8888, LV_STRIDE_AUTO);

    /* Тестовая операция изменения формы должна завершиться неудачей */
    lv_result_t result = lv_snapshot_reshape_draw_buf(obj, draw_buf);
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, result);

    lv_draw_buf_destroy(draw_buf);
}

void test_snapshot_take_to_draw_buf_supported_formats(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, 100, 80);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xFF0000), 0);

    /* Тестируйте поддерживаемые цветовые форматы, используя общее определение. */
    for(size_t i = 0; i < NUM_SUPPORTED_FORMATS; i++) {
        /* Создать буфер рисования того же размера, что и объект. */
        int32_t ext_size = lv_obj_get_ext_draw_size(obj);
        int32_t total_w = 100 + ext_size * 2;
        int32_t total_h = 80 + ext_size * 2;

        lv_draw_buf_t * draw_buf = lv_draw_buf_create(total_w, total_h, supported_color_formats[i], LV_STRIDE_AUTO);
        TEST_ASSERT_NOT_NULL(draw_buf);

        /* Проверка работы take_to_draw_buf */
        lv_result_t result = lv_snapshot_take_to_draw_buf(obj, supported_color_formats[i], draw_buf);
        TEST_ASSERT_EQUAL(LV_RESULT_OK, result);

        lv_draw_buf_destroy(draw_buf);
    }
}

void test_snapshot_take_to_draw_buf_unsupported_format(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, 50, 40);

    /* Создать буфер рисования */
    lv_draw_buf_t * draw_buf = lv_draw_buf_create(50, 40, LV_COLOR_FORMAT_ARGB8888, LV_STRIDE_AUTO);
    TEST_ASSERT_NOT_NULL(draw_buf);

    /* Проверьте неподдерживаемый цветовой формат */
    lv_result_t result = lv_snapshot_take_to_draw_buf(obj, LV_COLOR_FORMAT_UNKNOWN, draw_buf);
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, result);

    lv_draw_buf_destroy(draw_buf);
}

void test_snapshot_take_to_draw_buf_reshape_failure(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, 100, 80);

    /* Создайте недопустимый буфер отрисовки (например, размера 0). */
    lv_draw_buf_t * draw_buf = lv_draw_buf_create(0, 0, LV_COLOR_FORMAT_ARGB8888, LV_STRIDE_AUTO);

    /* Тестовая операцияtake_to_draw_bufдолжна завершиться неудачно (поскольку изменение формы не спасти) */
    lv_result_t result = lv_snapshot_take_to_draw_buf(obj, LV_COLOR_FORMAT_ARGB8888, draw_buf);
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, result);

    lv_draw_buf_destroy(draw_buf);
}

void test_snapshot_take_to_draw_buf_zero_size_object(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, 0, 0);

    /* Создать буфер рисования */
    lv_draw_buf_t * draw_buf = lv_draw_buf_create(50, 40, LV_COLOR_FORMAT_ARGB8888, LV_STRIDE_AUTO);
    TEST_ASSERT_NOT_NULL(draw_buf);

    /* Тестовая операцияtake_to_draw_bufдолжна завершиться неудачно (поскольку размер объекта равен 0). */
    lv_result_t result = lv_snapshot_take_to_draw_buf(obj, LV_COLOR_FORMAT_ARGB8888, draw_buf);
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, result);

    lv_draw_buf_destroy(draw_buf);
}

void test_snapshot_take_to_draw_buf_invalid_size(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, 50, 40);

    /* Создать буфер рисования */
    lv_draw_buf_t * draw_buf = lv_draw_buf_create(10, 10, LV_COLOR_FORMAT_ARGB8888, LV_STRIDE_AUTO);
    TEST_ASSERT_NOT_NULL(draw_buf);

    lv_result_t result = lv_snapshot_take_to_draw_buf(obj, LV_COLOR_FORMAT_ARGB8888, draw_buf);
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, result);

    lv_draw_buf_destroy(draw_buf);
}

void test_snapshot_refresh_logic_sibling_order(void)
{
    lv_obj_t * parent = lv_obj_create(lv_screen_active());
    lv_obj_set_size(parent, 250, 250);
    lv_obj_set_style_bg_color(parent, lv_color_hex(0xCCCCCC), 0);

    /* Создание нескольких одноуровневых объектов */
    lv_obj_t * siblings[5];
    for(int i = 0; i < 5; i++) {
        siblings[i] = lv_obj_create(parent);
        lv_obj_set_size(siblings[i], 40, 40);
        lv_obj_set_style_bg_color(siblings[i], lv_color_hex(0x0000FF + i * 0x101010), 0);
        lv_obj_set_pos(siblings[i], i * 50, 50);
    }

    /* Проверьте снимок среднего объекта, проверьте логику обновления родственного объекта. */
    lv_draw_buf_t * draw_buf = lv_snapshot_take(siblings[2], LV_COLOR_FORMAT_ARGB8888);
    TEST_ASSERT_NOT_NULL(draw_buf);

    /* Убедитесь, что размер снимка правильный */
    int32_t ext_size = lv_obj_get_ext_draw_size(siblings[2]);
    TEST_ASSERT_EQUAL_UINT32(40 + ext_size * 2, draw_buf->header.w);
    TEST_ASSERT_EQUAL_UINT32(40 + ext_size * 2, draw_buf->header.h);

    lv_draw_buf_destroy(draw_buf);
    lv_obj_delete(parent);
}

void test_snapshot_refresh_logic_edge_cases(void)
{
    /* Объект тестирования — сам экран */
    lv_draw_buf_t * draw_buf1 = lv_snapshot_take(lv_screen_active(), LV_COLOR_FORMAT_ARGB8888);
    TEST_ASSERT_NOT_NULL(draw_buf1);

    /* Проверьте размер снимка экрана */
    int32_t screen_w = lv_display_get_horizontal_resolution(NULL);
    int32_t screen_h = lv_display_get_vertical_resolution(NULL);
    int32_t ext_size = lv_obj_get_ext_draw_size(lv_screen_active());
    TEST_ASSERT_EQUAL_UINT32(screen_w + ext_size * 2, draw_buf1->header.w);
    TEST_ASSERT_EQUAL_UINT32(screen_h + ext_size * 2, draw_buf1->header.h);

    lv_draw_buf_destroy(draw_buf1);

    /* Тестирование однослойной структуры объекта */
    lv_obj_t * simple_obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(simple_obj, 50, 50);
    lv_obj_set_style_bg_color(simple_obj, lv_color_hex(0x00FF00), 0);

    lv_draw_buf_t * draw_buf2 = lv_snapshot_take(simple_obj, LV_COLOR_FORMAT_ARGB8888);
    TEST_ASSERT_NOT_NULL(draw_buf2);

    int32_t ext_size2 = lv_obj_get_ext_draw_size(simple_obj);
    TEST_ASSERT_EQUAL_UINT32(50 + ext_size2 * 2, draw_buf2->header.w);
    TEST_ASSERT_EQUAL_UINT32(50 + ext_size2 * 2, draw_buf2->header.h);

    lv_draw_buf_destroy(draw_buf2);
    lv_obj_delete(simple_obj);
}

void test_snapshot_extreme_size_objects(void)
{
    /* Тестирование объектов экстремального размера */
    lv_obj_t * large_obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(large_obj, 500, 400);  /* Большой размер */
    lv_obj_set_style_bg_color(large_obj, lv_palette_main(LV_PALETTE_PURPLE), 0);

    lv_draw_buf_t * large_buf = lv_snapshot_take(large_obj, LV_COLOR_FORMAT_RGB565);
    TEST_ASSERT_NOT_NULL(large_buf);

    int32_t ext_size_large = lv_obj_get_ext_draw_size(large_obj);
    TEST_ASSERT_EQUAL_UINT32(500 + ext_size_large * 2, large_buf->header.w);
    TEST_ASSERT_EQUAL_UINT32(400 + ext_size_large * 2, large_buf->header.h);

    lv_draw_buf_destroy(large_buf);
    lv_obj_delete(large_obj);

    /* Тестируйте очень маленькие объекты */
    lv_obj_t * small_obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(small_obj, 5, 5);  /* Очень маленький размер */
    lv_obj_set_style_bg_color(small_obj, lv_palette_main(LV_PALETTE_CYAN), 0);

    lv_draw_buf_t * small_buf = lv_snapshot_take(small_obj, LV_COLOR_FORMAT_ARGB8888);
    TEST_ASSERT_NOT_NULL(small_buf);

    int32_t ext_size_small = lv_obj_get_ext_draw_size(small_obj);
    TEST_ASSERT_EQUAL_UINT32(5 + ext_size_small * 2, small_buf->header.w);
    TEST_ASSERT_EQUAL_UINT32(5 + ext_size_small * 2, small_buf->header.h);

    lv_draw_buf_destroy(small_buf);
    lv_obj_delete(small_obj);
}

#endif
