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

void test_image_cache_dump(void)
{
    /* Дамп не должен давать сбоев или утечек памяти. */
    lv_image_cache_dump();
    lv_image_header_cache_dump();
}

#endif
