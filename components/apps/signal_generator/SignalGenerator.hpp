#pragma once

#include <stddef.h>
#include <stdint.h>

#include "driver/i2s_std.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "lite_app.h"
#include "lvgl.h"

class SignalGenerator : public LiteApp {
public:
    SignalGenerator();
    ~SignalGenerator() override;

    const char *title(void) const override { return "Generator"; }
    const char *icon_text(void) const override { return "~"; }
    lv_color_t accent_color(void) const override { return lv_color_hex(0xFF8A3D); }

    bool init(void) override;
    bool open(lv_obj_t *parent) override;
    void close(void) override;

private:
    static constexpr size_t kWaveTableSize = 1024;
    static constexpr size_t kMaxWaveforms = 16;
    static constexpr size_t kWaveNameMax = 32;
    static constexpr size_t kOptionsMax = 768;
    static constexpr size_t kMaxControlEvents = 96;
    static constexpr size_t kAudioFrames = 1024;
    static constexpr size_t kAudioDmaFrames = 512;
    static constexpr size_t kAudioDmaDescNum = 16;
    static constexpr size_t kWavePreviewWidth = 640;
    static constexpr size_t kWavePreviewHeight = 86;
    static constexpr size_t kScopeCanvasWidth = 640;
    static constexpr size_t kScopeCanvasHeight = 118;

    enum class WaveSet : uint8_t {
        Carrier,
        Mod,
    };

    enum class Control : uint8_t {
        StartStop,
        ReloadWaves,
        SampleRate,
        VolumeDec,
        VolumeInc,
        SelectChannel,
        ChEnable,
        CarrierWave,
        CarrierFreqDec,
        CarrierFreqInc,
        AmEnable,
        AmWave,
        AmFreqDec,
        AmFreqInc,
        AmFreqSlider,
        FmEnable,
        FmWave,
        FmBaseDec,
        FmBaseInc,
        FmDevDec,
        FmDevInc,
        FmFreqDec,
        FmFreqInc,
    };

    struct Waveform {
        char name[kWaveNameMax];
        float samples[kWaveTableSize];
        bool from_file;
    };

    struct ChannelConfig {
        bool enabled;
        int carrier_wave;
        int am_wave;
        int fm_wave;
        uint32_t carrier_freq_hz;
        bool am_enabled;
        float am_freq_hz;
        bool fm_enabled;
        uint32_t fm_base_hz;
        float fm_dev_hz;
        float fm_freq_hz;
        float gain;
        uint32_t carrier_phase;
        uint32_t am_phase;
        uint32_t fm_phase;
    };

    struct GeneratorState {
        uint32_t sample_rate_hz;
        uint8_t volume_percent;
        ChannelConfig ch[2];
    };

    struct ControlEventData {
        SignalGenerator *app;
        Control control;
        uint8_t channel;
    };

    Waveform *carrier_waves_ = nullptr;
    Waveform *mod_waves_ = nullptr;
    size_t carrier_wave_count_ = 0;
    size_t mod_wave_count_ = 0;
    char carrier_options_[kOptionsMax] = {};
    char mod_options_[kOptionsMax] = {};

    GeneratorState state_ = {};
    SemaphoreHandle_t state_lock_ = nullptr;
    SemaphoreHandle_t scope_lock_ = nullptr;
    i2s_chan_handle_t tx_chan_ = nullptr;
    TaskHandle_t audio_task_handle_ = nullptr;
    volatile bool audio_task_stop_ = false;
    bool i2s_channel_enabled_ = false;
    bool audio_running_ = false;
    int16_t audio_buffer_[kAudioFrames * 2] = {};
    int16_t scope_samples_[kAudioFrames * 2] = {};
    int16_t scope_render_samples_[kAudioFrames * 2] = {};
    uint32_t scope_sequence_ = 0;
    uint32_t scope_rendered_sequence_ = UINT32_MAX;
    uint32_t scope_publish_counter_ = 0;

