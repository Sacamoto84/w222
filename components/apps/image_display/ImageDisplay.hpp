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

    bool scan_images(void);
    void rebuild_list(void);
    bool open_image(uint32_t index);
    void close_image(void);
    bool open_next(int delta);
    void set_status(const char *fmt, ...);

    static void reload_event_cb(lv_event_t *event);
    static void entry_event_cb(lv_event_t *event);
    static void close_view_event_cb(lv_event_t *event);
    static void prev_event_cb(lv_event_t *event);
    static void next_event_cb(lv_event_t *event);
};
