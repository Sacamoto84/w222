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
}

void test_memmove(void)
{
#if LV_USE_STDLIB_STRING == LV_STDLIB_BUILTIN
    char buf[6] = {0, 1, 2, 3, 4, 5};
    char * dst;
    char * src;

    dst = buf;
    src = buf;

    /*Случай dst == src*/
    lv_memmove(dst, src, 4);
    for(int i = 0; i < 4; i++) {
        TEST_ASSERT_EQUAL_INT(i, buf[i]);
    }

    /* Случай dst < src */
    dst = &buf[0];
    src = &buf[1];
    lv_memmove(dst, src, 4);
    for(int i = 0; i < 4; i++) {
        TEST_ASSERT_EQUAL_INT(i + 1, buf[i]);
    }

    /* Случай dst > src */
    for(int i = 0; i < 5; i++) buf[i] = i;
    dst = &buf[1];
    src = &buf[0];
    lv_memmove(dst, src, 4);
    TEST_ASSERT_EQUAL_INT(0, buf[0]);
    for(int i = 1; i < 5; i++) {
        TEST_ASSERT_EQUAL_INT(i - 1, buf[i]);
    }
#endif
}

#endif
