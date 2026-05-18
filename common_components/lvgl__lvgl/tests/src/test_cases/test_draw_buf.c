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

void test_draw_buf_stride_adjust(void)
{
#if LV_BIN_DECODER_RAM_LOAD == 1
    const char * color_formats[] = {
        "I1",
        "I2",
        "I4",
        "I8",
#if 0   /* Декодер в любом случае преобразует их в A8.*/
        "A1",
        "A2",
        "A4",
#endif
        "A8",
        "RGB565",
#if 0   /* RGB565 с альфа-версией не поддерживается.*/
        "RGB565A8",
        "ARGB8565",
#endif
        "RGB888",
        "ARGB8888",
        "XRGB8888",
    };

    lv_obj_t * img = lv_image_create(lv_screen_active());
    lv_obj_center(img);

    const lv_image_decoder_args_t args = {
        .no_cache = true,
        .premultiply = false,
        .stride_align = false,
        .use_indexed = true,
    };

    for(unsigned long i = 0; i < sizeof(color_formats) / sizeof(color_formats[0]); i++) {
        char img_src[256];
        char ref_image[256];
        snprintf(img_src, sizeof(img_src), "A:test_images/stride_align1/UNCOMPRESSED/test_%s.bin", color_formats[i]);
        snprintf(ref_image, sizeof(ref_image), "draw/temp_%s.o", color_formats[i]); /*Используйте имя файла .o, чтобы git его игнорировал.*/

        lv_image_set_src(img, img_src);
        TEST_ASSERT_EQUAL_SCREENSHOT(ref_image); /*Создайте эталонное изображение, используйте .o, чтобы git проигнорировал его.*/

        lv_image_cache_drop(img_src); /* Изображение может быть добавлено в кеш во время lv_image_set_src.*/

        lv_image_decoder_dsc_t decoder_dsc;
        lv_result_t res = lv_image_decoder_open(&decoder_dsc, img_src, &args);
        TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
        lv_draw_buf_t * decoded = lv_draw_buf_dup(decoder_dsc.decoded);
        TEST_ASSERT_NOT_NULL(decoded);

        const lv_image_header_t header = decoder_dsc.decoded->header;
        uint32_t image_width = header.w;
        uint32_t image_height = header.h;
        uint32_t image_stride = header.stride;
        uint32_t min_stride = (image_width * lv_color_format_get_bpp(header.cf) + 7) >> 3;

        /*Закройте декодер, так как мы скопировали декодированный буфер отрисовки.*/
        lv_image_decoder_close(&decoder_dsc);

        /* Уменьшить шаг ниже минимального шага (на -1 в коде ниже) должно завершиться неудачно. */
        res = lv_draw_buf_adjust_stride(decoded, min_stride - 1);
        TEST_ASSERT_EQUAL(LV_RESULT_INVALID, res);

        /*Расширение шага должно завершиться неудачно, если шаг слишком велик и размер буфера переполняется.*/
        res = lv_draw_buf_adjust_stride(decoded, image_stride + 1);
        TEST_ASSERT_EQUAL(LV_RESULT_INVALID, res);

        /*Создать больший буфер прорисовки*/
        lv_draw_buf_t * larger = lv_draw_buf_create(image_width, image_height, header.cf, min_stride + 100);

        /*Скопируйте буфер рисования, он должен выглядеть так же.*/
        lv_draw_buf_copy(larger, NULL, decoded, NULL);
        lv_image_cache_drop(larger);
        lv_image_set_src(img, larger);
        TEST_ASSERT_EQUAL_SCREENSHOT(ref_image); /*Изображение должно выглядеть так же*/

        /* Сократить шаг до минимального должно добиться успеха. */
        res = lv_draw_buf_adjust_stride(larger, min_stride);
        TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
        lv_image_cache_drop(larger);
        lv_image_set_src(img, larger);
        TEST_ASSERT_EQUAL_SCREENSHOT(ref_image); /*Протестируйте с использованием приведенного выше эталонного изображения.*/

        /* Увеличение шага должно работать, используйте правильное значение шага.*/
        res = lv_draw_buf_adjust_stride(larger, min_stride + 20);
        TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
        lv_image_cache_drop(larger);
        lv_image_set_src(img, larger);
        TEST_ASSERT_EQUAL_SCREENSHOT(ref_image); /*Изображение должно выглядеть так же*/

        lv_draw_buf_destroy(larger);
        lv_draw_buf_destroy(decoded);
    }

    lv_obj_delete(img);
#endif
}

