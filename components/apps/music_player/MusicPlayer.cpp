#include "music_player/MusicPlayer.hpp"

#include <dirent.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "driver/gpio.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "sdkconfig.h"

namespace {

static const char *TAG = "music_player";
static constexpr const char *kMusicDir = CONFIG_BSP_SD_MOUNT_POINT "/music";
static constexpr i2s_port_t kExternalI2sPort = I2S_NUM_0;
static constexpr gpio_num_t kI2sBclkPin = GPIO_NUM_33;
static constexpr gpio_num_t kI2sWsPin = GPIO_NUM_31;
static constexpr gpio_num_t kI2sDoutPin = GPIO_NUM_30;
static constexpr uint32_t kDefaultSampleRateHz = 44100;
static constexpr uint32_t kDefaultBitsPerSample = 16;
static constexpr uint32_t kI2sDmaDescNum = 8;
static constexpr uint32_t kI2sDmaFrames = 512;
static constexpr uint32_t kUiRefreshMs = 160;
static constexpr int32_t kSeekSliderMax = 1000;
#if CONFIG_FREERTOS_UNICORE
static constexpr BaseType_t kAudioTaskCore = tskNO_AFFINITY;
#else
static constexpr BaseType_t kAudioTaskCore = 0;
#endif
static constexpr UBaseType_t kAudioTaskPriority = tskIDLE_PRIORITY + 5;

struct Mp3StreamContext {
    MusicPlayer *app;
    FILE *fp;
    uint32_t file_size;
    uint32_t start_offset;
};

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

static bool is_mp3_name(const char *name)
{
    return str_ends_with_ignore_case(name, ".mp3");
}

static void join_path(char *out, size_t out_size, const char *dir, const char *name)
{
    const size_t len = strlen(dir);
    if ((len > 0) && (dir[len - 1] == '/')) {
        snprintf(out, out_size, "%s%s", dir, name);
    } else {
        snprintf(out, out_size, "%s/%s", dir, name);
    }
}

static void make_plain_container(lv_obj_t *obj)
{
    lv_obj_remove_style_all(obj);
    lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_pad_all(obj, 0, 0);
}

static lv_obj_t *create_label(lv_obj_t *parent, const char *text, const lv_font_t *font, uint32_t color)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_label_set_long_mode(label, LV_LABEL_LONG_DOT);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
    return label;
}

static void make_child_passthrough(lv_obj_t *obj)
{
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_EVENT_BUBBLE);
}

