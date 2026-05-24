#include "music_player/MusicPlayer.hpp"
#include "music_player/MusicPlayerInternal.hpp"

#include <math.h>

#include "esp_dsp.h"

using namespace music_player_internal;

// Oscilloscope sample buffering and canvas rendering. Audio callbacks publish
// samples into a ring buffer; the UI timer renders snapshots from that buffer.

static int32_t clamp_i32(int32_t value, int32_t min_value, int32_t max_value) {
  if (value < min_value) {
    return min_value;
  }
  if (value > max_value) {
    return max_value;
  }
  return value;
}

static int32_t sample_to_y(int16_t sample, int32_t mid_y, int32_t amplitude,
                           int32_t min_y, int32_t max_y) {
  const int32_t y =
      mid_y - ((static_cast<int32_t>(sample) * amplitude) / 32768);
  return clamp_i32(y, min_y, max_y);
}

static void anim_set_obj_width(void *obj, int32_t value) {
  lv_obj_set_width(static_cast<lv_obj_t *>(obj), static_cast<lv_coord_t>(value));
}

static void anim_set_obj_opa(void *obj, int32_t value) {
  lv_obj_set_style_opa(static_cast<lv_obj_t *>(obj),
                       static_cast<lv_opa_t>(value), 0);
}

static void info_panel_collapse_completed(lv_anim_t *anim) {
  lv_obj_t *panel = static_cast<lv_obj_t *>(lv_anim_get_user_data(anim));
  if (panel != nullptr) {
    lv_obj_add_flag(panel, LV_OBJ_FLAG_HIDDEN);
  }
}

static void fill_rect(uint16_t *buffer, int32_t width, int32_t height,
                      int32_t x0, int32_t y0, int32_t x1, int32_t y1,
                      uint16_t color) {
  if (buffer == nullptr) {
    return;
  }
  x0 = clamp_i32(x0, 0, width - 1);
  x1 = clamp_i32(x1, 0, width - 1);
  y0 = clamp_i32(y0, 0, height - 1);
  y1 = clamp_i32(y1, 0, height - 1);
  if ((x1 < x0) || (y1 < y0)) {
    return;
  }
  for (int32_t y = y0; y <= y1; y++) {
    uint16_t *row = &buffer[y * width];
    for (int32_t x = x0; x <= x1; x++) {
      row[x] = color;
    }
  }
}

static uint16_t spectrum_bar_color(int32_t y, int32_t height) {
  const int32_t level = ((height - 1 - y) * 255) / (height - 1);
  uint8_t red = 0;
  uint8_t green = 0;
  uint8_t blue = 0;

  if (level < 112) {
    red = 0;
    green = static_cast<uint8_t>((level * 120) / 112);
    blue = static_cast<uint8_t>(150 + ((level * 105) / 112));
  } else if (level < 204) {
    const int32_t t = level - 112;
    red = static_cast<uint8_t>((t * 255) / 92);
    green = static_cast<uint8_t>(120 + ((t * 80) / 92));
    blue = static_cast<uint8_t>(255 - ((t * 190) / 92));
  } else {
    const int32_t t = level - 204;
    red = 255;
    green = static_cast<uint8_t>(200 - ((t * 140) / 51));
    blue = static_cast<uint8_t>(65 - ((t * 45) / 51));
  }

  return rgb565((static_cast<uint32_t>(red) << 16) |
                (static_cast<uint32_t>(green) << 8) | blue);
}

