#include "app_jpeg_image.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "driver/jpeg_decode.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "sdkconfig.h"

struct app_jpeg_image {
    lv_image_dsc_t dsc;
    uint8_t *jpeg_data;
    uint8_t *pixel_data;
    size_t jpeg_data_size;
    size_t pixel_data_size;
};

static const char *TAG = "jpeg_image";

/*
 * Важный нюанс ESP32-P4 JPEG-декодера:
 * для RGB565 значение JPEG_DEC_RGB_ELEMENT_ORDER_RGB включает big-endian порядок
 * байт. LVGL в этом проекте работает с обычным little-endian RGB565 в памяти.
 * Если оставить RGB, красный и синий выглядят перепутанными. Поэтому для
 * LVGL-картинки используем BGR: в терминах драйвера это нужный порядок байт
 * для 16-битного RGB565-буфера.
 */
#define APP_JPEG_RGB565_ORDER JPEG_DEC_RGB_ELEMENT_ORDER_BGR

static uint32_t align_up_u32(uint32_t value, uint32_t align)
{
    return (value + align - 1U) / align * align;
}

/*
 * Аппаратный JPEG-декодер пишет полные MCU-блоки. Например, YUV420 JPG шириной
 * 547 пикселей внутри декодируется как 560 пикселей. LVGL умеет учитывать это
 * через header.stride: видимая картинка остается 547x800, но строка в памяти
 * может быть шире.
 */
static void get_process_size(const jpeg_decode_picture_info_t *info, uint32_t *process_w, uint32_t *process_h)
{
    uint32_t mcu_w = 8;
    uint32_t mcu_h = 8;

    switch (info->sample_method)
    {
    case JPEG_DOWN_SAMPLING_YUV420:
        mcu_w = 16;
        mcu_h = 16;
        break;
    case JPEG_DOWN_SAMPLING_YUV422:
        mcu_w = 16;
        mcu_h = 8;
        break;
    case JPEG_DOWN_SAMPLING_YUV444:
    case JPEG_DOWN_SAMPLING_GRAY:
    default:
        mcu_w = 8;
        mcu_h = 8;
        break;
    }

    *process_w = align_up_u32(info->width, mcu_w);
    *process_h = align_up_u32(info->height, mcu_h);
}

/*
 * LVGL видит файлы как "S:/...", а fopen() нужен реальный ESP-IDF VFS путь.
 * В текущей конфигурации диск S: указывает на /sdcard/.
 */
static const char *normalize_path(const char *path, char *buf, size_t buf_size)
{
#if defined(CONFIG_LV_FS_STDIO_LETTER) && defined(CONFIG_LV_FS_STDIO_PATH)
    if (path != NULL && path[0] != '\0' && path[1] == ':' &&
        (path[0] == (char)CONFIG_LV_FS_STDIO_LETTER || path[0] == (char)(CONFIG_LV_FS_STDIO_LETTER + 32)))
    {
        const char *tail = path + 2;
        if (*tail == '/' || *tail == '\\')
        {
            tail++;
        }
        snprintf(buf, buf_size, "%s%s", CONFIG_LV_FS_STDIO_PATH, tail);
        return buf;
    }
#endif

    return path;
}

/*
 * Аппаратному JPEG-драйверу нужны DMA/cache-aligned буферы. Поэтому даже для
 * сжатого входного JPG используем jpeg_alloc_decoder_mem(), а не обычный malloc().
 */
static esp_err_t read_file_to_jpeg_buffer(const char *path, uint8_t **out_data, size_t *out_size)
{
    FILE *file = fopen(path, "rb");
    ESP_RETURN_ON_FALSE(file != NULL, ESP_ERR_NOT_FOUND, TAG, "cannot open %s", path);

    esp_err_t ret = ESP_OK;

    if (fseek(file, 0, SEEK_END) != 0)
    {
        ret = ESP_FAIL;
        goto cleanup;
    }

    long file_size = ftell(file);
    if (file_size <= 0)
    {
        ret = ESP_ERR_INVALID_SIZE;
        goto cleanup;
    }

    if (fseek(file, 0, SEEK_SET) != 0)
    {
        ret = ESP_FAIL;
        goto cleanup;
    }

    jpeg_decode_memory_alloc_cfg_t tx_mem_cfg = {
        .buffer_direction = JPEG_DEC_ALLOC_INPUT_BUFFER,
    };

    size_t allocated_size = 0;
    uint8_t *data = jpeg_alloc_decoder_mem((size_t)file_size, &tx_mem_cfg, &allocated_size);
    if (data == NULL)
    {
        ret = ESP_ERR_NO_MEM;
        goto cleanup;
    }

    size_t read_size = fread(data, 1, (size_t)file_size, file);
    if (read_size != (size_t)file_size)
    {
        free(data);
        ret = ESP_FAIL;
        goto cleanup;
    }

    *out_data = data;
    *out_size = (size_t)file_size;

cleanup:
    fclose(file);
    return ret;
}

