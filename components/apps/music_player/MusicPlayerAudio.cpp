#include "music_player/MusicPlayer.hpp"
#include "music_player/MusicPlayerInternal.hpp"

using namespace music_player_internal;

// Audio-player setup, I2S channel management, and the final decoded PCM write
// path. Volume scaling happens here so the scope sees the same samples as I2S.

bool MusicPlayer::ensure_audio_player(void) {
  if (player_ready_) {
    return true;
  }

  esp_err_t err = init_i2s(kDefaultSampleRateHz, kDefaultBitsPerSample,
                           I2S_SLOT_MODE_STEREO);
  if (err != ESP_OK) {
    set_status("I2S init failed: %s", esp_err_to_name(err));
    return false;
  }

  active_player_ = this;

  audio_player_config_t config = {};
  config.mute_fn = audio_mute_cb;
  config.clk_set_fn = audio_clock_cb;
  config.write_fn = audio_write_cb;
  config.priority = kAudioTaskPriority;
  config.coreID = kAudioTaskCore;

  err = audio_player_new(config);
  if (err != ESP_OK) {
    active_player_ = nullptr;
    deinit_i2s();
    set_status("Audio player init failed: %s", esp_err_to_name(err));
    return false;
  }

  audio_player_callback_register(audio_player_event_cb, this);
  player_ready_ = true;
  muted_ = true;
  return true;
}

// Очистка аудио плеера
void MusicPlayer::teardown_audio_player(void) {
  if (player_ready_) {
    stop_requested_ = true;
    playback_started_ = false;
    audio_player_stop();
    audio_player_delete();
    player_ready_ = false;
  }

  if (active_player_ == this) {
    active_player_ = nullptr;
  }

  deinit_i2s();
}

// Инициализация I2S
esp_err_t MusicPlayer::init_i2s(uint32_t sample_rate_hz,
                                uint32_t bits_per_sample,
                                i2s_slot_mode_t channel_mode) {
  deinit_i2s();

  i2s_chan_config_t chan_cfg =
      I2S_CHANNEL_DEFAULT_CONFIG(kExternalI2sPort, I2S_ROLE_MASTER);
  chan_cfg.dma_desc_num = kI2sDmaDescNum;
  chan_cfg.dma_frame_num = kI2sDmaFrames;
  chan_cfg.auto_clear = true;

  ESP_RETURN_ON_ERROR(i2s_new_channel(&chan_cfg, &tx_chan_, nullptr), TAG,
                      "i2s_new_channel failed");

  i2s_std_config_t std_cfg = {
      .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(sample_rate_hz),
      .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(
          i2s_bit_width(bits_per_sample), channel_mode),
      .gpio_cfg =
          {
              .mclk = GPIO_NUM_NC,
              .bclk = kI2sBclkPin,
              .ws = kI2sWsPin,
              .dout = kI2sDoutPin,
              .din = GPIO_NUM_NC,
              .invert_flags =
                  {
                      .mclk_inv = false,
                      .bclk_inv = false,
                      .ws_inv = false,
                  },
          },
  };
  std_cfg.clk_cfg.clk_src = I2S_CLK_SRC_APLL;
  std_cfg.clk_cfg.mclk_multiple = mclk_multiple_for_bits(bits_per_sample);

  esp_err_t err = i2s_channel_init_std_mode(tx_chan_, &std_cfg);
  if (err != ESP_OK) {
    deinit_i2s();
    ESP_RETURN_ON_ERROR(err, TAG, "i2s_channel_init_std_mode failed");
  }

  err = i2s_channel_enable(tx_chan_);
  if (err != ESP_OK) {
    deinit_i2s();
    ESP_RETURN_ON_ERROR(err, TAG, "i2s_channel_enable failed");
  }

  i2s_enabled_ = true;
  current_sample_rate_ = sample_rate_hz;
  current_bits_per_sample_ = bits_per_sample;
  current_channel_mode_ = channel_mode;
  return ESP_OK;
}

