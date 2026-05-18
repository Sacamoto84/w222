#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void setUp(void)
{
    /* Функция запускается перед каждым тестом */
}

void tearDown(void)
{
    /* Функция запускается после каждого теста */

}

void test_color_format_get_bpp(void)
{
    struct {
        lv_color_format_t format;
        uint8_t expected_bpp;
    } test_cases[] = {
        {LV_COLOR_FORMAT_I1, 1},
        {LV_COLOR_FORMAT_A1, 1},
        {LV_COLOR_FORMAT_I2, 2},
        {LV_COLOR_FORMAT_A2, 2},
        {LV_COLOR_FORMAT_I4, 4},
        {LV_COLOR_FORMAT_A4, 4},
        {LV_COLOR_FORMAT_L8, 8},
        {LV_COLOR_FORMAT_A8, 8},
        {LV_COLOR_FORMAT_I8, 8},
        {LV_COLOR_FORMAT_ARGB2222, 8},
        {LV_COLOR_FORMAT_RGB565A8, 16},
        {LV_COLOR_FORMAT_RGB565, 16},
        {LV_COLOR_FORMAT_YUY2, 16},
        {LV_COLOR_FORMAT_ARGB1555, 16},
        {LV_COLOR_FORMAT_ARGB4444, 16},
        {LV_COLOR_FORMAT_ARGB8565, 24},
        {LV_COLOR_FORMAT_RGB888, 24},
        {LV_COLOR_FORMAT_ARGB8888, 32},
        {LV_COLOR_FORMAT_XRGB8888, 32},
        {LV_COLOR_FORMAT_UNKNOWN, 0}
    };

    for(size_t i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++) {
        TEST_ASSERT_EQUAL_UINT8(test_cases[i].expected_bpp,
                                lv_color_format_get_bpp(test_cases[i].format));
    }
}

void test_color_format_has_alpha(void)
{
    struct {
        lv_color_format_t format;
        bool expected_has_alpha;
    } test_cases[] = {
        {LV_COLOR_FORMAT_A1, true},
        {LV_COLOR_FORMAT_A2, true},
        {LV_COLOR_FORMAT_A4, true},
        {LV_COLOR_FORMAT_A8, true},
        {LV_COLOR_FORMAT_I1, true},
        {LV_COLOR_FORMAT_I2, true},
        {LV_COLOR_FORMAT_I4, true},
        {LV_COLOR_FORMAT_I8, true},
        {LV_COLOR_FORMAT_RGB565A8, true},
        {LV_COLOR_FORMAT_ARGB8565, true},
        {LV_COLOR_FORMAT_ARGB8888, true},
        {LV_COLOR_FORMAT_ARGB2222, true},
        {LV_COLOR_FORMAT_ARGB1555, true},
        {LV_COLOR_FORMAT_ARGB4444, true},
        {LV_COLOR_FORMAT_XRGB8888, false},
        {LV_COLOR_FORMAT_RGB888, false},
        {LV_COLOR_FORMAT_YUY2, false},
        {LV_COLOR_FORMAT_RGB565, false},
        {LV_COLOR_FORMAT_L8, false},
        {LV_COLOR_FORMAT_UNKNOWN, false}
    };

    for(size_t i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++) {
        TEST_ASSERT_EQUAL_UINT8(test_cases[i].expected_has_alpha,
                                lv_color_format_has_alpha(test_cases[i].format));
    }
}

void test_color_to_32(void)
{
    lv_color_t test_color = { .red = 255, .green = 128, .blue = 64 };
    lv_opa_t test_opa = 192;

    lv_color32_t result_color = lv_color_to_32(test_color, test_opa);

    TEST_ASSERT_EQUAL_UINT8(255, result_color.red);
    TEST_ASSERT_EQUAL_UINT8(128, result_color.green);
    TEST_ASSERT_EQUAL_UINT8(64, result_color.blue);
    TEST_ASSERT_EQUAL_UINT8(192, result_color.alpha);
}

void test_color_to_u16(void)
{
    lv_color_t test_color = { .red = 255, .green = 128, .blue = 64 };
    uint16_t result_color_u16 = lv_color_to_u16(test_color);
    TEST_ASSERT_EQUAL_UINT16(0xFC08, result_color_u16);
}