bool MusicPlayer::ensure_scope_canvas_buffer(void) {
  if (scope_canvas_buffer_ != nullptr) {
    return true;
  }

  const size_t bytes =
      kScopeExpandedCanvasWidth * kScopeCanvasHeight * sizeof(uint16_t);
  scope_canvas_buffer_ = static_cast<uint16_t *>(
      heap_caps_malloc(bytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
  if (scope_canvas_buffer_ == nullptr) {
    scope_canvas_buffer_ = static_cast<uint16_t *>(
        heap_caps_malloc(bytes, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
  }

  if (scope_canvas_buffer_ != nullptr) {
    memset(scope_canvas_buffer_, 0, bytes);
  }

  return scope_canvas_buffer_ != nullptr;
}

void MusicPlayer::configure_scope_canvas_width(lv_coord_t width) {
  if ((scope_canvas_ == nullptr) || (scope_canvas_buffer_ == nullptr)) {
    return;
  }

  if (width < kScopeCollapsedBoxWidth) {
    width = kScopeCollapsedBoxWidth;
  }
  const lv_coord_t max_width =
      static_cast<lv_coord_t>(kScopeExpandedCanvasWidth);
  if (width > max_width) {
    width = max_width;
  }

  if (scope_canvas_configured_width_ == width) {
    scope_canvas_width_ = width;
    return;
  }

  scope_canvas_width_ = width;
  scope_canvas_configured_width_ = width;
  lv_canvas_set_buffer(scope_canvas_, scope_canvas_buffer_,
                       static_cast<int32_t>(width),
                       static_cast<int32_t>(kScopeCanvasHeight),
                       LV_COLOR_FORMAT_RGB565);
  lv_obj_set_size(scope_canvas_, width,
                  static_cast<lv_coord_t>(kScopeCanvasHeight));
  lv_image_set_inner_align(scope_canvas_, LV_IMAGE_ALIGN_DEFAULT);
  lv_obj_align(scope_canvas_, LV_ALIGN_LEFT_MID, 0, 0);
  scope_rendered_sequence_ = UINT32_MAX;
}

bool MusicPlayer::ensure_spectrum_buffers(void) {
  if (spectrum_canvas_buffer_ == nullptr) {
    const size_t bytes =
        kSpectrumCanvasWidth * kSpectrumCanvasHeight * sizeof(uint16_t);
    spectrum_canvas_buffer_ = static_cast<uint16_t *>(
        heap_caps_malloc(bytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
    if (spectrum_canvas_buffer_ == nullptr) {
      spectrum_canvas_buffer_ = static_cast<uint16_t *>(
          heap_caps_malloc(bytes, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
    }
    if (spectrum_canvas_buffer_ != nullptr) {
      memset(spectrum_canvas_buffer_, 0, bytes);
    }
  }

  if (spectrum_fft_buffer_ == nullptr) {
    const size_t bytes = kSpectrumFftSize * 2 * sizeof(float);
    spectrum_fft_buffer_ = static_cast<float *>(
        heap_caps_malloc(bytes, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
    if (spectrum_fft_buffer_ == nullptr) {
      spectrum_fft_buffer_ = static_cast<float *>(
          heap_caps_malloc(bytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
    }
  }

  if (spectrum_window_ == nullptr) {
    const size_t bytes = kSpectrumFftSize * sizeof(float);
    spectrum_window_ = static_cast<float *>(
        heap_caps_malloc(bytes, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
    if (spectrum_window_ == nullptr) {
      spectrum_window_ = static_cast<float *>(
          heap_caps_malloc(bytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
    }
    if (spectrum_window_ != nullptr) {
      dsps_wind_hann_f32(spectrum_window_,
                         static_cast<int>(kSpectrumFftSize));
    }
  }

  if (!spectrum_fft_ready_) {
    const esp_err_t err =
        dsps_fft2r_init_fc32(nullptr, static_cast<int>(kSpectrumFftSize));
    if (err == ESP_OK) {
      spectrum_fft_ready_ = true;
    } else {
      ESP_LOGW(TAG, "FFT init failed: %s", esp_err_to_name(err));
    }
  }

  return spectrum_canvas_buffer_ != nullptr;
}

// Очистка образцов спектра
void MusicPlayer::clear_scope_samples(void) {
  if (scope_lock_ == nullptr) {
    return;
  }

  SemaphoreHandle_t lock = semaphore_from_handle(scope_lock_);
  if (xSemaphoreTake(lock, pdMS_TO_TICKS(5)) != pdTRUE) {
    return;
  }

  memset(scope_ring_left_, 0, sizeof(scope_ring_left_));
  memset(scope_ring_right_, 0, sizeof(scope_ring_right_));
  memset(spectrum_levels_, 0, sizeof(spectrum_levels_));
  scope_ring_write_idx_ = 0;
  scope_sequence_++;
  spectrum_rendered_sequence_ = UINT32_MAX;
  xSemaphoreGive(lock);
}

// Публикация образцов спектра
void MusicPlayer::publish_scope_samples(const int16_t *samples,
                                        size_t sample_count,
                                        size_t channel_count) {
  if ((scope_lock_ == nullptr) || (samples == nullptr) || (sample_count == 0) ||
      (channel_count == 0)) {
    return;
  }

  const size_t frame_count = sample_count / channel_count;
  if (frame_count == 0) {
    return;
  }

  SemaphoreHandle_t lock = semaphore_from_handle(scope_lock_);
  if (xSemaphoreTake(lock, 0) != pdTRUE) {
    return;
  }

  for (size_t i = 0; i < frame_count; i++) {
    const int16_t *frame = &samples[i * channel_count];
    const size_t idx = scope_ring_write_idx_ & (kScopeRingSize - 1);
    scope_ring_left_[idx] = frame[0];
    scope_ring_right_[idx] = (channel_count > 1) ? frame[1] : frame[0];
    scope_ring_write_idx_++;
  }

  scope_sequence_++;
  xSemaphoreGive(lock);
}

// Отрисовка спектра
void MusicPlayer::render_scope(void) {
  if ((scope_canvas_ == nullptr) || (scope_canvas_buffer_ == nullptr) ||
      (scope_lock_ == nullptr)) {
    return;
  }

  const bool visual_ready = (info_panel_mode_ != InfoPanelMode::Text) &&
                            (spectrum_canvas_ != nullptr) &&
                            ensure_spectrum_buffers();
  bool visual_needs_render = false;

  SemaphoreHandle_t lock = semaphore_from_handle(scope_lock_);
  if (xSemaphoreTake(lock, pdMS_TO_TICKS(2)) != pdTRUE) {
    return;
  }

  const uint32_t sequence = scope_sequence_;
  const bool scope_needs_render = sequence != scope_rendered_sequence_;
  visual_needs_render =
      visual_ready && (sequence != spectrum_rendered_sequence_);
  if (!scope_needs_render && !visual_needs_render) {
    xSemaphoreGive(lock);
    return;
  }

  int32_t samples_per_point = 1;
  if (scope_scale_idx_ >= 2) {
    samples_per_point = 1 << (scope_scale_idx_ - 1);
  }
  const int32_t canvas_width = static_cast<int32_t>(scope_canvas_width_);
  const bool scope_wide = canvas_width > static_cast<int32_t>(kScopeCanvasWidth);
  const int32_t point_capacity = static_cast<int32_t>(
      scope_wide ? kScopeExpandedPointCount : kScopePointCount);
  const int32_t pixel_stride = (scope_scale_idx_ == 0) ? 2 : 1;
  const int32_t points = point_capacity / pixel_stride;
  const int32_t samples_needed = points * samples_per_point;

  const uint32_t end = scope_ring_write_idx_;
  int64_t start = static_cast<int64_t>(end) - samples_needed;
  if (start < 0) {
    start = 0;
  }

  for (int32_t i = 0; i < points; i++) {
    int16_t left_min = INT16_MAX;
    int16_t left_max = INT16_MIN;
    int16_t right_min = INT16_MAX;
    int16_t right_max = INT16_MIN;

    const int64_t bucket_start =
        start + static_cast<int64_t>(i) * samples_per_point;
    for (int32_t sample_offset = 0; sample_offset < samples_per_point;
         sample_offset++) {
      const int64_t sample_idx = bucket_start + sample_offset;
      const size_t ring_idx =
          static_cast<size_t>(sample_idx) & (kScopeRingSize - 1);
      const int16_t left = scope_ring_left_[ring_idx];
      const int16_t right = scope_ring_right_[ring_idx];
      if (left < left_min) {
        left_min = left;
      }
      if (left > left_max) {
        left_max = left;
      }
      if (right < right_min) {
        right_min = right;
      }
      if (right > right_max) {
        right_max = right;
      }
    }

    scope_render_left_[i] = left_min;
    scope_render_left_max_[i] = left_max;
    scope_render_right_[i] = right_min;
    scope_render_right_max_[i] = right_max;
  }

  if (visual_needs_render && (info_panel_mode_ == InfoPanelMode::Spectrum) &&
      (spectrum_fft_buffer_ != nullptr) && (spectrum_window_ != nullptr)) {
    const int64_t spectrum_start =
        static_cast<int64_t>(end) - static_cast<int64_t>(kSpectrumFftSize);
    for (size_t i = 0; i < kSpectrumFftSize; i++) {
      const int64_t sample_idx = spectrum_start + static_cast<int64_t>(i);
      float sample = 0.0f;
      if (sample_idx >= 0) {
        const size_t ring_idx =
            static_cast<size_t>(sample_idx) & (kScopeRingSize - 1);
        const int32_t mono = (static_cast<int32_t>(scope_ring_left_[ring_idx]) +
                              static_cast<int32_t>(scope_ring_right_[ring_idx])) /
                             2;
        sample = static_cast<float>(mono) / 32768.0f;
      }
      spectrum_fft_buffer_[i * 2] = sample * spectrum_window_[i];
      spectrum_fft_buffer_[i * 2 + 1] = 0.0f;
    }
  } else if (visual_needs_render && (info_panel_mode_ == InfoPanelMode::XY)) {
    const int64_t xy_start =
        static_cast<int64_t>(end) - static_cast<int64_t>(kSpectrumFftSize);
    for (size_t i = 0; i < kSpectrumFftSize; i++) {
      const int64_t sample_idx = xy_start + static_cast<int64_t>(i);
      if (sample_idx >= 0) {
        const size_t ring_idx =
            static_cast<size_t>(sample_idx) & (kScopeRingSize - 1);
        xy_render_left_[i] = scope_ring_left_[ring_idx];
        xy_render_right_[i] = scope_ring_right_[ring_idx];
      } else {
        xy_render_left_[i] = 0;
        xy_render_right_[i] = 0;
      }
    }
  }

  xSemaphoreGive(lock);
  scope_rendered_sequence_ = sequence;

  if (scope_label_ != nullptr) {
    const char *scale_texts[kScopeScaleCount] = {
        "0.5x", "1x", "2x",  "4x",
        "8x",   "16x", "32x", "64x",
    };
    lv_label_set_text(scope_label_, scale_texts[scope_scale_idx_]);
  }

  const int32_t width = static_cast<int32_t>(scope_canvas_width_);
  const int32_t height = static_cast<int32_t>(kScopeCanvasHeight);
  const int32_t grid_columns =
      (width > static_cast<int32_t>(kScopeCanvasWidth)) ? 8 : 4;
  const bool is_stereo = (current_channel_mode_ == I2S_SLOT_MODE_STEREO);
  const uint16_t bg = rgb565(0x061006);
  const uint16_t grid = rgb565(0x12351B);
  const uint16_t center = rgb565(0x2F6C3B);
  const uint16_t wave = rgb565(0x69FF7A);
  const uint16_t wave_shadow = rgb565(0xF0A020);

  for (size_t i = 0;
       i < (static_cast<size_t>(width) * kScopeCanvasHeight); i++) {
    scope_canvas_buffer_[i] = bg;
  }

  for (int32_t div = 0; div <= grid_columns; div++) {
    const int32_t x = (div == grid_columns) ? (width - 1)
                                            : ((div * width) / grid_columns);
    scope_draw_line(scope_canvas_buffer_, width, height, x, 0, x, height - 1,
                    grid);
  }
  for (int32_t y = height / 4; y < height; y += height / 4) {
    scope_draw_line(scope_canvas_buffer_, width, height, 0, y, width - 1, y,
                    grid);
  }

  if (is_stereo) {
    const int32_t half_h = height / 2;
    const int32_t mid_l = half_h / 2;
    const int32_t mid_r = half_h + half_h / 2;
    const int32_t amp = (half_h / 2) - 4;

    scope_draw_line(scope_canvas_buffer_, width, height, 0, mid_l, width - 1,
                    mid_l, center);
    scope_draw_line(scope_canvas_buffer_, width, height, 0, mid_r, width - 1,
                    mid_r, center);
    scope_draw_line(scope_canvas_buffer_, width, height, 0, half_h, width - 1,
                    half_h, grid);

    int32_t prev_x = 0;
    int32_t prev_y = mid_l;
    for (int32_t i = 0; i < points; i++) {
      const int32_t x =
          (points > 1) ? ((i * (width - 1)) / (points - 1)) : (width / 2);
      const int32_t y_min =
          sample_to_y(scope_render_left_max_[i], mid_l, amp, 1, half_h - 2);
      const int32_t y_max =
          sample_to_y(scope_render_left_[i], mid_l, amp, 1, half_h - 2);
      const int32_t y_mid = (y_min + y_max) / 2;

      scope_draw_line(scope_canvas_buffer_, width, height, x, y_min + 1, x,
                      y_max + 1, wave_shadow);
      scope_draw_line(scope_canvas_buffer_, width, height, x, y_min, x, y_max,
                      wave);

      if (i > 0) {
        scope_draw_line(scope_canvas_buffer_, width, height, prev_x, prev_y + 1,
                        x, y_mid + 1, wave_shadow);
        scope_draw_line(scope_canvas_buffer_, width, height, prev_x, prev_y, x,
                        y_mid, wave);
      }

      prev_x = x;
      prev_y = y_mid;
    }

    prev_x = 0;
    prev_y = mid_r;
    for (int32_t i = 0; i < points; i++) {
      const int32_t x =
          (points > 1) ? ((i * (width - 1)) / (points - 1)) : (width / 2);
      const int32_t y_min = sample_to_y(scope_render_right_max_[i], mid_r, amp,
                                        half_h + 1, height - 2);
      const int32_t y_max = sample_to_y(scope_render_right_[i], mid_r, amp,
                                        half_h + 1, height - 2);
      const int32_t y_mid = (y_min + y_max) / 2;

      scope_draw_line(scope_canvas_buffer_, width, height, x, y_min + 1, x,
                      y_max + 1, wave_shadow);
      scope_draw_line(scope_canvas_buffer_, width, height, x, y_min, x, y_max,
                      wave);

      if (i > 0) {
        scope_draw_line(scope_canvas_buffer_, width, height, prev_x, prev_y + 1,
                        x, y_mid + 1, wave_shadow);
        scope_draw_line(scope_canvas_buffer_, width, height, prev_x, prev_y, x,
                        y_mid, wave);
      }

      prev_x = x;
      prev_y = y_mid;
    }
  } else {
    const int32_t mid_y = height / 2;
    const int32_t amplitude = (height / 2) - 5;

    scope_draw_line(scope_canvas_buffer_, width, height, 0, mid_y, width - 1,
                    mid_y, center);

    int32_t prev_x = 0;
    int32_t prev_y = mid_y;
    for (int32_t i = 0; i < points; i++) {
      const int32_t x =
          (points > 1) ? ((i * (width - 1)) / (points - 1)) : (width / 2);
      const int32_t y_min =
          sample_to_y(scope_render_left_max_[i], mid_y, amplitude, 1, height - 2);
      const int32_t y_max =
          sample_to_y(scope_render_left_[i], mid_y, amplitude, 1, height - 2);
      const int32_t y_mid = (y_min + y_max) / 2;

      scope_draw_line(scope_canvas_buffer_, width, height, x, y_min + 1, x,
                      y_max + 1, wave_shadow);
      scope_draw_line(scope_canvas_buffer_, width, height, x, y_min, x, y_max,
                      wave);

      if (i > 0) {
        scope_draw_line(scope_canvas_buffer_, width, height, prev_x, prev_y + 1,
                        x, y_mid + 1, wave_shadow);
        scope_draw_line(scope_canvas_buffer_, width, height, prev_x, prev_y, x,
                        y_mid, wave);
      }

      prev_x = x;
      prev_y = y_mid;
    }
  }

  lv_obj_invalidate(scope_canvas_);
  if (visual_needs_render) {
    if (info_panel_mode_ == InfoPanelMode::Spectrum) {
      render_spectrum(sequence);
    } else if (info_panel_mode_ == InfoPanelMode::XY) {
      render_xy_scope(sequence);
    }
  }
}

void MusicPlayer::render_spectrum(uint32_t sequence) {
  if ((spectrum_canvas_ == nullptr) || (spectrum_canvas_buffer_ == nullptr)) {
    return;
  }

  const int32_t width = static_cast<int32_t>(kSpectrumCanvasWidth);
  const int32_t height = static_cast<int32_t>(kSpectrumCanvasHeight);
  const uint16_t bg = rgb565(0x020510);
  const uint16_t grid = rgb565(0x102A58);
  const uint16_t glow = rgb565(0x1B46A8);

  for (size_t i = 0; i < (kSpectrumCanvasWidth * kSpectrumCanvasHeight); i++) {
    spectrum_canvas_buffer_[i] = bg;
  }

  for (int32_t y = height / 4; y < height; y += height / 4) {
    scope_draw_line(spectrum_canvas_buffer_, width, height, 0, y, width - 1, y,
                    grid);
  }

  if (!spectrum_fft_ready_ || (spectrum_fft_buffer_ == nullptr) ||
      (spectrum_window_ == nullptr)) {
    scope_draw_line(spectrum_canvas_buffer_, width, height, 0, height - 1,
                    width - 1, height - 1, glow);
    spectrum_rendered_sequence_ = sequence;
    lv_obj_invalidate(spectrum_canvas_);
    return;
  }

  dsps_fft2r_fc32(spectrum_fft_buffer_, static_cast<int>(kSpectrumFftSize));
  dsps_bit_rev_fc32(spectrum_fft_buffer_, static_cast<int>(kSpectrumFftSize));

  const int32_t first_bin = 1;
  const int32_t last_bin = static_cast<int32_t>(kSpectrumFftSize / 2) - 1;
  const int32_t band_gap = 2;
  const int32_t band_w =
      (width - (static_cast<int32_t>(kSpectrumBandCount) - 1) * band_gap) /
      static_cast<int32_t>(kSpectrumBandCount);

  for (int32_t band = 0; band < static_cast<int32_t>(kSpectrumBandCount);
       band++) {
    const int32_t start =
        first_bin + ((band * band) * (last_bin - first_bin)) /
                        (static_cast<int32_t>(kSpectrumBandCount) *
                         static_cast<int32_t>(kSpectrumBandCount));
    int32_t end = first_bin + (((band + 1) * (band + 1)) *
                               (last_bin - first_bin)) /
                              (static_cast<int32_t>(kSpectrumBandCount) *
                               static_cast<int32_t>(kSpectrumBandCount));
    if (end <= start) {
      end = start + 1;
    }
    if (end > last_bin) {
      end = last_bin;
    }

    float peak = 0.0f;
    for (int32_t bin = start; bin <= end; bin++) {
      const float re = spectrum_fft_buffer_[bin * 2];
      const float im = spectrum_fft_buffer_[bin * 2 + 1];
      const float mag =
          sqrtf((re * re) + (im * im)) / static_cast<float>(kSpectrumFftSize);
      if (mag > peak) {
        peak = mag;
      }
    }

    const float db = 20.0f * log10f(peak + 0.00001f);
    float target = (db + 72.0f) / 60.0f;
    if (target < 0.0f) {
      target = 0.0f;
    } else if (target > 1.0f) {
      target = 1.0f;
    }

    const float previous = spectrum_levels_[band];
    spectrum_levels_[band] =
        target > previous ? (previous * 0.35f) + (target * 0.65f)
                          : (previous * 0.82f) + (target * 0.18f);

    const int32_t bar_h =
        2 + static_cast<int32_t>(spectrum_levels_[band] * (height - 5));
    const int32_t x0 = band * (band_w + band_gap);
    const int32_t x1 = x0 + band_w - 1;
    const int32_t y0 = height - bar_h;

    fill_rect(spectrum_canvas_buffer_, width, height, x0, y0 + 1, x1 + 1,
              height - 1, glow);
    for (int32_t y = y0; y < height; y++) {
      fill_rect(spectrum_canvas_buffer_, width, height, x0, y, x1, y,
                spectrum_bar_color(y, height));
    }
  }

  spectrum_rendered_sequence_ = sequence;
  lv_obj_invalidate(spectrum_canvas_);
}

void MusicPlayer::render_xy_scope(uint32_t sequence) {
  if ((spectrum_canvas_ == nullptr) || (spectrum_canvas_buffer_ == nullptr)) {
    return;
  }

  const int32_t width = static_cast<int32_t>(kSpectrumCanvasWidth);
  const int32_t height = static_cast<int32_t>(kSpectrumCanvasHeight);
  const int32_t mid_x = width / 2;
  const int32_t mid_y = height / 2;
  const int32_t amp_x = (width / 2) - 8;
  const int32_t amp_y = (height / 2) - 8;
  const uint16_t bg = rgb565(0x02040A);
  const uint16_t grid = rgb565(0x102A58);
  const uint16_t axis = rgb565(0x245BC0);
  const uint16_t trace = rgb565(0x58D8FF);
  const uint16_t hot_trace = rgb565(0xFF405C);

  for (size_t i = 0; i < (kSpectrumCanvasWidth * kSpectrumCanvasHeight); i++) {
    spectrum_canvas_buffer_[i] = bg;
  }

  for (int32_t x = width / 4; x < width; x += width / 4) {
    scope_draw_line(spectrum_canvas_buffer_, width, height, x, 0, x,
                    height - 1, grid);
  }
  for (int32_t y = height / 4; y < height; y += height / 4) {
    scope_draw_line(spectrum_canvas_buffer_, width, height, 0, y, width - 1, y,
                    grid);
  }
  scope_draw_line(spectrum_canvas_buffer_, width, height, mid_x, 0, mid_x,
                  height - 1, axis);
  scope_draw_line(spectrum_canvas_buffer_, width, height, 0, mid_y, width - 1,
                  mid_y, axis);

  int32_t prev_x = mid_x;
  int32_t prev_y = mid_y;
  for (size_t i = 0; i < kSpectrumFftSize; i++) {
    const int32_t x =
        clamp_i32(mid_x + ((static_cast<int32_t>(xy_render_left_[i]) * amp_x) /
                           32768),
                  1, width - 2);
    const int32_t y =
        clamp_i32(mid_y - ((static_cast<int32_t>(xy_render_right_[i]) * amp_y) /
                           32768),
                  1, height - 2);
    if (i > 0) {
      const uint16_t color = (i > (kSpectrumFftSize * 3 / 4)) ? hot_trace
                                                              : trace;
      scope_draw_line(spectrum_canvas_buffer_, width, height, prev_x,
                      prev_y + 1, x, y + 1, rgb565(0x12366E));
      scope_draw_line(spectrum_canvas_buffer_, width, height, prev_x, prev_y,
                      x, y, color);
    }
    prev_x = x;
    prev_y = y;
  }

  spectrum_rendered_sequence_ = sequence;
  lv_obj_invalidate(spectrum_canvas_);
}

void MusicPlayer::set_info_panel_mode(InfoPanelMode mode) {
  if ((mode != InfoPanelMode::Text) && (spectrum_canvas_ == nullptr)) {
    mode = InfoPanelMode::Text;
  }
  info_panel_mode_ = mode;
  const bool visual_visible = info_panel_mode_ != InfoPanelMode::Text;
  if (track_label_ != nullptr) {
    if (visual_visible) {
      lv_obj_add_flag(track_label_, LV_OBJ_FLAG_HIDDEN);
    } else {
      lv_obj_clear_flag(track_label_, LV_OBJ_FLAG_HIDDEN);
    }
  }
  if (file_label_ != nullptr) {
    if (visual_visible) {
      lv_obj_add_flag(file_label_, LV_OBJ_FLAG_HIDDEN);
    } else {
      lv_obj_clear_flag(file_label_, LV_OBJ_FLAG_HIDDEN);
    }
  }
  if (state_label_ != nullptr) {
    if (visual_visible) {
      lv_obj_add_flag(state_label_, LV_OBJ_FLAG_HIDDEN);
    } else {
      lv_obj_clear_flag(state_label_, LV_OBJ_FLAG_HIDDEN);
    }
  }
  if (spectrum_canvas_ != nullptr) {
    if (visual_visible) {
      lv_obj_clear_flag(spectrum_canvas_, LV_OBJ_FLAG_HIDDEN);
      spectrum_rendered_sequence_ = UINT32_MAX;
      render_scope();
    } else {
      lv_obj_add_flag(spectrum_canvas_, LV_OBJ_FLAG_HIDDEN);
    }
  }
}

void MusicPlayer::advance_info_panel_mode(void) {
  if (scope_expanded_) {
    return;
  }

  switch (info_panel_mode_) {
  case InfoPanelMode::Text:
    set_info_panel_mode(InfoPanelMode::Spectrum);
    break;
  case InfoPanelMode::Spectrum:
    set_info_panel_mode(InfoPanelMode::XY);
    break;
  case InfoPanelMode::XY:
  default:
    set_info_panel_mode(InfoPanelMode::Text);
    break;
  }
}

void MusicPlayer::set_scope_expanded(bool expanded, bool animate) {
  if ((scope_box_ == nullptr) || (info_panel_ == nullptr)) {
    scope_expanded_ = expanded;
    return;
  }

  if (scope_expanded_ == expanded) {
    return;
  }

  scope_expanded_ = expanded;
  const lv_coord_t start_scope_width = lv_obj_get_width(scope_box_);
  lv_coord_t expanded_width = kScopeCollapsedBoxWidth;
  lv_obj_t *lcd = lv_obj_get_parent(scope_box_);
  if (lcd != nullptr) {
    lv_obj_update_layout(lcd);
    const lv_coord_t content_width = lv_obj_get_content_width(lcd);
    if (content_width > kScopeCollapsedBoxWidth) {
      expanded_width = content_width;
    }
  }
  const lv_coord_t max_canvas_width =
      static_cast<lv_coord_t>(kScopeExpandedCanvasWidth);
  if (expanded_width > max_canvas_width) {
    expanded_width = max_canvas_width;
  }
  const lv_coord_t end_scope_width =
      expanded ? expanded_width : kScopeCollapsedBoxWidth;
  const lv_coord_t start_info_width = lv_obj_get_width(info_panel_);
  const lv_coord_t end_info_width = expanded ? 1 : 1;
  const int32_t start_opa = expanded ? LV_OPA_COVER : LV_OPA_TRANSP;
  const int32_t end_opa = expanded ? LV_OPA_TRANSP : LV_OPA_COVER;

  lv_anim_delete(scope_box_, anim_set_obj_width);
  lv_anim_delete(info_panel_, anim_set_obj_width);
  lv_anim_delete(info_panel_, anim_set_obj_opa);

  if (!expanded) {
    lv_obj_clear_flag(info_panel_, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_flex_grow(info_panel_, 1);
  } else {
    lv_obj_set_flex_grow(info_panel_, 0);
  }

  if (!animate) {
    configure_scope_canvas_width(end_scope_width);
    lv_obj_set_width(scope_box_, end_scope_width);
    lv_obj_set_width(info_panel_, end_info_width);
    lv_obj_set_style_opa(info_panel_, static_cast<lv_opa_t>(end_opa), 0);
    if (expanded) {
      lv_obj_add_flag(info_panel_, LV_OBJ_FLAG_HIDDEN);
    }
    return;
  }

  if (expanded) {
    configure_scope_canvas_width(end_scope_width);
  }

  lv_anim_t scope_anim;
  lv_anim_init(&scope_anim);
  lv_anim_set_var(&scope_anim, scope_box_);
  lv_anim_set_exec_cb(&scope_anim, anim_set_obj_width);
  lv_anim_set_values(&scope_anim, start_scope_width, end_scope_width);
  lv_anim_set_duration(&scope_anim, kScopeExpandAnimMs);
  lv_anim_set_path_cb(&scope_anim, lv_anim_path_ease_in_out);
  if (!expanded) {
    lv_anim_set_completed_cb(&scope_anim,
                             scope_collapse_anim_completed_cb);
    lv_anim_set_user_data(&scope_anim, this);
  }
  lv_anim_start(&scope_anim);

  lv_anim_t info_width_anim;
  lv_anim_init(&info_width_anim);
  lv_anim_set_var(&info_width_anim, info_panel_);
  lv_anim_set_exec_cb(&info_width_anim, anim_set_obj_width);
  lv_anim_set_values(&info_width_anim, start_info_width, end_info_width);
  lv_anim_set_duration(&info_width_anim, kScopeExpandAnimMs);
  lv_anim_set_path_cb(&info_width_anim, lv_anim_path_ease_in_out);
  if (expanded) {
    lv_anim_set_completed_cb(&info_width_anim, info_panel_collapse_completed);
    lv_anim_set_user_data(&info_width_anim, info_panel_);
  }
  lv_anim_start(&info_width_anim);

  lv_anim_t info_opa_anim;
  lv_anim_init(&info_opa_anim);
  lv_anim_set_var(&info_opa_anim, info_panel_);
  lv_anim_set_exec_cb(&info_opa_anim, anim_set_obj_opa);
  lv_anim_set_values(&info_opa_anim, start_opa, end_opa);
  lv_anim_set_duration(&info_opa_anim, kScopeExpandAnimMs);
  lv_anim_set_path_cb(&info_opa_anim, lv_anim_path_ease_in_out);
  lv_anim_start(&info_opa_anim);
}

void MusicPlayer::toggle_scope_expanded(void) {
  set_scope_expanded(!scope_expanded_, true);
}

void MusicPlayer::scope_collapse_anim_completed_cb(lv_anim_t *anim) {
  auto *app = static_cast<MusicPlayer *>(lv_anim_get_user_data(anim));
  if (app == nullptr) {
    return;
  }
  app->configure_scope_canvas_width(kScopeCollapsedBoxWidth);
}



void MusicPlayer::scope_event_cb(lv_event_t *event) {
  if (lv_event_get_code(event) != LV_EVENT_CLICKED) {
    return;
  }

  auto *app = static_cast<MusicPlayer *>(lv_event_get_user_data(event));
  if (app == nullptr) {
    return;
  }
  if (app->scope_long_press_consumed_) {
    app->scope_long_press_consumed_ = false;
    return;
  }
  app->scope_scale_idx_ = (app->scope_scale_idx_ + 1) % kScopeScaleCount;
}

void MusicPlayer::scope_long_event_cb(lv_event_t *event) {
  if (lv_event_get_code(event) != LV_EVENT_LONG_PRESSED) {
    return;
  }

  auto *app = static_cast<MusicPlayer *>(lv_event_get_user_data(event));
  if (app == nullptr) {
    return;
  }
  app->scope_long_press_consumed_ = true;
  app->toggle_scope_expanded();
}