// Очистка I2S
void MusicPlayer::deinit_i2s(void) {
  if (tx_chan_ == nullptr) {
    return;
  }

  if (i2s_enabled_) {
    i2s_channel_disable(tx_chan_);
    i2s_enabled_ = false;
  }

  i2s_del_channel(tx_chan_);
  tx_chan_ = nullptr;
}

// Перенастройка I2S
esp_err_t MusicPlayer::reconfigure_i2s(uint32_t sample_rate_hz,
                                       uint32_t bits_per_sample,
                                       i2s_slot_mode_t channel_mode) {
  if (tx_chan_ == nullptr) {
    return init_i2s(sample_rate_hz, bits_per_sample, channel_mode);
  }

  if (i2s_enabled_) {
    ESP_RETURN_ON_ERROR(i2s_channel_disable(tx_chan_), TAG,
                        "i2s_channel_disable failed");
    i2s_enabled_ = false;
  }

  i2s_std_clk_config_t clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(sample_rate_hz);
  clk_cfg.clk_src = I2S_CLK_SRC_APLL;
  clk_cfg.mclk_multiple = mclk_multiple_for_bits(bits_per_sample);
  ESP_RETURN_ON_ERROR(i2s_channel_reconfig_std_clock(tx_chan_, &clk_cfg), TAG,
                      "i2s_channel_reconfig_std_clock failed");

  i2s_std_slot_config_t slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(
      i2s_bit_width(bits_per_sample), channel_mode);
  ESP_RETURN_ON_ERROR(i2s_channel_reconfig_std_slot(tx_chan_, &slot_cfg), TAG,
                      "i2s_channel_reconfig_std_slot failed");

  ESP_RETURN_ON_ERROR(i2s_channel_enable(tx_chan_), TAG,
                      "i2s_channel_enable failed");
  i2s_enabled_ = true;
  current_sample_rate_ = sample_rate_hz;
  current_bits_per_sample_ = bits_per_sample;
  current_channel_mode_ = channel_mode;
  return ESP_OK;
}


