#include "music_player/MusicPlayer.hpp"
#include "music_player/MusicPlayerInternal.hpp"

using namespace music_player_internal;

// Oscilloscope sample buffering and canvas rendering. Audio callbacks publish
// samples into a ring buffer; the UI timer renders snapshots from that buffer.

bool MusicPlayer::ensure_scope_canvas_buffer(void) {
  if (scope_canvas_buffer_ != nullptr) {
    return true;
  }

  const size_t bytes =
      kScopeCanvasWidth * kScopeCanvasHeight * sizeof(uint16_t);
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
  scope_ring_write_idx_ = 0;
  scope_sequence_++;
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

  SemaphoreHandle_t lock = semaphore_from_handle(scope_lock_);
  if (xSemaphoreTake(lock, pdMS_TO_TICKS(2)) != pdTRUE) {
    return;
  }

  const uint32_t sequence = scope_sequence_;
  if (sequence == scope_rendered_sequence_) {
    xSemaphoreGive(lock);
    return;
  }

  int32_t samples_per_point = 1;
  if (scope_scale_idx_ >= 2) {
    samples_per_point = 1 << (scope_scale_idx_ - 1);
  }
  const int32_t pixel_stride = (scope_scale_idx_ == 0) ? 2 : 1;
  const int32_t points = static_cast<int32_t>(kScopePointCount) / pixel_stride;
  const int32_t samples_needed = points * samples_per_point;

  const uint32_t end = scope_ring_write_idx_;
  int64_t start = static_cast<int64_t>(end) - samples_needed;
  if (start < 0) {
    start = 0;
  }

  for (int32_t i = 0; i < points; i++) {
    const int64_t sample_idx =
        start + static_cast<int64_t>(i) * samples_per_point;
    const size_t ring_idx =
        static_cast<size_t>(sample_idx) & (kScopeRingSize - 1);
    scope_render_left_[i] = scope_ring_left_[ring_idx];
    scope_render_right_[i] = scope_ring_right_[ring_idx];
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

  const int32_t width = static_cast<int32_t>(kScopeCanvasWidth);
  const int32_t height = static_cast<int32_t>(kScopeCanvasHeight);
  const bool is_stereo = (current_channel_mode_ == I2S_SLOT_MODE_STEREO);
  const uint16_t bg = rgb565(0x061006);
  const uint16_t grid = rgb565(0x12351B);
  const uint16_t center = rgb565(0x2F6C3B);
  const uint16_t wave = rgb565(0x69FF7A);
  const uint16_t wave_shadow = rgb565(0xF0A020);

  for (size_t i = 0; i < (kScopeCanvasWidth * kScopeCanvasHeight); i++) {
    scope_canvas_buffer_[i] = bg;
  }

  for (int32_t x = 0; x < width; x += width / 4) {
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
      int32_t y =
          mid_l - ((static_cast<int32_t>(scope_render_left_[i]) * amp) / 32768);
      if (y < 1) {
        y = 1;
      } else if (y >= (half_h - 1)) {
        y = half_h - 2;
      }

      if (i > 0) {
        scope_draw_line(scope_canvas_buffer_, width, height, prev_x, prev_y + 1,
                        x, y + 1, wave_shadow);
        scope_draw_line(scope_canvas_buffer_, width, height, prev_x, prev_y, x,
                        y, wave);
      }

      prev_x = x;
      prev_y = y;
    }

    prev_x = 0;
    prev_y = mid_r;
    for (int32_t i = 0; i < points; i++) {
      const int32_t x =
          (points > 1) ? ((i * (width - 1)) / (points - 1)) : (width / 2);
      int32_t y =
          mid_r -
          ((static_cast<int32_t>(scope_render_right_[i]) * amp) / 32768);
      if (y < (half_h + 1)) {
        y = half_h + 1;
      } else if (y >= (height - 1)) {
        y = height - 2;
      }

      if (i > 0) {
        scope_draw_line(scope_canvas_buffer_, width, height, prev_x, prev_y + 1,
                        x, y + 1, wave_shadow);
        scope_draw_line(scope_canvas_buffer_, width, height, prev_x, prev_y, x,
                        y, wave);
      }

      prev_x = x;
      prev_y = y;
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
      int32_t y =
          mid_y -
          ((static_cast<int32_t>(scope_render_left_[i]) * amplitude) / 32768);
      if (y < 1) {
        y = 1;
      } else if (y >= (height - 1)) {
        y = height - 2;
      }

      if (i > 0) {
        scope_draw_line(scope_canvas_buffer_, width, height, prev_x, prev_y + 1,
                        x, y + 1, wave_shadow);
        scope_draw_line(scope_canvas_buffer_, width, height, prev_x, prev_y, x,
                        y, wave);
      }

      prev_x = x;
      prev_y = y;
    }
  }

  lv_obj_invalidate(scope_canvas_);
}



void MusicPlayer::scope_event_cb(lv_event_t *event) {
  auto *app = static_cast<MusicPlayer *>(lv_event_get_user_data(event));
  if (app == nullptr) {
    return;
  }
  app->scope_scale_idx_ = (app->scope_scale_idx_ + 1) % kScopeScaleCount;
}