void test_color_to_u32(void)
{
    /*Тестирование базового преобразования цветов*/
    lv_color_t test_color = { .red = 255, .green = 128, .blue = 64 };
    TEST_ASSERT_EQUAL_UINT32(0xFFFF8040, lv_color_to_u32(test_color));

    /*Минимальные значения теста*/
    lv_color_t test_color_min = { .red = 0, .green = 0, .blue = 0 };
    TEST_ASSERT_EQUAL_UINT32(0xFF000000, lv_color_to_u32(test_color_min));

    /*Максимальные значения теста*/
    lv_color_t test_color_max = { .red = 255, .green = 255, .blue = 255 };
    TEST_ASSERT_EQUAL_UINT32(0xFFFFFFFF, lv_color_to_u32(test_color_max));

    /*Тест случайного цвета*/
    lv_color_t test_color_random = { .red = 123, .green = 45, .blue = 67 };
    TEST_ASSERT_EQUAL_UINT32(0xFF7B2D43, lv_color_to_u32(test_color_random));
}

void test_color_lighten(void)
{
    /*Тестирование основного цвета осветления*/
    lv_color_t test_color = { .red = 100, .green = 100, .blue = 100 };
    lv_color_t lightened = lv_color_lighten(test_color, 128);
    TEST_ASSERT_EQUAL_UINT32(0xFFB1B1B1, lv_color_to_u32(lightened));

    /*Минимальный уровень проверки (должен вернуть исходный цвет)*/
    lv_color_t test_color_min = { .red = 50, .green = 100, .blue = 150 };
    lv_color_t lightened_min = lv_color_lighten(test_color_min, 0);
    TEST_ASSERT_EQUAL_UINT32(0xFF326496, lv_color_to_u32(lightened_min));

    /*Максимальный уровень проверки (должен вернуть белый цвет)*/
    lv_color_t lightened_max = lv_color_lighten(test_color, 255);
    TEST_ASSERT_EQUAL_UINT32(0xFFFFFFFF, lv_color_to_u32(lightened_max));

    /*Тест на осветление черного цвета*/
    lv_color_t test_color_black = { .red = 0, .green = 0, .blue = 0 };
    lv_color_t lightened_black = lv_color_lighten(test_color_black, 64);
    TEST_ASSERT_EQUAL_UINT32(0xFF404040, lv_color_to_u32(lightened_black));
}

void test_color_darken(void)
{
    /* Тестирование основного цвета, затемнение */
    lv_color_t test_color = { .red = 100, .green = 100, .blue = 100 };
    lv_color_t darkened = lv_color_darken(test_color, 128);
    TEST_ASSERT_EQUAL_UINT32(0xFF313131, lv_color_to_u32(darkened));

    /*Минимальный уровень проверки (должен вернуть исходный цвет)*/
    lv_color_t test_color_min = { .red = 50, .green = 100, .blue = 150 };
    lv_color_t darkened_min = lv_color_darken(test_color_min, 0);
    TEST_ASSERT_EQUAL_UINT32(0xFF326496, lv_color_to_u32(darkened_min));

    /*Максимальный уровень проверки (должен вернуть черный цвет)*/
    lv_color_t darkened_max = lv_color_darken(test_color, 255);
    TEST_ASSERT_EQUAL_UINT32(0xFF000000, lv_color_to_u32(darkened_max));

    /*Тестовый белый цвет затемняется*/
    lv_color_t test_color_white = { .red = 255, .green = 255, .blue = 255 };
    lv_color_t darkened_white = lv_color_darken(test_color_white, 64);
    TEST_ASSERT_EQUAL_UINT32(0xFFBFBFBF, lv_color_to_u32(darkened_white));
}

void test_color_hsv_to_rgb(void)
{
    /* HSV (0, 0, 0) должен быть черным. */
    lv_color_t black = lv_color_hsv_to_rgb(0, 0, 0);
    TEST_ASSERT_EQUAL_UINT8(0, black.red);
    TEST_ASSERT_EQUAL_UINT8(0, black.green);
    TEST_ASSERT_EQUAL_UINT8(0, black.blue);

    /*HSV (0,0,100) должен быть белым.*/
    lv_color_t white = lv_color_hsv_to_rgb(0, 0, 100);
    TEST_ASSERT_EQUAL_UINT8(255, white.red);
    TEST_ASSERT_EQUAL_UINT8(255, white.green);
    TEST_ASSERT_EQUAL_UINT8(255, white.blue);

    /*HSV (0,100,100) должен быть красным.*/
    lv_color_t red = lv_color_hsv_to_rgb(0, 100, 100);
    TEST_ASSERT_EQUAL_UINT8(255, red.red);
    TEST_ASSERT_EQUAL_UINT8(0, red.green);
    TEST_ASSERT_EQUAL_UINT8(0, red.blue);

    /* Из-за ограничений точности был добавлен допуск.*/
    /*HSV (120,100,100) должен быть зеленым.*/
    lv_color_t green = lv_color_hsv_to_rgb(120, 100, 100);
    TEST_ASSERT_UINT8_WITHIN(3, 0, green.red);
    TEST_ASSERT_EQUAL_UINT8(255, green.green);
    TEST_ASSERT_EQUAL_UINT8(0, green.blue);

    /*HSV (240 100 100) должен быть синим.*/
    lv_color_t blue = lv_color_hsv_to_rgb(240, 100, 100);
    TEST_ASSERT_EQUAL_UINT8(0, blue.red);
    TEST_ASSERT_UINT8_WITHIN(9, 0, blue.green);
    TEST_ASSERT_EQUAL_UINT8(255, blue.blue);

    /*Граничные значения теста*/
    lv_color_t c1 = lv_color_hsv_to_rgb(360, 100, 100);
    TEST_ASSERT_EQUAL_UINT8(255, c1.red);
    TEST_ASSERT_EQUAL_UINT8(0, c1.green);
    TEST_ASSERT_UINT8_WITHIN(15, 0, c1.blue);

    lv_color_t c2 = lv_color_hsv_to_rgb(180, 0, 50);
    TEST_ASSERT_EQUAL_UINT8(127, c2.blue);
    TEST_ASSERT_EQUAL_UINT8(127, c2.green);
    TEST_ASSERT_EQUAL_UINT8(127, c2.red);
}

