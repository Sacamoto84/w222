#pragma once

#include "music_player/MusicPlayer.hpp"

#include <dirent.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "driver/gpio.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "misc/lv_text.h"
#include "sdkconfig.h"

namespace music_player_internal {

extern const char *TAG;
static constexpr const char *kMusicDir = CONFIG_BSP_SD_MOUNT_POINT "/music";
static constexpr i2s_port_t kExternalI2sPort = I2S_NUM_0;
static constexpr gpio_num_t kI2sBclkPin = GPIO_NUM_33;
static constexpr gpio_num_t kI2sWsPin = GPIO_NUM_31;
static constexpr gpio_num_t kI2sDoutPin = GPIO_NUM_30;
static constexpr uint32_t kDefaultSampleRateHz = 44100;
static constexpr uint32_t kDefaultBitsPerSample = 16;
static constexpr uint32_t kI2sDmaDescNum = 8;
static constexpr uint32_t kI2sDmaFrames = 512;
static constexpr uint32_t kUiRefreshMs = 160;
#if CONFIG_FREERTOS_UNICORE
static constexpr BaseType_t kAudioTaskCore = tskNO_AFFINITY;
#else
static constexpr BaseType_t kAudioTaskCore = 0;
#endif
static constexpr UBaseType_t kAudioTaskPriority = tskIDLE_PRIORITY + 5;

struct Mp3StreamContext {
  MusicPlayer *app;
  FILE *fp;
  uint32_t file_size;
  uint32_t start_offset;
};

struct Mp3HeaderInfo {
  uint32_t bitrate_kbps;
  uint32_t sample_rate_hz;
  uint32_t samples_per_frame;
  uint32_t frame_size;
  uint8_t channel_mode;
  bool mpeg1;
};

// Case-insensitive filename compare used for deterministic playlist ordering.
int str_case_cmp(const char *left, const char *right);

// Returns true for names ending with ".mp3" regardless of ASCII case.
bool is_mp3_name(const char *name);

// Converts an RGB888 literal to the LVGL RGB565 canvas pixel format.
uint16_t rgb565(uint32_t color);

// Reads enough MP3 frame metadata to estimate playback duration without
// decoding the whole file.
uint32_t probe_mp3_duration_ms(const char *path, uint32_t file_size);

// Joins an SD card directory and filename while handling an existing slash.
void join_path(char *out, size_t out_size, const char *dir, const char *name);

// Formats bytes as KB/MB text for compact playlist rows.
void format_size(char *out, size_t out_size, uint32_t bytes);

// Formats milliseconds as M:SS or H:MM:SS.
void format_time_ms(char *out, size_t out_size, uint32_t ms);

// Formats unknown duration as "--:--", otherwise delegates to format_time_ms.
void format_duration_text(char *out, size_t out_size, uint32_t duration_ms);

// Applies the compact black/green playlist row look to an LVGL list button.
void style_list_button(lv_obj_t *button);

// Draws a clipped Bresenham line directly into the scope canvas buffer.
void scope_draw_line(uint16_t *buffer, int32_t width, int32_t height,
                     int32_t x0, int32_t y0, int32_t x1, int32_t y1,
                     uint16_t color);

// Cast helpers keep FreeRTOS handle types out of the class member layout.
QueueHandle_t event_queue_from_handle(void *handle);
SemaphoreHandle_t semaphore_from_handle(void *handle);

// Maps decoded audio bit depth to the nearest I2S clock/slot configuration.
i2s_mclk_multiple_t mclk_multiple_for_bits(uint32_t bits_per_sample);
i2s_data_bit_width_t i2s_bit_width(uint32_t bits_per_sample);

} // namespace music_player_internal