static lv_obj_t *create_icon_button(lv_obj_t *parent, const char *symbol, lv_coord_t width, uint32_t bg_color)
{
    lv_obj_t *button = lv_button_create(parent);
    lv_obj_set_size(button, width, 42);
    lv_obj_set_ext_click_area(button, 10);
    lv_obj_add_flag(button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(button, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(button, 3, 0);
    lv_obj_set_style_bg_color(button, lv_color_hex(bg_color), 0);
    lv_obj_set_style_bg_grad_color(button, lv_color_hex(0xD9DCE0), 0);
    lv_obj_set_style_bg_grad_dir(button, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_bg_color(button, lv_color_hex(0x58606C), LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(button, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(button, 2, 0);
    lv_obj_set_style_border_color(button, lv_color_hex(0x111318), 0);
    lv_obj_set_style_shadow_width(button, 0, 0);

    lv_obj_t *label = create_label(button, symbol, &lv_font_montserrat_16, 0x111318);
    lv_obj_center(label);
    make_child_passthrough(label);
    return button;
}

static void style_panel(lv_obj_t *panel)
{
    lv_obj_set_style_radius(panel, 4, 0);
    lv_obj_set_style_bg_color(panel, lv_color_hex(0x6E7681), 0);
    lv_obj_set_style_bg_grad_color(panel, lv_color_hex(0x2A2F38), 0);
    lv_obj_set_style_bg_grad_dir(panel, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_bg_opa(panel, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(panel, 2, 0);
    lv_obj_set_style_border_color(panel, lv_color_hex(0x111318), 0);
    lv_obj_set_style_pad_all(panel, 8, 0);
}

static lv_obj_t *create_title_bar(lv_obj_t *parent, const char *title, const char *right_text)
{
    lv_obj_t *bar = lv_obj_create(parent);
    make_plain_container(bar);
    lv_obj_set_width(bar, lv_pct(100));
    lv_obj_set_height(bar, 28);
    lv_obj_set_style_bg_color(bar, lv_color_hex(0x0D2B7A), 0);
    lv_obj_set_style_bg_grad_color(bar, lv_color_hex(0x3F85D7), 0);
    lv_obj_set_style_bg_grad_dir(bar, LV_GRAD_DIR_HOR, 0);
    lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(bar, 2, 0);
    lv_obj_set_style_pad_left(bar, 8, 0);
    lv_obj_set_style_pad_right(bar, 8, 0);
    lv_obj_set_flex_flow(bar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(bar, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    create_label(bar, title, &lv_font_montserrat_14, 0xFFFFFF);
    create_label(bar, right_text, &lv_font_montserrat_14, 0xFFD46B);
    return bar;
}

static void style_lcd(lv_obj_t *lcd)
{
    lv_obj_set_style_radius(lcd, 3, 0);
    lv_obj_set_style_bg_color(lcd, lv_color_hex(0x041006), 0);
    lv_obj_set_style_bg_grad_color(lcd, lv_color_hex(0x0B2110), 0);
    lv_obj_set_style_bg_grad_dir(lcd, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_bg_opa(lcd, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(lcd, 2, 0);
    lv_obj_set_style_border_color(lcd, lv_color_hex(0x101318), 0);
    lv_obj_set_style_pad_all(lcd, 8, 0);
}

static void style_winamp_slider(lv_obj_t *slider)
{
    lv_obj_set_style_radius(slider, 1, LV_PART_MAIN);
    lv_obj_set_style_bg_color(slider, lv_color_hex(0x14171D), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(slider, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(slider, lv_color_hex(0xF0A020), LV_PART_INDICATOR);
    lv_obj_set_style_bg_grad_color(slider, lv_color_hex(0xFFE06C), LV_PART_INDICATOR);
    lv_obj_set_style_bg_grad_dir(slider, LV_GRAD_DIR_HOR, LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(slider, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(slider, lv_color_hex(0xDDE2E8), LV_PART_KNOB);
    lv_obj_set_style_bg_opa(slider, LV_OPA_COVER, LV_PART_KNOB);
    lv_obj_set_style_radius(slider, 2, LV_PART_KNOB);
    lv_obj_set_style_pad_all(slider, 7, LV_PART_KNOB);
}

static lv_obj_t *create_eq_bar(lv_obj_t *parent)
{
    lv_obj_t *bar = lv_bar_create(parent);
    lv_obj_set_size(bar, 8, 48);
    lv_bar_set_range(bar, 0, 100);
    lv_bar_set_value(bar, 12, LV_ANIM_OFF);
    lv_obj_set_style_radius(bar, 1, LV_PART_MAIN);
    lv_obj_set_style_bg_color(bar, lv_color_hex(0x061006), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(bar, 1, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(bar, lv_color_hex(0x37FF60), LV_PART_INDICATOR);
    lv_obj_set_style_bg_grad_color(bar, lv_color_hex(0xF0A020), LV_PART_INDICATOR);
    lv_obj_set_style_bg_grad_dir(bar, LV_GRAD_DIR_VER, LV_PART_INDICATOR);
    return bar;
}

static void style_list_button(lv_obj_t *button)
{
    lv_obj_set_height(button, 44);
    lv_obj_set_style_bg_color(button, lv_color_hex(0x06080B), 0);
    lv_obj_set_style_bg_color(button, lv_color_hex(0x11251A), LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(button, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(button, 1, 0);
    lv_obj_set_style_border_color(button, lv_color_hex(0x172018), 0);
    lv_obj_set_style_radius(button, 2, 0);
    lv_obj_set_style_margin_bottom(button, 3, 0);
    lv_obj_set_style_text_color(button, lv_color_hex(0x69FF7A), 0);

    const uint32_t child_count = lv_obj_get_child_count(button);
    for (uint32_t i = 0; i < child_count; i++) {
        lv_obj_t *child = lv_obj_get_child(button, i);
        lv_obj_set_style_text_color(child, lv_color_hex(0x69FF7A), 0);
    }
}

static void format_size(char *out, size_t out_size, uint32_t bytes)
{
    if (bytes >= (1024U * 1024U)) {
        snprintf(out, out_size, "%lu.%lu MB",
                 static_cast<unsigned long>(bytes / (1024U * 1024U)),
                 static_cast<unsigned long>(((bytes % (1024U * 1024U)) * 10U) / (1024U * 1024U)));
    } else {
        snprintf(out, out_size, "%lu KB", static_cast<unsigned long>((bytes + 1023U) / 1024U));
    }
}

static QueueHandle_t event_queue_from_handle(void *handle)
{
    return static_cast<QueueHandle_t>(handle);
}

static i2s_mclk_multiple_t mclk_multiple_for_bits(uint32_t bits_per_sample)
{
    return bits_per_sample == 24 ? I2S_MCLK_MULTIPLE_384 : I2S_MCLK_MULTIPLE_256;
}

static i2s_data_bit_width_t i2s_bit_width(uint32_t bits_per_sample)
{
    switch (bits_per_sample) {
    case 8:
        return I2S_DATA_BIT_WIDTH_8BIT;
    case 24:
        return I2S_DATA_BIT_WIDTH_24BIT;
    case 32:
        return I2S_DATA_BIT_WIDTH_32BIT;
    case 16:
    default:
        return I2S_DATA_BIT_WIDTH_16BIT;
    }
}

} // namespace

MusicPlayer *MusicPlayer::active_player_ = nullptr;

MusicPlayer::MusicPlayer()
{
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
}

MusicPlayer::~MusicPlayer()
{
    close();

    if (event_queue_ != nullptr) {
        vQueueDelete(event_queue_from_handle(event_queue_));
        event_queue_ = nullptr;
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
}

bool MusicPlayer::init(void)
{
    if (entries_ == nullptr) {
        entries_ = static_cast<TrackEntry *>(heap_caps_calloc(kMaxEntries, sizeof(TrackEntry),
                                                              MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
    }

    if (entry_events_ == nullptr) {
        entry_events_ = static_cast<EntryEventData *>(heap_caps_calloc(kMaxEntries, sizeof(EntryEventData),
                                                                       MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
    }

    if (event_queue_ == nullptr) {
        event_queue_ = xQueueCreate(8, sizeof(int));
    }

    return (entries_ != nullptr) && (entry_events_ != nullptr) && (event_queue_ != nullptr);
}

bool MusicPlayer::open(lv_obj_t *parent)
{
    if ((parent == nullptr) || !init()) {
        return false;
    }

    lv_obj_set_style_bg_color(parent, lv_color_hex(0x151820), 0);
    lv_obj_set_style_bg_opa(parent, LV_OPA_COVER, 0);

    root_ = lv_obj_create(parent);
    make_plain_container(root_);
    lv_obj_set_size(root_, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(root_, lv_color_hex(0x151820), 0);
    lv_obj_set_style_bg_grad_color(root_, lv_color_hex(0x07090D), 0);
    lv_obj_set_style_bg_grad_dir(root_, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_bg_opa(root_, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(root_, 10, 0);
    lv_obj_set_style_pad_row(root_, 10, 0);
    lv_obj_set_flex_flow(root_, LV_FLEX_FLOW_COLUMN);

    lv_obj_t *deck = lv_obj_create(root_);
    style_panel(deck);
    lv_obj_set_width(deck, lv_pct(100));
    lv_obj_set_height(deck, 286);
    lv_obj_set_style_pad_row(deck, 6, 0);
    lv_obj_set_flex_flow(deck, LV_FLEX_FLOW_COLUMN);
    lv_obj_clear_flag(deck, LV_OBJ_FLAG_SCROLLABLE);

    create_title_bar(deck, "W222 AMP", "MP3 / PCM5102");

    lv_obj_t *lcd = lv_obj_create(deck);
    style_lcd(lcd);
    lv_obj_set_width(lcd, lv_pct(100));
    lv_obj_set_height(lcd, 92);
    lv_obj_set_style_pad_column(lcd, 8, 0);
    lv_obj_set_flex_flow(lcd, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(lcd, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(lcd, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *lcd_text = lv_obj_create(lcd);
    make_plain_container(lcd_text);
    lv_obj_set_width(lcd_text, 1);
    lv_obj_set_height(lcd_text, lv_pct(100));
    lv_obj_set_flex_grow(lcd_text, 1);
    lv_obj_set_style_pad_row(lcd_text, 3, 0);
    lv_obj_set_flex_flow(lcd_text, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(lcd_text, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    track_label_ = create_label(lcd_text, "NO TRACK", &lv_font_montserrat_22, 0x69FF7A);
    lv_obj_set_width(track_label_, lv_pct(100));
    file_label_ = create_label(lcd_text, "", &lv_font_montserrat_14, 0xF0A020);
    lv_obj_set_width(file_label_, lv_pct(100));
    state_label_ = create_label(lcd_text, "IDLE", &lv_font_montserrat_14, 0xC8FFC8);
    lv_obj_set_width(state_label_, lv_pct(100));

    lv_obj_t *spectrum = lv_obj_create(lcd);
    make_plain_container(spectrum);
    lv_obj_set_size(spectrum, 112, 54);
    lv_obj_set_style_pad_column(spectrum, 2, 0);
    lv_obj_set_flex_flow(spectrum, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(spectrum, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_END);
    for (size_t i = 0; i < sizeof(eq_bars_) / sizeof(eq_bars_[0]); i++) {
        eq_bars_[i] = create_eq_bar(spectrum);
    }

    lv_obj_t *seek_row = lv_obj_create(deck);
    make_plain_container(seek_row);
    lv_obj_set_width(seek_row, lv_pct(100));
    lv_obj_set_height(seek_row, 28);
    lv_obj_set_style_pad_column(seek_row, 8, 0);
    lv_obj_set_flex_flow(seek_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(seek_row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    create_label(seek_row, "POS", &lv_font_montserrat_14, 0x151820);

    seek_slider_ = lv_slider_create(seek_row);
    lv_obj_set_width(seek_slider_, 1);
    lv_obj_set_height(seek_slider_, 16);
    lv_obj_set_flex_grow(seek_slider_, 1);
    lv_obj_set_ext_click_area(seek_slider_, 12);
    lv_slider_set_range(seek_slider_, 0, kSeekSliderMax);
    lv_slider_set_value(seek_slider_, 0, LV_ANIM_OFF);
    style_winamp_slider(seek_slider_);
    lv_obj_add_event_cb(seek_slider_, control_event_cb, LV_EVENT_VALUE_CHANGED, &seek_event_);
    lv_obj_add_event_cb(seek_slider_, control_event_cb, LV_EVENT_RELEASED, &seek_event_);
    lv_obj_add_event_cb(seek_slider_, control_event_cb, LV_EVENT_PRESS_LOST, &seek_event_);

    seek_label_ = create_label(seek_row, "0%", &lv_font_montserrat_14, 0x151820);
    lv_obj_set_width(seek_label_, 48);
    lv_obj_set_style_text_align(seek_label_, LV_TEXT_ALIGN_RIGHT, 0);

    lv_obj_t *controls = lv_obj_create(deck);
    make_plain_container(controls);
    lv_obj_set_width(controls, lv_pct(100));
    lv_obj_set_height(controls, 46);
    lv_obj_set_style_pad_column(controls, 8, 0);
    lv_obj_set_flex_flow(controls, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(controls, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *prev_button = create_icon_button(controls, LV_SYMBOL_PREV, 58, 0xA7ACB3);
    lv_obj_add_event_cb(prev_button, control_event_cb, LV_EVENT_CLICKED, &previous_event_);

    lv_obj_t *play_button = create_icon_button(controls, LV_SYMBOL_PLAY, 72, 0xF0A020);
    play_label_ = lv_obj_get_child(play_button, 0);
    lv_obj_add_event_cb(play_button, control_event_cb, LV_EVENT_CLICKED, &play_event_);

    lv_obj_t *stop_button = create_icon_button(controls, LV_SYMBOL_STOP, 58, 0xA7ACB3);
    lv_obj_add_event_cb(stop_button, control_event_cb, LV_EVENT_CLICKED, &stop_event_);

    lv_obj_t *next_button = create_icon_button(controls, LV_SYMBOL_NEXT, 58, 0xA7ACB3);
    lv_obj_add_event_cb(next_button, control_event_cb, LV_EVENT_CLICKED, &next_event_);

    lv_obj_t *volume_row = lv_obj_create(deck);
    make_plain_container(volume_row);
    lv_obj_set_width(volume_row, lv_pct(100));
    lv_obj_set_height(volume_row, 34);
    lv_obj_set_style_pad_column(volume_row, 8, 0);
    lv_obj_set_flex_flow(volume_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(volume_row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    create_label(volume_row, "VOL", &lv_font_montserrat_14, 0x151820);

    volume_slider_ = lv_slider_create(volume_row);
    lv_obj_set_width(volume_slider_, 1);
    lv_obj_set_height(volume_slider_, 18);
    lv_obj_set_flex_grow(volume_slider_, 1);
    lv_obj_set_ext_click_area(volume_slider_, 12);
    lv_slider_set_range(volume_slider_, 0, 100);
    lv_slider_set_value(volume_slider_, volume_percent_, LV_ANIM_OFF);
    style_winamp_slider(volume_slider_);
    lv_obj_add_event_cb(volume_slider_, control_event_cb, LV_EVENT_VALUE_CHANGED, &volume_event_);

    volume_label_ = create_label(volume_row, "", &lv_font_montserrat_14, 0x151820);
    lv_obj_set_width(volume_label_, 48);
    lv_obj_set_style_text_align(volume_label_, LV_TEXT_ALIGN_RIGHT, 0);

    lv_obj_t *playlist = lv_obj_create(root_);
    style_panel(playlist);
    lv_obj_set_width(playlist, lv_pct(100));
    lv_obj_set_flex_grow(playlist, 1);
    lv_obj_set_style_pad_row(playlist, 6, 0);
    lv_obj_set_flex_flow(playlist, LV_FLEX_FLOW_COLUMN);

    create_title_bar(playlist, "PLAYLIST EDITOR", "SD:/music");

    lv_obj_t *playlist_tools = lv_obj_create(playlist);
    make_plain_container(playlist_tools);
    lv_obj_set_width(playlist_tools, lv_pct(100));
    lv_obj_set_height(playlist_tools, 38);
    lv_obj_set_style_pad_column(playlist_tools, 8, 0);
    lv_obj_set_flex_flow(playlist_tools, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(playlist_tools, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *refresh_button = create_icon_button(playlist_tools, LV_SYMBOL_REFRESH, 48, 0xA7ACB3);
    lv_obj_add_event_cb(refresh_button, control_event_cb, LV_EVENT_CLICKED, &refresh_event_);

    status_label_ = create_label(playlist_tools, "", &lv_font_montserrat_14, 0xF0A020);
    lv_obj_set_width(status_label_, 1);
    lv_obj_set_flex_grow(status_label_, 1);

    list_ = lv_list_create(playlist);
    lv_obj_set_width(list_, lv_pct(100));
    lv_obj_set_flex_grow(list_, 1);
    lv_obj_set_style_bg_color(list_, lv_color_hex(0x030406), 0);
    lv_obj_set_style_bg_opa(list_, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(list_, 2, 0);
    lv_obj_set_style_border_color(list_, lv_color_hex(0x111318), 0);
    lv_obj_set_style_radius(list_, 2, 0);
    lv_obj_set_style_pad_all(list_, 5, 0);
    lv_obj_set_style_pad_row(list_, 3, 0);
    lv_obj_set_style_text_color(list_, lv_color_hex(0x69FF7A), 0);
    lv_obj_set_scrollbar_mode(list_, LV_SCROLLBAR_MODE_AUTO);

    rebuild_list();
    refresh_ui();
    ui_timer_ = lv_timer_create(ui_timer_cb, kUiRefreshMs, this);
    return true;
}

void MusicPlayer::close(void)
{
    if (ui_timer_ != nullptr) {
        lv_timer_delete(ui_timer_);
        ui_timer_ = nullptr;
    }

    teardown_audio_player();

    root_ = nullptr;
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
    seek_dragging_ = false;
    for (size_t i = 0; i < sizeof(eq_bars_) / sizeof(eq_bars_[0]); i++) {
        eq_bars_[i] = nullptr;
    }
}

bool MusicPlayer::scan_tracks(void)
{
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
    while (((dir_entry = readdir(dir)) != nullptr) && (entry_count_ < kMaxEntries)) {
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

    if ((current_index_ >= static_cast<int>(entry_count_)) || (entry_count_ == 0)) {
        current_index_ = -1;
    }

    return true;
}

void MusicPlayer::rebuild_list(void)
{
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

        char text[kNameMax + 32] = {};
        snprintf(text, sizeof(text), "%s  %s", entries_[i].name, size_text);

        lv_obj_t *button = lv_list_add_button(list_, LV_SYMBOL_AUDIO, text);
        style_list_button(button);
        lv_obj_add_event_cb(button, entry_event_cb, LV_EVENT_CLICKED, &entry_events_[i]);
    }

    set_status("%lu MP3 files | PCM5102: BCLK33 LRCK31 DIN30",
               static_cast<unsigned long>(entry_count_));
}

bool MusicPlayer::play_index(uint32_t index)
{
    return play_index_from_offset(index, 0);
}

bool MusicPlayer::play_index_from_offset(uint32_t index, uint32_t offset)
{
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
        static_cast<Mp3StreamContext *>(heap_caps_calloc(1, sizeof(Mp3StreamContext),
                                                         MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
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
        set_status("Seek: %lu%%",
                   static_cast<unsigned long>((static_cast<uint64_t>(offset) * 100ULL) / file_size));
    } else {
        set_status("Queued: %s", entries_[index].name);
    }
    refresh_ui();
    return true;
}

void MusicPlayer::play_previous(void)
{
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

void MusicPlayer::play_next(void)
{
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

void MusicPlayer::toggle_play_pause(void)
{
    if (!player_ready_) {
        const uint32_t index = current_index_ >= 0 ? static_cast<uint32_t>(current_index_) : 0;
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
        const uint32_t index = current_index_ >= 0 ? static_cast<uint32_t>(current_index_) : 0;
        play_index(index);
        return;
    }

    refresh_ui();
}

void MusicPlayer::stop_playback(void)
{
    stop_requested_ = true;
    playback_started_ = false;
    playback_byte_offset_ = 0;

    if (player_ready_) {
        audio_player_stop();
    }

    set_status("Stopped");
    refresh_ui();
}

void MusicPlayer::seek_to_slider_value(int32_t value)
{
    if ((current_index_ < 0) || (static_cast<uint32_t>(current_index_) >= entry_count_)) {
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

    uint32_t offset = static_cast<uint32_t>((static_cast<uint64_t>(file_size) *
                                             static_cast<uint32_t>(value)) /
                                            static_cast<uint32_t>(kSeekSliderMax));
    if ((file_size > 4096U) && (offset > (file_size - 4096U))) {
        offset = file_size - 4096U;
    }
    if (offset > 8192U) {
        offset -= 8192U;
    } else {
        offset = 0;
    }

    play_index_from_offset(static_cast<uint32_t>(current_index_), offset);
}

void MusicPlayer::update_seek_label(uint32_t slider_value)
{
    if (seek_label_ == nullptr) {
        return;
    }

    if (slider_value > static_cast<uint32_t>(kSeekSliderMax)) {
        slider_value = static_cast<uint32_t>(kSeekSliderMax);
    }

    const uint32_t percent =
        (slider_value * 100U) / static_cast<uint32_t>(kSeekSliderMax);
    lv_label_set_text_fmt(seek_label_, "%lu%%", static_cast<unsigned long>(percent));
}

void MusicPlayer::handle_control(Control control, lv_event_t *event)
{
    const lv_event_code_t event_code = event != nullptr ? lv_event_get_code(event) : LV_EVENT_ALL;

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
            } else if ((event_code == LV_EVENT_RELEASED) || (event_code == LV_EVENT_PRESS_LOST)) {
                seek_dragging_ = false;
                seek_to_slider_value(value);
            }
        }
        break;
    }

    refresh_ui();
}

void MusicPlayer::handle_player_event(int event)
{
    const audio_player_callback_event_t audio_event = static_cast<audio_player_callback_event_t>(event);
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

void MusicPlayer::refresh_ui(void)
{
    const audio_player_state_t state = player_ready_ ? audio_player_get_state() : AUDIO_PLAYER_STATE_IDLE;
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
        lv_label_set_text_fmt(state_label_, "%s  %luHZ  %luBIT  VOL %u%%",
                              state_text,
                              static_cast<unsigned long>(current_sample_rate_),
                              static_cast<unsigned long>(current_bits_per_sample_),
                              static_cast<unsigned>(volume_percent_));
    }

    if (volume_slider_ != nullptr && !lv_slider_is_dragged(volume_slider_)) {
        lv_slider_set_value(volume_slider_, volume_percent_, LV_ANIM_OFF);
    }
    if (volume_label_ != nullptr) {
        lv_label_set_text_fmt(volume_label_, "%u%%", static_cast<unsigned>(volume_percent_));
    }

    if (seek_slider_ != nullptr) {
        uint32_t slider_value = 0;
        if ((current_index_ >= 0) && (static_cast<uint32_t>(current_index_) < entry_count_)) {
            const uint32_t file_size = entries_[current_index_].size;
            uint32_t byte_offset = playback_byte_offset_;
            if (byte_offset > file_size) {
                byte_offset = file_size;
            }
            if (file_size > 0) {
                slider_value = static_cast<uint32_t>((static_cast<uint64_t>(byte_offset) *
                                                      static_cast<uint32_t>(kSeekSliderMax)) /
                                                     file_size);
            }
        }
        if (slider_value > static_cast<uint32_t>(kSeekSliderMax)) {
            slider_value = static_cast<uint32_t>(kSeekSliderMax);
        }
        if (!seek_dragging_ && !lv_slider_is_dragged(seek_slider_)) {
            lv_slider_set_value(seek_slider_, static_cast<int32_t>(slider_value), LV_ANIM_OFF);
            update_seek_label(slider_value);
        }
    } else {
        update_seek_label(0);
    }

    if ((current_index_ >= 0) && (static_cast<uint32_t>(current_index_) < entry_count_)) {
        if (track_label_ != nullptr) {
            lv_label_set_text(track_label_, entries_[current_index_].name);
        }
        if (file_label_ != nullptr) {
            char size_text[24] = {};
            format_size(size_text, sizeof(size_text), entries_[current_index_].size);
            lv_label_set_text_fmt(file_label_, "%lu/%lu  %s",
                                  static_cast<unsigned long>(current_index_ + 1),
                                  static_cast<unsigned long>(entry_count_),
                                  size_text);
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

    const uint32_t tick = lv_tick_get();
    for (size_t i = 0; i < sizeof(eq_bars_) / sizeof(eq_bars_[0]); i++) {
        if (eq_bars_[i] == nullptr) {
            continue;
        }
        uint32_t value = 8;
        if (is_playing) {
            const uint32_t phase = ((tick / 80U) + (i * 19U)) % 100U;
            value = phase < 50U ? (phase * 2U) : ((100U - phase) * 2U);
            value = 14U + ((value * ((i % 3U) + 2U)) / 4U);
            if (value > 100U) {
                value = 100U;
            }
        } else if (is_paused) {
            value = 20U + static_cast<uint32_t>(i % 4U);
        }
        lv_bar_set_value(eq_bars_[i], static_cast<int32_t>(value), LV_ANIM_OFF);
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
            for (uint32_t child_index = 0; child_index < nested_count; child_index++) {
                lv_obj_set_style_text_color(lv_obj_get_child(child, child_index), lv_color_hex(text_color), 0);
            }
        }
    }
}

void MusicPlayer::set_status(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsnprintf(runtime_status_, sizeof(runtime_status_), fmt, args);
    va_end(args);

    if (status_label_ != nullptr) {
        lv_label_set_text(status_label_, runtime_status_);
    }
}

bool MusicPlayer::ensure_audio_player(void)
{
    if (player_ready_) {
        return true;
    }

    esp_err_t err = init_i2s(kDefaultSampleRateHz, kDefaultBitsPerSample, I2S_SLOT_MODE_STEREO);
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

void MusicPlayer::teardown_audio_player(void)
{
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

esp_err_t MusicPlayer::init_i2s(uint32_t sample_rate_hz, uint32_t bits_per_sample, i2s_slot_mode_t channel_mode)
{
    deinit_i2s();

    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(kExternalI2sPort, I2S_ROLE_MASTER);
    chan_cfg.dma_desc_num = kI2sDmaDescNum;
    chan_cfg.dma_frame_num = kI2sDmaFrames;
    chan_cfg.auto_clear = true;

    ESP_RETURN_ON_ERROR(i2s_new_channel(&chan_cfg, &tx_chan_, nullptr), TAG, "i2s_new_channel failed");

    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(sample_rate_hz),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(i2s_bit_width(bits_per_sample), channel_mode),
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

void MusicPlayer::deinit_i2s(void)
{
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

esp_err_t MusicPlayer::reconfigure_i2s(uint32_t sample_rate_hz,
                                       uint32_t bits_per_sample,
                                       i2s_slot_mode_t channel_mode)
{
    if (tx_chan_ == nullptr) {
        return init_i2s(sample_rate_hz, bits_per_sample, channel_mode);
    }

    if (i2s_enabled_) {
        ESP_RETURN_ON_ERROR(i2s_channel_disable(tx_chan_), TAG, "i2s_channel_disable failed");
        i2s_enabled_ = false;
    }

    i2s_std_clk_config_t clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(sample_rate_hz);
    clk_cfg.clk_src = I2S_CLK_SRC_APLL;
    clk_cfg.mclk_multiple = mclk_multiple_for_bits(bits_per_sample);
    ESP_RETURN_ON_ERROR(i2s_channel_reconfig_std_clock(tx_chan_, &clk_cfg),
                        TAG,
                        "i2s_channel_reconfig_std_clock failed");

    i2s_std_slot_config_t slot_cfg =
        I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(i2s_bit_width(bits_per_sample), channel_mode);
    ESP_RETURN_ON_ERROR(i2s_channel_reconfig_std_slot(tx_chan_, &slot_cfg),
                        TAG,
                        "i2s_channel_reconfig_std_slot failed");

    ESP_RETURN_ON_ERROR(i2s_channel_enable(tx_chan_), TAG, "i2s_channel_enable failed");
    i2s_enabled_ = true;
    current_sample_rate_ = sample_rate_hz;
    current_bits_per_sample_ = bits_per_sample;
    current_channel_mode_ = channel_mode;
    return ESP_OK;
}

bool MusicPlayer::ensure_volume_buffer(size_t len)
{
    if (volume_buffer_bytes_ >= len) {
        return true;
    }

    void *buffer = heap_caps_realloc(volume_buffer_, len, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    if (buffer == nullptr) {
        return false;
    }

    volume_buffer_ = static_cast<int16_t *>(buffer);
    volume_buffer_bytes_ = len;
    return true;
}

esp_err_t MusicPlayer::write_i2s(void *audio_buffer, size_t len, size_t *bytes_written, uint32_t timeout_ms)
{
    if ((tx_chan_ == nullptr) || (audio_buffer == nullptr) || (len == 0)) {
        if (bytes_written != nullptr) {
            *bytes_written = 0;
        }
        return ESP_ERR_INVALID_STATE;
    }

    if (current_bits_per_sample_ != 16) {
        return i2s_channel_write(tx_chan_, audio_buffer, len, bytes_written, timeout_ms);
    }

    const bool should_scale = volume_percent_ < 100;
    if (!muted_ && !should_scale) {
        return i2s_channel_write(tx_chan_, audio_buffer, len, bytes_written, timeout_ms);
    }

    if (!ensure_volume_buffer(len)) {
        if (muted_ || (volume_percent_ == 0)) {
            const size_t channels = current_channel_mode_ == I2S_SLOT_MODE_MONO ? 1U : 2U;
            const size_t frame_bytes = sizeof(int16_t) * channels;
            const uint32_t frame_count = frame_bytes > 0 ? static_cast<uint32_t>(len / frame_bytes) : 0;
            const uint32_t delay_ms = current_sample_rate_ > 0 ?
                                      (frame_count * 1000U) / current_sample_rate_ : 1U;
            vTaskDelay(pdMS_TO_TICKS(delay_ms > 0 ? delay_ms : 1U));
            if (bytes_written != nullptr) {
                *bytes_written = len;
            }
            return ESP_OK;
        }
        return i2s_channel_write(tx_chan_, audio_buffer, len, bytes_written, timeout_ms);
    }

    if (muted_ || (volume_percent_ == 0)) {
        memset(volume_buffer_, 0, len);
    } else {
        const int16_t *input = static_cast<const int16_t *>(audio_buffer);
        const size_t sample_count = len / sizeof(int16_t);
        const int volume = volume_percent_;
        for (size_t i = 0; i < sample_count; i++) {
            volume_buffer_[i] = static_cast<int16_t>((static_cast<int32_t>(input[i]) * volume) / 100);
        }
    }

    return i2s_channel_write(tx_chan_, volume_buffer_, len, bytes_written, timeout_ms);
}

esp_err_t MusicPlayer::set_mute(bool muted)
{
    muted_ = muted;
    return ESP_OK;
}

void MusicPlayer::entry_event_cb(lv_event_t *event)
{
    EntryEventData *data = static_cast<EntryEventData *>(lv_event_get_user_data(event));
    if ((data != nullptr) && (data->app != nullptr)) {
        data->app->play_index(data->index);
    }
}

void MusicPlayer::control_event_cb(lv_event_t *event)
{
    ControlEventData *data = static_cast<ControlEventData *>(lv_event_get_user_data(event));
    if ((data != nullptr) && (data->app != nullptr)) {
        data->app->handle_control(data->control, event);
    }
}

void MusicPlayer::ui_timer_cb(lv_timer_t *timer)
{
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

    app->refresh_ui();
}

void MusicPlayer::audio_player_event_cb(audio_player_cb_ctx_t *ctx)
{
    MusicPlayer *app = ctx != nullptr ? static_cast<MusicPlayer *>(ctx->user_ctx) : nullptr;
    if ((app == nullptr) || (app->event_queue_ == nullptr)) {
        return;
    }

    const int event_id = static_cast<int>(ctx->audio_event);
    xQueueSend(event_queue_from_handle(app->event_queue_), &event_id, 0);
}

int MusicPlayer::mp3_stream_read_cb(void *user_ctx, uint8_t *buffer, size_t len, bool *is_eof)
{
    Mp3StreamContext *stream_ctx = static_cast<Mp3StreamContext *>(user_ctx);
    if ((stream_ctx == nullptr) || (stream_ctx->fp == nullptr) || (buffer == nullptr)) {
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

void MusicPlayer::mp3_stream_close_cb(void *user_ctx)
{
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

esp_err_t MusicPlayer::audio_mute_cb(AUDIO_PLAYER_MUTE_SETTING setting)
{
    return active_player_ != nullptr ? active_player_->set_mute(setting == AUDIO_PLAYER_MUTE)
                                    : ESP_ERR_INVALID_STATE;
}

esp_err_t MusicPlayer::audio_clock_cb(uint32_t rate, uint32_t bits_cfg, i2s_slot_mode_t channel_mode)
{
    return active_player_ != nullptr ? active_player_->reconfigure_i2s(rate, bits_cfg, channel_mode)
                                    : ESP_ERR_INVALID_STATE;
}

esp_err_t MusicPlayer::audio_write_cb(void *audio_buffer, size_t len, size_t *bytes_written, uint32_t timeout_ms)
{
    return active_player_ != nullptr ? active_player_->write_i2s(audio_buffer, len, bytes_written, timeout_ms)
                                    : ESP_ERR_INVALID_STATE;
}
