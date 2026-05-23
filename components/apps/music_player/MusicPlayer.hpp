#pragma once

#include <stddef.h>
#include <stdint.h>

#include "audio_player.h"
#include "driver/i2s_std.h"
#include "lite_app.h"
#include "lvgl.h"

class MusicPlayer : public LiteApp {
public:
    MusicPlayer();
    ~MusicPlayer() override;

    const char *title(void) const override { return "Music"; }
    const char *icon_text(void) const override { return LV_SYMBOL_AUDIO; }
    lv_color_t accent_color(void) const override { return lv_color_hex(0xE05A47); }

    bool init(void) override;
    bool open(lv_obj_t *parent) override;
    void close(void) override;

private:
    static constexpr size_t kMaxEntries = 256;
    static constexpr size_t kNameMax = 128;
    static constexpr size_t kPathMax = 224;

    struct TrackEntry {
        char name[kNameMax];
        char path[kPathMax];
        uint32_t size;
    };

    struct EntryEventData {
        MusicPlayer *app;
        uint32_t index;
    };

    enum class Control {
        Refresh,
        Previous,
        PlayPause,
        Stop,
        Next,
        Volume,
        Seek,
    };

    struct ControlEventData {
        MusicPlayer *app;
        Control control;
    };

    TrackEntry *entries_ = nullptr;
    EntryEventData *entry_events_ = nullptr;
    uint32_t entry_count_ = 0;

    lv_obj_t *root_ = nullptr;
    lv_obj_t *list_ = nullptr;
    lv_obj_t *status_label_ = nullptr;
    lv_obj_t *track_label_ = nullptr;
    lv_obj_t *file_label_ = nullptr;
    lv_obj_t *state_label_ = nullptr;
    lv_obj_t *play_label_ = nullptr;
    lv_obj_t *volume_slider_ = nullptr;
    lv_obj_t *volume_label_ = nullptr;
    lv_obj_t *seek_slider_ = nullptr;
    lv_obj_t *seek_label_ = nullptr;
    lv_obj_t *eq_bars_[12] = {};
    lv_timer_t *ui_timer_ = nullptr;

    void *event_queue_ = nullptr;
    i2s_chan_handle_t tx_chan_ = nullptr;
    bool i2s_enabled_ = false;
    bool player_ready_ = false;
    bool muted_ = true;
    bool playback_started_ = false;
    bool stop_requested_ = false;
    bool seek_dragging_ = false;
    uint8_t volume_percent_ = 100;
    int current_index_ = -1;
    volatile uint32_t playback_byte_offset_ = 0;
    uint32_t current_sample_rate_ = 44100;
    uint32_t current_bits_per_sample_ = 16;
    i2s_slot_mode_t current_channel_mode_ = I2S_SLOT_MODE_STEREO;
    int16_t *volume_buffer_ = nullptr;
    size_t volume_buffer_bytes_ = 0;
    char runtime_status_[96] = {};

    ControlEventData refresh_event_ = {};
    ControlEventData previous_event_ = {};
    ControlEventData play_event_ = {};
    ControlEventData stop_event_ = {};
    ControlEventData next_event_ = {};
    ControlEventData volume_event_ = {};
    ControlEventData seek_event_ = {};

    bool scan_tracks(void);
    void rebuild_list(void);
    bool play_index(uint32_t index);
    bool play_index_from_offset(uint32_t index, uint32_t offset);
    void play_previous(void);
    void play_next(void);
    void toggle_play_pause(void);
    void stop_playback(void);
    void seek_to_slider_value(int32_t value);
    void update_seek_label(uint32_t slider_value);
    void handle_control(Control control, lv_event_t *event);
    void handle_player_event(int event);
    void refresh_ui(void);
    void set_status(const char *fmt, ...);

    bool ensure_audio_player(void);
    void teardown_audio_player(void);
    esp_err_t init_i2s(uint32_t sample_rate_hz, uint32_t bits_per_sample, i2s_slot_mode_t channel_mode);
    void deinit_i2s(void);
    esp_err_t reconfigure_i2s(uint32_t sample_rate_hz, uint32_t bits_per_sample, i2s_slot_mode_t channel_mode);
    esp_err_t write_i2s(void *audio_buffer, size_t len, size_t *bytes_written, uint32_t timeout_ms);
    esp_err_t set_mute(bool muted);
    bool ensure_volume_buffer(size_t len);

    static void entry_event_cb(lv_event_t *event);
    static void control_event_cb(lv_event_t *event);
    static void ui_timer_cb(lv_timer_t *timer);
    static void audio_player_event_cb(audio_player_cb_ctx_t *ctx);
    static int mp3_stream_read_cb(void *user_ctx, uint8_t *buffer, size_t len, bool *is_eof);
    static void mp3_stream_close_cb(void *user_ctx);
    static esp_err_t audio_mute_cb(AUDIO_PLAYER_MUTE_SETTING setting);
    static esp_err_t audio_clock_cb(uint32_t rate, uint32_t bits_cfg, i2s_slot_mode_t channel_mode);
    static esp_err_t audio_write_cb(void *audio_buffer, size_t len, size_t *bytes_written, uint32_t timeout_ms);

    static MusicPlayer *active_player_;
};
