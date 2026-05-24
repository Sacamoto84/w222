#include "music_player/MusicPlayer.hpp"
#include "music_player/MusicPlayerInternal.hpp"

using namespace music_player_internal;

// App lifetime and shared status plumbing. Heavier playlist, playback, audio,
// scope, and UI callback code lives in the split companion translation units.

MusicPlayer *MusicPlayer::active_player_ = nullptr;

MusicPlayer::MusicPlayer() {
  snprintf(runtime_status_, sizeof(runtime_status_), "Ready");
  refresh_event_.app = this;
  refresh_event_.control = Control::Refresh;
  previous_event_.app = this;
  previous_event_.control = Control::Previous;
  play_event_.app = this;
  play_event_.control = Control::PlayPause;
  stop_event_.app = this;
  stop_event_.control = Control::Stop;
  next_event_.app = this;
  next_event_.control = Control::Next;
  volume_event_.app = this;
  volume_event_.control = Control::Volume;
  seek_event_.app = this;
  seek_event_.control = Control::Seek;
  close_event_.app = this;
  close_event_.control = Control::Close;
}

MusicPlayer::~MusicPlayer() {
  close();

  if (event_queue_ != nullptr) {
    vQueueDelete(event_queue_from_handle(event_queue_));
    event_queue_ = nullptr;
  }

  if (scope_lock_ != nullptr) {
    vSemaphoreDelete(semaphore_from_handle(scope_lock_));
    scope_lock_ = nullptr;
  }

  if (entries_ != nullptr) {
    heap_caps_free(entries_);
    entries_ = nullptr;
  }

  if (entry_events_ != nullptr) {
    heap_caps_free(entry_events_);
    entry_events_ = nullptr;
  }

  if (volume_buffer_ != nullptr) {
    heap_caps_free(volume_buffer_);
    volume_buffer_ = nullptr;
    volume_buffer_bytes_ = 0;
  }

  if (scope_canvas_buffer_ != nullptr) {
    heap_caps_free(scope_canvas_buffer_);
    scope_canvas_buffer_ = nullptr;
  }
}

bool MusicPlayer::init(void) {
  if (entries_ == nullptr) {
    entries_ = static_cast<TrackEntry *>(heap_caps_calloc(
        kMaxEntries, sizeof(TrackEntry), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
  }

  if (entry_events_ == nullptr) {
    entry_events_ = static_cast<EntryEventData *>(
        heap_caps_calloc(kMaxEntries, sizeof(EntryEventData),
                         MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
  }

  if (event_queue_ == nullptr) {
    event_queue_ = xQueueCreate(8, sizeof(int));
  }

  if (scope_lock_ == nullptr) {
    scope_lock_ = xSemaphoreCreateMutex();
  }

  return (entries_ != nullptr) && (entry_events_ != nullptr) &&
         (event_queue_ != nullptr) && (scope_lock_ != nullptr);
}

// Открытие приложения
bool MusicPlayer::open(lv_obj_t *parent) {
  if ((parent == nullptr) || !init()) {
    return false;
  }

  root_ = create_root_(parent);
  create_deck_(root_);
  create_playlist_(root_);

  rebuild_list();
  refresh_ui();
  ui_timer_ = lv_timer_create(ui_timer_cb, kUiRefreshMs, this);
  return true;
}

// Закрытие приложения
void MusicPlayer::close(void) {
  if (ui_timer_ != nullptr) {
    lv_timer_delete(ui_timer_);
    ui_timer_ = nullptr;
  }

  teardown_audio_player();

  if (root_ != nullptr) {
    lv_obj_del(root_);
    root_ = nullptr;
  }

  list_ = nullptr;
  status_label_ = nullptr;
  track_label_ = nullptr;
  file_label_ = nullptr;
  state_label_ = nullptr;
  play_label_ = nullptr;
  volume_slider_ = nullptr;
  volume_label_ = nullptr;
  seek_slider_ = nullptr;
  seek_label_ = nullptr;
  scope_canvas_ = nullptr;
  scope_label_ = nullptr;
  seek_dragging_ = false;
}

// Сканирование треков

void MusicPlayer::set_status(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vsnprintf(runtime_status_, sizeof(runtime_status_), fmt, args);
  va_end(args);

  if (status_label_ != nullptr) {
    lv_label_set_text(status_label_, runtime_status_);
  }
}

