#include "music_player/MusicPlayer.hpp"
#include "music_player/MusicPlayerInternal.hpp"

using namespace music_player_internal;

// Playlist scanning and LVGL list population. This file owns the boundary
// between files on the SD card and clickable rows in the UI.

bool MusicPlayer::scan_tracks(void) {
  if (entries_ == nullptr) {
    return false;
  }

  entry_count_ = 0;
  mkdir(kMusicDir, 0775);

  DIR *dir = opendir(kMusicDir);
  if (dir == nullptr) {
    ESP_LOGW(TAG, "opendir failed: %s", kMusicDir);
    return false;
  }

  struct dirent *dir_entry = nullptr;
  while (((dir_entry = readdir(dir)) != nullptr) &&
         (entry_count_ < kMaxEntries)) {
    const char *name = dir_entry->d_name;
    if ((name == nullptr) || (name[0] == '.') || !is_mp3_name(name)) {
      continue;
    }

    TrackEntry *entry = &entries_[entry_count_];
    memset(entry, 0, sizeof(*entry));
    snprintf(entry->name, sizeof(entry->name), "%s", name);
    join_path(entry->path, sizeof(entry->path), kMusicDir, name);

    struct stat st = {};
    if ((stat(entry->path, &st) != 0) || S_ISDIR(st.st_mode)) {
      continue;
    }

    entry->size = static_cast<uint32_t>(st.st_size);
    entry->duration_ms = probe_mp3_duration_ms(entry->path, entry->size);
    entry_count_++;
  }
  closedir(dir);

  for (uint32_t i = 1; i < entry_count_; i++) {
    TrackEntry current = entries_[i];
    uint32_t j = i;
    while ((j > 0) && (str_case_cmp(entries_[j - 1].name, current.name) > 0)) {
      entries_[j] = entries_[j - 1];
      j--;
    }
    entries_[j] = current;
  }

  if ((current_index_ >= static_cast<int>(entry_count_)) ||
      (entry_count_ == 0)) {
    current_index_ = -1;
  }

  return true;
}

// Перестроение списка треков
void MusicPlayer::rebuild_list(void) {
  if (list_ == nullptr) {
    return;
  }

  const bool scan_ok = scan_tracks();
  lv_obj_clean(list_);

  if (!scan_ok) {
    set_status("Folder not found: %s", kMusicDir);
    return;
  }

  if (entry_count_ == 0) {
    set_status("No MP3 files in %s", kMusicDir);
    return;
  }

  for (uint32_t i = 0; i < entry_count_; i++) {
    entry_events_[i] = {
        .app = this,
        .index = i,
    };

    char size_text[24] = {};
    format_size(size_text, sizeof(size_text), entries_[i].size);
    char duration_text[16] = {};
    format_duration_text(duration_text, sizeof(duration_text),
                         entries_[i].duration_ms);

    char text[kNameMax + 56] = {};
    snprintf(text, sizeof(text), "%lu.  %s  %s  %s",
             static_cast<unsigned long>(i + 1U), entries_[i].name,
             duration_text, size_text);

    lv_obj_t *button = lv_list_add_button(list_, nullptr, text);
    style_list_button(button);
    lv_obj_add_event_cb(button, entry_event_cb, LV_EVENT_CLICKED,
                        &entry_events_[i]);
  }

  set_status("%lu MP3 files | PCM5102: BCLK33 LRCK31 DIN30",
             static_cast<unsigned long>(entry_count_));
}

// Воспроизведение трека по индексу
