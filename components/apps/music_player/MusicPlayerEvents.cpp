#include "music_player/MusicPlayer.hpp"
#include "music_player/MusicPlayerInternal.hpp"

using namespace music_player_internal;

// Static callback adapters. They keep LVGL/audio-player C callbacks tiny and
// forward work back to the owning MusicPlayer instance.

void MusicPlayer::entry_event_cb(lv_event_t *event) {
  EntryEventData *data =
      static_cast<EntryEventData *>(lv_event_get_user_data(event));
  if ((data != nullptr) && (data->app != nullptr)) {
    data->app->play_index(data->index);
  }
}

// Обработка события управления
void MusicPlayer::control_event_cb(lv_event_t *event) {
  ControlEventData *data =
      static_cast<ControlEventData *>(lv_event_get_user_data(event));
  if ((data != nullptr) && (data->app != nullptr)) {
    data->app->handle_control(data->control, event);
  }
}


void MusicPlayer::ui_timer_cb(lv_timer_t *timer) {
  MusicPlayer *app = static_cast<MusicPlayer *>(lv_timer_get_user_data(timer));
  if (app == nullptr) {
    return;
  }

  if (app->event_queue_ != nullptr) {
    int event_id = 0;
    QueueHandle_t queue = event_queue_from_handle(app->event_queue_);
    while (xQueueReceive(queue, &event_id, 0) == pdPASS) {
      app->handle_player_event(event_id);
    }
  }

  app->render_scope();
  app->refresh_ui();
}

