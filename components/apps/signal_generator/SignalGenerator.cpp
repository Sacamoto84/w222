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

static int16_t float_to_i16(float value)
{
    value = clamp_float(value, -1.0f, 1.0f);
    return static_cast<int16_t>(value * 32767.0f);
}

static void advance_phase(double &phase, double increment)
{
    phase += increment;
    phase -= floor(phase);
    if (phase < 0.0) {
        phase += 1.0;
    }
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

    if (carrier_waves_ != nullptr) {
        heap_caps_free(carrier_waves_);
        carrier_waves_ = nullptr;
    }

    if (mod_waves_ != nullptr) {
        heap_caps_free(mod_waves_);
        mod_waves_ = nullptr;
    }
}

bool SignalGenerator::init(void)
{
    if (state_lock_ == nullptr) {
        state_lock_ = xSemaphoreCreateMutex();
    }

    if (!allocate_waveforms()) {
        return false;
    }

    load_waveforms();
    return state_lock_ != nullptr;
}

bool SignalGenerator::open(lv_obj_t *parent)
{
    if ((parent == nullptr) || !init()) {
        return false;
    }

    stop_audio();
    load_waveforms();
    create_ui(parent);
    refresh_wave_dropdown_options();
    refresh_ui();
    return true;
}

void SignalGenerator::close(void)
{
    stop_audio();

    root_ = nullptr;
    status_label_ = nullptr;
    start_label_ = nullptr;
    sample_rate_dropdown_ = nullptr;
    volume_label_ = nullptr;
    for (size_t i = 0; i < 2; i++) {
        ch_enable_switch_[i] = nullptr;
        carrier_wave_dropdown_[i] = nullptr;
        am_wave_dropdown_[i] = nullptr;
        fm_wave_dropdown_[i] = nullptr;
        carrier_freq_label_[i] = nullptr;
        am_freq_label_[i] = nullptr;
        fm_base_label_[i] = nullptr;
        fm_dev_label_[i] = nullptr;
        fm_freq_label_[i] = nullptr;
        am_enable_switch_[i] = nullptr;
        fm_enable_switch_[i] = nullptr;
    }
    control_event_count_ = 0;
}

