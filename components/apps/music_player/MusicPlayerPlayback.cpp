#include "music_player/MusicPlayer.hpp"
#include "music_player/MusicPlayerInternal.hpp"

using namespace music_player_internal;

// High-level playback commands and UI-facing state synchronization. Stream
// read/close callbacks stay here because they update playback progress.

bool MusicPlayer::play_index(uint32_t index) {
  return play_index_from_offset(index, 0);
}

// Воспроизведение трека по индексу с offset
bool MusicPlayer::play_index_from_offset(uint32_t index, uint32_t offset) {
  if ((entries_ == nullptr) || (index >= entry_count_)) {
    set_status("No track selected");
    refresh_ui();
    return false;
  }

  if (!ensure_audio_player()) {
    refresh_ui();
    return false;
  }

  const uint32_t file_size = entries_[index].size;
  if (file_size > 0 && offset >= file_size) {
    offset = file_size - 1U;
  }

  FILE *fp = fopen(entries_[index].path, "rb");
  if (fp == nullptr) {
    set_status("Open failed: %s", entries_[index].name);
    refresh_ui();
    return false;
  }

  if ((offset > 0) && (fseek(fp, static_cast<long>(offset), SEEK_SET) != 0)) {
    fclose(fp);
    set_status("Seek failed: %s", entries_[index].name);
    refresh_ui();
    return false;
  }

  Mp3StreamContext *stream_ctx =
      static_cast<Mp3StreamContext *>(heap_caps_calloc(
          1, sizeof(Mp3StreamContext), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
  if (stream_ctx == nullptr) {
    fclose(fp);
    set_status("No memory for MP3 stream");
    refresh_ui();
    return false;
  }

  stream_ctx->app = this;
  stream_ctx->fp = fp;
  stream_ctx->file_size = file_size;
  stream_ctx->start_offset = offset;

  audio_player_stream_t stream = {
      .read_fn = mp3_stream_read_cb,
      .close_fn = mp3_stream_close_cb,
      .user_ctx = stream_ctx,
  };

  const esp_err_t err = audio_player_play_mp3_stream(stream);
  if (err != ESP_OK) {
    fclose(fp);
    heap_caps_free(stream_ctx);
    playback_started_ = false;
    set_status("Play failed: %s", esp_err_to_name(err));
    refresh_ui();
    return false;
  }

  current_index_ = static_cast<int>(index);
  playback_started_ = true;
  stop_requested_ = false;
  playback_byte_offset_ = offset;
  if ((offset > 0) && (file_size > 0)) {
    char time_text[16] = {};
    const uint32_t position_ms =
        entries_[index].duration_ms > 0
            ? static_cast<uint32_t>(
                  (static_cast<uint64_t>(entries_[index].duration_ms) *
                   offset) /
                  file_size)
            : 0;
    format_time_ms(time_text, sizeof(time_text), position_ms);
    set_status("Seek: %s", time_text);
  } else {
    set_status("Queued: %s", entries_[index].name);
  }
  refresh_ui();
  return true;
}

// Воспроизведение предыдущего трека
void MusicPlayer::play_previous(void) {
  if (entry_count_ == 0) {
    set_status("No MP3 files");
    refresh_ui();
    return;
  }

  uint32_t next_index = 0;
  if (current_index_ > 0) {
    next_index = static_cast<uint32_t>(current_index_ - 1);
  } else {
    next_index = entry_count_ - 1;
  }
  play_index(next_index);
}

// Воспроизведение следующего трека
void MusicPlayer::play_next(void) {
  if (entry_count_ == 0) {
    set_status("No MP3 files");
    refresh_ui();
    return;
  }

  uint32_t next_index = 0;
  if (current_index_ >= 0) {
    next_index = (static_cast<uint32_t>(current_index_) + 1U) % entry_count_;
  }
  play_index(next_index);
}


// Переключение воспроизведения/паузы
void MusicPlayer::toggle_play_pause(void) {
  if (!player_ready_) {
    const uint32_t index =
        current_index_ >= 0 ? static_cast<uint32_t>(current_index_) : 0;
    play_index(index);
    return;
  }

  const audio_player_state_t state = audio_player_get_state();
  if (state == AUDIO_PLAYER_STATE_PLAYING) {
    if (audio_player_pause() == ESP_OK) {
      set_status("Pause requested");
    }
  } else if (state == AUDIO_PLAYER_STATE_PAUSE) {
    if (audio_player_resume() == ESP_OK) {
      playback_started_ = true;
      stop_requested_ = false;
      set_status("Resume requested");
    }
  } else {
    const uint32_t index =
        current_index_ >= 0 ? static_cast<uint32_t>(current_index_) : 0;
    play_index(index);
    return;
  }

  refresh_ui();
}

// Остановка воспроизведения
void MusicPlayer::stop_playback(void) {
  stop_requested_ = true;
  playback_started_ = false;
  playback_byte_offset_ = 0;
  clear_scope_samples();

  if (player_ready_) {
    audio_player_stop();
  }

  set_status("Stopped");
  render_scope();
  refresh_ui();
}

// Перемотка к значению слайдера
void MusicPlayer::seek_to_slider_value(int32_t value) {
  if ((current_index_ < 0) ||
      (static_cast<uint32_t>(current_index_) >= entry_count_)) {
    set_status("No track selected");
    return;
  }

  if (value < 0) {
    value = 0;
  } else if (value > kSeekSliderMax) {
    value = kSeekSliderMax;
  }

  const uint32_t file_size = entries_[current_index_].size;
  if (file_size == 0) {
    set_status("Empty track");
    return;
  }

  uint32_t offset = static_cast<uint32_t>(
      (static_cast<uint64_t>(file_size) * static_cast<uint32_t>(value)) /
      static_cast<uint32_t>(kSeekSliderMax));
  if ((file_size > 4096U) && (offset > (file_size - 4096U))) {
    offset = file_size - 4096U;
  }
  if (offset > 8192U) {
    offset -= 8192U;
  } else {
    offset = 0;
  }

  silence_output_for(kSeekOutputMuteMs);
  play_index_from_offset(static_cast<uint32_t>(current_index_), offset);
}

// Обновление метки перемотки
void MusicPlayer::update_seek_label(uint32_t slider_value) {
  if (seek_label_ == nullptr) {
    return;
  }

  if (slider_value > static_cast<uint32_t>(kSeekSliderMax)) {
    slider_value = static_cast<uint32_t>(kSeekSliderMax);
  }

  uint32_t position_ms = 0;
  uint32_t duration_ms = 0;
  if ((current_index_ >= 0) &&
      (static_cast<uint32_t>(current_index_) < entry_count_)) {
    duration_ms = entries_[current_index_].duration_ms;
    if (duration_ms > 0) {
      position_ms = static_cast<uint32_t>(
          (static_cast<uint64_t>(duration_ms) * slider_value) /
          static_cast<uint32_t>(kSeekSliderMax));
    }
  }

  char position_text[16] = {};
  char duration_text[16] = {};
  format_time_ms(position_text, sizeof(position_text), position_ms);
  format_duration_text(duration_text, sizeof(duration_text), duration_ms);
  lv_label_set_text_fmt(seek_label_, "%s/%s", position_text, duration_text);
}

// Обработка управления кнопок управления плеером
void MusicPlayer::handle_control(Control control, lv_event_t *event) {
  const lv_event_code_t event_code =
      event != nullptr ? lv_event_get_code(event) : LV_EVENT_ALL;

  switch (control) {

  case Control::Refresh:
    rebuild_list();
    break;
  case Control::Previous:
    play_previous();
    break;
  case Control::PlayPause:
    toggle_play_pause();
    break;
  case Control::Stop:
    stop_playback();
    break;
  case Control::Next:
    play_next();
    break;
  case Control::Volume:
    if (volume_slider_ != nullptr) {
      volume_percent_ = static_cast<uint8_t>(lv_slider_get_value(volume_slider_));
      set_status("Volume: %u%%", static_cast<unsigned>(volume_percent_));
    }
    break;
  case Control::Seek:
    if (seek_slider_ != nullptr) {
      const int32_t value = lv_slider_get_value(seek_slider_);
      update_seek_label(static_cast<uint32_t>(value));
      if (event_code == LV_EVENT_VALUE_CHANGED) {
        seek_dragging_ = lv_slider_is_dragged(seek_slider_);
      } else if ((event_code == LV_EVENT_RELEASED) ||
                 (event_code == LV_EVENT_PRESS_LOST)) {
        seek_dragging_ = false;
        seek_to_slider_value(value);
      }
    }
    break;
  case Control::Close:
    request_close();
    break;
  }

  refresh_ui();
}

// Обработка события плеера
void MusicPlayer::handle_player_event(int event) {
  const audio_player_callback_event_t audio_event =
      static_cast<audio_player_callback_event_t>(event);
  switch (audio_event) {
  case AUDIO_PLAYER_CALLBACK_EVENT_PLAYING:
    playback_started_ = true;
    stop_requested_ = false;
    set_status("Playing");
    break;
  case AUDIO_PLAYER_CALLBACK_EVENT_COMPLETED_PLAYING_NEXT:
    playback_started_ = true;
    stop_requested_ = false;
    set_status("Next track");
    break;
  case AUDIO_PLAYER_CALLBACK_EVENT_PAUSE:
    set_status("Paused");
    break;
  case AUDIO_PLAYER_CALLBACK_EVENT_IDLE: {
    const bool should_advance = playback_started_ && !stop_requested_ &&
                                (current_index_ >= 0) && (entry_count_ > 1);
    playback_started_ = false;
    stop_requested_ = false;
    if (should_advance) {
      play_next();
      return;
    }
    set_status("Idle");
    break;
  }
  case AUDIO_PLAYER_CALLBACK_EVENT_ERROR:
    playback_started_ = false;
    stop_requested_ = false;
    set_status("Playback error");
    break;
  case AUDIO_PLAYER_CALLBACK_EVENT_UNKNOWN_FILE_TYPE:
    playback_started_ = false;
    stop_requested_ = false;
    set_status("Unknown file type");
    break;
  case AUDIO_PLAYER_CALLBACK_EVENT_SHUTDOWN:
    playback_started_ = false;
    stop_requested_ = false;
    set_status("Audio stopped");
    break;
  case AUDIO_PLAYER_CALLBACK_EVENT_UNKNOWN:
  default:
    break;
  }

  refresh_ui();
}

// Обновление UI
void MusicPlayer::refresh_ui(void) {
  const audio_player_state_t state =
      player_ready_ ? audio_player_get_state() : AUDIO_PLAYER_STATE_IDLE;
  const bool is_playing = state == AUDIO_PLAYER_STATE_PLAYING;
  const bool is_paused = state == AUDIO_PLAYER_STATE_PAUSE;

  if (play_label_ != nullptr) {
    lv_label_set_text(play_label_, is_playing ? LV_SYMBOL_PAUSE : LV_SYMBOL_PLAY);
  }

  if (state_label_ != nullptr) {
    const char *state_text = "IDLE";
    if (is_playing) {
      state_text = "PLAY";
    } else if (is_paused) {
      state_text = "PAUSE";
    } else if (state == AUDIO_PLAYER_STATE_SHUTDOWN) {
      state_text = "STOP";
    }
    const char *mode_text =
        (current_channel_mode_ == I2S_SLOT_MODE_STEREO) ? "STEREO" : "MONO";
    lv_label_set_text_fmt(state_label_, "%s  %luHZ  %luBIT  %s", state_text,
                          static_cast<unsigned long>(current_sample_rate_),
                          static_cast<unsigned long>(current_bits_per_sample_),
                          mode_text);
  }

  if (volume_slider_ != nullptr && !lv_slider_is_dragged(volume_slider_)) {
    lv_slider_set_value(volume_slider_, volume_percent_, LV_ANIM_OFF);
  }
  if (volume_label_ != nullptr) {
    lv_label_set_text_fmt(volume_label_, "%u%%",
                          static_cast<unsigned>(volume_percent_));
  }

  if (seek_slider_ != nullptr) {
    uint32_t slider_value = 0;
    if ((current_index_ >= 0) &&
        (static_cast<uint32_t>(current_index_) < entry_count_)) {
      const uint32_t file_size = entries_[current_index_].size;
      uint32_t byte_offset = playback_byte_offset_;
      if (byte_offset > file_size) {
        byte_offset = file_size;
      }
      if (file_size > 0) {
        slider_value =
            static_cast<uint32_t>((static_cast<uint64_t>(byte_offset) *
                                   static_cast<uint32_t>(kSeekSliderMax)) /
                                  file_size);
      }
    }
    if (slider_value > static_cast<uint32_t>(kSeekSliderMax)) {
      slider_value = static_cast<uint32_t>(kSeekSliderMax);
    }

    if (!seek_dragging_ && !lv_slider_is_dragged(seek_slider_)) {
      lv_slider_set_value(seek_slider_, static_cast<int32_t>(slider_value),
                          LV_ANIM_OFF);
      update_seek_label(slider_value);
    }

  } else {
    update_seek_label(0);
  }

  if ((current_index_ >= 0) &&
      (static_cast<uint32_t>(current_index_) < entry_count_)) {
    if (track_label_ != nullptr) {
      lv_label_set_text_fmt(track_label_, "%lu. %s",
                            static_cast<unsigned long>(current_index_ + 1),
                            entries_[current_index_].name);
    }
    if (file_label_ != nullptr) {
      char size_text[24] = {};
      format_size(size_text, sizeof(size_text), entries_[current_index_].size);
      char duration_text[16] = {};
      format_duration_text(duration_text, sizeof(duration_text),
                           entries_[current_index_].duration_ms);
      lv_label_set_text_fmt(file_label_, "%lu/%lu  %s  %s",
                            static_cast<unsigned long>(current_index_ + 1),
                            static_cast<unsigned long>(entry_count_),
                            duration_text, size_text);
    }
  } else {
    if (track_label_ != nullptr) {
      lv_label_set_text(track_label_, "NO TRACK");
    }
    if (file_label_ != nullptr) {
      lv_label_set_text(file_label_, "");
    }
  }

  if (status_label_ != nullptr) {
    lv_label_set_text(status_label_, runtime_status_);
  }

  if (list_ != nullptr) {
    const uint32_t child_count = lv_obj_get_child_count(list_);
    for (uint32_t i = 0; i < child_count; i++) {
      lv_obj_t *child = lv_obj_get_child(list_, i);
      const bool selected = static_cast<int>(i) == current_index_;
      const uint32_t text_color = selected ? 0xF0A020 : 0x69FF7A;
      if (static_cast<int>(i) == current_index_) {
        lv_obj_set_style_bg_color(child, lv_color_hex(0x11251A), 0);
        lv_obj_set_style_border_color(child, lv_color_hex(0xF0A020), 0);
      } else {
        lv_obj_set_style_bg_color(child, lv_color_hex(0x06080B), 0);
        lv_obj_set_style_border_color(child, lv_color_hex(0x172018), 0);
      }
      const uint32_t nested_count = lv_obj_get_child_count(child);
      for (uint32_t child_index = 0; child_index < nested_count;
           child_index++) {
        lv_obj_set_style_text_color(lv_obj_get_child(child, child_index),
                                    lv_color_hex(text_color), 0);
      }
    }
  }
}

// Установка статуса

int MusicPlayer::mp3_stream_read_cb(void *user_ctx, uint8_t *buffer, size_t len,
                                    bool *is_eof) {
  Mp3StreamContext *stream_ctx = static_cast<Mp3StreamContext *>(user_ctx);
  if ((stream_ctx == nullptr) || (stream_ctx->fp == nullptr) ||
      (buffer == nullptr)) {
    if (is_eof != nullptr) {
      *is_eof = true;
    }
    return -1;
  }

  const size_t bytes_read = fread(buffer, 1, len, stream_ctx->fp);
  const long pos = ftell(stream_ctx->fp);
  if ((pos >= 0) && (stream_ctx->app != nullptr)) {
    stream_ctx->app->playback_byte_offset_ = static_cast<uint32_t>(pos);
  }

  if (is_eof != nullptr) {
    *is_eof = (bytes_read == 0) || feof(stream_ctx->fp);
  }

  if ((bytes_read == 0) && ferror(stream_ctx->fp)) {
    return -1;
  }

  return static_cast<int>(bytes_read);
}

// Закрытие потока MP3
void MusicPlayer::mp3_stream_close_cb(void *user_ctx) {
  Mp3StreamContext *stream_ctx = static_cast<Mp3StreamContext *>(user_ctx);
  if (stream_ctx == nullptr) {
    return;
  }

  if (stream_ctx->fp != nullptr) {
    fclose(stream_ctx->fp);
    stream_ctx->fp = nullptr;
  }

  heap_caps_free(stream_ctx);
}

