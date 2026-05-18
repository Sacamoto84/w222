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
}

void test_render_to_argb2222(void)
{
    /**
     * Существует небольшое отклонение цвета между thorvg на 32-битных и 64-битных платформах.
     * Отклонение будет усиливаться при использовании цветовых форматов с более низкой точностью.
     * Здесь тестируются только 64-битные платформы.
     */
#if LV_USE_DRAW_VG_LITE && LV_USE_SNAPSHOT && !defined(NON_AMD64_BUILD)

    lv_opa_t opa_values[2] = {0xff, 0x80};
    uint32_t opa;
    for(opa = 0; opa < 2; opa++) {
        uint32_t i;
        for(i = 0; i < LV_DEMO_RENDER_SCENE_NUM; i++) {

            /*Пропустить тест с преобразованными индексированными изображениями, если они не загружены в RAM.*/
            if(LV_BIN_DECODER_RAM_LOAD == 0 &&
               (i == LV_DEMO_RENDER_SCENE_IMAGE_NORMAL_3 ||
                i == LV_DEMO_RENDER_SCENE_IMAGE_RECOLOR_3)) continue;

            lv_demo_render(i, opa_values[opa]);
            lv_draw_buf_t * draw_buf = lv_snapshot_take(lv_screen_active(), LV_COLOR_FORMAT_ARGB2222);
            lv_obj_t * img = lv_image_create(lv_layer_top());
            lv_image_set_src(img, draw_buf);

            char buf[128];
            lv_snprintf(buf, sizeof(buf), "draw/render/argb2222/demo_render_%s_opa_%d.png",
                        lv_demo_render_get_scene_name(i), opa_values[opa]);
            TEST_ASSERT_EQUAL_SCREENSHOT(buf);
            lv_obj_delete(img);
            lv_draw_buf_destroy(draw_buf);
        }
    }
#else
    TEST_PASS();
#endif
}

#endif