void SignalGenerator::set_default_state(void)
{
    state_ = {};
    state_.sample_rate_hz = 192000;
    state_.volume_percent = 30;

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
        .carrier_phase = 0.0,
        .am_phase = 0.0,
        .fm_phase = 0.0,
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
                                                                  MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
        if (carrier_waves_ == nullptr) {
            carrier_waves_ = static_cast<Waveform *>(heap_caps_calloc(kMaxWaveforms, sizeof(Waveform),
                                                                      MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
        }
    }

    if (mod_waves_ == nullptr) {
        mod_waves_ = static_cast<Waveform *>(heap_caps_calloc(kMaxWaveforms, sizeof(Waveform),
                                                              MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
        if (mod_waves_ == nullptr) {
            mod_waves_ = static_cast<Waveform *>(heap_caps_calloc(kMaxWaveforms, sizeof(Waveform),
                                                                  MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
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
    chan_cfg.dma_desc_num = 8;
    chan_cfg.dma_frame_num = kAudioFrames;

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

    return ESP_OK;
}

void SignalGenerator::deinit_i2s(void)
{
    if (tx_chan_ == nullptr) {
        return;
    }

    i2s_channel_disable(tx_chan_);
    i2s_del_channel(tx_chan_);
    tx_chan_ = nullptr;
}

bool SignalGenerator::start_audio(void)
{
    if (audio_running_) {
        return true;
    }

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
    set_runtime_status("Stopped | PCM5102: BCLK33 LRCK31 DIN30 | CH1=L CH2=R");
    refresh_ui();
}

void SignalGenerator::audio_task_main(void)
{
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
        return;
    }

    if (xSemaphoreTake(state_lock_, pdMS_TO_TICKS(20)) != pdTRUE) {
        memset(audio_buffer_, 0, sizeof(audio_buffer_));
        return;
    }

    const uint32_t sample_rate_hz = state_.sample_rate_hz;
    const float volume = static_cast<float>(state_.volume_percent) / 100.0f;

    for (size_t i = 0; i < kAudioFrames; i++) {
        audio_buffer_[i * 2] = float_to_i16(render_channel(state_.ch[0], sample_rate_hz) * volume);
        audio_buffer_[(i * 2) + 1] = float_to_i16(render_channel(state_.ch[1], sample_rate_hz) * volume);
    }

    xSemaphoreGive(state_lock_);
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

float SignalGenerator::render_channel(ChannelConfig &channel, uint32_t sample_rate_hz)
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
        const float fm_lfo = lookup_wave(fm_wave, channel.fm_phase);
        carrier_freq_hz = static_cast<float>(channel.fm_base_hz) + (channel.fm_dev_hz * fm_lfo);
        advance_phase(channel.fm_phase, static_cast<double>(channel.fm_freq_hz) / sample_rate_hz);
    }

    const float nyquist_safe_hz = static_cast<float>(sample_rate_hz) * 0.45f;
    carrier_freq_hz = clamp_float(carrier_freq_hz, 0.0f, nyquist_safe_hz);

    float sample = lookup_wave(carrier_wave, channel.carrier_phase);
    advance_phase(channel.carrier_phase, static_cast<double>(carrier_freq_hz) / sample_rate_hz);

    if (channel.am_enabled) {
        const float am_lfo = lookup_wave(am_wave, channel.am_phase);
        const float envelope = 0.5f + (0.5f * am_lfo);
        sample *= envelope;
        advance_phase(channel.am_phase, static_cast<double>(channel.am_freq_hz) / sample_rate_hz);
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

    lv_obj_t *channels = lv_obj_create(root_);
    make_plain_container(channels);
    lv_obj_set_width(channels, lv_pct(100));
    lv_obj_set_flex_grow(channels, 1);
    lv_obj_set_style_pad_column(channels, 8, 0);
    lv_obj_set_flex_flow(channels, LV_FLEX_FLOW_ROW);

    create_channel_panel(channels, 0);
    create_channel_panel(channels, 1);
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

    status_label_ = create_text_label(toolbar, runtime_status_, &lv_font_montserrat_14, 0x9FB0C2);
    lv_obj_set_flex_grow(status_label_, 1);
    lv_obj_set_width(status_label_, 1);
}

void SignalGenerator::create_channel_panel(lv_obj_t *parent, uint8_t channel)
{
    lv_obj_t *panel = lv_obj_create(parent);
    lv_obj_set_width(panel, 1);
    lv_obj_set_flex_grow(panel, 1);
    lv_obj_set_height(panel, lv_pct(100));
    lv_obj_set_style_radius(panel, 8, 0);
    lv_obj_set_style_bg_color(panel, lv_color_hex(0x171E28), 0);
    lv_obj_set_style_bg_opa(panel, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(panel, 1, 0);
    lv_obj_set_style_border_color(panel, lv_color_hex(0x2B3645), 0);
    lv_obj_set_style_pad_all(panel, 8, 0);
    lv_obj_set_style_pad_row(panel, 5, 0);
    lv_obj_set_flex_flow(panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scrollbar_mode(panel, LV_SCROLLBAR_MODE_AUTO);

    char title[16] = {};
    snprintf(title, sizeof(title), "CH%u  %s", channel + 1, channel == 0 ? "Left" : "Right");
    create_text_label(panel, title, &lv_font_montserrat_16, channel == 0 ? 0x7CD3FF : 0xFFB36B);

    create_switch_row(panel, "Output", &ch_enable_switch_[channel], Control::ChEnable, channel);
    create_dropdown_row(panel, "Carrier", &carrier_wave_dropdown_[channel], Control::CarrierWave, channel);
    create_adjust_row(panel, "Carrier F", &carrier_freq_label_[channel],
                      Control::CarrierFreqDec, Control::CarrierFreqInc, channel);

    create_switch_row(panel, "AM", &am_enable_switch_[channel], Control::AmEnable, channel);
    create_dropdown_row(panel, "AM wave", &am_wave_dropdown_[channel], Control::AmWave, channel);
    create_adjust_row(panel, "AM F", &am_freq_label_[channel],
                      Control::AmFreqDec, Control::AmFreqInc, channel);

    create_switch_row(panel, "FM", &fm_enable_switch_[channel], Control::FmEnable, channel);
    create_dropdown_row(panel, "FM wave", &fm_wave_dropdown_[channel], Control::FmWave, channel);
    create_adjust_row(panel, "FM base", &fm_base_label_[channel],
                      Control::FmBaseDec, Control::FmBaseInc, channel);
    create_adjust_row(panel, "FM dev", &fm_dev_label_[channel],
                      Control::FmDevDec, Control::FmDevInc, channel);
    create_adjust_row(panel, "FM F", &fm_freq_label_[channel],
                      Control::FmFreqDec, Control::FmFreqInc, channel);
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
    lv_obj_set_height(row, 34);
    lv_obj_set_style_pad_column(row, 5, 0);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *name_label = create_text_label(row, name, &lv_font_montserrat_14, 0xC8D3E0);
    lv_obj_set_width(name_label, 72);

    lv_obj_t *dec_button = create_button(row, "-", 34);
    attach_event(dec_button, dec_control, channel, LV_EVENT_CLICKED);

    *value_label = create_text_label(row, "", &lv_font_montserrat_14, 0xEEF4FA);
    lv_obj_set_width(*value_label, 76);
    lv_obj_set_style_text_align(*value_label, LV_TEXT_ALIGN_CENTER, 0);

    lv_obj_t *inc_button = create_button(row, "+", 34);
    attach_event(inc_button, inc_control, channel, LV_EVENT_CLICKED);

    return row;
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
    lv_obj_set_height(row, 36);
    lv_obj_set_style_pad_column(row, 5, 0);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *name_label = create_text_label(row, name, &lv_font_montserrat_14, 0xC8D3E0);
    lv_obj_set_width(name_label, 72);

    *dropdown = lv_dropdown_create(row);
    lv_obj_set_height(*dropdown, 34);
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
    lv_obj_set_height(row, 34);
    lv_obj_set_style_pad_column(row, 5, 0);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *name_label = create_text_label(row, name, &lv_font_montserrat_14, 0xC8D3E0);
    lv_obj_set_flex_grow(name_label, 1);
    lv_obj_set_width(name_label, 1);

    *switch_obj = lv_switch_create(row);
    lv_obj_set_size(*switch_obj, 54, 28);
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
    if ((data == nullptr) || (data->channel >= 2)) {
        return;
    }

    const Control control = data->control;
    const uint8_t channel_index = data->channel;

    if (control == Control::StartStop) {
        if (audio_running_) {
            stop_audio();
        } else {
            start_audio();
        }
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

    bool restart_audio = false;
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
            ch.enabled = lv_obj_has_state(ch_enable_switch_[channel_index], LV_STATE_CHECKED);
            break;
        case Control::CarrierWave:
            ch.carrier_wave = static_cast<int>(lv_dropdown_get_selected(carrier_wave_dropdown_[channel_index]));
            break;
        case Control::CarrierFreqDec:
            ch.carrier_freq_hz = ch.carrier_freq_hz > 100 ? ch.carrier_freq_hz - 100 : 20;
            break;
        case Control::CarrierFreqInc:
            ch.carrier_freq_hz = clamp_u32(ch.carrier_freq_hz + 100, 20, 40000);
            break;
        case Control::AmEnable:
            ch.am_enabled = lv_obj_has_state(am_enable_switch_[channel_index], LV_STATE_CHECKED);
            break;
        case Control::AmWave:
            ch.am_wave = static_cast<int>(lv_dropdown_get_selected(am_wave_dropdown_[channel_index]));
            break;
        case Control::AmFreqDec:
            ch.am_freq_hz = clamp_float(ch.am_freq_hz - (ch.am_freq_hz <= 10.0f ? 0.1f : 1.0f), 0.1f, 500.0f);
            break;
        case Control::AmFreqInc:
            ch.am_freq_hz = clamp_float(ch.am_freq_hz + (ch.am_freq_hz < 10.0f ? 0.1f : 1.0f), 0.1f, 500.0f);
            break;
        case Control::FmEnable:
            ch.fm_enabled = lv_obj_has_state(fm_enable_switch_[channel_index], LV_STATE_CHECKED);
            break;
        case Control::FmWave:
            ch.fm_wave = static_cast<int>(lv_dropdown_get_selected(fm_wave_dropdown_[channel_index]));
            break;
        case Control::FmBaseDec:
            ch.fm_base_hz = ch.fm_base_hz > 100 ? ch.fm_base_hz - 100 : 20;
            break;
        case Control::FmBaseInc:
            ch.fm_base_hz = clamp_u32(ch.fm_base_hz + 100, 20, 40000);
            break;
        case Control::FmDevDec:
            ch.fm_dev_hz = clamp_float(ch.fm_dev_hz - (ch.fm_dev_hz <= 10.0f ? 0.1f : (ch.fm_dev_hz < 200.0f ? 1.0f : 100.0f)),
                                       0.0f, 20000.0f);
            break;
        case Control::FmDevInc:
            ch.fm_dev_hz = clamp_float(ch.fm_dev_hz + (ch.fm_dev_hz < 10.0f ? 0.1f : (ch.fm_dev_hz < 200.0f ? 1.0f : 100.0f)),
                                       0.0f, 20000.0f);
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

    for (size_t i = 0; i < 2; i++) {
        const ChannelConfig &ch = snapshot.ch[i];

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
        }
        if (am_wave_dropdown_[i] != nullptr) {
            lv_dropdown_set_selected(am_wave_dropdown_[i], ch.am_wave);
        }
        if (fm_wave_dropdown_[i] != nullptr) {
            lv_dropdown_set_selected(fm_wave_dropdown_[i], ch.fm_wave);
        }

        if (carrier_freq_label_[i] != nullptr) {
            lv_label_set_text_fmt(carrier_freq_label_[i], "%lu Hz", static_cast<unsigned long>(ch.carrier_freq_hz));
        }
        if (am_freq_label_[i] != nullptr) {
            char text[24] = {};
            format_hz(text, sizeof(text), ch.am_freq_hz);
            lv_label_set_text(am_freq_label_[i], text);
        }
        if (fm_base_label_[i] != nullptr) {
            lv_label_set_text_fmt(fm_base_label_[i], "%lu Hz", static_cast<unsigned long>(ch.fm_base_hz));
        }
        if (fm_dev_label_[i] != nullptr) {
            char text[24] = {};
            format_hz(text, sizeof(text), ch.fm_dev_hz);
            lv_label_set_text(fm_dev_label_[i], text);
        }
        if (fm_freq_label_[i] != nullptr) {
            char text[24] = {};
            format_hz(text, sizeof(text), ch.fm_freq_hz);
            lv_label_set_text(fm_freq_label_[i], text);
        }
    }
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

void SignalGenerator::audio_task_entry(void *arg)
{
    SignalGenerator *app = static_cast<SignalGenerator *>(arg);
    if (app != nullptr) {
        app->audio_task_main();
    }

    vTaskDelete(nullptr);
}