    lv_obj_t *root_ = nullptr;
    lv_obj_t *status_label_ = nullptr;
    lv_obj_t *start_label_ = nullptr;
    lv_obj_t *sample_rate_dropdown_ = nullptr;
    lv_obj_t *volume_label_ = nullptr;
    lv_obj_t *scope_canvas_ = nullptr;
    lv_obj_t *scope_status_label_ = nullptr;
    lv_timer_t *scope_timer_ = nullptr;
    uint16_t *scope_canvas_buffer_ = nullptr;
    lv_obj_t *tab_bar_ = nullptr;
    lv_obj_t *tab_button_[2] = {};
    lv_obj_t *tab_label_[2] = {};
    lv_obj_t *channel_stack_ = nullptr;
    lv_obj_t *channel_panel_[2] = {};
    lv_obj_t *carrier_preview_canvas_[2] = {};
    lv_obj_t *am_preview_canvas_[2] = {};
    lv_obj_t *fm_preview_canvas_[2] = {};
    uint16_t *carrier_preview_buffer_[2] = {};
    uint16_t *am_preview_buffer_[2] = {};
    uint16_t *fm_preview_buffer_[2] = {};
    lv_obj_t *ch_enable_switch_[2] = {};
    lv_obj_t *carrier_wave_dropdown_[2] = {};
    lv_obj_t *am_wave_dropdown_[2] = {};
    lv_obj_t *fm_wave_dropdown_[2] = {};
    lv_obj_t *carrier_freq_label_[2] = {};
    lv_obj_t *am_freq_label_[2] = {};
    lv_obj_t *am_freq_slider_[2] = {};
    lv_obj_t *fm_base_label_[2] = {};
    lv_obj_t *fm_dev_label_[2] = {};
    lv_obj_t *fm_freq_label_[2] = {};
    lv_obj_t *am_enable_switch_[2] = {};
    lv_obj_t *fm_enable_switch_[2] = {};

    ControlEventData control_events_[kMaxControlEvents] = {};
    size_t control_event_count_ = 0;
    uint8_t active_channel_ = 0;
    char runtime_status_[160] = {};

    void set_default_state(void);
    bool allocate_waveforms(void);
    void load_waveforms(void);
    void build_options(WaveSet set);
    void add_builtin_waveforms(WaveSet set);
    bool add_file_waveform(WaveSet set, const char *path, const char *name);
    bool scan_waveform_dir(WaveSet set, const char *dir_path);
    Waveform *waveforms(WaveSet set);
    size_t waveform_count(WaveSet set) const;

    bool start_audio(void);
    void stop_audio(void);
    esp_err_t init_i2s(uint32_t sample_rate_hz);
    void deinit_i2s(void);
    void audio_task_main(void);
    void fill_audio_buffer(void);
    float lookup_wave(const Waveform *waveform, double phase) const;
    float lookup_wave_q32(const Waveform *waveform, uint32_t phase) const;
    uint32_t phase_increment(float freq_hz, float phase_scale) const;
    float render_channel(ChannelConfig &channel, uint32_t sample_rate_hz, float phase_scale);

    void create_ui(lv_obj_t *parent);
    void create_toolbar(lv_obj_t *parent);
    void create_scope_window(lv_obj_t *parent);
    void create_channel_tabs(lv_obj_t *parent);
    void create_channel_panel(lv_obj_t *parent, uint8_t channel);
    lv_obj_t *create_button(lv_obj_t *parent, const char *text, lv_coord_t width);
    lv_obj_t *create_text_label(lv_obj_t *parent, const char *text, const lv_font_t *font, uint32_t color);
    lv_obj_t *create_adjust_row(lv_obj_t *parent,
                                const char *name,
                                lv_obj_t **value_label,
                                Control dec_control,
                                Control inc_control,
                                uint8_t channel);
    lv_obj_t *create_am_freq_row(lv_obj_t *parent, uint8_t channel);
    lv_obj_t *create_dropdown_row(lv_obj_t *parent,
                                  const char *name,
                                  lv_obj_t **dropdown,
                                  Control control,
                                  uint8_t channel);
    lv_obj_t *create_switch_row(lv_obj_t *parent,
                                const char *name,
                                lv_obj_t **switch_obj,
                                Control control,
                                uint8_t channel);
    ControlEventData *make_control_event(Control control, uint8_t channel);
    void attach_event(lv_obj_t *obj, Control control, uint8_t channel, lv_event_code_t code);
    void handle_control(ControlEventData *data, lv_event_t *event);
    void refresh_ui(void);
    void refresh_wave_dropdown_options(void);
    void create_carrier_preview(lv_obj_t *parent, uint8_t channel);
    void create_mod_preview(lv_obj_t *parent, uint8_t channel, bool fm_preview);
    void create_wave_preview(lv_obj_t *parent, lv_obj_t **canvas_slot, uint16_t **buffer_slot);
    bool ensure_wave_preview_buffer(uint16_t **buffer_slot);
    void render_wave_preview(lv_obj_t *canvas, uint16_t *buffer, const Waveform *waveform, uint32_t color);
    void render_channel_previews(uint8_t channel, const ChannelConfig &channel_state);
    bool ensure_scope_canvas_buffer(void);
    void publish_scope_samples(bool force);
    void clear_scope_samples(void);
    void render_scope(void);
    void set_runtime_status(const char *fmt, ...);

    static void control_event_cb(lv_event_t *event);
    static void scope_timer_cb(lv_timer_t *timer);
    static void audio_task_entry(void *arg);
};
