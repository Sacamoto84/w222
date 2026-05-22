#include "image_display/ImageDisplay.hpp"

#include <dirent.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "app_jpeg_image.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "sdkconfig.h"

namespace {

static const char *TAG = "image_display";
static constexpr const char *kImageDir = CONFIG_BSP_SD_MOUNT_POINT "/images";
static constexpr lv_coord_t kImageCenterYOffset = -22;
static constexpr lv_coord_t kSwipeMinDistance = 70;
static constexpr lv_coord_t kSwipeMaxOffAxis = 90;
static constexpr uint32_t kSwipeMaxDurationMs = 900;
static constexpr uint32_t kPreloadTaskStackBytes = 8192;
static constexpr UBaseType_t kPreloadTaskPriority = tskIDLE_PRIORITY + 2;
static constexpr uint32_t kPreloadWaitMs = 8000;
static constexpr uint32_t kPreloadPollMs = 10;
#if CONFIG_FREERTOS_UNICORE
static constexpr BaseType_t kPreloadTaskCore = tskNO_AFFINITY;
#else
static constexpr BaseType_t kPreloadTaskCore = 0;
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

static bool is_jpeg_name(const char *name)
{
    return str_ends_with_ignore_case(name, ".jpg") ||
           str_ends_with_ignore_case(name, ".jpeg");
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

static lv_obj_t *create_icon_button(lv_obj_t *parent, const char *symbol, lv_coord_t width = 88)
{
    lv_obj_t *button = lv_button_create(parent);
    lv_obj_set_size(button, width, 42);
    lv_obj_add_flag(button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(button, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_ext_click_area(button, 10);
    lv_obj_set_style_radius(button, 8, 0);
    lv_obj_set_style_bg_color(button, lv_color_hex(0x253140), 0);
    lv_obj_set_style_bg_opa(button, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(button, 1, 0);
    lv_obj_set_style_border_color(button, lv_color_hex(0x3A4656), 0);

    lv_obj_t *label = create_label(button, symbol, &lv_font_montserrat_16, 0xFFFFFF);
    lv_obj_center(label);
    make_child_passthrough(label);
    return button;
}

static void style_list_button(lv_obj_t *button)
{
    lv_obj_set_height(button, 48);
    lv_obj_set_style_bg_color(button, lv_color_hex(0x202836), 0);
    lv_obj_set_style_bg_opa(button, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(button, 1, 0);
    lv_obj_set_style_border_color(button, lv_color_hex(0x2D3848), 0);
    lv_obj_set_style_radius(button, 6, 0);
    lv_obj_set_style_margin_bottom(button, 5, 0);
    lv_obj_set_style_text_color(button, lv_color_hex(0xEAF1FA), 0);

    const uint32_t child_count = lv_obj_get_child_count(button);
    for (uint32_t i = 0; i < child_count; i++) {
        lv_obj_t *child = lv_obj_get_child(button, i);
        lv_obj_set_style_text_color(child, lv_color_hex(0xEAF1FA), 0);
    }
}

static uint32_t calc_fit_scale(uint32_t img_w, uint32_t img_h, uint32_t max_w, uint32_t max_h)
{
    if ((img_w == 0) || (img_h == 0) || (max_w == 0) || (max_h == 0)) {
        return 256;
    }

    uint32_t scale_w = (max_w * 256U) / img_w;
    uint32_t scale_h = (max_h * 256U) / img_h;
    uint32_t scale = scale_w < scale_h ? scale_w : scale_h;

    if (scale == 0) {
        scale = 1;
    }
    if (scale > 256) {
        scale = 256;
    }

    return scale;
}

static SemaphoreHandle_t preload_sem(void *handle)
{
    return static_cast<SemaphoreHandle_t>(handle);
}

} // namespace

ImageDisplay::ImageDisplay()
{
    for (size_t i = 0; i < kPreloadSlots; i++) {
        preload_image_indices_[i] = -1;
        preload_desired_indices_[i] = -1;
        preload_request_indices_[i] = -1;
    }
}

ImageDisplay::~ImageDisplay()
{
    close();
    wait_preload_idle();

    if (entries_ != nullptr) {
        heap_caps_free(entries_);
        entries_ = nullptr;
    }

    if (entry_events_ != nullptr) {
        heap_caps_free(entry_events_);
        entry_events_ = nullptr;
    }

    if (preload_lock_ != nullptr) {
        vSemaphoreDelete(preload_sem(preload_lock_));
        preload_lock_ = nullptr;
    }
}

bool ImageDisplay::init(void)
{
    if (entries_ == nullptr) {
        entries_ = static_cast<ImageEntry *>(heap_caps_calloc(kMaxEntries, sizeof(ImageEntry),
                                                              MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
    }

    if (entry_events_ == nullptr) {
        entry_events_ = static_cast<EntryEventData *>(heap_caps_calloc(kMaxEntries, sizeof(EntryEventData),
                                                                       MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
    }

    if (preload_lock_ == nullptr) {
        preload_lock_ = xSemaphoreCreateMutex();
    }

    return (entries_ != nullptr) && (entry_events_ != nullptr) && (preload_lock_ != nullptr);
}

bool ImageDisplay::open(lv_obj_t *parent)
{
    if ((parent == nullptr) || !init()) {
        return false;
    }

    lv_obj_update_layout(parent);
    view_width_ = lv_obj_get_content_width(parent);
    view_height_ = lv_obj_get_content_height(parent);
    if (view_width_ <= 0) {
        view_width_ = lv_obj_get_width(parent);
    }
    if (view_height_ <= 0) {
        view_height_ = lv_obj_get_height(parent);
    }

    lv_obj_set_style_bg_color(parent, lv_color_hex(0x11151D), 0);
    lv_obj_set_style_bg_opa(parent, LV_OPA_COVER, 0);

    root_ = lv_obj_create(parent);
    make_plain_container(root_);
    lv_obj_set_size(root_, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(root_, lv_color_hex(0x11151D), 0);
    lv_obj_set_style_bg_opa(root_, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(root_, 12, 0);
    lv_obj_set_style_pad_row(root_, 10, 0);
    lv_obj_set_flex_flow(root_, LV_FLEX_FLOW_COLUMN);

    lv_obj_t *toolbar = lv_obj_create(root_);
    make_plain_container(toolbar);
    lv_obj_set_width(toolbar, lv_pct(100));
    lv_obj_set_height(toolbar, 44);
    lv_obj_set_style_pad_column(toolbar, 8, 0);
    lv_obj_set_flex_flow(toolbar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(toolbar, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *reload_button = create_icon_button(toolbar, LV_SYMBOL_REFRESH, 44);
    lv_obj_add_event_cb(reload_button, reload_event_cb, LV_EVENT_CLICKED, this);

    path_label_ = create_label(toolbar, kImageDir, &lv_font_montserrat_14, 0xC8D3E0);
    lv_obj_set_flex_grow(path_label_, 1);
    lv_obj_set_width(path_label_, 1);

    list_ = lv_list_create(root_);
    lv_obj_set_width(list_, lv_pct(100));
    lv_obj_set_flex_grow(list_, 1);
    lv_obj_set_style_bg_color(list_, lv_color_hex(0x171C25), 0);
    lv_obj_set_style_bg_opa(list_, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(list_, 1, 0);
    lv_obj_set_style_border_color(list_, lv_color_hex(0x293342), 0);
    lv_obj_set_style_radius(list_, 8, 0);
    lv_obj_set_style_pad_all(list_, 6, 0);
    lv_obj_set_style_pad_row(list_, 5, 0);
    lv_obj_set_style_text_color(list_, lv_color_hex(0xEAF1FA), 0);
    lv_obj_set_scrollbar_mode(list_, LV_SCROLLBAR_MODE_AUTO);

    status_label_ = create_label(root_, "", &lv_font_montserrat_14, 0x93A4B8);
    lv_obj_set_width(status_label_, lv_pct(100));

    rebuild_list();
    return true;
}

bool ImageDisplay::back(void)
{
    if (view_root_ != nullptr) {
        close_image();
        return true;
    }

    return false;
}

void ImageDisplay::close(void)
{
    close_image();

    root_ = nullptr;
    list_ = nullptr;
    status_label_ = nullptr;
    path_label_ = nullptr;
    caption_label_ = nullptr;
    view_width_ = 0;
    view_height_ = 0;
    fit_scale_ = 256;
    swipe_start_ = {};
    swipe_start_tick_ = 0;
    swipe_tracking_ = false;
}

bool ImageDisplay::scan_images(void)
{
    if (entries_ == nullptr) {
        return false;
    }

    entry_count_ = 0;

    DIR *dir = opendir(kImageDir);
    if (dir == nullptr) {
        ESP_LOGW(TAG, "opendir failed: %s", kImageDir);
        return false;
    }

    struct dirent *dir_entry = nullptr;
    while (((dir_entry = readdir(dir)) != nullptr) && (entry_count_ < kMaxEntries)) {
        const char *name = dir_entry->d_name;
        if ((name == nullptr) || (name[0] == '.') || !is_jpeg_name(name)) {
            continue;
        }

        ImageEntry *entry = &entries_[entry_count_];
        memset(entry, 0, sizeof(*entry));
        snprintf(entry->name, sizeof(entry->name), "%s", name);
        join_path(entry->path, sizeof(entry->path), kImageDir, name);

        struct stat st = {};
        if (stat(entry->path, &st) != 0 || S_ISDIR(st.st_mode)) {
            continue;
        }

        entry->size = static_cast<uint32_t>(st.st_size);
        entry_count_++;
    }

    closedir(dir);
    for (uint32_t i = 1; i < entry_count_; i++) {
        ImageEntry current = entries_[i];
        uint32_t j = i;
        while ((j > 0) && (str_case_cmp(entries_[j - 1].name, current.name) > 0)) {
            entries_[j] = entries_[j - 1];
            j--;
        }
        entries_[j] = current;
    }

    return true;
}

void ImageDisplay::rebuild_list(void)
{
    if (list_ == nullptr) {
        return;
    }

    cancel_preload();
    const bool scan_ok = scan_images();
    lv_obj_clean(list_);

    if (!scan_ok) {
        set_status("Folder not found: %s", kImageDir);
        return;
    }

    if (entry_count_ == 0) {
        set_status("No JPG images in %s", kImageDir);
        return;
    }

    for (uint32_t i = 0; i < entry_count_; i++) {
        entry_events_[i] = {
            .app = this,
            .index = i,
        };

        char text[kNameMax + 32] = {};
        snprintf(text, sizeof(text), "%s  %lu KB",
                 entries_[i].name,
                 static_cast<unsigned long>((entries_[i].size + 1023U) / 1024U));

        lv_obj_t *button = lv_list_add_button(list_, LV_SYMBOL_IMAGE, text);
        style_list_button(button);
        lv_obj_add_event_cb(button, entry_event_cb, LV_EVENT_CLICKED, &entry_events_[i]);
    }

    set_status("%lu JPG images", static_cast<unsigned long>(entry_count_));
}

bool ImageDisplay::open_image(uint32_t index)
{
    if ((entries_ == nullptr) || (index >= entry_count_)) {
        return false;
    }

    close_image();
    set_status("Loading: %s", entries_[index].name);

    app_jpeg_image_t *loaded_image = nullptr;
    const esp_err_t err = app_jpeg_image_load_rgb565(entries_[index].path, &loaded_image);
    if ((err != ESP_OK) || (loaded_image == nullptr)) {
        set_status("JPEG load failed: %s", esp_err_to_name(err));
        return false;
    }

    if (!show_loaded_image(index, loaded_image)) {
        app_jpeg_image_free(loaded_image);
        return false;
    }

    start_preload_next();
    return true;
}

bool ImageDisplay::show_loaded_image(uint32_t index, app_jpeg_image_t *loaded_image)
{
    if ((entries_ == nullptr) || (index >= entry_count_) || (loaded_image == nullptr) || (root_ == nullptr)) {
        return false;
    }

    lv_obj_t *parent = lv_obj_get_parent(root_);
    if (parent == nullptr) {
        return false;
    }

    if (root_ != nullptr) {
        lv_obj_add_flag(root_, LV_OBJ_FLAG_HIDDEN);
    }

    view_root_ = lv_obj_create(parent);
    make_plain_container(view_root_);
    lv_obj_set_size(view_root_, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(view_root_, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(view_root_, LV_OPA_COVER, 0);
    lv_obj_add_flag(view_root_, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(view_root_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(view_root_, view_touch_event_cb, LV_EVENT_PRESSED, this);
    lv_obj_add_event_cb(view_root_, view_touch_event_cb, LV_EVENT_RELEASED, this);
    lv_obj_add_event_cb(view_root_, view_touch_event_cb, LV_EVENT_PRESS_LOST, this);

    current_image_ = loaded_image;
    current_index_ = static_cast<int>(index);

    image_obj_ = lv_image_create(view_root_);
    const lv_image_dsc_t *dsc = app_jpeg_image_get_dsc(current_image_);
    lv_image_set_src(image_obj_, dsc);
    lv_obj_add_flag(image_obj_, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(image_obj_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(image_obj_, view_touch_event_cb, LV_EVENT_PRESSED, this);
    lv_obj_add_event_cb(image_obj_, view_touch_event_cb, LV_EVENT_RELEASED, this);
    lv_obj_add_event_cb(image_obj_, view_touch_event_cb, LV_EVENT_PRESS_LOST, this);
    reset_image_transform(dsc->header.w, dsc->header.h);

    lv_obj_t *bottom_bar = lv_obj_create(view_root_);
    make_plain_container(bottom_bar);
    lv_obj_set_width(bottom_bar, lv_pct(100));
    lv_obj_set_height(bottom_bar, 58);
    lv_obj_set_style_bg_color(bottom_bar, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(bottom_bar, LV_OPA_60, 0);
    lv_obj_set_style_pad_left(bottom_bar, 8, 0);
    lv_obj_set_style_pad_right(bottom_bar, 8, 0);
    lv_obj_set_style_pad_top(bottom_bar, 8, 0);
    lv_obj_set_style_pad_bottom(bottom_bar, 8, 0);
    lv_obj_set_style_pad_column(bottom_bar, 8, 0);
    lv_obj_set_flex_flow(bottom_bar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(bottom_bar, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_align(bottom_bar, LV_ALIGN_BOTTOM_MID, 0, 0);

    caption_label_ = create_label(bottom_bar, entries_[index].name, &lv_font_montserrat_14, 0xFFFFFF);
    lv_obj_set_flex_grow(caption_label_, 1);
    lv_obj_set_width(caption_label_, 1);

    lv_obj_t *close_button = create_icon_button(bottom_bar, LV_SYMBOL_CLOSE);
    lv_obj_add_event_cb(close_button, close_view_event_cb, LV_EVENT_CLICKED, this);

    lv_obj_t *prev_button = create_icon_button(bottom_bar, LV_SYMBOL_LEFT);
    lv_obj_add_event_cb(prev_button, prev_event_cb, LV_EVENT_CLICKED, this);

    lv_obj_t *next_button = create_icon_button(bottom_bar, LV_SYMBOL_RIGHT);
    lv_obj_add_event_cb(next_button, next_event_cb, LV_EVENT_CLICKED, this);

    set_status("Opened: %s", entries_[index].name);
    return true;
}

void ImageDisplay::close_image(void)
{
    close_image(true);
}

void ImageDisplay::close_image(bool cancel_preload_cache)
{
    if (cancel_preload_cache) {
        cancel_preload();
    }

    if (view_root_ != nullptr) {
        lv_obj_delete(view_root_);
        view_root_ = nullptr;
    }

    image_obj_ = nullptr;
    caption_label_ = nullptr;

    if (current_image_ != nullptr) {
        app_jpeg_image_free(current_image_);
        current_image_ = nullptr;
    }

    current_index_ = -1;
    fit_scale_ = 256;
    swipe_start_ = {};
    swipe_start_tick_ = 0;
    swipe_tracking_ = false;

    if (root_ != nullptr) {
        lv_obj_clear_flag(root_, LV_OBJ_FLAG_HIDDEN);
    }
}

bool ImageDisplay::open_next(int delta)
{
    if ((entry_count_ == 0) || (current_index_ < 0)) {
        return false;
    }

    int next_index = current_index_ + delta;
    if (next_index < 0) {
        next_index = static_cast<int>(entry_count_) - 1;
    } else if (next_index >= static_cast<int>(entry_count_)) {
        next_index = 0;
    }

    app_jpeg_image_t *preloaded_image = nullptr;
    if ((delta > 0) &&
        (take_preloaded_image(static_cast<uint32_t>(next_index), &preloaded_image) ||
         wait_preloaded_image(static_cast<uint32_t>(next_index), &preloaded_image, kPreloadWaitMs))) {
        close_image(false);

        if (!show_loaded_image(static_cast<uint32_t>(next_index), preloaded_image)) {
            app_jpeg_image_free(preloaded_image);
            return false;
        }

        start_preload_next();
        return true;
    }

    return open_image(static_cast<uint32_t>(next_index));
}

void ImageDisplay::reset_image_transform(uint32_t image_width, uint32_t image_height)
{
    const uint32_t max_w = view_width_ > 20 ? static_cast<uint32_t>(view_width_ - 20) : 1U;
    const uint32_t max_h = view_height_ > 96 ? static_cast<uint32_t>(view_height_ - 96) : 1U;
    fit_scale_ = calc_fit_scale(image_width, image_height, max_w, max_h);

    if (image_obj_ != nullptr) {
        lv_image_set_pivot(image_obj_, static_cast<int32_t>(image_width / 2U), static_cast<int32_t>(image_height / 2U));
        lv_image_set_scale(image_obj_, fit_scale_);
        lv_obj_align(image_obj_, LV_ALIGN_CENTER, 0, kImageCenterYOffset);
    }
}

void ImageDisplay::handle_view_touch(lv_event_t *event)
{
    if (event == nullptr) {
        return;
    }

    lv_event_stop_bubbling(event);

    lv_indev_t *indev = lv_indev_active();
    if (indev == nullptr) {
        return;
    }

    const lv_event_code_t code = lv_event_get_code(event);
    if (code == LV_EVENT_PRESSED) {
        lv_indev_get_point(indev, &swipe_start_);
        swipe_start_tick_ = lv_tick_get();
        swipe_tracking_ = true;
        return;
    }

    if (code == LV_EVENT_PRESS_LOST) {
        swipe_tracking_ = false;
        return;
    }

    if ((code != LV_EVENT_RELEASED) || !swipe_tracking_) {
        return;
    }

    lv_point_t swipe_end = {};
    lv_indev_get_point(indev, &swipe_end);
    swipe_tracking_ = false;

    const int dx = static_cast<int>(swipe_end.x) - static_cast<int>(swipe_start_.x);
    const int dy = static_cast<int>(swipe_end.y) - static_cast<int>(swipe_start_.y);
    const uint32_t elapsed_ms = lv_tick_elaps(swipe_start_tick_);

    if ((abs(dx) < kSwipeMinDistance) || (abs(dy) > kSwipeMaxOffAxis) || (elapsed_ms > kSwipeMaxDurationMs)) {
        return;
    }

    if (dx < 0) {
        open_next(1);
    } else {
        open_next(-1);
    }
}

void ImageDisplay::start_preload_next(void)
{
    if ((entry_count_ < 2) || (current_index_ < 0)) {
        return;
    }

    uint32_t indices[kPreloadSlots] = {};
    size_t count = entry_count_ - 1;
    if (count > kPreloadSlots) {
        count = kPreloadSlots;
    }

    for (size_t i = 0; i < count; i++) {
        int next_index = current_index_ + static_cast<int>(i) + 1;
        while (next_index >= static_cast<int>(entry_count_)) {
            next_index -= static_cast<int>(entry_count_);
        }
        indices[i] = static_cast<uint32_t>(next_index);
    }

    request_preload_window(indices, count);
}

void ImageDisplay::request_preload_window(const uint32_t *indices, size_t count)
{
    if ((entries_ == nullptr) || (indices == nullptr) || (preload_lock_ == nullptr)) {
        return;
    }

    if (count > kPreloadSlots) {
        count = kPreloadSlots;
    }

    app_jpeg_image_t *images_to_free[kPreloadSlots] = {};
    size_t free_count = 0;
    bool start_worker = false;

    if (xSemaphoreTake(preload_sem(preload_lock_), portMAX_DELAY) == pdTRUE) {
        preload_desired_count_ = 0;
        for (size_t i = 0; i < count; i++) {
            if (indices[i] >= entry_count_) {
                continue;
            }

            const int desired_index = static_cast<int>(indices[i]);
            bool duplicate = false;
            for (size_t j = 0; j < preload_desired_count_; j++) {
                if (preload_desired_indices_[j] == desired_index) {
                    duplicate = true;
                    break;
                }
            }

            if (!duplicate) {
                preload_desired_indices_[preload_desired_count_++] = desired_index;
            }
        }
        for (size_t i = preload_desired_count_; i < kPreloadSlots; i++) {
            preload_desired_indices_[i] = -1;
        }

        for (size_t slot = 0; slot < kPreloadSlots; slot++) {
            const int cached_index = preload_image_indices_[slot];
            bool still_desired = false;
            for (size_t i = 0; i < preload_desired_count_; i++) {
                if (preload_desired_indices_[i] == cached_index) {
                    still_desired = true;
                    break;
                }
            }

            if (!still_desired && (preload_images_[slot] != nullptr)) {
                images_to_free[free_count++] = preload_images_[slot];
                preload_images_[slot] = nullptr;
                preload_image_indices_[slot] = -1;
            }
        }

        preload_request_count_ = 0;
        for (size_t i = 0; i < preload_desired_count_; i++) {
            const int desired_index = preload_desired_indices_[i];
            bool already_cached = false;
            for (size_t slot = 0; slot < kPreloadSlots; slot++) {
                if ((preload_images_[slot] != nullptr) && (preload_image_indices_[slot] == desired_index)) {
                    already_cached = true;
                    break;
                }
            }

            if (already_cached || (preload_loading_index_ == desired_index)) {
                continue;
            }

            preload_request_indices_[preload_request_count_] = desired_index;
            snprintf(preload_request_paths_[preload_request_count_],
                     sizeof(preload_request_paths_[preload_request_count_]),
                     "%s",
                     entries_[desired_index].path);
            preload_request_count_++;
        }
        for (size_t i = preload_request_count_; i < kPreloadSlots; i++) {
            preload_request_indices_[i] = -1;
            preload_request_paths_[i][0] = '\0';
        }

        if (!preload_worker_running_ && (preload_request_count_ > 0)) {
            preload_worker_running_ = true;
            start_worker = true;
        }

        xSemaphoreGive(preload_sem(preload_lock_));
    }

    for (size_t i = 0; i < free_count; i++) {
        app_jpeg_image_free(images_to_free[i]);
    }

    if (start_worker) {
        const BaseType_t ok = xTaskCreatePinnedToCore(preload_task_entry,
                                                      "img_preload",
                                                      kPreloadTaskStackBytes,
                                                      this,
                                                      kPreloadTaskPriority,
                                                      nullptr,
                                                      kPreloadTaskCore);
        if (ok != pdPASS) {
            ESP_LOGW(TAG, "preload task create failed");
            if (xSemaphoreTake(preload_sem(preload_lock_), portMAX_DELAY) == pdTRUE) {
                preload_worker_running_ = false;
                preload_loading_index_ = -1;
                preload_request_count_ = 0;
                for (size_t i = 0; i < kPreloadSlots; i++) {
                    preload_request_indices_[i] = -1;
                    preload_request_paths_[i][0] = '\0';
                }
                xSemaphoreGive(preload_sem(preload_lock_));
            }
        }
    }
}

void ImageDisplay::cancel_preload(void)
{
    app_jpeg_image_t *images_to_free[kPreloadSlots] = {};
    size_t free_count = 0;

    if ((preload_lock_ != nullptr) && (xSemaphoreTake(preload_sem(preload_lock_), portMAX_DELAY) == pdTRUE)) {
        preload_desired_count_ = 0;
        preload_request_count_ = 0;
        preload_generation_++;

        for (size_t i = 0; i < kPreloadSlots; i++) {
            preload_desired_indices_[i] = -1;
            preload_request_indices_[i] = -1;
            preload_request_paths_[i][0] = '\0';

            if (preload_images_[i] != nullptr) {
                images_to_free[free_count++] = preload_images_[i];
                preload_images_[i] = nullptr;
            }
            preload_image_indices_[i] = -1;
        }

        if (!preload_worker_running_) {
            preload_loading_index_ = -1;
        }

        xSemaphoreGive(preload_sem(preload_lock_));
    }

    for (size_t i = 0; i < free_count; i++) {
        app_jpeg_image_free(images_to_free[i]);
    }
}

bool ImageDisplay::take_preloaded_image(uint32_t index, app_jpeg_image_t **out_image)
{
    if (out_image == nullptr) {
        return false;
    }

    *out_image = nullptr;
    if (preload_lock_ == nullptr) {
        return false;
    }

    bool taken = false;
    if (xSemaphoreTake(preload_sem(preload_lock_), portMAX_DELAY) == pdTRUE) {
        for (size_t slot = 0; slot < kPreloadSlots; slot++) {
            if ((preload_images_[slot] != nullptr) &&
                (preload_image_indices_[slot] == static_cast<int>(index))) {
                *out_image = preload_images_[slot];
                preload_images_[slot] = nullptr;
                preload_image_indices_[slot] = -1;
                taken = true;
                break;
            }
        }

        if (taken) {
            for (size_t i = 0; i < preload_request_count_;) {
                if (preload_request_indices_[i] != static_cast<int>(index)) {
                    i++;
                    continue;
                }

                for (size_t j = i + 1; j < preload_request_count_; j++) {
                    preload_request_indices_[j - 1] = preload_request_indices_[j];
                    snprintf(preload_request_paths_[j - 1],
                             sizeof(preload_request_paths_[j - 1]),
                             "%s",
                             preload_request_paths_[j]);
                }
                preload_request_count_--;
                preload_request_indices_[preload_request_count_] = -1;
                preload_request_paths_[preload_request_count_][0] = '\0';
            }
        }

        xSemaphoreGive(preload_sem(preload_lock_));
    }

    return taken;
}

bool ImageDisplay::wait_preloaded_image(uint32_t index, app_jpeg_image_t **out_image, uint32_t timeout_ms)
{
    const TickType_t start_tick = xTaskGetTickCount();
    const TickType_t timeout_ticks = pdMS_TO_TICKS(timeout_ms);
    const TickType_t poll_ticks = pdMS_TO_TICKS(kPreloadPollMs);

    while (true) {
        if (take_preloaded_image(index, out_image)) {
            return true;
        }

        if (!is_preload_pending(index)) {
            return false;
        }

        if ((xTaskGetTickCount() - start_tick) >= timeout_ticks) {
            return false;
        }

        vTaskDelay(poll_ticks > 0 ? poll_ticks : 1);
    }
}

bool ImageDisplay::is_preload_pending(uint32_t index)
{
    if (preload_lock_ == nullptr) {
        return false;
    }

    bool pending = false;
    if (xSemaphoreTake(preload_sem(preload_lock_), portMAX_DELAY) == pdTRUE) {
        const int wanted_index = static_cast<int>(index);
        pending = preload_worker_running_ && (preload_loading_index_ == wanted_index);
        for (size_t i = 0; !pending && (i < preload_request_count_); i++) {
            pending = preload_request_indices_[i] == wanted_index;
        }
        xSemaphoreGive(preload_sem(preload_lock_));
    }

    return pending;
}

void ImageDisplay::wait_preload_idle(void)
{
    if (preload_lock_ == nullptr) {
        return;
    }

    while (true) {
        bool running = false;
        if (xSemaphoreTake(preload_sem(preload_lock_), portMAX_DELAY) == pdTRUE) {
            running = preload_worker_running_;
            xSemaphoreGive(preload_sem(preload_lock_));
        }

        if (!running || (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)) {
            return;
        }

        vTaskDelay(pdMS_TO_TICKS(kPreloadPollMs));
    }
}

void ImageDisplay::preload_task_main(void)
{
    while (true) {
        int request_index = -1;
        uint32_t request_generation = 0;
        char request_path[kPathMax] = {};

        if (xSemaphoreTake(preload_sem(preload_lock_), portMAX_DELAY) == pdTRUE) {
            if (preload_request_count_ == 0) {
                preload_loading_index_ = -1;
                preload_worker_running_ = false;
                xSemaphoreGive(preload_sem(preload_lock_));
                return;
            }

            request_index = preload_request_indices_[0];
            request_generation = preload_generation_;
            snprintf(request_path, sizeof(request_path), "%s", preload_request_paths_[0]);

            for (size_t i = 1; i < preload_request_count_; i++) {
                preload_request_indices_[i - 1] = preload_request_indices_[i];
                snprintf(preload_request_paths_[i - 1],
                         sizeof(preload_request_paths_[i - 1]),
                         "%s",
                         preload_request_paths_[i]);
            }
            preload_request_count_--;
            preload_request_indices_[preload_request_count_] = -1;
            preload_request_paths_[preload_request_count_][0] = '\0';
            preload_loading_index_ = request_index;

            xSemaphoreGive(preload_sem(preload_lock_));
        }

        // The worker only decodes JPEG data; LVGL objects stay on the UI task.
        app_jpeg_image_t *loaded_image = nullptr;
        const esp_err_t err = app_jpeg_image_load_rgb565(request_path, &loaded_image);
        bool continue_worker = false;

        if (xSemaphoreTake(preload_sem(preload_lock_), portMAX_DELAY) == pdTRUE) {
            // A newer request can arrive while JPEG decode is running; stale results are freed below.
            bool still_desired = false;
            for (size_t i = 0; i < preload_desired_count_; i++) {
                if (preload_desired_indices_[i] == request_index) {
                    still_desired = true;
                    break;
                }
            }

            bool already_cached = false;
            for (size_t slot = 0; slot < kPreloadSlots; slot++) {
                already_cached = already_cached ||
                                 ((preload_images_[slot] != nullptr) &&
                                  (preload_image_indices_[slot] == request_index));
            }

            int free_slot = -1;
            for (size_t slot = 0; slot < kPreloadSlots; slot++) {
                if (preload_images_[slot] == nullptr) {
                    free_slot = static_cast<int>(slot);
                    break;
                }
            }

            const bool accept_image = (err == ESP_OK) &&
                                      (loaded_image != nullptr) &&
                                      (request_generation == preload_generation_) &&
                                      still_desired &&
                                      !already_cached &&
                                      (free_slot >= 0);

            if (accept_image) {
                preload_images_[free_slot] = loaded_image;
                preload_image_indices_[free_slot] = request_index;
                loaded_image = nullptr;
                ESP_LOGI(TAG, "Preloaded: %s", request_path);
            } else if (err != ESP_OK) {
                ESP_LOGW(TAG, "Preload failed: %s (%s)", request_path, esp_err_to_name(err));
            }

            if (preload_loading_index_ == request_index) {
                preload_loading_index_ = -1;
            }

            continue_worker = preload_request_count_ > 0;
            if (!continue_worker) {
                preload_worker_running_ = false;
            }

            xSemaphoreGive(preload_sem(preload_lock_));
        }

        if (loaded_image != nullptr) {
            app_jpeg_image_free(loaded_image);
        }

        if (!continue_worker) {
            return;
        }
    }
}

void ImageDisplay::set_status(const char *fmt, ...)
{
    if (status_label_ == nullptr) {
        return;
    }

    char text[160] = {};
    va_list args;
    va_start(args, fmt);
    vsnprintf(text, sizeof(text), fmt, args);
    va_end(args);

    lv_label_set_text(status_label_, text);
}

void ImageDisplay::reload_event_cb(lv_event_t *event)
{
    ImageDisplay *app = static_cast<ImageDisplay *>(lv_event_get_user_data(event));
    if (app != nullptr) {
        app->rebuild_list();
    }
}

void ImageDisplay::entry_event_cb(lv_event_t *event)
{
    EntryEventData *data = static_cast<EntryEventData *>(lv_event_get_user_data(event));
    if ((data != nullptr) && (data->app != nullptr)) {
        data->app->open_image(data->index);
    }
}

void ImageDisplay::view_touch_event_cb(lv_event_t *event)
{
    ImageDisplay *app = static_cast<ImageDisplay *>(lv_event_get_user_data(event));
    if (app != nullptr) {
        app->handle_view_touch(event);
    }
}

void ImageDisplay::close_view_event_cb(lv_event_t *event)
{
    ImageDisplay *app = static_cast<ImageDisplay *>(lv_event_get_user_data(event));
    if (app != nullptr) {
        app->close_image();
    }
}

void ImageDisplay::prev_event_cb(lv_event_t *event)
{
    ImageDisplay *app = static_cast<ImageDisplay *>(lv_event_get_user_data(event));
    if (app != nullptr) {
        app->open_next(-1);
    }
}

void ImageDisplay::next_event_cb(lv_event_t *event)
{
    ImageDisplay *app = static_cast<ImageDisplay *>(lv_event_get_user_data(event));
    if (app != nullptr) {
        app->open_next(1);
    }
}

void ImageDisplay::preload_task_entry(void *arg)
{
    ImageDisplay *app = static_cast<ImageDisplay *>(arg);
    if (app != nullptr) {
        app->preload_task_main();
    }

    vTaskDelete(nullptr);
}