// Проверка буфера громкости
bool MusicPlayer::ensure_volume_buffer(size_t len) {
  if (volume_buffer_bytes_ >= len) {
    return true;
  }

  void *buffer = heap_caps_realloc(volume_buffer_, len,
                                   MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
  if (buffer == nullptr) {
    return false;
  }

  volume_buffer_ = static_cast<int16_t *>(buffer);
  volume_buffer_bytes_ = len;
  return true;
}

void MusicPlayer::silence_output_for(uint32_t duration_ms) {
  TickType_t ticks = pdMS_TO_TICKS(duration_ms);
  if (ticks == 0) {
    ticks = 1;
  }
  audio_silence_until_tick_ =
      static_cast<uint32_t>(xTaskGetTickCount() + ticks);
}

bool MusicPlayer::is_output_silenced(void) const {
  if (audio_silence_until_tick_ == 0) {
    return false;
  }

  const uint32_t now = static_cast<uint32_t>(xTaskGetTickCount());
  return static_cast<int32_t>(audio_silence_until_tick_ - now) > 0;
}

esp_err_t MusicPlayer::write_i2s(void *audio_buffer, size_t len,
                                 size_t *bytes_written, uint32_t timeout_ms) {
  if ((tx_chan_ == nullptr) || (audio_buffer == nullptr) || (len == 0)) {
    if (bytes_written != nullptr) {
      *bytes_written = 0;
    }
    return ESP_ERR_INVALID_STATE;
  }

  const bool output_muted = muted_ || is_output_silenced();

  if (current_bits_per_sample_ != 16) {
    if (output_muted || (volume_percent_ == 0)) {
      const size_t sample_bytes = current_bits_per_sample_ > 0
                                      ? current_bits_per_sample_ / 8U
                                      : sizeof(int16_t);
      const size_t channels =
          current_channel_mode_ == I2S_SLOT_MODE_MONO ? 1U : 2U;
      const size_t frame_bytes = sample_bytes * channels;
      const uint32_t frame_count =
          frame_bytes > 0 ? static_cast<uint32_t>(len / frame_bytes) : 0;
      const uint32_t delay_ms =
          current_sample_rate_ > 0
              ? (frame_count * 1000U) / current_sample_rate_
              : 1U;
      vTaskDelay(pdMS_TO_TICKS(delay_ms > 0 ? delay_ms : 1U));
      if (bytes_written != nullptr) {
        *bytes_written = len;
      }
      return ESP_OK;
    }
    return i2s_channel_write(tx_chan_, audio_buffer, len, bytes_written,
                             timeout_ms);
  }

  const size_t channels = current_channel_mode_ == I2S_SLOT_MODE_MONO ? 1U : 2U;
  const size_t sample_count = len / sizeof(int16_t);
  const bool should_scale = volume_percent_ < 100;
  if (!output_muted && !should_scale) {
    publish_scope_samples(static_cast<const int16_t *>(audio_buffer),
                          sample_count, channels);
    return i2s_channel_write(tx_chan_, audio_buffer, len, bytes_written,
                             timeout_ms);
  }

  if (!ensure_volume_buffer(len)) {
    if (output_muted || (volume_percent_ == 0)) {
      const size_t frame_bytes = sizeof(int16_t) * channels;
      const uint32_t frame_count =
          frame_bytes > 0 ? static_cast<uint32_t>(len / frame_bytes) : 0;
      const uint32_t delay_ms =
          current_sample_rate_ > 0
              ? (frame_count * 1000U) / current_sample_rate_
              : 1U;
      vTaskDelay(pdMS_TO_TICKS(delay_ms > 0 ? delay_ms : 1U));
      if (bytes_written != nullptr) {
        *bytes_written = len;
      }
      return ESP_OK;
    }
    publish_scope_samples(static_cast<const int16_t *>(audio_buffer),
                          sample_count, channels);
    return i2s_channel_write(tx_chan_, audio_buffer, len, bytes_written,
                             timeout_ms);
  }

  if (output_muted || (volume_percent_ == 0)) {
    memset(volume_buffer_, 0, len);
  } else {
    const int16_t *input = static_cast<const int16_t *>(audio_buffer);
    const int volume = volume_percent_;
    for (size_t i = 0; i < sample_count; i++) {
      volume_buffer_[i] =
          static_cast<int16_t>((static_cast<int32_t>(input[i]) * volume) / 100);
    }
  }

  publish_scope_samples(volume_buffer_, sample_count, channels);
  return i2s_channel_write(tx_chan_, volume_buffer_, len, bytes_written,
                           timeout_ms);
}

// Установка режима тишины
esp_err_t MusicPlayer::set_mute(bool muted) {
  muted_ = muted;
  return ESP_OK;
}

// Обработка события входа

void MusicPlayer::audio_player_event_cb(audio_player_cb_ctx_t *ctx) {
  MusicPlayer *app =
      ctx != nullptr ? static_cast<MusicPlayer *>(ctx->user_ctx) : nullptr;
  if ((app == nullptr) || (app->event_queue_ == nullptr)) {
    return;
  }

  const int event_id = static_cast<int>(ctx->audio_event);
  xQueueSend(event_queue_from_handle(app->event_queue_), &event_id, 0);
}


esp_err_t MusicPlayer::audio_mute_cb(AUDIO_PLAYER_MUTE_SETTING setting) {
  return active_player_ != nullptr
             ? active_player_->set_mute(setting == AUDIO_PLAYER_MUTE)
             : ESP_ERR_INVALID_STATE;
}

// Обработка события тактовой частоты
esp_err_t MusicPlayer::audio_clock_cb(uint32_t rate, uint32_t bits_cfg,
                                      i2s_slot_mode_t channel_mode) {
  return active_player_ != nullptr
             ? active_player_->reconfigure_i2s(rate, bits_cfg, channel_mode)
             : ESP_ERR_INVALID_STATE;
}


// Обработка события записи в I2S
esp_err_t MusicPlayer::audio_write_cb(void *audio_buffer, size_t len,
                                      size_t *bytes_written,
                                      uint32_t timeout_ms) {
  return active_player_ != nullptr
             ? active_player_->write_i2s(audio_buffer, len, bytes_written,
                                         timeout_ms)
             : ESP_ERR_INVALID_STATE;
}