void test_color_rgb_to_hsv(void)
{
    /* Тестовый черный */
    lv_color_hsv_t black = lv_color_rgb_to_hsv(0, 0, 0);
    TEST_ASSERT_EQUAL(0, black.h);
    TEST_ASSERT_EQUAL(0, black.s);
    TEST_ASSERT_EQUAL(0, black.v);

    /* Тестовый белый */
    lv_color_hsv_t white = lv_color_rgb_to_hsv(255, 255, 255);
    TEST_ASSERT_EQUAL(0, white.h);
    TEST_ASSERT_EQUAL(0, white.s);
    TEST_ASSERT_EQUAL(100, white.v);

    /* Тестовый красный */
    lv_color_hsv_t red = lv_color_rgb_to_hsv(255, 0, 0);
    TEST_ASSERT_EQUAL(0, red.h);
    TEST_ASSERT_EQUAL(100, red.s);
    TEST_ASSERT_EQUAL(100, red.v);

    /* Тестовый зеленый */
    lv_color_hsv_t green = lv_color_rgb_to_hsv(0, 255, 0);
    TEST_ASSERT_EQUAL(120, green.h);
    TEST_ASSERT_EQUAL(100, green.s);
    TEST_ASSERT_EQUAL(100, green.v);

    /* Тестовый синий */
    lv_color_hsv_t blue = lv_color_rgb_to_hsv(0, 0, 255);
    TEST_ASSERT_EQUAL(240, blue.h);
    TEST_ASSERT_EQUAL(100, blue.s);
    TEST_ASSERT_EQUAL(100, blue.v);

    /* Тестовый серый */
    lv_color_hsv_t gray = lv_color_rgb_to_hsv(128, 128, 128);
    TEST_ASSERT_EQUAL(0, gray.h);
    TEST_ASSERT_EQUAL(0, gray.s);
    TEST_ASSERT_EQUAL(50, gray.v);

    /* Граничные значения теста */
    lv_color_hsv_t c1 = lv_color_rgb_to_hsv(1, 1, 1);
    TEST_ASSERT_EQUAL(0, c1.h);
    TEST_ASSERT_EQUAL(0, c1.s);
    TEST_ASSERT_LESS_THAN(1, c1.v);
}

void test_color_to_hsv(void)
{
    /* Тестовый черный */
    lv_color_t black_color = { .red = 0, .green = 0, .blue = 0 };
    lv_color_hsv_t black = lv_color_to_hsv(black_color);
    TEST_ASSERT_EQUAL(0, black.h);
    TEST_ASSERT_EQUAL(0, black.s);
    TEST_ASSERT_EQUAL(0, black.v);

    /* Тестовый белый */
    lv_color_t white_color = { .red = 255, .green = 255, .blue = 255 };
    lv_color_hsv_t white = lv_color_to_hsv(white_color);
    TEST_ASSERT_EQUAL(0, white.h);
    TEST_ASSERT_EQUAL(0, white.s);
    TEST_ASSERT_EQUAL(100, white.v);

    /* Тестовый красный */
    lv_color_t red_color = { .red = 255, .green = 0, .blue = 0 };
    lv_color_hsv_t red = lv_color_to_hsv(red_color);
    TEST_ASSERT_EQUAL(0, red.h);
    TEST_ASSERT_EQUAL(100, red.s);
    TEST_ASSERT_EQUAL(100, red.v);

    /* Тестовый зеленый */
    lv_color_t green_color = { .red = 0, .green = 255, .blue = 0 };
    lv_color_hsv_t green = lv_color_to_hsv(green_color);
    TEST_ASSERT_EQUAL(120, green.h);
    TEST_ASSERT_EQUAL(100, green.s);
    TEST_ASSERT_EQUAL(100, green.v);

    /* Тестовый синий */
    lv_color_t blue_color = { .red = 0, .green = 0, .blue = 255 };
    lv_color_hsv_t blue = lv_color_to_hsv(blue_color);
    TEST_ASSERT_EQUAL(240, blue.h);
    TEST_ASSERT_EQUAL(100, blue.s);
    TEST_ASSERT_EQUAL(100, blue.v);
}

