#if LV_BUILD_TEST
#include "../../../src/misc/lv_palette.h"
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

/* Протестируйтеlv_palette_mainсо всеми цветочными палитрами. */
void test_palette_main_all_colors(void)
{
    for(lv_palette_t p = LV_PALETTE_RED; p < LV_PALETTE_LAST; p++) {
        lv_color_t color = lv_palette_main(p);
        TEST_ASSERT_FALSE(lv_color_eq(lv_color_black(), color));
    }
}

/* Проверьтеlv_palette_mainс недопустимой палитрой. */
void test_palette_main_invalid(void)
{
    lv_color_t color = lv_palette_main(LV_PALETTE_LAST);
    TEST_ASSERT_EQUAL_COLOR(lv_color_black(), color);

    color = lv_palette_main(LV_PALETTE_NONE);
    TEST_ASSERT_EQUAL_COLOR(lv_color_black(), color);
}

/* Проверьтеlv_palette_lightenна всех уровнях. */
void test_palette_lighten_all_levels(void)
{
    for(lv_palette_t p = LV_PALETTE_RED; p < LV_PALETTE_LAST; p++) {
        for(uint8_t lvl = 1; lvl <= 5; lvl++) {
            lv_color_t color = lv_palette_lighten(p, lvl);
            TEST_ASSERT_FALSE(lv_color_eq(lv_color_black(), color));
        }
    }
}

/* Проверьтеlv_palette_lightenс недопустимыми уровнями. */
void test_palette_lighten_invalid(void)
{
    lv_color_t color = lv_palette_lighten(LV_PALETTE_RED, 0);
    TEST_ASSERT_EQUAL_COLOR(lv_color_black(), color);

    color = lv_palette_lighten(LV_PALETTE_RED, 6);
    TEST_ASSERT_EQUAL_COLOR(lv_color_black(), color);
}

/* Проверьтеlv_palette_darkenна всех уровнях. */
void test_palette_darken_all_levels(void)
{
    for(lv_palette_t p = LV_PALETTE_RED; p < LV_PALETTE_LAST; p++) {
        for(uint8_t lvl = 1; lvl <= 4; lvl++) {
            lv_color_t color = lv_palette_darken(p, lvl);
            TEST_ASSERT_FALSE(lv_color_eq(lv_color_black(), color));
        }
    }
}

/* Проверьтеlv_palette_darkenс недопустимыми уровнями. */
void test_palette_darken_invalid(void)
{
    lv_color_t color = lv_palette_darken(LV_PALETTE_RED, 0);
    TEST_ASSERT_EQUAL_COLOR(lv_color_black(), color);

    color = lv_palette_darken(LV_PALETTE_RED, 5);
    TEST_ASSERT_EQUAL_COLOR(lv_color_black(), color);
}

/* Тестирование определенных значений цвета */
void test_palette_specific_values(void)
{
    /* Тестовый красный цвет */
    lv_color_t red = lv_palette_main(LV_PALETTE_RED);
    TEST_ASSERT_EQUAL_HEX32(0xF44336, lv_color_to_int(red));

    /* Тест светло-голубой уровень 3 */
    lv_color_t light_blue = lv_palette_lighten(LV_PALETTE_LIGHT_BLUE, 3);
    TEST_ASSERT_EQUAL_HEX32(0x81D4FA, lv_color_to_int(light_blue));
}

/* Тестlv_palette_mainс индексом палитры, выходящим за пределы допустимого соединения. */
void test_palette_main_out_of_range(void)
{
    lv_color_t color = lv_palette_main(LV_PALETTE_LAST + 1);
    TEST_ASSERT_EQUAL_COLOR(lv_color_black(), color);
}

/* Тестlv_palette_lightenс индексом палитры, выходящим за пределы допустимого соединения. */
void test_palette_lighten_out_of_range(void)
{
    lv_color_t color = lv_palette_lighten(LV_PALETTE_LAST + 1, 1);
    TEST_ASSERT_EQUAL_COLOR(lv_color_black(), color);
}

/* Тестlv_palette_darkenс индексом палитры, выходящим за пределы допустимого соединения. */
void test_palette_darken_out_of_range(void)
{
    lv_color_t color = lv_palette_darken(LV_PALETTE_LAST + 1, 1);
    TEST_ASSERT_EQUAL_COLOR(lv_color_black(), color);
}

#endif