esp_err_t app_jpeg_image_load_rgb565(const char *path, app_jpeg_image_t **out_image)
{
    ESP_RETURN_ON_FALSE(path != NULL, ESP_ERR_INVALID_ARG, TAG, "path is NULL");
    ESP_RETURN_ON_FALSE(out_image != NULL, ESP_ERR_INVALID_ARG, TAG, "out_image is NULL");

    *out_image = NULL;

    char normalized_path[256];
    const char *file_path = normalize_path(path, normalized_path, sizeof(normalized_path));

    app_jpeg_image_t *image = heap_caps_calloc(1, sizeof(app_jpeg_image_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_RETURN_ON_FALSE(image != NULL, ESP_ERR_NO_MEM, TAG, "no memory for image descriptor");

    esp_err_t ret = read_file_to_jpeg_buffer(file_path, &image->jpeg_data, &image->jpeg_data_size);
    ESP_GOTO_ON_ERROR(ret, cleanup, TAG, "read jpeg failed: %s", file_path);

    jpeg_decode_picture_info_t info = {0};
    ESP_GOTO_ON_ERROR(jpeg_decoder_get_info(image->jpeg_data, image->jpeg_data_size, &info),
                      cleanup, TAG, "jpeg header parse failed");

    uint32_t process_w = 0;
    uint32_t process_h = 0;
    get_process_size(&info, &process_w, &process_h);

    jpeg_decode_memory_alloc_cfg_t rx_mem_cfg = {
        .buffer_direction = JPEG_DEC_ALLOC_OUTPUT_BUFFER,
    };

    const size_t requested_pixel_size = (size_t)process_w * process_h * 2U;
    image->pixel_data = jpeg_alloc_decoder_mem(requested_pixel_size, &rx_mem_cfg, &image->pixel_data_size);
    ESP_GOTO_ON_FALSE(image->pixel_data != NULL, ESP_ERR_NO_MEM, cleanup, TAG, "no PSRAM for decoded image");

    /*
     * Декодируем сразу в RGB565 - родной формат цвета для этого LVGL-проекта.
     * Так не нужен RGB888-буфер и не будет конвертации RGB888->RGB565 при
     * последующих перерисовках.
     */
    jpeg_decoder_handle_t decoder = NULL;
    jpeg_decode_engine_cfg_t decode_eng_cfg = {
        .timeout_ms = -1,
    };
    ESP_GOTO_ON_ERROR(jpeg_new_decoder_engine(&decode_eng_cfg, &decoder), cleanup, TAG, "jpeg engine create failed");

    jpeg_decode_cfg_t decode_cfg = {
        .output_format = JPEG_DECODE_OUT_FORMAT_RGB565,
        .rgb_order = APP_JPEG_RGB565_ORDER,
        .conv_std = JPEG_YUV_RGB_CONV_STD_BT601,
    };

    uint32_t out_size = 0;
    ret = jpeg_decoder_process(decoder,
                               &decode_cfg,
                               image->jpeg_data,
                               (uint32_t)image->jpeg_data_size,
                               image->pixel_data,
                               (uint32_t)image->pixel_data_size,
                               &out_size);

    esp_err_t del_ret = jpeg_del_decoder_engine(decoder);
    if (ret == ESP_OK)
    {
        ret = del_ret;
    }
    ESP_GOTO_ON_ERROR(ret, cleanup, TAG, "jpeg hardware decode failed");

    /*
     * Это обычная LVGL variable image. После lv_image_set_src() LVGL будет
     * рисовать из image->pixel_data, а не из JPG-файла на SD-карте.
     */
    image->dsc.header.magic = LV_IMAGE_HEADER_MAGIC;
    image->dsc.header.cf = LV_COLOR_FORMAT_RGB565;
    image->dsc.header.flags = 0;
    image->dsc.header.w = info.width;
    image->dsc.header.h = info.height;
    image->dsc.header.stride = process_w * 2U;
    image->dsc.data_size = out_size;
    image->dsc.data = image->pixel_data;

    ESP_LOGI(TAG, "Decoded %s: %" PRIu32 "x%" PRIu32 ", stride=%" PRIu32 ", jpg=%u, raw=%" PRIu32,
             file_path, info.width, info.height, image->dsc.header.stride,
             (unsigned)image->jpeg_data_size, out_size);

    *out_image = image;
    return ESP_OK;

cleanup:
    app_jpeg_image_free(image);
    return ret;
}

const lv_image_dsc_t *app_jpeg_image_get_dsc(const app_jpeg_image_t *image)
{
    return image ? &image->dsc : NULL;
}

void app_jpeg_image_free(app_jpeg_image_t *image)
{
    if (image == NULL)
    {
        return;
    }

    free(image->jpeg_data);
    free(image->pixel_data);
    free(image);
}
