#pragma once

#include "esp_err.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct app_jpeg_image app_jpeg_image_t;

/**
 * Один раз декодирует JPEG-файл аппаратным JPEG-декодером ESP32-P4 и держит
 * готовые RGB565-пиксели в PSRAM.
 *
 * Зачем это нужно:
 * встроенный LVGL-декодер TJPGD читает JPG потоком прямо во время рисования.
 * Если анимация инвалидирует область картинки, LVGL может снова читать и
 * декодировать JPG, из-за чего резко падает FPS. Эта функция заранее превращает
 * JPG в обычную LVGL bitmap-картинку в памяти.
 *
 * Пример:
 *
 *   static app_jpeg_image_t *bg;
 *
 *   ESP_ERROR_CHECK(app_jpeg_image_load_rgb565("S:/images/bg.jpg", &bg));
 *   lv_image_set_src(ui_Image1, app_jpeg_image_get_dsc(bg));
 *
 * Возвращенный app_jpeg_image_t должен жить все время, пока LVGL может рисовать
 * эту картинку. Вызывать app_jpeg_image_free() можно только после того, как
 * объект lv_image больше ее не использует.
 *
 * Путь может быть LVGL stdio-путем вида "S:/images/bg.jpg" или реальным VFS
 * путем вида "/sdcard/images/bg.jpg".
 */
esp_err_t app_jpeg_image_load_rgb565(const char *path, app_jpeg_image_t **out_image);

/**
 * Возвращает LVGL-дескриптор картинки для lv_image_set_src().
 * Дескриптор и пиксельный буфер принадлежат app_jpeg_image_t.
 */
const lv_image_dsc_t *app_jpeg_image_get_dsc(const app_jpeg_image_t *image);

/**
 * Освобождает дескриптор, исходный JPG-буфер и декодированный RGB565-буфер.
 * Нельзя вызывать, пока LVGL еще может рисовать эту картинку.
 */
void app_jpeg_image_free(app_jpeg_image_t *image);

#ifdef __cplusplus
}
#endif
