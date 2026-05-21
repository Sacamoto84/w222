#pragma once

#include <stddef.h>
#include <stdint.h>

#include "lite_app.h"
#include "lvgl.h"

typedef struct app_jpeg_image app_jpeg_image_t;

class ImageDisplay: public LiteApp {
public:
    ImageDisplay();
    ~ImageDisplay() override;

    const char *title(void) const override { return "Images"; }
    const char *icon_text(void) const override { return LV_SYMBOL_IMAGE; }
    lv_color_t accent_color(void) const override { return lv_color_hex(0x26A69A); }

    bool init(void) override;
    bool open(lv_obj_t *parent) override;
    bool back(void) override;
    void close(void) override;

private:
    static constexpr size_t kMaxEntries = 128;
    static constexpr size_t kNameMax = 96;
    static constexpr size_t kPathMax = 192;

    struct ImageEntry {
        char name[kNameMax];
        char path[kPathMax];
        uint32_t size;
    };

    struct EntryEventData {
        ImageDisplay *app;
        uint32_t index;
    };

    ImageEntry *entries_ = nullptr;
    EntryEventData *entry_events_ = nullptr;
    uint32_t entry_count_ = 0;

    lv_obj_t *root_ = nullptr;
    lv_obj_t *list_ = nullptr;
    lv_obj_t *status_label_ = nullptr;
    lv_obj_t *path_label_ = nullptr;
    lv_obj_t *view_root_ = nullptr;
    lv_obj_t *image_obj_ = nullptr;
    lv_obj_t *caption_label_ = nullptr;

    app_jpeg_image_t *current_image_ = nullptr;
    int current_index_ = -1;
    lv_coord_t view_width_ = 0;
    lv_coord_t view_height_ = 0;
    uint32_t fit_scale_ = 256;
    lv_point_t swipe_start_ = {};
    uint32_t swipe_start_tick_ = 0;
    bool swipe_tracking_ = false;

    void *preload_lock_ = nullptr;
    app_jpeg_image_t *preload_image_ = nullptr;
    int preload_image_index_ = -1;
    int preload_request_index_ = -1;
    char preload_request_path_[kPathMax] = {};
    uint32_t preload_generation_ = 0;
    bool preload_worker_running_ = false;

    bool scan_images(void);
    void rebuild_list(void);
    bool open_image(uint32_t index);
    bool show_loaded_image(uint32_t index, app_jpeg_image_t *loaded_image);
    void close_image(void);
    bool open_next(int delta);
    void reset_image_transform(uint32_t image_width, uint32_t image_height);
    void handle_view_touch(lv_event_t *event);
    void start_preload_next(void);
    void request_preload(uint32_t index);
    void cancel_preload(void);
    bool take_preloaded_image(uint32_t index, app_jpeg_image_t **out_image);
    bool wait_preloaded_image(uint32_t index, app_jpeg_image_t **out_image, uint32_t timeout_ms);
    bool is_preload_pending(uint32_t index);
    void wait_preload_idle(void);
    void preload_task_main(void);
    void set_status(const char *fmt, ...);

    static void reload_event_cb(lv_event_t *event);
    static void entry_event_cb(lv_event_t *event);
    static void view_touch_event_cb(lv_event_t *event);
    static void close_view_event_cb(lv_event_t *event);
    static void prev_event_cb(lv_event_t *event);
    static void next_event_cb(lv_event_t *event);
    static void preload_task_entry(void *arg);
};
