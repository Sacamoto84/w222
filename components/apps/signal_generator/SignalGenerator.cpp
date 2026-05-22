#include "signal_generator/SignalGenerator.hpp"

#include <dirent.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "driver/gpio.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "sdkconfig.h"

namespace {

static const char *TAG = "signal_generator";
static constexpr const char *kCarrierDir = CONFIG_BSP_SD_MOUNT_POINT "/Carrier";
static constexpr const char *kModDir = CONFIG_BSP_SD_MOUNT_POINT "/Mod";
static constexpr const char *kScriptDir = CONFIG_BSP_SD_MOUNT_POINT "/Script";
static constexpr double kPi = 3.14159265358979323846;
static constexpr uint32_t kSampleRates[] = {48000, 96000, 192000, 384000};
static constexpr const char *kSampleRateOptions = "48 kHz\n96 kHz\n192 kHz\n384 kHz";
static constexpr i2s_port_t kExternalI2sPort = I2S_NUM_0;
static constexpr gpio_num_t kI2sBclkPin = GPIO_NUM_33;
static constexpr gpio_num_t kI2sWsPin = GPIO_NUM_31;
static constexpr gpio_num_t kI2sDoutPin = GPIO_NUM_30;
static constexpr uint32_t kI2sWriteTimeoutMs = 100;
static constexpr uint32_t kAudioTaskStackBytes = 8192;
static constexpr UBaseType_t kAudioTaskPriority = tskIDLE_PRIORITY + 4;
static constexpr float kAmFreqMinHz = 0.1f;
static constexpr float kAmFreqMaxHz = 100.0f;
static constexpr int32_t kAmFreqSliderSteps = 1000;
static constexpr uint32_t kScopeRefreshMs = 80;
static constexpr uint32_t kScopePublishEveryBuffers = 8;
static constexpr uint32_t kAudioYieldEveryBuffers = 64;
static constexpr float kQ32PhaseScale = 4294967296.0f;
static constexpr const char *kCarrierFreqPresets =
    "20\n100\n600\n800\n1000\n1500\n2000\n2500\n3000\n3500\n4000\n4800\n6000\n9600\n12000\n20000\n24000\n26000";
static constexpr const char *kAmFmFreqPresets = "0.1\n1.0\n5.5\n10.0\n40.0\n100.0";
static constexpr const char *kFmDevPresets = "1\n10\n50\n100\n250\n500\n1000\n2500\n5000\n10000";
#if CONFIG_FREERTOS_UNICORE
static constexpr BaseType_t kAudioTaskCore = tskNO_AFFINITY;
#else
static constexpr BaseType_t kAudioTaskCore = 0;
#endif

static char ascii_lower(char value)
{
    if ((value >= 'A') && (value <= 'Z')) {
        return static_cast<char>(value + ('a' - 'A'));
    }

    return value;
}

static int str_case_cmp(const char *left, const char *right)
{
    while ((*left != '\0') || (*right != '\0')) {
        const char left_ch = ascii_lower(*left);
        const char right_ch = ascii_lower(*right);
        if (left_ch != right_ch) {
            return static_cast<int>(static_cast<unsigned char>(left_ch)) -
                   static_cast<int>(static_cast<unsigned char>(right_ch));
        }
        if (left_ch == '\0') {
            return 0;
        }

        left++;
        right++;
    }

    return 0;
}

static bool str_ends_with_ignore_case(const char *text, const char *suffix)
{
    const size_t text_len = strlen(text);
    const size_t suffix_len = strlen(suffix);
    if (text_len < suffix_len) {
        return false;
    }

    text += text_len - suffix_len;
    for (size_t i = 0; i < suffix_len; i++) {
        if (ascii_lower(text[i]) != ascii_lower(suffix[i])) {
            return false;
        }
    }

    return true;
}

static void join_path(char *out, size_t out_size, const char *dir, const char *name)
{
    const size_t dir_len = strlen(dir);
    if ((dir_len > 0) && (dir[dir_len - 1] == '/')) {
        snprintf(out, out_size, "%s%s", dir, name);
    } else {
        snprintf(out, out_size, "%s/%s", dir, name);
    }
}

static float clamp_float(float value, float min_value, float max_value)
{
    if (value < min_value) {
        return min_value;
    }
    if (value > max_value) {
        return max_value;
    }

    return value;
}

static uint32_t clamp_u32(uint32_t value, uint32_t min_value, uint32_t max_value)
{
    if (value < min_value) {
        return min_value;
    }
    if (value > max_value) {
        return max_value;
    }

    return value;
}

static size_t clamp_wave_index(int index, size_t count)
{
    if (count == 0) {
        return 0;
    }
    if (index < 0) {
        return 0;
    }
    if (static_cast<size_t>(index) >= count) {
        return count - 1;
    }

    return static_cast<size_t>(index);
}

static uint16_t rgb565(uint32_t color)
{
    return lv_color_to_u16(lv_color_hex(color));
}

static void preview_put_px(uint16_t *buffer, int32_t width, int32_t height, int32_t x, int32_t y, uint16_t color)
{
    if ((buffer == nullptr) || (x < 0) || (y < 0) || (x >= width) || (y >= height)) {
        return;
    }

    buffer[(y * width) + x] = color;
}

static void preview_draw_line(uint16_t *buffer,
                              int32_t width,
                              int32_t height,
                              int32_t x0,
                              int32_t y0,
                              int32_t x1,
                              int32_t y1,
                              uint16_t color)
{
    int32_t dx = abs(x1 - x0);
    int32_t sx = x0 < x1 ? 1 : -1;
    int32_t dy = -abs(y1 - y0);
    int32_t sy = y0 < y1 ? 1 : -1;
    int32_t err = dx + dy;

    while (true) {
        preview_put_px(buffer, width, height, x0, y0, color);
        if ((x0 == x1) && (y0 == y1)) {
            break;
        }

        const int32_t e2 = err * 2;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

static int16_t float_to_i16(float value)
{
    value = clamp_float(value, -1.0f, 1.0f);
    return static_cast<int16_t>(value * 32767.0f);
}

static void make_plain_container(lv_obj_t *obj)
{
    lv_obj_remove_style_all(obj);
    lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_pad_all(obj, 0, 0);
}

static void make_child_passthrough(lv_obj_t *obj)
{
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_EVENT_BUBBLE);
}

static void style_control_box(lv_obj_t *obj)
{
    lv_obj_set_style_radius(obj, 6, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x202834), 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(obj, 1, 0);
    lv_obj_set_style_border_color(obj, lv_color_hex(0x354151), 0);
    lv_obj_set_style_text_color(obj, lv_color_hex(0xEEF4FA), 0);
}

static uint16_t sample_rate_to_dropdown_index(uint32_t sample_rate_hz)
{
    for (uint16_t i = 0; i < (sizeof(kSampleRates) / sizeof(kSampleRates[0])); i++) {
        if (kSampleRates[i] == sample_rate_hz) {
            return i;
        }
    }

    return 2;
}

static void format_hz(char *out, size_t out_size, float value)
{
    if (value < 10.0f) {
        snprintf(out, out_size, "%.1f Hz", static_cast<double>(value));
    } else {
        snprintf(out, out_size, "%.0f Hz", static_cast<double>(value));
    }
}

static i2s_mclk_multiple_t i2s_mclk_multiple_for_rate(uint32_t sample_rate_hz)
{
    return sample_rate_hz >= 384000 ? I2S_MCLK_MULTIPLE_128 : I2S_MCLK_MULTIPLE_256;
}

static float am_freq_from_slider(int32_t slider_value)
{
    if (slider_value < 0) {
        slider_value = 0;
    } else if (slider_value > kAmFreqSliderSteps) {
        slider_value = kAmFreqSliderSteps;
    }

    const float position = static_cast<float>(slider_value) /
                           static_cast<float>(kAmFreqSliderSteps);
    const float log_min = logf(kAmFreqMinHz);
    const float log_max = logf(kAmFreqMaxHz);
    return clamp_float(expf(log_min + ((log_max - log_min) * position)), kAmFreqMinHz, kAmFreqMaxHz);
}

static int32_t am_freq_to_slider(float freq_hz)
{
    const float freq = clamp_float(freq_hz, kAmFreqMinHz, kAmFreqMaxHz);
    const float log_min = logf(kAmFreqMinHz);
    const float log_max = logf(kAmFreqMaxHz);
    const float position = (logf(freq) - log_min) / (log_max - log_min);
    return static_cast<int32_t>((position * static_cast<float>(kAmFreqSliderSteps)) + 0.5f);
}

static float dropdown_selected_float(lv_obj_t *dropdown, float fallback)
{
    if (dropdown == nullptr) {
        return fallback;
    }

    char text[24] = {};
    lv_dropdown_get_selected_str(dropdown, text, sizeof(text));
    char *end = nullptr;
    const float value = strtof(text, &end);
    return end != text ? value : fallback;
}

static float drag_amount_from_indev(void)
{
    lv_indev_t *indev = lv_indev_active();
    if (indev == nullptr) {
        return 0.0f;
    }

    lv_point_t vect = {};
    lv_indev_get_vect(indev, &vect);
    return static_cast<float>(vect.x - vect.y);
}

static void block_dropdown_release_cb(lv_event_t *event)
{
    lv_event_stop_processing(event);
}

} // namespace

SignalGenerator::SignalGenerator()
{
    set_default_state();
    snprintf(runtime_status_, sizeof(runtime_status_),
             "Stopped | PCM5102: BCLK33 LRCK31 DIN30 | CH1=L CH2=R");
}

SignalGenerator::~SignalGenerator()
{
    close();

    if (state_lock_ != nullptr) {
        vSemaphoreDelete(state_lock_);
        state_lock_ = nullptr;
    }

    if (scope_lock_ != nullptr) {
        vSemaphoreDelete(scope_lock_);
        scope_lock_ = nullptr;
    }

    if (carrier_waves_ != nullptr) {
        heap_caps_free(carrier_waves_);
        carrier_waves_ = nullptr;
    }

    if (mod_waves_ != nullptr) {
        heap_caps_free(mod_waves_);
        mod_waves_ = nullptr;
    }

    for (size_t i = 0; i < 2; i++) {
        if (carrier_preview_buffer_[i] != nullptr) {
            heap_caps_free(carrier_preview_buffer_[i]);
            carrier_preview_buffer_[i] = nullptr;
        }
        if (am_preview_buffer_[i] != nullptr) {
            heap_caps_free(am_preview_buffer_[i]);
            am_preview_buffer_[i] = nullptr;
        }
        if (fm_preview_buffer_[i] != nullptr) {
            heap_caps_free(fm_preview_buffer_[i]);
            fm_preview_buffer_[i] = nullptr;
        }
    }

    if (scope_canvas_buffer_ != nullptr) {
        heap_caps_free(scope_canvas_buffer_);
        scope_canvas_buffer_ = nullptr;
    }
}

bool SignalGenerator::init(void)
{
    if (state_lock_ == nullptr) {
        state_lock_ = xSemaphoreCreateMutex();
    }
    if (scope_lock_ == nullptr) {
        scope_lock_ = xSemaphoreCreateMutex();
    }

    if (!allocate_waveforms()) {
        return false;
    }

    load_waveforms();
    return (state_lock_ != nullptr) && (scope_lock_ != nullptr);
}

bool SignalGenerator::open(lv_obj_t *parent)
{
    if ((parent == nullptr) || !init()) {
        return false;
    }

    stop_audio();
    load_waveforms();
    if (script_line_count_ == 0) {
        script_load_default();
        script_load_first_file();
    }
    create_ui(parent);
    refresh_wave_dropdown_options();
    refresh_ui();
    return true;
}

void SignalGenerator::close(void)
{
    script_running_ = false;
    script_wait_until_ = 0;
    stop_audio();

    if (scope_timer_ != nullptr) {
        lv_timer_delete(scope_timer_);
        scope_timer_ = nullptr;
    }
    if (script_timer_ != nullptr) {
        lv_timer_delete(script_timer_);
        script_timer_ = nullptr;
    }

    root_ = nullptr;
    status_label_ = nullptr;
    start_label_ = nullptr;
    script_mode_label_ = nullptr;
    sample_rate_dropdown_ = nullptr;
    volume_label_ = nullptr;
    scope_box_ = nullptr;
    scope_canvas_ = nullptr;
    scope_status_label_ = nullptr;
    tab_bar_ = nullptr;
    channel_stack_ = nullptr;
    script_root_ = nullptr;
    script_name_label_ = nullptr;
    script_pc_label_ = nullptr;
    script_status_label_ = nullptr;
    script_run_label_ = nullptr;
    script_mode_ = false;
    script_list_ = nullptr;
    script_keyboard_ = nullptr;
    for (size_t i = 0; i < 2; i++) {
        tab_button_[i] = nullptr;
        tab_label_[i] = nullptr;
        channel_panel_[i] = nullptr;
        carrier_preview_canvas_[i] = nullptr;
        am_preview_canvas_[i] = nullptr;
        fm_preview_canvas_[i] = nullptr;
        rendered_carrier_wave_[i] = -1;
        rendered_am_wave_[i] = -1;
        rendered_fm_wave_[i] = -1;
        ch_enable_switch_[i] = nullptr;
        carrier_wave_dropdown_[i] = nullptr;
        am_wave_dropdown_[i] = nullptr;
        fm_wave_dropdown_[i] = nullptr;
        carrier_freq_label_[i] = nullptr;
        am_freq_label_[i] = nullptr;
        fm_base_label_[i] = nullptr;
        fm_dev_label_[i] = nullptr;
        fm_freq_label_[i] = nullptr;
        am_freq_slider_[i] = nullptr;
        am_enable_switch_[i] = nullptr;
        fm_enable_switch_[i] = nullptr;
    }
    for (size_t i = 0; i < kScriptVisibleLines; i++) {
        script_line_button_[i] = nullptr;
        script_line_label_[i] = nullptr;
    }
    for (size_t i = 0; i < kScriptKeyCount; i++) {
        script_key_button_[i] = nullptr;
        script_key_label_[i] = nullptr;
    }
    control_event_count_ = 0;
    active_channel_ = 0;
}

void SignalGenerator::set_default_state(void)
{
    state_ = {};
    state_.sample_rate_hz = 192000;
    state_.volume_percent = 100;

    state_.ch[0] = {
        .enabled = true,
        .carrier_wave = 0,
        .am_wave = 0,
        .fm_wave = 0,
        .carrier_freq_hz = 1000,
        .am_enabled = false,
        .am_freq_hz = 5.0f,
        .fm_enabled = false,
        .fm_base_hz = 1000,
        .fm_dev_hz = 100.0f,
        .fm_freq_hz = 5.0f,
        .gain = 0.85f,
        .carrier_phase = 0,
        .am_phase = 0,
        .fm_phase = 0,
    };

    state_.ch[1] = state_.ch[0];
    state_.ch[1].enabled = false;
    state_.ch[1].carrier_freq_hz = 500;
    state_.ch[1].fm_base_hz = 500;
}

bool SignalGenerator::allocate_waveforms(void)
{
    if (carrier_waves_ == nullptr) {
        carrier_waves_ = static_cast<Waveform *>(heap_caps_calloc(kMaxWaveforms, sizeof(Waveform),
                                                                  MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
        if (carrier_waves_ == nullptr) {
            carrier_waves_ = static_cast<Waveform *>(heap_caps_calloc(kMaxWaveforms, sizeof(Waveform),
                                                                      MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
        }
    }

    if (mod_waves_ == nullptr) {
        mod_waves_ = static_cast<Waveform *>(heap_caps_calloc(kMaxWaveforms, sizeof(Waveform),
                                                              MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
        if (mod_waves_ == nullptr) {
            mod_waves_ = static_cast<Waveform *>(heap_caps_calloc(kMaxWaveforms, sizeof(Waveform),
                                                                  MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
        }
    }

    return (carrier_waves_ != nullptr) && (mod_waves_ != nullptr);
}

SignalGenerator::Waveform *SignalGenerator::waveforms(WaveSet set)
{
    return set == WaveSet::Carrier ? carrier_waves_ : mod_waves_;
}

size_t SignalGenerator::waveform_count(WaveSet set) const
{
    return set == WaveSet::Carrier ? carrier_wave_count_ : mod_wave_count_;
}

int SignalGenerator::find_wave_index(WaveSet set, const char *name) const
{
    if ((name == nullptr) || (name[0] == '\0')) {
        return -1;
    }

    const Waveform *items = set == WaveSet::Carrier ? carrier_waves_ : mod_waves_;
    const size_t count = waveform_count(set);
    if (items == nullptr) {
        return -1;
    }

    for (size_t i = 0; i < count; i++) {
        if (str_case_cmp(items[i].name, name) == 0) {
            return static_cast<int>(i);
        }
    }

    char name_with_ext[kWaveNameMax + 5] = {};
    snprintf(name_with_ext, sizeof(name_with_ext), "%s.dat", name);
    for (size_t i = 0; i < count; i++) {
        if (str_case_cmp(items[i].name, name_with_ext) == 0) {
            return static_cast<int>(i);
        }
    }

    for (size_t i = 0; i < count; i++) {
        char item_name[kWaveNameMax] = {};
        snprintf(item_name, sizeof(item_name), "%s", items[i].name);
        if (str_ends_with_ignore_case(item_name, ".dat")) {
            item_name[strlen(item_name) - 4] = '\0';
        }
        if (str_case_cmp(item_name, name) == 0) {
            return static_cast<int>(i);
        }
    }

    return -1;
}

void SignalGenerator::load_waveforms(void)
{
    if (!allocate_waveforms()) {
        return;
    }

    carrier_wave_count_ = 0;
    mod_wave_count_ = 0;
    memset(carrier_waves_, 0, sizeof(Waveform) * kMaxWaveforms);
    memset(mod_waves_, 0, sizeof(Waveform) * kMaxWaveforms);

    add_builtin_waveforms(WaveSet::Carrier);
    add_builtin_waveforms(WaveSet::Mod);
    scan_waveform_dir(WaveSet::Carrier, kCarrierDir);
    scan_waveform_dir(WaveSet::Mod, kModDir);
    build_options(WaveSet::Carrier);
    build_options(WaveSet::Mod);
    invalidate_wave_previews();

    if (state_lock_ != nullptr && xSemaphoreTake(state_lock_, portMAX_DELAY) == pdTRUE) {
        for (size_t i = 0; i < 2; i++) {
            state_.ch[i].carrier_wave = static_cast<int>(clamp_u32(state_.ch[i].carrier_wave, 0,
                                                                   carrier_wave_count_ > 0 ? carrier_wave_count_ - 1 : 0));
            state_.ch[i].am_wave = static_cast<int>(clamp_u32(state_.ch[i].am_wave, 0,
                                                              mod_wave_count_ > 0 ? mod_wave_count_ - 1 : 0));
            state_.ch[i].fm_wave = static_cast<int>(clamp_u32(state_.ch[i].fm_wave, 0,
                                                              mod_wave_count_ > 0 ? mod_wave_count_ - 1 : 0));
        }
        xSemaphoreGive(state_lock_);
    }
}

void SignalGenerator::build_options(WaveSet set)
{
    const Waveform *items = set == WaveSet::Carrier ? carrier_waves_ : mod_waves_;
    const size_t count = waveform_count(set);
    char *options = set == WaveSet::Carrier ? carrier_options_ : mod_options_;
    options[0] = '\0';

    size_t used = 0;
    for (size_t i = 0; i < count; i++) {
        const int written = snprintf(options + used,
                                     kOptionsMax - used,
                                     "%s%s",
                                     i == 0 ? "" : "\n",
                                     items[i].name);
        if (written <= 0) {
            break;
        }

        const size_t add = static_cast<size_t>(written);
        if (add >= (kOptionsMax - used)) {
            options[kOptionsMax - 1] = '\0';
            break;
        }

        used += add;
    }
}

void SignalGenerator::add_builtin_waveforms(WaveSet set)
{
    Waveform *items = waveforms(set);
    size_t *count = set == WaveSet::Carrier ? &carrier_wave_count_ : &mod_wave_count_;
    if ((items == nullptr) || (*count + 4 > kMaxWaveforms)) {
        return;
    }

    const char *names[] = {"Sine", "Square", "Triangle", "Saw"};
    for (size_t wave_index = 0; wave_index < 4; wave_index++) {
        Waveform &wave = items[*count];
        snprintf(wave.name, sizeof(wave.name), "%s", names[wave_index]);
        wave.from_file = false;

        for (size_t i = 0; i < kWaveTableSize; i++) {
            const float phase = static_cast<float>(i) / static_cast<float>(kWaveTableSize);
            float value = 0.0f;
            switch (wave_index) {
            case 0:
                value = sinf(static_cast<float>(2.0 * kPi) * phase);
                break;
            case 1:
                value = phase < 0.5f ? 1.0f : -1.0f;
                break;
            case 2:
                value = (2.0f * fabsf((2.0f * phase) - 1.0f)) - 1.0f;
                break;
            default:
                value = (2.0f * phase) - 1.0f;
                break;
            }
            wave.samples[i] = value;
        }

        (*count)++;
    }
}

bool SignalGenerator::add_file_waveform(WaveSet set, const char *path, const char *name)
{
    Waveform *items = waveforms(set);
    size_t *count = set == WaveSet::Carrier ? &carrier_wave_count_ : &mod_wave_count_;
    if ((items == nullptr) || (path == nullptr) || (name == nullptr) || (*count >= kMaxWaveforms)) {
        return false;
    }

    FILE *file = fopen(path, "rb");
    if (file == nullptr) {
        return false;
    }

    uint8_t raw[kWaveTableSize * 2] = {};
    const size_t bytes_read = fread(raw, 1, sizeof(raw), file);
    fclose(file);
    if (bytes_read < sizeof(raw)) {
        ESP_LOGW(TAG, "Wave file is too small: %s", path);
        return false;
    }

    Waveform &wave = items[*count];
    snprintf(wave.name, sizeof(wave.name), "%s", name);
    wave.from_file = true;

    for (size_t i = 0; i < kWaveTableSize; i++) {
        const uint16_t sample = static_cast<uint16_t>(raw[i * 2]) |
                                (static_cast<uint16_t>(raw[(i * 2) + 1]) << 8);
        wave.samples[i] = clamp_float((static_cast<float>(sample) - 2048.0f) / 2048.0f, -1.0f, 1.0f);
    }

    (*count)++;
    return true;
}

bool SignalGenerator::scan_waveform_dir(WaveSet set, const char *dir_path)
{
    DIR *dir = opendir(dir_path);
    if (dir == nullptr) {
        ESP_LOGW(TAG, "Waveform folder not found: %s", dir_path);
        return false;
    }

    char paths[kMaxWaveforms][160] = {};
    char names[kMaxWaveforms][kWaveNameMax] = {};
    size_t found = 0;

    struct dirent *entry = nullptr;
    while (((entry = readdir(dir)) != nullptr) && (found < kMaxWaveforms)) {
        const char *name = entry->d_name;
        if ((name == nullptr) || (name[0] == '.') || !str_ends_with_ignore_case(name, ".dat")) {
            continue;
        }

        char path[160] = {};
        join_path(path, sizeof(path), dir_path, name);
        struct stat st = {};
        if ((stat(path, &st) != 0) || S_ISDIR(st.st_mode)) {
            continue;
        }

        snprintf(paths[found], sizeof(paths[found]), "%s", path);
        snprintf(names[found], sizeof(names[found]), "%s", name);
        found++;
    }

    closedir(dir);

    for (size_t i = 1; i < found; i++) {
        char path[160] = {};
        char name[kWaveNameMax] = {};
        snprintf(path, sizeof(path), "%s", paths[i]);
        snprintf(name, sizeof(name), "%s", names[i]);

        size_t j = i;
        while ((j > 0) && (str_case_cmp(names[j - 1], name) > 0)) {
            snprintf(paths[j], sizeof(paths[j]), "%s", paths[j - 1]);
            snprintf(names[j], sizeof(names[j]), "%s", names[j - 1]);
            j--;
        }

        snprintf(paths[j], sizeof(paths[j]), "%s", path);
        snprintf(names[j], sizeof(names[j]), "%s", name);
    }

    for (size_t i = 0; i < found; i++) {
        if (waveform_count(set) >= kMaxWaveforms) {
            break;
        }
        add_file_waveform(set, paths[i], names[i]);
    }

    return true;
}

esp_err_t SignalGenerator::init_i2s(uint32_t sample_rate_hz)
{
    deinit_i2s();

    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(kExternalI2sPort, I2S_ROLE_MASTER);
    chan_cfg.dma_desc_num = kAudioDmaDescNum;
    chan_cfg.dma_frame_num = kAudioDmaFrames;

    ESP_RETURN_ON_ERROR(i2s_new_channel(&chan_cfg, &tx_chan_, nullptr), TAG, "i2s_new_channel failed");

    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(sample_rate_hz),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = GPIO_NUM_NC,
            .bclk = kI2sBclkPin,
            .ws = kI2sWsPin,
            .dout = kI2sDoutPin,
            .din = GPIO_NUM_NC,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };
    std_cfg.clk_cfg.clk_src = I2S_CLK_SRC_APLL;
    std_cfg.clk_cfg.mclk_multiple = i2s_mclk_multiple_for_rate(sample_rate_hz);

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
    i2s_channel_enabled_ = true;

    return ESP_OK;
}

void SignalGenerator::deinit_i2s(void)
{
    if (tx_chan_ == nullptr) {
        return;
    }

    if (i2s_channel_enabled_) {
        i2s_channel_disable(tx_chan_);
        i2s_channel_enabled_ = false;
    }
    i2s_del_channel(tx_chan_);
    tx_chan_ = nullptr;
}

bool SignalGenerator::start_audio(void)
{
    if (audio_running_) {
        return true;
    }

    clear_scope_samples();

    uint32_t sample_rate_hz = 192000;
    if ((state_lock_ != nullptr) && (xSemaphoreTake(state_lock_, portMAX_DELAY) == pdTRUE)) {
        sample_rate_hz = state_.sample_rate_hz;
        xSemaphoreGive(state_lock_);
    }

    const esp_err_t err = init_i2s(sample_rate_hz);
    if (err != ESP_OK) {
        set_runtime_status("I2S start failed: %s", esp_err_to_name(err));
        refresh_ui();
        return false;
    }

    audio_task_stop_ = false;
    const BaseType_t ok = xTaskCreatePinnedToCore(audio_task_entry,
                                                  "sig_gen_i2s",
                                                  kAudioTaskStackBytes,
                                                  this,
                                                  kAudioTaskPriority,
                                                  &audio_task_handle_,
                                                  kAudioTaskCore);
    if (ok != pdPASS) {
        audio_task_handle_ = nullptr;
        deinit_i2s();
        set_runtime_status("Audio task create failed");
        refresh_ui();
        return false;
    }

    audio_running_ = true;
    set_runtime_status("Running | PCM5102: BCLK33 LRCK31 DIN30 | CH1=L CH2=R");
    refresh_ui();
    return true;
}

void SignalGenerator::stop_audio(void)
{
    if (!audio_running_ && (audio_task_handle_ == nullptr) && (tx_chan_ == nullptr)) {
        return;
    }

    audio_task_stop_ = true;
    const TickType_t start_tick = xTaskGetTickCount();
    while ((audio_task_handle_ != nullptr) &&
           ((xTaskGetTickCount() - start_tick) < pdMS_TO_TICKS(1000))) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    deinit_i2s();
    audio_running_ = false;
    audio_task_stop_ = false;
    clear_scope_samples();
    set_runtime_status("Stopped | PCM5102: BCLK33 LRCK31 DIN30 | CH1=L CH2=R");
    refresh_ui();
}

void SignalGenerator::audio_task_main(void)
{
    uint32_t buffers_since_yield = 0;

    while (!audio_task_stop_) {
        fill_audio_buffer();

        size_t bytes_written = 0;
        const esp_err_t err = i2s_channel_write(tx_chan_,
                                                audio_buffer_,
                                                sizeof(audio_buffer_),
                                                &bytes_written,
                                                kI2sWriteTimeoutMs);
        if ((err != ESP_OK) && (err != ESP_ERR_TIMEOUT)) {
            ESP_LOGW(TAG, "i2s_channel_write: %s", esp_err_to_name(err));
            vTaskDelay(pdMS_TO_TICKS(10));
        }

        buffers_since_yield++;
        if (buffers_since_yield >= kAudioYieldEveryBuffers) {
            buffers_since_yield = 0;
            vTaskDelay(1);
        }
    }

    audio_task_handle_ = nullptr;
}

void SignalGenerator::fill_audio_buffer(void)
{
    if ((state_lock_ == nullptr) ||
        (carrier_waves_ == nullptr) ||
        (mod_waves_ == nullptr) ||
        (carrier_wave_count_ == 0) ||
        (mod_wave_count_ == 0)) {
        memset(audio_buffer_, 0, sizeof(audio_buffer_));
        publish_scope_samples(true);
        return;
    }

    if (xSemaphoreTake(state_lock_, pdMS_TO_TICKS(20)) != pdTRUE) {
        memset(audio_buffer_, 0, sizeof(audio_buffer_));
        publish_scope_samples(true);
        return;
    }

    const uint32_t sample_rate_hz = state_.sample_rate_hz;
    const float volume = static_cast<float>(state_.volume_percent) / 100.0f;
    const float phase_scale = sample_rate_hz > 0 ? kQ32PhaseScale / static_cast<float>(sample_rate_hz) : 0.0f;

    for (size_t i = 0; i < kAudioFrames; i++) {
        audio_buffer_[i * 2] = float_to_i16(render_channel(state_.ch[0], sample_rate_hz, phase_scale) * volume);
        audio_buffer_[(i * 2) + 1] = float_to_i16(render_channel(state_.ch[1], sample_rate_hz, phase_scale) * volume);
    }

    xSemaphoreGive(state_lock_);
    publish_scope_samples(false);
}

float SignalGenerator::lookup_wave(const Waveform *waveform, double phase) const
{
    if (waveform == nullptr) {
        return 0.0f;
    }

    phase -= floor(phase);
    if (phase < 0.0) {
        phase += 1.0;
    }

    const double pos = phase * static_cast<double>(kWaveTableSize);
    const size_t index = static_cast<size_t>(pos) & (kWaveTableSize - 1);
    const size_t next = (index + 1) & (kWaveTableSize - 1);
    const float frac = static_cast<float>(pos - floor(pos));
    return waveform->samples[index] + ((waveform->samples[next] - waveform->samples[index]) * frac);
}

float SignalGenerator::lookup_wave_q32(const Waveform *waveform, uint32_t phase) const
{
    if (waveform == nullptr) {
        return 0.0f;
    }

    static constexpr uint32_t kPhaseIndexShift = 22;
    static constexpr uint32_t kPhaseFracMask = (1UL << kPhaseIndexShift) - 1;
    static constexpr float kPhaseFracScale = 1.0f / static_cast<float>(1UL << kPhaseIndexShift);

    const size_t index = (phase >> kPhaseIndexShift) & (kWaveTableSize - 1);
    const size_t next = (index + 1) & (kWaveTableSize - 1);
    const float frac = static_cast<float>(phase & kPhaseFracMask) * kPhaseFracScale;
    return waveform->samples[index] + ((waveform->samples[next] - waveform->samples[index]) * frac);
}

uint32_t SignalGenerator::phase_increment(float freq_hz, float phase_scale) const
{
    if ((phase_scale <= 0.0f) || (freq_hz <= 0.0f)) {
        return 0;
    }

    const float phase = freq_hz * phase_scale;
    if (phase <= 0.0f) {
        return 0;
    }
    if (phase >= kQ32PhaseScale - 1.0f) {
        return UINT32_MAX;
    }

    return static_cast<uint32_t>(phase);
}

float SignalGenerator::render_channel(ChannelConfig &channel, uint32_t sample_rate_hz, float phase_scale)
{
    if (!channel.enabled || (sample_rate_hz == 0)) {
        return 0.0f;
    }

    const int carrier_index = static_cast<int>(clamp_u32(channel.carrier_wave, 0, carrier_wave_count_ - 1));
    const int am_index = static_cast<int>(clamp_u32(channel.am_wave, 0, mod_wave_count_ - 1));
    const int fm_index = static_cast<int>(clamp_u32(channel.fm_wave, 0, mod_wave_count_ - 1));

    const Waveform *carrier_wave = &carrier_waves_[carrier_index];
    const Waveform *am_wave = &mod_waves_[am_index];
    const Waveform *fm_wave = &mod_waves_[fm_index];

    float carrier_freq_hz = static_cast<float>(channel.carrier_freq_hz);
    if (channel.fm_enabled) {
        const float fm_lfo = lookup_wave_q32(fm_wave, channel.fm_phase);
        carrier_freq_hz = static_cast<float>(channel.fm_base_hz) + (channel.fm_dev_hz * fm_lfo);
        channel.fm_phase += phase_increment(channel.fm_freq_hz, phase_scale);
    }

    const float nyquist_safe_hz = static_cast<float>(sample_rate_hz) * 0.45f;
    carrier_freq_hz = clamp_float(carrier_freq_hz, 0.0f, nyquist_safe_hz);

    float sample = lookup_wave_q32(carrier_wave, channel.carrier_phase);
    channel.carrier_phase += phase_increment(carrier_freq_hz, phase_scale);

    if (channel.am_enabled) {
        const float am_lfo = lookup_wave_q32(am_wave, channel.am_phase);
        const float envelope = 0.5f + (0.5f * am_lfo);
        sample *= envelope;
        channel.am_phase += phase_increment(channel.am_freq_hz, phase_scale);
    }

    return sample * channel.gain;
}

void SignalGenerator::create_ui(lv_obj_t *parent)
{
    lv_obj_set_style_bg_color(parent, lv_color_hex(0x10151B), 0);
    lv_obj_set_style_bg_opa(parent, LV_OPA_COVER, 0);

    control_event_count_ = 0;
    root_ = lv_obj_create(parent);
    make_plain_container(root_);
    lv_obj_set_size(root_, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(root_, lv_color_hex(0x10151B), 0);
    lv_obj_set_style_bg_opa(root_, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(root_, 10, 0);
    lv_obj_set_style_pad_row(root_, 8, 0);
    lv_obj_set_flex_flow(root_, LV_FLEX_FLOW_COLUMN);

    create_toolbar(root_);
    create_scope_window(root_);
    create_channel_tabs(root_);

    channel_stack_ = lv_obj_create(root_);
    make_plain_container(channel_stack_);
    lv_obj_set_width(channel_stack_, lv_pct(100));
    lv_obj_set_flex_grow(channel_stack_, 1);
    lv_obj_set_scrollbar_mode(channel_stack_, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(channel_stack_, LV_OBJ_FLAG_SCROLLABLE);

    create_channel_panel(channel_stack_, 0);
    create_channel_panel(channel_stack_, 1);
    create_script_view(root_);
}

void SignalGenerator::create_toolbar(lv_obj_t *parent)
{
    lv_obj_t *toolbar = lv_obj_create(parent);
    make_plain_container(toolbar);
    lv_obj_set_width(toolbar, lv_pct(100));
    lv_obj_set_height(toolbar, 48);
    lv_obj_set_style_pad_column(toolbar, 8, 0);
    lv_obj_set_flex_flow(toolbar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(toolbar, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *start_button = create_button(toolbar, "START", 82);
    start_label_ = lv_obj_get_child(start_button, 0);
    attach_event(start_button, Control::StartStop, 0, LV_EVENT_CLICKED);

    lv_obj_t *reload_button = create_button(toolbar, LV_SYMBOL_REFRESH, 44);
    attach_event(reload_button, Control::ReloadWaves, 0, LV_EVENT_CLICKED);

    sample_rate_dropdown_ = lv_dropdown_create(toolbar);
    lv_obj_set_size(sample_rate_dropdown_, 112, 38);
    style_control_box(sample_rate_dropdown_);
    lv_dropdown_set_options(sample_rate_dropdown_, kSampleRateOptions);
    attach_event(sample_rate_dropdown_, Control::SampleRate, 0, LV_EVENT_VALUE_CHANGED);

    lv_obj_t *volume_box = lv_obj_create(toolbar);
    make_plain_container(volume_box);
    lv_obj_set_width(volume_box, 172);
    lv_obj_set_height(volume_box, 40);
    lv_obj_set_style_pad_column(volume_box, 5, 0);
    lv_obj_set_flex_flow(volume_box, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(volume_box, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    create_text_label(volume_box, "Vol", &lv_font_montserrat_14, 0xC8D3E0);
    lv_obj_t *vol_dec = create_button(volume_box, "-", 34);
    attach_event(vol_dec, Control::VolumeDec, 0, LV_EVENT_CLICKED);
    volume_label_ = create_text_label(volume_box, "", &lv_font_montserrat_14, 0xEEF4FA);
    lv_obj_set_width(volume_label_, 46);
    lv_obj_set_style_text_align(volume_label_, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_t *vol_inc = create_button(volume_box, "+", 34);
    attach_event(vol_inc, Control::VolumeInc, 0, LV_EVENT_CLICKED);

    lv_obj_t *script_button = create_button(toolbar, "SCRIPT", 76);
    script_mode_label_ = lv_obj_get_child(script_button, 0);
    attach_event(script_button, Control::ScriptMode, 0, LV_EVENT_CLICKED);

    status_label_ = create_text_label(toolbar, runtime_status_, &lv_font_montserrat_14, 0x9FB0C2);
    lv_obj_set_flex_grow(status_label_, 1);
    lv_obj_set_width(status_label_, 1);

    lv_obj_t *back_button = create_button(toolbar, LV_SYMBOL_LEFT, 44);
    attach_event(back_button, Control::Back, 0, LV_EVENT_CLICKED);
}

void SignalGenerator::create_scope_window(lv_obj_t *parent)
{
    lv_obj_t *scope_box = lv_obj_create(parent);
    scope_box_ = scope_box;
    lv_obj_set_width(scope_box, lv_pct(100));
    lv_obj_set_height(scope_box, static_cast<lv_coord_t>(kScopeCanvasHeight + 38));
    lv_obj_set_style_radius(scope_box, 8, 0);
    lv_obj_set_style_bg_color(scope_box, lv_color_hex(0x111923), 0);
    lv_obj_set_style_bg_opa(scope_box, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(scope_box, 1, 0);
    lv_obj_set_style_border_color(scope_box, lv_color_hex(0x2B3645), 0);
    lv_obj_set_style_pad_all(scope_box, 7, 0);
    lv_obj_set_style_pad_row(scope_box, 5, 0);
    lv_obj_set_flex_flow(scope_box, LV_FLEX_FLOW_COLUMN);
    lv_obj_clear_flag(scope_box, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *header = lv_obj_create(scope_box);
    make_plain_container(header);
    lv_obj_set_width(header, lv_pct(100));
    lv_obj_set_height(header, 20);
    lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    create_text_label(header, "DAC Scope", &lv_font_montserrat_14, 0xC8D3E0);
    scope_status_label_ = create_text_label(header, "", &lv_font_montserrat_14, 0x9FB0C2);

    if (!ensure_scope_canvas_buffer()) {
        lv_obj_t *label = create_text_label(scope_box, "Scope buffer alloc failed", &lv_font_montserrat_14, 0xFFB36B);
        lv_obj_center(label);
        return;
    }

    scope_canvas_ = lv_canvas_create(scope_box);
    lv_canvas_set_buffer(scope_canvas_,
                         scope_canvas_buffer_,
                         static_cast<int32_t>(kScopeCanvasWidth),
                         static_cast<int32_t>(kScopeCanvasHeight),
                         LV_COLOR_FORMAT_RGB565);
    lv_obj_center(scope_canvas_);

    if (scope_timer_ != nullptr) {
        lv_timer_delete(scope_timer_);
    }
    scope_timer_ = lv_timer_create(scope_timer_cb, kScopeRefreshMs, this);
    scope_rendered_sequence_ = UINT32_MAX;
    render_scope();
}

void SignalGenerator::create_channel_tabs(lv_obj_t *parent)
{
    tab_bar_ = lv_obj_create(parent);
    make_plain_container(tab_bar_);
    lv_obj_set_width(tab_bar_, lv_pct(100));
    lv_obj_set_height(tab_bar_, 42);
    lv_obj_set_style_pad_column(tab_bar_, 8, 0);
    lv_obj_set_flex_flow(tab_bar_, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(tab_bar_, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    const char *names[2] = {"CH1  Left", "CH2  Right"};
    for (uint8_t i = 0; i < 2; i++) {
        tab_button_[i] = create_button(tab_bar_, names[i], 1);
        lv_obj_set_flex_grow(tab_button_[i], 1);
        tab_label_[i] = lv_obj_get_child(tab_button_[i], 0);
        attach_event(tab_button_[i], Control::SelectChannel, i, LV_EVENT_CLICKED);
    }
}

void SignalGenerator::create_channel_panel(lv_obj_t *parent, uint8_t channel)
{
    lv_obj_t *panel = lv_obj_create(parent);
    channel_panel_[channel] = panel;
    lv_obj_set_width(panel, lv_pct(100));
    lv_obj_set_height(panel, lv_pct(100));
    lv_obj_align(panel, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_radius(panel, 8, 0);
    lv_obj_set_style_bg_color(panel, lv_color_hex(0x171E28), 0);
    lv_obj_set_style_bg_opa(panel, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(panel, 1, 0);
    lv_obj_set_style_border_color(panel, lv_color_hex(0x2B3645), 0);
    lv_obj_set_style_pad_all(panel, 6, 0);
    lv_obj_set_style_pad_row(panel, 5, 0);
    lv_obj_set_style_pad_column(panel, 8, 0);
    lv_obj_set_flex_flow(panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scrollbar_mode(panel, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);

    char title[16] = {};
    snprintf(title, sizeof(title), "CH%u  %s", channel + 1, channel == 0 ? "Left" : "Right");
    lv_obj_t *title_label = create_text_label(panel, title, &lv_font_montserrat_16, channel == 0 ? 0x7CD3FF : 0xFFB36B);
    lv_obj_set_width(title_label, lv_pct(100));
    lv_obj_set_style_text_align(title_label, LV_TEXT_ALIGN_CENTER, 0);

    create_frequency_preview_row(panel, "Carrier F", "ON", &ch_enable_switch_[channel],
                                 Control::ChEnable, channel == 0 ? 0x61D1FF : 0xFFB36B,
                                 &carrier_freq_label_[channel],
                                 kCarrierFreqPresets, Control::CarrierFreqEdit,
                                 &carrier_preview_canvas_[channel], &carrier_preview_buffer_[channel],
                                 &carrier_wave_dropdown_[channel], Control::CarrierWave, channel);

    create_frequency_preview_row(panel, "AM F", "AM", &am_enable_switch_[channel],
                                 Control::AmEnable, 0x57F28A,
                                 &am_freq_label_[channel],
                                 kAmFmFreqPresets, Control::AmFreqEdit,
                                 &am_preview_canvas_[channel], &am_preview_buffer_[channel],
                                 &am_wave_dropdown_[channel], Control::AmWave, channel);

    create_frequency_preview_row(panel, "FM F", "FM", &fm_enable_switch_[channel],
                                 Control::FmEnable, 0xD1A1FF,
                                 &fm_freq_label_[channel],
                                 kAmFmFreqPresets, Control::FmFreqEdit,
                                 &fm_preview_canvas_[channel], &fm_preview_buffer_[channel],
                                 &fm_wave_dropdown_[channel], Control::FmWave, channel);

    create_dual_frequency_row(panel,
                              "FM base", &fm_base_label_[channel], kCarrierFreqPresets, Control::FmBaseEdit,
                              "FM dev", &fm_dev_label_[channel], kFmDevPresets, Control::FmDevEdit,
                              channel);
}

void SignalGenerator::create_carrier_preview(lv_obj_t *parent, uint8_t channel)
{
    if (channel >= 2) {
        return;
    }

    create_wave_preview(parent,
                        &carrier_preview_canvas_[channel],
                        &carrier_preview_buffer_[channel],
                        &carrier_wave_dropdown_[channel],
                        Control::CarrierWave,
                        channel,
                        true);
}

void SignalGenerator::create_mod_preview(lv_obj_t *parent, uint8_t channel, bool fm_preview)
{
    if (channel >= 2) {
        return;
    }

    if (fm_preview) {
        create_wave_preview(parent,
                            &fm_preview_canvas_[channel],
                            &fm_preview_buffer_[channel],
                            &fm_wave_dropdown_[channel],
                            Control::FmWave,
                            channel,
                            true);
    } else {
        create_wave_preview(parent,
                            &am_preview_canvas_[channel],
                            &am_preview_buffer_[channel],
                            &am_wave_dropdown_[channel],
                            Control::AmWave,
                            channel,
                            true);
    }
}

void SignalGenerator::create_wave_preview(lv_obj_t *parent,
                                          lv_obj_t **canvas_slot,
                                          uint16_t **buffer_slot,
                                          lv_obj_t **dropdown_slot,
                                          Control control,
                                          uint8_t channel,
                                          bool fill_width)
{
    if ((parent == nullptr) || (canvas_slot == nullptr) || (buffer_slot == nullptr) || (dropdown_slot == nullptr)) {
        return;
    }

    lv_obj_t *holder = lv_obj_create(parent);
    make_plain_container(holder);
    lv_obj_set_width(holder, fill_width ? lv_pct(100) : static_cast<lv_coord_t>(kWavePreviewWidth + 8));
    lv_obj_set_height(holder, static_cast<lv_coord_t>(kWavePreviewHeight + 8));
    lv_obj_set_flex_flow(holder, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(holder, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(holder, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(holder, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *preview_box = lv_obj_create(holder);
    lv_obj_set_size(preview_box,
                    static_cast<lv_coord_t>(kWavePreviewWidth + 8),
                    static_cast<lv_coord_t>(kWavePreviewHeight + 8));
    lv_obj_set_style_radius(preview_box, 8, 0);
    lv_obj_set_style_bg_color(preview_box, lv_color_hex(0x101820), 0);
    lv_obj_set_style_bg_opa(preview_box, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(preview_box, 1, 0);
    lv_obj_set_style_border_color(preview_box, lv_color_hex(0x2C3A48), 0);
    lv_obj_set_style_pad_all(preview_box, 4, 0);
    lv_obj_set_scrollbar_mode(preview_box, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(preview_box, LV_OBJ_FLAG_SCROLLABLE);

    if (!ensure_wave_preview_buffer(buffer_slot)) {
        lv_obj_t *label = create_text_label(preview_box, "Preview buffer alloc failed", &lv_font_montserrat_14, 0xFFB36B);
        lv_obj_center(label);
        return;
    }

    lv_obj_t *canvas = lv_canvas_create(preview_box);
    *canvas_slot = canvas;
    lv_canvas_set_buffer(canvas,
                         *buffer_slot,
                         static_cast<int32_t>(kWavePreviewWidth),
                         static_cast<int32_t>(kWavePreviewHeight),
                         LV_COLOR_FORMAT_RGB565);
    lv_obj_center(canvas);

    lv_obj_t *dropdown = lv_dropdown_create(preview_box);
    *dropdown_slot = dropdown;
    lv_obj_set_size(dropdown,
                    static_cast<lv_coord_t>(kWavePreviewWidth),
                    static_cast<lv_coord_t>(kWavePreviewHeight));
    lv_obj_center(dropdown);
    lv_obj_set_ext_click_area(dropdown, 10);
    lv_dropdown_set_symbol(dropdown, LV_SYMBOL_DOWN);
    lv_dropdown_set_dir(dropdown, LV_DIR_BOTTOM);
    lv_dropdown_set_text(dropdown, "");
    lv_obj_set_style_bg_opa(dropdown, LV_OPA_TRANSP, 0);
    lv_obj_set_style_bg_opa(dropdown, LV_OPA_30, LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(dropdown, lv_color_hex(0x18202A), LV_STATE_PRESSED);
    lv_obj_set_style_border_width(dropdown, 0, 0);
    lv_obj_set_style_text_color(dropdown, lv_color_hex(0xEEF4FA), 0);
    lv_obj_set_style_text_font(dropdown, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_align(dropdown, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_top(dropdown, 8, 0);
    lv_obj_set_style_pad_bottom(dropdown, 8, 0);
    lv_obj_clear_flag(dropdown, LV_OBJ_FLAG_SCROLLABLE);
    attach_event(dropdown, control, channel, LV_EVENT_VALUE_CHANGED);
}

bool SignalGenerator::ensure_wave_preview_buffer(uint16_t **buffer_slot)
{
    if (buffer_slot == nullptr) {
        return false;
    }

    if (*buffer_slot != nullptr) {
        return true;
    }

    const size_t bytes = kWavePreviewWidth * kWavePreviewHeight * sizeof(uint16_t);
    *buffer_slot = static_cast<uint16_t *>(heap_caps_malloc(bytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
    if (*buffer_slot == nullptr) {
        *buffer_slot = static_cast<uint16_t *>(heap_caps_malloc(bytes, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
    }

    if (*buffer_slot != nullptr) {
        memset(*buffer_slot, 0, bytes);
    }

    return *buffer_slot != nullptr;
}

void SignalGenerator::render_wave_preview(lv_obj_t *canvas, uint16_t *buffer, const Waveform *waveform, uint32_t color)
{
    if ((canvas == nullptr) || (buffer == nullptr) || (waveform == nullptr)) {
        return;
    }

    const int32_t width = static_cast<int32_t>(kWavePreviewWidth);
    const int32_t height = static_cast<int32_t>(kWavePreviewHeight);
    const int32_t pad = 7;
    const int32_t mid_y = height / 2;
    const float amplitude = static_cast<float>((height / 2) - pad);

    const uint16_t bg = rgb565(0x101820);
    const uint16_t grid = rgb565(0x263340);
    const uint16_t center = rgb565(0x2F6EA0);
    const uint16_t wave_color = rgb565(color);

    for (size_t i = 0; i < (kWavePreviewWidth * kWavePreviewHeight); i++) {
        buffer[i] = bg;
    }

    for (int32_t x = 0; x < width; x += width / 8) {
        preview_draw_line(buffer, width, height, x, pad, x, height - pad - 1, grid);
    }
    for (int32_t y = pad; y < height - pad; y += 18) {
        preview_draw_line(buffer, width, height, 0, y, width - 1, y, grid);
    }

    preview_draw_line(buffer, width, height, 0, mid_y, width - 1, mid_y, center);

    int32_t prev_x = 0;
    int32_t prev_y = mid_y;
    for (int32_t x = 0; x < width; x++) {
        const double phase = (static_cast<double>(x) / static_cast<double>(width - 1)) * 2.0;
        const float sample = lookup_wave(waveform, phase);
        int32_t y = mid_y - static_cast<int32_t>(sample * amplitude);
        if (y < pad) {
            y = pad;
        } else if (y >= (height - pad)) {
            y = height - pad - 1;
        }

        if (x == 0) {
            prev_y = y;
        } else {
            preview_draw_line(buffer, width, height, prev_x, prev_y, x, y, wave_color);
            preview_draw_line(buffer, width, height, prev_x, prev_y + 1, x, y + 1, wave_color);
        }

        prev_x = x;
        prev_y = y;
    }

    lv_obj_invalidate(canvas);
}

void SignalGenerator::render_channel_previews(uint8_t channel, const ChannelConfig &channel_state)
{
    if (channel >= 2) {
        return;
    }

    if ((carrier_waves_ != nullptr) && (carrier_wave_count_ > 0)) {
        const size_t index = clamp_wave_index(channel_state.carrier_wave, carrier_wave_count_);
        if (rendered_carrier_wave_[channel] != static_cast<int>(index)) {
            render_wave_preview(carrier_preview_canvas_[channel],
                                carrier_preview_buffer_[channel],
                                &carrier_waves_[index],
                                channel == 0 ? 0x61D1FF : 0xFFB36B);
            rendered_carrier_wave_[channel] = static_cast<int>(index);
        }
    }

    if ((mod_waves_ != nullptr) && (mod_wave_count_ > 0)) {
        const size_t am_wave_index = clamp_wave_index(channel_state.am_wave, mod_wave_count_);
        const size_t fm_wave_index = clamp_wave_index(channel_state.fm_wave, mod_wave_count_);
        if (rendered_am_wave_[channel] != static_cast<int>(am_wave_index)) {
            render_wave_preview(am_preview_canvas_[channel],
                                am_preview_buffer_[channel],
                                &mod_waves_[am_wave_index],
                                0x7CFF9B);
            rendered_am_wave_[channel] = static_cast<int>(am_wave_index);
        }
        if (rendered_fm_wave_[channel] != static_cast<int>(fm_wave_index)) {
            render_wave_preview(fm_preview_canvas_[channel],
                                fm_preview_buffer_[channel],
                                &mod_waves_[fm_wave_index],
                                0xD1A1FF);
            rendered_fm_wave_[channel] = static_cast<int>(fm_wave_index);
        }
    }
}

void SignalGenerator::invalidate_wave_previews(void)
{
    for (size_t i = 0; i < 2; i++) {
        rendered_carrier_wave_[i] = -1;
        rendered_am_wave_[i] = -1;
        rendered_fm_wave_[i] = -1;
    }
}

bool SignalGenerator::ensure_scope_canvas_buffer(void)
{
    if (scope_canvas_buffer_ != nullptr) {
        return true;
    }

    const size_t bytes = kScopeCanvasWidth * kScopeCanvasHeight * sizeof(uint16_t);
    scope_canvas_buffer_ = static_cast<uint16_t *>(heap_caps_malloc(bytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
    if (scope_canvas_buffer_ == nullptr) {
        scope_canvas_buffer_ = static_cast<uint16_t *>(heap_caps_malloc(bytes, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
    }

    if (scope_canvas_buffer_ != nullptr) {
        memset(scope_canvas_buffer_, 0, bytes);
    }

    return scope_canvas_buffer_ != nullptr;
}

void SignalGenerator::publish_scope_samples(bool force)
{
    if (scope_lock_ == nullptr) {
        return;
    }

    if (!force) {
        scope_publish_counter_++;
        if (scope_publish_counter_ < kScopePublishEveryBuffers) {
            return;
        }
    }
    scope_publish_counter_ = 0;

    const TickType_t wait_ticks = force ? pdMS_TO_TICKS(20) : 0;
    if (xSemaphoreTake(scope_lock_, wait_ticks) != pdTRUE) {
        return;
    }

    memcpy(scope_samples_, audio_buffer_, sizeof(scope_samples_));
    scope_sequence_++;
    xSemaphoreGive(scope_lock_);
}

void SignalGenerator::clear_scope_samples(void)
{
    if (scope_lock_ == nullptr) {
        return;
    }

    if (xSemaphoreTake(scope_lock_, pdMS_TO_TICKS(20)) != pdTRUE) {
        return;
    }

    memset(scope_samples_, 0, sizeof(scope_samples_));
    scope_sequence_++;
    scope_publish_counter_ = 0;
    xSemaphoreGive(scope_lock_);
}

void SignalGenerator::render_scope(void)
{
    if ((scope_canvas_ == nullptr) || (scope_canvas_buffer_ == nullptr)) {
        return;
    }

    if (scope_status_label_ != nullptr) {
        lv_label_set_text(scope_status_label_, audio_running_ ? "Live  CH1 L / CH2 R" : "Stopped  CH1 L / CH2 R");
    }

    uint32_t sequence = 0;
    if ((scope_lock_ != nullptr) && (xSemaphoreTake(scope_lock_, pdMS_TO_TICKS(5)) == pdTRUE)) {
        sequence = scope_sequence_;
        if (sequence == scope_rendered_sequence_) {
            xSemaphoreGive(scope_lock_);
            return;
        }
        memcpy(scope_render_samples_, scope_samples_, sizeof(scope_render_samples_));
        xSemaphoreGive(scope_lock_);
    } else {
        return;
    }
    scope_rendered_sequence_ = sequence;

    const int32_t width = static_cast<int32_t>(kScopeCanvasWidth);
    const int32_t height = static_cast<int32_t>(kScopeCanvasHeight);
    const int32_t pad = 7;
    const int32_t gap = 8;
    const int32_t plot_height = (height - (pad * 2) - gap) / 2;
    const int32_t top_y = pad;
    const int32_t bottom_y = pad + plot_height + gap;
    const int32_t top_mid = top_y + (plot_height / 2);
    const int32_t bottom_mid = bottom_y + (plot_height / 2);
    const float amplitude = static_cast<float>((plot_height / 2) - 4);

    const uint16_t bg = rgb565(0x101820);
    const uint16_t grid = rgb565(0x263340);
    const uint16_t center = rgb565(0x34516C);
    const uint16_t left_color = rgb565(0x61D1FF);
    const uint16_t right_color = rgb565(0xFFB36B);

    for (size_t i = 0; i < (kScopeCanvasWidth * kScopeCanvasHeight); i++) {
        scope_canvas_buffer_[i] = bg;
    }

    for (int32_t x = 0; x < width; x += width / 8) {
        preview_draw_line(scope_canvas_buffer_, width, height, x, top_y, x, top_y + plot_height - 1, grid);
        preview_draw_line(scope_canvas_buffer_, width, height, x, bottom_y, x, bottom_y + plot_height - 1, grid);
    }
    for (int32_t y = top_y; y < top_y + plot_height; y += plot_height / 2) {
        preview_draw_line(scope_canvas_buffer_, width, height, 0, y, width - 1, y, grid);
    }
    for (int32_t y = bottom_y; y < bottom_y + plot_height; y += plot_height / 2) {
        preview_draw_line(scope_canvas_buffer_, width, height, 0, y, width - 1, y, grid);
    }

    preview_draw_line(scope_canvas_buffer_, width, height, 0, top_mid, width - 1, top_mid, center);
    preview_draw_line(scope_canvas_buffer_, width, height, 0, bottom_mid, width - 1, bottom_mid, center);

    int32_t prev_x = 0;
    int32_t prev_left_y = top_mid;
    int32_t prev_right_y = bottom_mid;
    for (size_t i = 0; i < kAudioFrames; i++) {
        const int32_t x = static_cast<int32_t>((i * static_cast<size_t>(width - 1)) / (kAudioFrames - 1));
        int32_t left_y = top_mid - static_cast<int32_t>((static_cast<float>(scope_render_samples_[i * 2]) / 32768.0f) * amplitude);
        int32_t right_y = bottom_mid - static_cast<int32_t>((static_cast<float>(scope_render_samples_[(i * 2) + 1]) / 32768.0f) * amplitude);

        if (left_y < top_y) {
            left_y = top_y;
        } else if (left_y >= top_y + plot_height) {
            left_y = top_y + plot_height - 1;
        }
        if (right_y < bottom_y) {
            right_y = bottom_y;
        } else if (right_y >= bottom_y + plot_height) {
            right_y = bottom_y + plot_height - 1;
        }

        if (i > 0) {
            preview_draw_line(scope_canvas_buffer_, width, height, prev_x, prev_left_y, x, left_y, left_color);
            preview_draw_line(scope_canvas_buffer_, width, height, prev_x, prev_right_y, x, right_y, right_color);
        }

        prev_x = x;
        prev_left_y = left_y;
        prev_right_y = right_y;
    }

    lv_obj_invalidate(scope_canvas_);
}

lv_obj_t *SignalGenerator::create_button(lv_obj_t *parent, const char *text, lv_coord_t width)
{
    lv_obj_t *button = lv_button_create(parent);
    lv_obj_set_size(button, width, 38);
    lv_obj_set_ext_click_area(button, 8);
    style_control_box(button);
    lv_obj_set_style_bg_color(button, lv_color_hex(0x273444), 0);
    lv_obj_set_style_bg_color(button, lv_color_hex(0x3C4A5D), LV_STATE_PRESSED);

    lv_obj_t *label = create_text_label(button, text, &lv_font_montserrat_14, 0xFFFFFF);
    lv_obj_center(label);
    make_child_passthrough(label);
    return button;
}

lv_obj_t *SignalGenerator::create_glow_toggle_button(lv_obj_t *parent,
                                                     const char *text,
                                                     Control control,
                                                     uint8_t channel,
                                                     uint32_t active_color)
{
    lv_obj_t *button = lv_button_create(parent);
    lv_obj_set_size(button, 50, 50);
    lv_obj_set_ext_click_area(button, 10);
    lv_obj_set_style_radius(button, 8, 0);
    lv_obj_set_style_bg_color(button, lv_color_hex(0x202834), 0);
    lv_obj_set_style_bg_color(button, lv_color_hex(active_color), LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(button, lv_color_hex(active_color), LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(button, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(button, 1, 0);
    lv_obj_set_style_border_width(button, 2, LV_STATE_CHECKED);
    lv_obj_set_style_border_color(button, lv_color_hex(0x354151), 0);
    lv_obj_set_style_border_color(button, lv_color_hex(active_color), LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(button, 0, 0);
    lv_obj_set_style_shadow_width(button, 14, LV_STATE_CHECKED);
    lv_obj_set_style_shadow_opa(button, LV_OPA_40, LV_STATE_CHECKED);
    lv_obj_set_style_shadow_color(button, lv_color_hex(active_color), LV_STATE_CHECKED);

    lv_obj_t *label = create_text_label(button, text, &lv_font_montserrat_14, 0xFFFFFF);
    lv_obj_set_style_text_color(label, lv_color_hex(0x10151B), LV_STATE_CHECKED);
    lv_obj_center(label);
    make_child_passthrough(label);
    attach_event(button, control, channel, LV_EVENT_CLICKED);
    return button;
}

lv_obj_t *SignalGenerator::create_text_label(lv_obj_t *parent, const char *text, const lv_font_t *font, uint32_t color)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_label_set_long_mode(label, LV_LABEL_LONG_DOT);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
    return label;
}

lv_obj_t *SignalGenerator::create_adjust_row(lv_obj_t *parent,
                                             const char *name,
                                             lv_obj_t **value_label,
                                             Control dec_control,
                                             Control inc_control,
                                             uint8_t channel)
{
    lv_obj_t *row = lv_obj_create(parent);
    make_plain_container(row);
    lv_obj_set_width(row, lv_pct(100));
    lv_obj_set_height(row, 42);
    lv_obj_set_style_pad_column(row, 8, 0);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *name_label = create_text_label(row, name, &lv_font_montserrat_14, 0xC8D3E0);
    lv_obj_set_width(name_label, 108);

    lv_obj_t *dec_button = create_button(row, "-", 48);
    attach_event(dec_button, dec_control, channel, LV_EVENT_CLICKED);

    *value_label = create_text_label(row, "", &lv_font_montserrat_14, 0xEEF4FA);
    lv_obj_set_width(*value_label, 1);
    lv_obj_set_flex_grow(*value_label, 1);
    lv_obj_set_style_text_align(*value_label, LV_TEXT_ALIGN_CENTER, 0);

    lv_obj_t *inc_button = create_button(row, "+", 48);
    attach_event(inc_button, inc_control, channel, LV_EVENT_CLICKED);

    return row;
}

lv_obj_t *SignalGenerator::create_frequency_row(lv_obj_t *parent,
                                                const char *name,
                                                lv_obj_t **value_control,
                                                const char *preset_options,
                                                Control edit_control,
                                                uint8_t channel)
{
    lv_obj_t *row = lv_obj_create(parent);
    make_plain_container(row);
    lv_obj_set_width(row, lv_pct(100));
    lv_obj_set_height(row, 42);
    lv_obj_set_style_pad_column(row, 8, 0);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *name_label = create_text_label(row, name, &lv_font_montserrat_14, 0xC8D3E0);
    lv_obj_set_width(name_label, 108);

    *value_control = lv_dropdown_create(row);
    lv_obj_set_height(*value_control, 38);
    lv_obj_set_width(*value_control, 1);
    lv_obj_set_flex_grow(*value_control, 1);
    lv_obj_set_ext_click_area(*value_control, 12);
    style_control_box(*value_control);
    lv_obj_set_style_text_align(*value_control, LV_TEXT_ALIGN_CENTER, 0);
    lv_dropdown_set_symbol(*value_control, LV_SYMBOL_DOWN);
    lv_dropdown_set_dir(*value_control, LV_DIR_BOTTOM);
    lv_dropdown_set_options(*value_control, preset_options);
    lv_dropdown_set_text(*value_control, "");
    lv_obj_add_event_cb(*value_control,
                        block_dropdown_release_cb,
                        static_cast<lv_event_code_t>(LV_EVENT_RELEASED | LV_EVENT_PREPROCESS),
                        nullptr);
    attach_event(*value_control, edit_control, channel, LV_EVENT_VALUE_CHANGED);
    attach_event(*value_control, edit_control, channel, LV_EVENT_PRESSING);
    attach_event(*value_control, edit_control, channel, LV_EVENT_DOUBLE_CLICKED);

    return row;
}

lv_obj_t *SignalGenerator::create_frequency_preview_row(lv_obj_t *parent,
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
                                                        uint8_t channel)
{
    lv_obj_t *row = lv_obj_create(parent);
    make_plain_container(row);
    lv_obj_set_width(row, lv_pct(100));
    lv_obj_set_height(row, static_cast<lv_coord_t>(kWavePreviewHeight + 10));
    lv_obj_set_style_pad_column(row, 7, 0);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(row, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);

    if ((enable_button != nullptr) && (enable_text != nullptr)) {
        *enable_button = create_glow_toggle_button(row, enable_text, enable_control, channel, enable_color);
    }

    lv_obj_t *control_box = lv_obj_create(row);
    make_plain_container(control_box);
    lv_obj_set_width(control_box, 1);
    lv_obj_set_height(control_box, static_cast<lv_coord_t>(kWavePreviewHeight + 10));
    lv_obj_set_flex_grow(control_box, 1);
    lv_obj_set_style_pad_row(control_box, 3, 0);
    lv_obj_set_flex_flow(control_box, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(control_box, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(control_box, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(control_box, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *name_label = create_text_label(control_box, name, &lv_font_montserrat_14, 0xC8D3E0);
    lv_obj_set_width(name_label, lv_pct(100));
    lv_obj_set_style_text_align(name_label, LV_TEXT_ALIGN_CENTER, 0);

    *value_control = lv_dropdown_create(control_box);
    lv_obj_set_size(*value_control, lv_pct(100), 34);
    lv_obj_set_ext_click_area(*value_control, 12);
    style_control_box(*value_control);
    lv_obj_set_style_text_align(*value_control, LV_TEXT_ALIGN_CENTER, 0);
    lv_dropdown_set_symbol(*value_control, LV_SYMBOL_DOWN);
    lv_dropdown_set_dir(*value_control, LV_DIR_BOTTOM);
    lv_dropdown_set_options(*value_control, preset_options);
    lv_dropdown_set_text(*value_control, "");
    lv_obj_clear_flag(*value_control, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(*value_control,
                        block_dropdown_release_cb,
                        static_cast<lv_event_code_t>(LV_EVENT_RELEASED | LV_EVENT_PREPROCESS),
                        nullptr);
    attach_event(*value_control, edit_control, channel, LV_EVENT_VALUE_CHANGED);
    attach_event(*value_control, edit_control, channel, LV_EVENT_PRESSING);
    attach_event(*value_control, edit_control, channel, LV_EVENT_DOUBLE_CLICKED);

    create_wave_preview(row, canvas_slot, buffer_slot, dropdown_slot, wave_control, channel, false);
    return row;
}

lv_obj_t *SignalGenerator::create_dual_frequency_row(lv_obj_t *parent,
                                                     const char *left_name,
                                                     lv_obj_t **left_control,
                                                     const char *left_presets,
                                                     Control left_edit_control,
                                                     const char *right_name,
                                                     lv_obj_t **right_control,
                                                     const char *right_presets,
                                                     Control right_edit_control,
                                                     uint8_t channel)
{
    lv_obj_t *row = lv_obj_create(parent);
    make_plain_container(row);
    lv_obj_set_width(row, lv_pct(100));
    lv_obj_set_height(row, static_cast<lv_coord_t>(kWavePreviewHeight + 10));
    lv_obj_set_style_pad_column(row, 8, 0);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(row, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);

    const char *names[2] = {left_name, right_name};
    lv_obj_t **controls[2] = {left_control, right_control};
    const char *presets[2] = {left_presets, right_presets};
    const Control controls_event[2] = {left_edit_control, right_edit_control};

    for (size_t i = 0; i < 2; i++) {
        lv_obj_t *box = lv_obj_create(row);
        make_plain_container(box);
        lv_obj_set_width(box, 1);
        lv_obj_set_height(box, static_cast<lv_coord_t>(kWavePreviewHeight + 10));
        lv_obj_set_flex_grow(box, 1);
        lv_obj_set_style_pad_row(box, 3, 0);
        lv_obj_set_flex_flow(box, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(box, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_scrollbar_mode(box, LV_SCROLLBAR_MODE_OFF);
        lv_obj_clear_flag(box, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_t *label = create_text_label(box, names[i], &lv_font_montserrat_14, 0xC8D3E0);
        lv_obj_set_width(label, lv_pct(100));
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);

        *controls[i] = lv_dropdown_create(box);
        lv_obj_set_size(*controls[i], lv_pct(100), 34);
        lv_obj_set_ext_click_area(*controls[i], 12);
        style_control_box(*controls[i]);
        lv_obj_set_style_text_align(*controls[i], LV_TEXT_ALIGN_CENTER, 0);
        lv_dropdown_set_symbol(*controls[i], LV_SYMBOL_DOWN);
        lv_dropdown_set_dir(*controls[i], LV_DIR_BOTTOM);
        lv_dropdown_set_options(*controls[i], presets[i]);
        lv_dropdown_set_text(*controls[i], "");
        lv_obj_clear_flag(*controls[i], LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_event_cb(*controls[i],
                            block_dropdown_release_cb,
                            static_cast<lv_event_code_t>(LV_EVENT_RELEASED | LV_EVENT_PREPROCESS),
                            nullptr);
        attach_event(*controls[i], controls_event[i], channel, LV_EVENT_VALUE_CHANGED);
        attach_event(*controls[i], controls_event[i], channel, LV_EVENT_PRESSING);
        attach_event(*controls[i], controls_event[i], channel, LV_EVENT_DOUBLE_CLICKED);
    }

    return row;
}

lv_obj_t *SignalGenerator::create_am_freq_row(lv_obj_t *parent, uint8_t channel)
{
    am_freq_slider_[channel] = nullptr;
    return create_frequency_row(parent, "AM F", &am_freq_label_[channel],
                                kAmFmFreqPresets, Control::AmFreqEdit, channel);
}

lv_obj_t *SignalGenerator::create_dropdown_row(lv_obj_t *parent,
                                               const char *name,
                                               lv_obj_t **dropdown,
                                               Control control,
                                               uint8_t channel)
{
    lv_obj_t *row = lv_obj_create(parent);
    make_plain_container(row);
    lv_obj_set_width(row, lv_pct(100));
    lv_obj_set_height(row, 44);
    lv_obj_set_style_pad_column(row, 8, 0);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *name_label = create_text_label(row, name, &lv_font_montserrat_14, 0xC8D3E0);
    lv_obj_set_width(name_label, 108);

    *dropdown = lv_dropdown_create(row);
    lv_obj_set_height(*dropdown, 38);
    lv_obj_set_flex_grow(*dropdown, 1);
    style_control_box(*dropdown);
    attach_event(*dropdown, control, channel, LV_EVENT_VALUE_CHANGED);
    return row;
}

lv_obj_t *SignalGenerator::create_switch_row(lv_obj_t *parent,
                                             const char *name,
                                             lv_obj_t **switch_obj,
                                             Control control,
                                             uint8_t channel)
{
    lv_obj_t *row = lv_obj_create(parent);
    make_plain_container(row);
    lv_obj_set_width(row, lv_pct(100));
    lv_obj_set_height(row, 42);
    lv_obj_set_style_pad_column(row, 8, 0);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *name_label = create_text_label(row, name, &lv_font_montserrat_14, 0xC8D3E0);
    lv_obj_set_flex_grow(name_label, 1);
    lv_obj_set_width(name_label, 1);

    *switch_obj = lv_switch_create(row);
    lv_obj_set_size(*switch_obj, 62, 32);
    attach_event(*switch_obj, control, channel, LV_EVENT_VALUE_CHANGED);
    return row;
}

SignalGenerator::ControlEventData *SignalGenerator::make_control_event(Control control, uint8_t channel)
{
    if (control_event_count_ >= kMaxControlEvents) {
        return nullptr;
    }

    ControlEventData *data = &control_events_[control_event_count_++];
    data->app = this;
    data->control = control;
    data->channel = channel;
    return data;
}

void SignalGenerator::attach_event(lv_obj_t *obj, Control control, uint8_t channel, lv_event_code_t code)
{
    ControlEventData *data = make_control_event(control, channel);
    if (data != nullptr) {
        lv_obj_add_event_cb(obj, control_event_cb, code, data);
    }
}

void SignalGenerator::handle_control(ControlEventData *data, lv_event_t *event)
{
    if (data == nullptr) {
        return;
    }

    const Control control = data->control;
    const uint8_t channel_index = data->channel;

    if (control == Control::Back) {
        request_close();
        return;
    }

    if (control == Control::StartStop) {
        if (audio_running_) {
            stop_audio();
        } else {
            start_audio();
        }
        return;
    }

    if (control == Control::ScriptMode) {
        script_mode_ = !script_mode_;
        refresh_ui();
        return;
    }

    if (control == Control::ScriptRunStop) {
        if (script_running_) {
            script_stop(false);
        } else {
            script_start();
        }
        refresh_ui();
        return;
    }

    if (control == Control::ScriptNew) {
        script_keyboard_home();
        script_new();
        refresh_ui();
        return;
    }

    if (control == Control::ScriptLoad) {
        script_keyboard_home();
        if (!script_load_first_file()) {
            script_load_default();
            set_runtime_status("Script: no .sk files in %s", kScriptDir);
        }
        refresh_ui();
        return;
    }

    if (control == Control::ScriptSave) {
        if (script_save_file(script_name_)) {
            set_runtime_status("Script saved: %s.sk", script_name_);
        }
        refresh_ui();
        return;
    }

    if (control == Control::ScriptSaveAs) {
        if (script_save_as_next_file()) {
            set_runtime_status("Script saved as: %s.sk", script_name_);
        }
        refresh_ui();
        return;
    }

    if (control == Control::ScriptAdd) {
        script_keyboard_home();
        script_insert_after_selected("?");
        refresh_ui();
        return;
    }

    if (control == Control::ScriptAddEnd) {
        script_keyboard_home();
        script_insert_after_selected("END");
        refresh_ui();
        return;
    }

    if (control == Control::ScriptDelete) {
        script_keyboard_home();
        script_delete_selected();
        refresh_ui();
        return;
    }

    if (control == Control::ScriptUp) {
        script_keyboard_home();
        script_move_selected(-1);
        refresh_ui();
        return;
    }

    if (control == Control::ScriptDown) {
        script_keyboard_home();
        script_move_selected(1);
        refresh_ui();
        return;
    }

    if (control == Control::ScriptSelectLine) {
        const size_t selected = script_scroll_offset_ + channel_index;
        if (selected < script_line_count_) {
            script_selected_line_ = selected;
            script_keyboard_home();
        }
        refresh_script_ui();
        return;
    }

    if (control == Control::ScriptListGesture) {
        lv_indev_t *indev = lv_indev_active();
        if (indev != nullptr) {
            const lv_dir_t dir = lv_indev_get_gesture_dir(indev);
            if (dir == LV_DIR_TOP) {
                script_scroll_lines(3);
            } else if (dir == LV_DIR_BOTTOM) {
                script_scroll_lines(-3);
            }
        }
        refresh_script_ui();
        return;
    }

    if (control == Control::ScriptTemplate) {
        script_keyboard_press(channel_index);
        refresh_ui();
        return;
    }

    if (control == Control::ScriptPresetSave) {
        if (save_preset_file("default")) {
            set_runtime_status("Preset saved: default.txt");
        }
        refresh_ui();
        return;
    }

    if (control == Control::ScriptPresetLoad) {
        const bool was_running = audio_running_;
        if (load_preset_file("default")) {
            if (was_running) {
                stop_audio();
                start_audio();
            }
            set_runtime_status("Preset loaded: default.txt");
        }
        refresh_ui();
        return;
    }

    if (control == Control::SelectChannel) {
        if (channel_index >= 2) {
            return;
        }
        active_channel_ = channel_index;
        refresh_ui();
        return;
    }

    if (control == Control::ReloadWaves) {
        const bool was_running = audio_running_;
        if (was_running) {
            stop_audio();
        }
        load_waveforms();
        refresh_wave_dropdown_options();
        set_runtime_status("Waves: %u carrier, %u mod",
                           static_cast<unsigned>(carrier_wave_count_),
                           static_cast<unsigned>(mod_wave_count_));
        if (was_running) {
            start_audio();
        } else {
            refresh_ui();
        }
        return;
    }

    if (channel_index >= 2) {
        return;
    }

    const lv_event_code_t event_code = event != nullptr ? lv_event_get_code(event) : LV_EVENT_ALL;
    if (event_code == LV_EVENT_DOUBLE_CLICKED) {
        switch (control) {
        case Control::CarrierFreqEdit:
        case Control::AmFreqEdit:
        case Control::FmBaseEdit:
        case Control::FmDevEdit:
        case Control::FmFreqEdit: {
            lv_obj_t *target = static_cast<lv_obj_t *>(lv_event_get_target(event));
            if (target != nullptr) {
                lv_dropdown_open(target);
            }
            return;
        }
        default:
            break;
        }
    }

    bool restart_audio = false;
    const float drag_amount = event_code == LV_EVENT_PRESSING ? drag_amount_from_indev() : 0.0f;
    if ((state_lock_ != nullptr) && (xSemaphoreTake(state_lock_, portMAX_DELAY) == pdTRUE)) {
        ChannelConfig &ch = state_.ch[channel_index];

        switch (control) {
        case Control::SampleRate: {
            const uint16_t selected = lv_dropdown_get_selected(sample_rate_dropdown_);
            const uint16_t index = selected < (sizeof(kSampleRates) / sizeof(kSampleRates[0])) ? selected : 2;
            const uint32_t new_rate = kSampleRates[index];
            restart_audio = audio_running_ && (state_.sample_rate_hz != new_rate);
            state_.sample_rate_hz = new_rate;
            break;
        }
        case Control::VolumeDec:
            state_.volume_percent = state_.volume_percent >= 5 ? state_.volume_percent - 5 : 0;
            break;
        case Control::VolumeInc:
            state_.volume_percent = state_.volume_percent <= 95 ? state_.volume_percent + 5 : 100;
            break;
        case Control::ChEnable:
            ch.enabled = !ch.enabled;
            break;
        case Control::CarrierWave:
            ch.carrier_wave = static_cast<int>(lv_dropdown_get_selected(carrier_wave_dropdown_[channel_index]));
            break;
        case Control::CarrierFreqEdit:
            if (event_code == LV_EVENT_VALUE_CHANGED) {
                ch.carrier_freq_hz = clamp_u32(static_cast<uint32_t>(
                                                   dropdown_selected_float(carrier_freq_label_[channel_index],
                                                                           static_cast<float>(ch.carrier_freq_hz)) + 0.5f),
                                               20,
                                               40000);
            } else if (drag_amount != 0.0f) {
                int32_t next = static_cast<int32_t>(ch.carrier_freq_hz) +
                               static_cast<int32_t>(drag_amount * 5.0f);
                if (next < 20) {
                    next = 20;
                } else if (next > 40000) {
                    next = 40000;
                }
                ch.carrier_freq_hz = static_cast<uint32_t>(next);
            }
            break;
        case Control::CarrierFreqDec:
            ch.carrier_freq_hz = ch.carrier_freq_hz > 100 ? ch.carrier_freq_hz - 100 : 20;
            break;
        case Control::CarrierFreqInc:
            ch.carrier_freq_hz = clamp_u32(ch.carrier_freq_hz + 100, 20, 40000);
            break;
        case Control::AmEnable:
            ch.am_enabled = !ch.am_enabled;
            break;
        case Control::AmWave:
            ch.am_wave = static_cast<int>(lv_dropdown_get_selected(am_wave_dropdown_[channel_index]));
            break;
        case Control::AmFreqEdit:
            if (event_code == LV_EVENT_VALUE_CHANGED) {
                ch.am_freq_hz = clamp_float(dropdown_selected_float(am_freq_label_[channel_index],
                                                                    ch.am_freq_hz),
                                            kAmFreqMinHz,
                                            kAmFreqMaxHz);
            } else if (drag_amount != 0.0f) {
                const float step = ch.am_freq_hz < 10.0f ? 0.02f : 0.2f;
                ch.am_freq_hz = clamp_float(ch.am_freq_hz + (drag_amount * step),
                                            kAmFreqMinHz,
                                            kAmFreqMaxHz);
            }
            break;
        case Control::AmFreqDec:
            ch.am_freq_hz = clamp_float(ch.am_freq_hz - (ch.am_freq_hz <= 10.0f ? 0.1f : 1.0f),
                                        kAmFreqMinHz,
                                        kAmFreqMaxHz);
            break;
        case Control::AmFreqInc:
            ch.am_freq_hz = clamp_float(ch.am_freq_hz + (ch.am_freq_hz < 10.0f ? 0.1f : 1.0f),
                                        kAmFreqMinHz,
                                        kAmFreqMaxHz);
            break;
        case Control::AmFreqSlider:
            if (am_freq_slider_[channel_index] != nullptr) {
                ch.am_freq_hz = am_freq_from_slider(lv_slider_get_value(am_freq_slider_[channel_index]));
            }
            break;
        case Control::FmEnable:
            ch.fm_enabled = !ch.fm_enabled;
            break;
        case Control::FmWave:
            ch.fm_wave = static_cast<int>(lv_dropdown_get_selected(fm_wave_dropdown_[channel_index]));
            break;
        case Control::FmBaseEdit:
            if (event_code == LV_EVENT_VALUE_CHANGED) {
                ch.fm_base_hz = clamp_u32(static_cast<uint32_t>(
                                              dropdown_selected_float(fm_base_label_[channel_index],
                                                                      static_cast<float>(ch.fm_base_hz)) + 0.5f),
                                          20,
                                          40000);
            } else if (drag_amount != 0.0f) {
                int32_t next = static_cast<int32_t>(ch.fm_base_hz) +
                               static_cast<int32_t>(drag_amount * 5.0f);
                if (next < 20) {
                    next = 20;
                } else if (next > 40000) {
                    next = 40000;
                }
                ch.fm_base_hz = static_cast<uint32_t>(next);
            }
            break;
        case Control::FmBaseDec:
            ch.fm_base_hz = ch.fm_base_hz > 100 ? ch.fm_base_hz - 100 : 20;
            break;
        case Control::FmBaseInc:
            ch.fm_base_hz = clamp_u32(ch.fm_base_hz + 100, 20, 40000);
            break;
        case Control::FmDevEdit:
            if (event_code == LV_EVENT_VALUE_CHANGED) {
                ch.fm_dev_hz = clamp_float(dropdown_selected_float(fm_dev_label_[channel_index],
                                                                   ch.fm_dev_hz),
                                           0.0f,
                                           20000.0f);
            } else if (drag_amount != 0.0f) {
                const float step = ch.fm_dev_hz < 10.0f ? 0.05f : (ch.fm_dev_hz < 200.0f ? 0.5f : 5.0f);
                ch.fm_dev_hz = clamp_float(ch.fm_dev_hz + (drag_amount * step), 0.0f, 20000.0f);
            }
            break;
        case Control::FmDevDec:
            ch.fm_dev_hz = clamp_float(ch.fm_dev_hz - (ch.fm_dev_hz <= 10.0f ? 0.1f : (ch.fm_dev_hz < 200.0f ? 1.0f : 100.0f)),
                                       0.0f, 20000.0f);
            break;
        case Control::FmDevInc:
            ch.fm_dev_hz = clamp_float(ch.fm_dev_hz + (ch.fm_dev_hz < 10.0f ? 0.1f : (ch.fm_dev_hz < 200.0f ? 1.0f : 100.0f)),
                                       0.0f, 20000.0f);
            break;
        case Control::FmFreqEdit:
            if (event_code == LV_EVENT_VALUE_CHANGED) {
                ch.fm_freq_hz = clamp_float(dropdown_selected_float(fm_freq_label_[channel_index],
                                                                    ch.fm_freq_hz),
                                            0.1f,
                                            500.0f);
            } else if (drag_amount != 0.0f) {
                const float step = ch.fm_freq_hz < 10.0f ? 0.02f : 0.2f;
                ch.fm_freq_hz = clamp_float(ch.fm_freq_hz + (drag_amount * step), 0.1f, 500.0f);
            }
            break;
        case Control::FmFreqDec:
            ch.fm_freq_hz = clamp_float(ch.fm_freq_hz - (ch.fm_freq_hz <= 10.0f ? 0.1f : 1.0f), 0.1f, 500.0f);
            break;
        case Control::FmFreqInc:
            ch.fm_freq_hz = clamp_float(ch.fm_freq_hz + (ch.fm_freq_hz < 10.0f ? 0.1f : 1.0f), 0.1f, 500.0f);
            break;
        default:
            break;
        }

        ch.carrier_wave = static_cast<int>(clamp_u32(ch.carrier_wave, 0,
                                                     carrier_wave_count_ > 0 ? carrier_wave_count_ - 1 : 0));
        ch.am_wave = static_cast<int>(clamp_u32(ch.am_wave, 0,
                                                mod_wave_count_ > 0 ? mod_wave_count_ - 1 : 0));
        ch.fm_wave = static_cast<int>(clamp_u32(ch.fm_wave, 0,
                                                mod_wave_count_ > 0 ? mod_wave_count_ - 1 : 0));

        xSemaphoreGive(state_lock_);
    }

    if (restart_audio) {
        stop_audio();
        start_audio();
        return;
    }

    (void)event;
    refresh_ui();
}

void SignalGenerator::refresh_wave_dropdown_options(void)
{
    for (size_t i = 0; i < 2; i++) {
        if (carrier_wave_dropdown_[i] != nullptr) {
            lv_dropdown_set_options(carrier_wave_dropdown_[i], carrier_options_);
        }
        if (am_wave_dropdown_[i] != nullptr) {
            lv_dropdown_set_options(am_wave_dropdown_[i], mod_options_);
        }
        if (fm_wave_dropdown_[i] != nullptr) {
            lv_dropdown_set_options(fm_wave_dropdown_[i], mod_options_);
        }
    }
}

void SignalGenerator::refresh_ui(void)
{
    if (root_ == nullptr) {
        return;
    }

    GeneratorState snapshot = {};
    if ((state_lock_ != nullptr) && (xSemaphoreTake(state_lock_, portMAX_DELAY) == pdTRUE)) {
        snapshot = state_;
        xSemaphoreGive(state_lock_);
    } else {
        snapshot = state_;
    }

    if (start_label_ != nullptr) {
        lv_label_set_text(start_label_, audio_running_ ? "STOP" : "START");
    }
    if (sample_rate_dropdown_ != nullptr) {
        lv_dropdown_set_selected(sample_rate_dropdown_, sample_rate_to_dropdown_index(snapshot.sample_rate_hz));
    }
    if (volume_label_ != nullptr) {
        lv_label_set_text_fmt(volume_label_, "%u%%", snapshot.volume_percent);
    }
    if (status_label_ != nullptr) {
        lv_label_set_text(status_label_, runtime_status_);
    }
    if (script_mode_label_ != nullptr) {
        lv_label_set_text(script_mode_label_, script_mode_ ? "CTRL" : "SCRIPT");
    }
    if (scope_box_ != nullptr) {
        if (script_mode_) {
            lv_obj_add_flag(scope_box_, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_clear_flag(scope_box_, LV_OBJ_FLAG_HIDDEN);
        }
    }
    if (tab_bar_ != nullptr) {
        if (script_mode_) {
            lv_obj_add_flag(tab_bar_, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_clear_flag(tab_bar_, LV_OBJ_FLAG_HIDDEN);
        }
    }
    if (channel_stack_ != nullptr) {
        if (script_mode_) {
            lv_obj_add_flag(channel_stack_, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_clear_flag(channel_stack_, LV_OBJ_FLAG_HIDDEN);
        }
    }
    if (script_root_ != nullptr) {
        if (script_mode_) {
            lv_obj_clear_flag(script_root_, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(script_root_, LV_OBJ_FLAG_HIDDEN);
        }
    }

    for (size_t i = 0; i < 2; i++) {
        const ChannelConfig &ch = snapshot.ch[i];

        if (channel_panel_[i] != nullptr) {
            if (i == active_channel_) {
                lv_obj_clear_flag(channel_panel_[i], LV_OBJ_FLAG_HIDDEN);
            } else {
                lv_obj_add_flag(channel_panel_[i], LV_OBJ_FLAG_HIDDEN);
            }
        }

        if (tab_button_[i] != nullptr) {
            const bool active = i == active_channel_;
            lv_obj_set_style_bg_color(tab_button_[i],
                                      lv_color_hex(active ? (i == 0 ? 0x2179A8 : 0xB96A22) : 0x202834),
                                      0);
            lv_obj_set_style_border_color(tab_button_[i],
                                          lv_color_hex(active ? (i == 0 ? 0x65C8F4 : 0xFFB36B) : 0x354151),
                                          0);
        }

        if (tab_label_[i] != nullptr) {
            lv_obj_set_style_text_color(tab_label_[i],
                                        lv_color_hex(i == active_channel_ ? 0xFFFFFF : 0xC8D3E0),
                                        0);
        }

        if (ch_enable_switch_[i] != nullptr) {
            if (ch.enabled) {
                lv_obj_add_state(ch_enable_switch_[i], LV_STATE_CHECKED);
            } else {
                lv_obj_remove_state(ch_enable_switch_[i], LV_STATE_CHECKED);
            }
        }
        if (am_enable_switch_[i] != nullptr) {
            if (ch.am_enabled) {
                lv_obj_add_state(am_enable_switch_[i], LV_STATE_CHECKED);
            } else {
                lv_obj_remove_state(am_enable_switch_[i], LV_STATE_CHECKED);
            }
        }
        if (fm_enable_switch_[i] != nullptr) {
            if (ch.fm_enabled) {
                lv_obj_add_state(fm_enable_switch_[i], LV_STATE_CHECKED);
            } else {
                lv_obj_remove_state(fm_enable_switch_[i], LV_STATE_CHECKED);
            }
        }

        if (carrier_wave_dropdown_[i] != nullptr) {
            lv_dropdown_set_selected(carrier_wave_dropdown_[i], ch.carrier_wave);
            const size_t wave_index = clamp_wave_index(ch.carrier_wave, carrier_wave_count_);
            const char *name = (carrier_waves_ != nullptr) && (carrier_wave_count_ > 0) ?
                               carrier_waves_[wave_index].name : "---";
            char text[48] = {};
            snprintf(text, sizeof(text), "Carrier: %s", name);
            lv_dropdown_set_text(carrier_wave_dropdown_[i], text);
        }
        if (am_wave_dropdown_[i] != nullptr) {
            lv_dropdown_set_selected(am_wave_dropdown_[i], ch.am_wave);
            const size_t wave_index = clamp_wave_index(ch.am_wave, mod_wave_count_);
            const char *name = (mod_waves_ != nullptr) && (mod_wave_count_ > 0) ?
                               mod_waves_[wave_index].name : "---";
            char text[48] = {};
            snprintf(text, sizeof(text), "AM: %s", name);
            lv_dropdown_set_text(am_wave_dropdown_[i], text);
        }
        if (fm_wave_dropdown_[i] != nullptr) {
            lv_dropdown_set_selected(fm_wave_dropdown_[i], ch.fm_wave);
            const size_t wave_index = clamp_wave_index(ch.fm_wave, mod_wave_count_);
            const char *name = (mod_waves_ != nullptr) && (mod_wave_count_ > 0) ?
                               mod_waves_[wave_index].name : "---";
            char text[48] = {};
            snprintf(text, sizeof(text), "FM: %s", name);
            lv_dropdown_set_text(fm_wave_dropdown_[i], text);
        }

        if (carrier_freq_label_[i] != nullptr) {
            char text[24] = {};
            snprintf(text, sizeof(text), "%lu Hz", static_cast<unsigned long>(ch.carrier_freq_hz));
            lv_dropdown_set_text(carrier_freq_label_[i], text);
        }
        if (am_freq_label_[i] != nullptr) {
            char text[24] = {};
            format_hz(text, sizeof(text), ch.am_freq_hz);
            lv_dropdown_set_text(am_freq_label_[i], text);
        }
        if ((am_freq_slider_[i] != nullptr) && !lv_slider_is_dragged(am_freq_slider_[i])) {
            lv_slider_set_value(am_freq_slider_[i], am_freq_to_slider(ch.am_freq_hz), LV_ANIM_OFF);
        }
        if (fm_base_label_[i] != nullptr) {
            char text[24] = {};
            snprintf(text, sizeof(text), "%lu Hz", static_cast<unsigned long>(ch.fm_base_hz));
            lv_dropdown_set_text(fm_base_label_[i], text);
        }
        if (fm_dev_label_[i] != nullptr) {
            char text[24] = {};
            format_hz(text, sizeof(text), ch.fm_dev_hz);
            lv_dropdown_set_text(fm_dev_label_[i], text);
        }
        if (fm_freq_label_[i] != nullptr) {
            char text[24] = {};
            format_hz(text, sizeof(text), ch.fm_freq_hz);
            lv_dropdown_set_text(fm_freq_label_[i], text);
        }

        if (i == active_channel_) {
            render_channel_previews(static_cast<uint8_t>(i), ch);
        }
    }

    refresh_script_ui();
    render_scope();
}

void SignalGenerator::set_runtime_status(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsnprintf(runtime_status_, sizeof(runtime_status_), fmt, args);
    va_end(args);

    if (status_label_ != nullptr) {
        lv_label_set_text(status_label_, runtime_status_);
    }
}

void SignalGenerator::control_event_cb(lv_event_t *event)
{
    ControlEventData *data = static_cast<ControlEventData *>(lv_event_get_user_data(event));
    if ((data != nullptr) && (data->app != nullptr)) {
        data->app->handle_control(data, event);
    }
}

void SignalGenerator::scope_timer_cb(lv_timer_t *timer)
{
    SignalGenerator *app = static_cast<SignalGenerator *>(lv_timer_get_user_data(timer));
    if (app != nullptr) {
        app->render_scope();
    }
}

void SignalGenerator::script_timer_cb(lv_timer_t *timer)
{
    SignalGenerator *app = static_cast<SignalGenerator *>(lv_timer_get_user_data(timer));
    if (app != nullptr) {
        app->script_tick();
    }
}

void SignalGenerator::audio_task_entry(void *arg)
{
    SignalGenerator *app = static_cast<SignalGenerator *>(arg);
    if (app != nullptr) {
        app->audio_task_main();
    }

    vTaskDelete(nullptr);
}