void test_color_mix(void)
{
    /* Проверьте 50% смесь красного и синего. */
    lv_color_t red = { .red = 255, .green = 0, .blue = 0 };
    lv_color_t blue = { .red = 0, .green = 0, .blue = 255 };
    lv_color_t mixed = lv_color_mix(red, blue, 128);
    TEST_ASSERT_EQUAL_UINT8(128, mixed.red);
    TEST_ASSERT_EQUAL_UINT8(0, mixed.green);
    TEST_ASSERT_EQUAL_UINT8(127, mixed.blue);

    /* Тестовая 100% смесь (должен быть первый цвет) */
    lv_color_t mixed_max = lv_color_mix(red, blue, 255);
    TEST_ASSERT_EQUAL_UINT8(255, mixed_max.red);
    TEST_ASSERT_EQUAL_UINT8(0, mixed_max.green);
    TEST_ASSERT_EQUAL_UINT8(0, mixed_max.blue);

    /* Тестовая смесь 0% (должен быть второй цвет) */
    lv_color_t mixed_min = lv_color_mix(red, blue, 0);
    TEST_ASSERT_EQUAL_UINT8(0, mixed_min.red);
    TEST_ASSERT_EQUAL_UINT8(0, mixed_min.green);
    TEST_ASSERT_EQUAL_UINT8(255, mixed_min.blue);
}

void test_color_filter_shade(void)
{
    /* Тестируйте с непрозрачностью 50% (должен вернуться исходный цвет) */
    lv_color_t c = { .red = 100, .green = 100, .blue = 100 };
    lv_color_t filtered = lv_color_filter_shade.filter_cb(&lv_color_filter_shade, c, LV_OPA_50);
    TEST_ASSERT_EQUAL_UINT8(100, filtered.red);
    TEST_ASSERT_EQUAL_UINT8(100, filtered.green);
    TEST_ASSERT_EQUAL_UINT8(100, filtered.blue);

    /* Тест с непрозрачностью менее 50% (должен посветлеть) */
    lv_color_t filtered_light = lv_color_filter_shade.filter_cb(&lv_color_filter_shade, c, LV_OPA_20);
    TEST_ASSERT_EQUAL_UINT8(192, filtered_light.red);
    TEST_ASSERT_EQUAL_UINT8(192, filtered_light.green);
    TEST_ASSERT_EQUAL_UINT8(192, filtered_light.blue);

    /* Тест с непрозрачностью более 50% (должен потемнеть) */
    lv_color_t filtered_dark = lv_color_filter_shade.filter_cb(&lv_color_filter_shade, c, LV_OPA_80);
    TEST_ASSERT_EQUAL_UINT8(41, filtered_dark.red);
    TEST_ASSERT_EQUAL_UINT8(41, filtered_dark.green);
    TEST_ASSERT_EQUAL_UINT8(41, filtered_dark.blue);

    /* Тестируйте с непрозрачностью 0% (должно быть полностью осветлено) */
    lv_color_t filtered_min = lv_color_filter_shade.filter_cb(&lv_color_filter_shade, c, LV_OPA_0);
    TEST_ASSERT_EQUAL_UINT8(254, filtered_min.red);
    TEST_ASSERT_EQUAL_UINT8(254, filtered_min.green);
    TEST_ASSERT_EQUAL_UINT8(254, filtered_min.blue);

    /* Тест со 100% непрозрачностью (должен быть полностью затемнен) */
    /* Ограниченная точность цвета */
    lv_color_t filtered_max = lv_color_filter_shade.filter_cb(&lv_color_filter_shade, c, LV_OPA_100);
    TEST_ASSERT_EQUAL_UINT8(1, filtered_max.red);
    TEST_ASSERT_EQUAL_UINT8(1, filtered_max.green);
    TEST_ASSERT_EQUAL_UINT8(1, filtered_max.blue);
}

#endif