void test_draw_buf_xy_access(void)
{
    LV_DRAW_BUF_DEFINE_STATIC(draw_buf, 100, 100, LV_COLOR_FORMAT_RGB565);
    LV_DRAW_BUF_INIT_STATIC(draw_buf);

    uint8_t * ret = lv_draw_buf_goto_xy(&draw_buf, 50, 50);
    TEST_ASSERT_NOT_NULL(ret);
    ret = lv_draw_buf_goto_xy(&draw_buf, 100, 100);
    TEST_ASSERT_NULL(ret);
    ret = lv_draw_buf_goto_xy(&draw_buf, -10, -10);
    TEST_ASSERT_NULL(ret);
}


void test_draw_buf_premultiply(void)
{
    lv_draw_buf_t * draw_buf;
    lv_result_t res;
    int i;
    /* Тестовый формат ARGB8888 */
    {
        draw_buf = lv_draw_buf_create(2, 2, LV_COLOR_FORMAT_ARGB8888, 2 * 4);

        /* Заполните тестовыми данными: белый цвет с 50% альфа. */
        lv_color32_t * pixel = (lv_color32_t *)draw_buf->data;
        for(i = 0; i < 4; i++) {
            pixel[i] = (lv_color32_t) {
                .alpha = 128, .red = 255, .green = 255, .blue = 255
            };
        }

        res = lv_draw_buf_premultiply(draw_buf);
        TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
        TEST_ASSERT(draw_buf->header.flags & LV_IMAGE_FLAGS_PREMULTIPLIED);

        /* Проверьте результат предварительного умножения: цвет должен быть 127 = (255 * 128) >> 8, альфа должна быть 128. */
        for(i = 0; i < 4; i++) {
            TEST_ASSERT_EQUAL_UINT8(127, pixel[i].red);
            TEST_ASSERT_EQUAL_UINT8(127, pixel[i].green);
            TEST_ASSERT_EQUAL_UINT8(127, pixel[i].blue);
            TEST_ASSERT_EQUAL_UINT8(128, pixel[i].alpha); /* Альфа должна остаться неизменной */
        }

        lv_draw_buf_destroy(draw_buf);
    }

    /* Тестовый формат XRGB8888 */
    {
        draw_buf = lv_draw_buf_create(2, 2, LV_COLOR_FORMAT_XRGB8888, 2 * 4);

        /* Заполните тестовыми данными: белый цвет с 50% альфа. */
        lv_color32_t * pixel = (lv_color32_t *)draw_buf->data;
        for(i = 0; i < 4; i++) {
            pixel[i] = (lv_color32_t) {
                .alpha = 255, .red = 255, .green = 255, .blue = 255
            };
        }

        res = lv_draw_buf_premultiply(draw_buf);
        TEST_ASSERT_EQUAL(LV_RESULT_INVALID, res); /* XRGB8888 не поддерживается. */

        lv_draw_buf_destroy(draw_buf);
    }

    /* Тестовый формат RGB565A8 */
    {
        draw_buf = lv_draw_buf_create(2, 2, LV_COLOR_FORMAT_RGB565A8, 2 * 2);

        /* Заполните тестовые данные */
        uint16_t * pixel = (uint16_t *)draw_buf->data;
        lv_opa_t * alpha = (lv_opa_t *)(draw_buf->data + 2 * 2 * 2); /* Размер данных RGB565 */
        for(i = 0; i < 4; i++) {
            pixel[i] = lv_color_to_u16(lv_color_white()); /* White: 0xFFFF in RGB565 */
            alpha[i] = 128; /* 50% альфа */
        }

        res = lv_draw_buf_premultiply(draw_buf);
        TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
        TEST_ASSERT(draw_buf->header.flags & LV_IMAGE_FLAGS_PREMULTIPLIED);

        /* Проверьте результат предварительного умножения: белый ( 0xFFFF ) с альфа 50% должен стать серым. */
        for(i = 0; i < 4; i++) {
            /* RGB565: 5-6-5 bits, white premultiplied by 0.5 should be approximately half intensity */
            TEST_ASSERT_EQUAL_UINT16(0x7BEF, pixel[i]); /* Примерно половина белого в RGB565 */
        }

        lv_draw_buf_destroy(draw_buf);
    }

    /* Тестовый формат ARGB8565 */
    {
        draw_buf = lv_draw_buf_create(2, 2, LV_COLOR_FORMAT_ARGB8565, 2 * 3);

        /* Заполните тестовые данные */
        uint8_t * pixel = draw_buf->data;
        for(i = 0; i < 4; i++) {
            uint16_t c = lv_color_to_u16(lv_color_white()); /* White: 0xFFFF */
            pixel[0] = c & 0xFF;         /* Младший байт RGB565 */
            pixel[1] = (c >> 8) & 0xFF;  /* Старший байт RGB565 */
            pixel[2] = 128;                   /* Альфа */
            pixel += 3;
        }

        res = lv_draw_buf_premultiply(draw_buf);
        TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
        TEST_ASSERT(draw_buf->header.flags & LV_IMAGE_FLAGS_PREMULTIPLIED);

        /* Проверка предварительно умноженного результата */
        pixel = draw_buf->data;
        for(i = 0; i < 4; i++) {
            uint16_t c = (pixel[1] << 8) | pixel[0]; /* Реконструировать RGB565 */
            /* Белый, предварительно умноженный на 0,5, должен составлять примерно половину интенсивности. */
            TEST_ASSERT_EQUAL_UINT16(0x7BEF, c); /* Примерно половина белого в RGB565 */
            TEST_ASSERT_EQUAL_UINT8(128, pixel[2]); /* Альфа должна остаться неизменной */
            pixel += 3;
        }

        lv_draw_buf_destroy(draw_buf);
    }

    /* Тестовый индексированный формат (I1/I2/I4/I8) */
    lv_color_format_t color_formats[] = {
        LV_COLOR_FORMAT_I1,
        LV_COLOR_FORMAT_I2,
        LV_COLOR_FORMAT_I4,
        LV_COLOR_FORMAT_I8,
    };

    for(unsigned int fmt_i = 0; fmt_i < sizeof(color_formats) / sizeof(color_formats[0]); fmt_i++) {
        draw_buf = lv_draw_buf_create(2, 2, color_formats[fmt_i], 0);

        /* Заполните палитру тестовыми данными */
        lv_color32_t * palette = (lv_color32_t *)draw_buf->data;
        int palette_size = LV_COLOR_INDEXED_PALETTE_SIZE(color_formats[fmt_i]);
        for(i = 0; i < palette_size; i++) {
            palette[i] = (lv_color32_t) {
                .alpha = 128, .red = 255, .green = 255, .blue = 255
            };
        }

        res = lv_draw_buf_premultiply(draw_buf);
        TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
        TEST_ASSERT(draw_buf->header.flags & LV_IMAGE_FLAGS_PREMULTIPLIED);

        /* Убедитесь, что палитра была предварительно умножена */
        for(i = 0; i < palette_size; i++) {
            TEST_ASSERT_EQUAL_UINT8(127, palette[i].red);
            TEST_ASSERT_EQUAL_UINT8(127, palette[i].green);
            TEST_ASSERT_EQUAL_UINT8(127, palette[i].blue);
            TEST_ASSERT_EQUAL_UINT8(128, palette[i].alpha);
        }

        lv_draw_buf_destroy(draw_buf);
    }
}
#endif
