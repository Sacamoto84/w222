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
    bool use_launcher_header(void) const override { return false; }

    bool init(void) override;
    bool open(lv_obj_t *parent) override;
    void close(void) override;

private:
    static constexpr size_t kWaveTableSize = 1024;
    static constexpr size_t kMaxWaveforms = 16;
    static constexpr size_t kWaveNameMax = 32;
    static constexpr size_t kOptionsMax = 768;
    static constexpr size_t kMaxControlEvents = 200;
    static constexpr size_t kAudioFrames = 1024;
    static constexpr size_t kAudioDmaFrames = 512;
    static constexpr size_t kAudioDmaDescNum = 16;

    static constexpr size_t kWavePreviewWidth = 160;
    static constexpr size_t kWavePreviewHeight = 80;

    static constexpr size_t kScopeCanvasWidth = 640;
    static constexpr size_t kScopeCanvasHeight = 118;
    static constexpr size_t kScriptMaxLines = 128;
    static constexpr size_t kScriptLineMax = 64;
    static constexpr size_t kScriptNameMax = 32;
    static constexpr size_t kScriptVisibleLines = 10;
    static constexpr size_t kScriptRegisterCount = 10;
    static constexpr size_t kScriptKeyCount = 16;
    static constexpr size_t kScriptRouteStackMax = 8;
    static constexpr size_t kScriptTokenMax = 8;

    enum class WaveSet : uint8_t {
        Carrier,
        Mod,
    };

    enum class Control : uint8_t {
        StartStop,
        Back,
        ReloadWaves,
        SampleRate,
        VolumeDec,
        VolumeInc,
        SelectChannel,
        ChEnable,
        CarrierWave,
        CarrierFreqEdit,
        CarrierFreqDec,
        CarrierFreqInc,
        AmEnable,
        AmWave,
        AmFreqEdit,
        AmFreqDec,
        AmFreqInc,
        AmFreqSlider,
        FmEnable,
        FmWave,
        FmBaseEdit,
        FmBaseDec,
        FmBaseInc,
        FmDevEdit,
        FmDevDec,
        FmDevInc,
        FmFreqEdit,
        FmFreqDec,
        FmFreqInc,
        ScriptMode,
        ScriptRunStop,
        ScriptNew,
        ScriptSave,
        ScriptSaveAs,
        ScriptLoad,
        ScriptAdd,
        ScriptAddEnd,
        ScriptDelete,
        ScriptUp,
        ScriptDown,
        ScriptSelectLine,
        ScriptListGesture,
        ScriptTemplate,
        ScriptPresetSave,
        ScriptPresetLoad,
    };

    enum class ScriptKeyboardRoute : uint8_t {
        Home,
        Number,
        Register,
        OnOff,
        CramFm,
        CramValue,
        FmValue,
        Comparison,
        IfValue,
        ModCarrier,
        ModAm,
        ModFm,
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

    struct ScriptLine {
        char text[kScriptLineMax];
    };

    struct ScriptKeyboardFrame {
        ScriptKeyboardRoute route;
        uint8_t argument;
        ScriptKeyboardRoute next_route;
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
    lv_obj_t *script_mode_label_ = nullptr;
    lv_obj_t *sample_rate_dropdown_ = nullptr;
    lv_obj_t *volume_label_ = nullptr;
    lv_obj_t *scope_box_ = nullptr;
    lv_obj_t *scope_canvas_ = nullptr;
    lv_obj_t *scope_status_label_ = nullptr;
    lv_timer_t *scope_timer_ = nullptr;
    lv_timer_t *script_timer_ = nullptr;
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
    int rendered_carrier_wave_[2] = {-1, -1};
    int rendered_am_wave_[2] = {-1, -1};
    int rendered_fm_wave_[2] = {-1, -1};
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
    lv_obj_t *script_root_ = nullptr;
    lv_obj_t *script_name_label_ = nullptr;
    lv_obj_t *script_pc_label_ = nullptr;
    lv_obj_t *script_status_label_ = nullptr;
    lv_obj_t *script_run_label_ = nullptr;
    lv_obj_t *script_list_ = nullptr;
    lv_obj_t *script_line_button_[kScriptVisibleLines] = {};
    lv_obj_t *script_line_label_[kScriptVisibleLines] = {};
    lv_obj_t *script_keyboard_ = nullptr;
    lv_obj_t *script_key_button_[kScriptKeyCount] = {};
    lv_obj_t *script_key_label_[kScriptKeyCount] = {};

    ControlEventData control_events_[kMaxControlEvents] = {};
    size_t control_event_count_ = 0;
    uint8_t active_channel_ = 0;
    char runtime_status_[160] = {};
    bool script_mode_ = false;
    bool script_running_ = false;
    bool script_dirty_ = false;
    ScriptLine script_lines_[kScriptMaxLines] = {};
    size_t script_line_count_ = 0;
    size_t script_selected_line_ = 0;
    size_t script_scroll_offset_ = 0;
    size_t script_pc_ = 0;
    TickType_t script_wait_until_ = 0;
    float script_registers_[kScriptRegisterCount] = {};
    char script_name_[kScriptNameMax] = "main";
    ScriptKeyboardRoute script_keyboard_route_ = ScriptKeyboardRoute::Home;
    uint8_t script_keyboard_argument_ = 0;
    ScriptKeyboardRoute script_keyboard_next_route_ = ScriptKeyboardRoute::Home;
    ScriptKeyboardFrame script_keyboard_stack_[kScriptRouteStackMax] = {};
    size_t script_keyboard_stack_count_ = 0;

    void set_default_state(void);
    bool allocate_waveforms(void);
    void load_waveforms(void);
    void build_options(WaveSet set);
    void add_builtin_waveforms(WaveSet set);
    bool add_file_waveform(WaveSet set, const char *path, const char *name);
    bool scan_waveform_dir(WaveSet set, const char *dir_path);
    Waveform *waveforms(WaveSet set);
    size_t waveform_count(WaveSet set) const;
    int find_wave_index(WaveSet set, const char *name) const;

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
    void create_script_view(lv_obj_t *parent);
    void create_script_line(lv_obj_t *parent, size_t visible_index);
    void create_script_side_button(lv_obj_t *parent, const char *text, Control control);
    void create_script_keyboard_button(lv_obj_t *parent, uint8_t key_index);
    lv_obj_t *create_button(lv_obj_t *parent, const char *text, lv_coord_t width);
    lv_obj_t *create_glow_toggle_button(lv_obj_t *parent,
                                        const char *text,
                                        Control control,
                                        uint8_t channel,
                                        uint32_t active_color);
    lv_obj_t *create_text_label(lv_obj_t *parent, const char *text, const lv_font_t *font, uint32_t color);
    lv_obj_t *create_adjust_row(lv_obj_t *parent,
                                const char *name,
                                lv_obj_t **value_label,
                                Control dec_control,
                                Control inc_control,
                                uint8_t channel);
    lv_obj_t *create_frequency_row(lv_obj_t *parent,
                                   const char *name,
                                   lv_obj_t **value_control,
                                   const char *preset_options,
                                   Control edit_control,
                                   uint8_t channel);
    lv_obj_t *create_frequency_preview_row(lv_obj_t *parent,
                                           const char *name,
                                           const char *enable_text,
                                           lv_obj_t **enable_button,
                                           Control enable_control,
                                           uint32_t enable_color,
                                           lv_obj_t **value_control,
                                           const char *preset_options,
                                           Control edit_control,
                                           lv_obj_t **canvas_slot,
                                           uint16_t **buffer_slot,
                                           lv_obj_t **dropdown_slot,
                                           Control wave_control,
                                           uint8_t channel);
    lv_obj_t *create_dual_frequency_row(lv_obj_t *parent,
                                        const char *left_name,
                                        lv_obj_t **left_control,
                                        const char *left_presets,
                                        Control left_edit_control,
                                        const char *right_name,
                                        lv_obj_t **right_control,
                                        const char *right_presets,
                                        Control right_edit_control,
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
    void refresh_script_ui(void);
    void refresh_script_keyboard(void);
    void refresh_wave_dropdown_options(void);
    void create_carrier_preview(lv_obj_t *parent, uint8_t channel);
    void create_mod_preview(lv_obj_t *parent, uint8_t channel, bool fm_preview);
    void create_wave_preview(lv_obj_t *parent,
                             lv_obj_t **canvas_slot,
                             uint16_t **buffer_slot,
                             lv_obj_t **dropdown_slot,
                             Control control,
                             uint8_t channel,
                             bool fill_width);
    bool ensure_wave_preview_buffer(uint16_t **buffer_slot);
    void render_wave_preview(lv_obj_t *canvas, uint16_t *buffer, const Waveform *waveform, uint32_t color);
    void render_channel_previews(uint8_t channel, const ChannelConfig &channel_state);
    void invalidate_wave_previews(void);
    bool ensure_scope_canvas_buffer(void);
    void publish_scope_samples(bool force);
    void clear_scope_samples(void);
    void render_scope(void);
    void set_runtime_status(const char *fmt, ...);

    void script_load_default(void);
    bool script_load_first_file(void);
    bool script_load_file(const char *name);
    bool script_save_file(const char *name);
    bool script_save_as_next_file(void);
    void script_new(void);
    void script_start(void);
    void script_stop(bool reset_pc);
    void script_tick(void);
    bool script_execute_current_line(void);
    bool script_apply_generator_command(char *tokens[], size_t token_count);
    bool script_value_from_token(const char *token, float *value) const;
    int script_register_index(const char *token) const;
    void script_set_line(size_t index, const char *text);
    void script_insert_after_selected(const char *text);
    void script_delete_selected(void);
    void script_move_selected(int direction);
    void script_scroll_lines(int direction);
    void script_keyboard_press(uint8_t key_index);
    void script_keyboard_home(void);
    void script_keyboard_route_to(ScriptKeyboardRoute route,
                                  uint8_t argument,
                                  ScriptKeyboardRoute next_route = ScriptKeyboardRoute::Home);
    void script_keyboard_back(void);
    void script_begin_command(const char *token,
                              ScriptKeyboardRoute route,
                              uint8_t argument,
                              ScriptKeyboardRoute next_route = ScriptKeyboardRoute::Home);
    void script_replace_token(uint8_t token_index, const char *token);
    void script_remove_tokens_from(uint8_t token_index);
    bool script_get_token(uint8_t token_index, char *out, size_t out_size) const;
    void script_append_number(char ch);
    void script_delete_number_char(void);
    void script_finish_number(void);
    size_t script_find_else_or_endif(size_t from_pc, bool *found_else) const;
    size_t script_find_endif(size_t from_pc) const;
    bool save_preset_file(const char *name);
    bool load_preset_file(const char *name);

    static void control_event_cb(lv_event_t *event);
    static void scope_timer_cb(lv_timer_t *timer);
    static void script_timer_cb(lv_timer_t *timer);
    static void audio_task_entry(void *arg);
};
