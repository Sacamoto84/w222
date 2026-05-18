#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"
#include "../demos/lv_demos.h"

#include "unity/unity.h"

void setUp(void)
{
    /* Функция запускается перед каждым тестом */
}

void tearDown(void)
{
    /* Функция запускается после каждого теста */
    lv_display_set_color_format(NULL, LV_COLOR_FORMAT_XRGB8888);
}

void test_render_to_argb8888_premultiplied(void)
{
    lv_display_set_color_format(NULL, LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED);

    lv_opa_t opa_values[2] = {0xff, 0x80};
    uint32_t opa;
    for(opa = 0; opa < 2; opa++) {
        uint32_t i;
        for(i = 0; i < LV_DEMO_RENDER_SCENE_NUM; i++) {

            /*
             * Пропустить тест с преобразованными индексированными изображениями, если они не загружены в RAM.
             * также пропустите normal_3 и recolor_3 на VGLite
             * потому что RGB565A8 и I8 не поддерживаются
             */
            if((LV_BIN_DECODER_RAM_LOAD == 0 || LV_USE_DRAW_VG_LITE) &&
               (i == LV_DEMO_RENDER_SCENE_IMAGE_NORMAL_3 ||
                i == LV_DEMO_RENDER_SCENE_IMAGE_RECOLOR_3)) continue;

            lv_demo_render(i, opa_values[opa]);

            char buf[128];
            lv_snprintf(buf, sizeof(buf), "draw/render/argb8888_premultiplied/demo_render_%s_opa_%d.png",
                        lv_demo_render_get_scene_name(i), opa_values[opa]);
            TEST_ASSERT_EQUAL_SCREENSHOT(buf);
        }
    }
}

#endif
