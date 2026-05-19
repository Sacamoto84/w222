#include "../lv_examples.h"
#include "app_jpeg_image.h"

#include <dirent.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "esp_attr.h"
#include "esp_log.h"
#include "sdkconfig.h"

#if LV_BUILD_EXAMPLES

#define FM_ROOT_FS CONFIG_BSP_SD_MOUNT_POINT
#define FM_START_FS CONFIG_BSP_SD_MOUNT_POINT "/images"
#define FM_LVGL_DRIVE "S:"
#define FM_MAX_ENTRIES 128
#define FM_NAME_MAX 96
#define FM_PATH_MAX 192

typedef struct {
    char name[FM_NAME_MAX];
    char fs_path[FM_PATH_MAX];
    char lvgl_path[FM_PATH_MAX];
    uint32_t size;
    bool is_dir;
    bool is_parent;
    bool is_image;
    bool is_jpeg;
} fm_entry_t;

static const char *TAG = "file_manager";

static lv_obj_t *s_fm_root;
static lv_obj_t *s_path_label;
static lv_obj_t *s_status_label;
static lv_obj_t *s_file_list;
static lv_obj_t *s_view_root;
static app_jpeg_image_t *s_view_jpeg;
static char s_current_dir[FM_PATH_MAX];
static EXT_RAM_BSS_ATTR fm_entry_t s_entries[FM_MAX_ENTRIES];
static uint32_t s_entry_count;

static char ascii_lower(char c)
{
    if(c >= 'A' && c <= 'Z') return (char)(c + ('a' - 'A'));
    return c;
}

static int str_case_cmp(const char *a, const char *b)
{
    while(*a != '\0' || *b != '\0') {
        char ca = ascii_lower(*a);
        char cb = ascii_lower(*b);
        if(ca != cb) return (int)(unsigned char)ca - (int)(unsigned char)cb;
        if(ca == '\0') return 0;
        a++;
        b++;
    }

    return 0;
}

static bool str_ends_with_ignore_case(const char *text, const char *suffix)
{
    size_t text_len = strlen(text);
    size_t suffix_len = strlen(suffix);

    if(text_len < suffix_len) return false;

    text += text_len - suffix_len;
    for(size_t i = 0; i < suffix_len; i++) {
        if(ascii_lower(text[i]) != ascii_lower(suffix[i])) return false;
    }

    return true;
}

static bool is_jpeg_path(const char *path)
{
    return str_ends_with_ignore_case(path, ".jpg") || str_ends_with_ignore_case(path, ".jpeg");
}

static bool is_image_path(const char *path)
{
    return is_jpeg_path(path) ||
           str_ends_with_ignore_case(path, ".png") ||
           str_ends_with_ignore_case(path, ".bmp") ||
           str_ends_with_ignore_case(path, ".gif");
}

static void join_path(char *out, size_t out_size, const char *dir, const char *name)
{
    size_t len = strlen(dir);
    if(len > 0 && dir[len - 1] == '/') {
        snprintf(out, out_size, "%s%s", dir, name);
    }
    else {
        snprintf(out, out_size, "%s/%s", dir, name);
    }
}

static void copy_text(char *out, size_t out_size, const char *src)
{
    if(out_size == 0) return;

    size_t len = strnlen(src, out_size - 1);
    memmove(out, src, len);
    out[len] = '\0';
}

static void fs_path_to_lvgl_path(const char *fs_path, char *out, size_t out_size)
{
    size_t root_len = strlen(FM_ROOT_FS);

    if(strncmp(fs_path, FM_ROOT_FS, root_len) == 0) {
        const char *tail = fs_path + root_len;
        if(*tail == '/') tail++;
        if(*tail == '\0') snprintf(out, out_size, "%s/", FM_LVGL_DRIVE);
        else snprintf(out, out_size, "%s/%s", FM_LVGL_DRIVE, tail);
    }
    else {
        copy_text(out, out_size, fs_path);
    }
}

static bool is_root_dir(const char *path)
{
    return strcmp(path, FM_ROOT_FS) == 0 || strcmp(path, CONFIG_LV_FS_STDIO_PATH) == 0;
}

static void set_parent_dir(void)
{
    if(is_root_dir(s_current_dir)) return;

    char *last_slash = strrchr(s_current_dir, '/');
    if(last_slash == NULL || last_slash <= s_current_dir + strlen(FM_ROOT_FS)) {
        snprintf(s_current_dir, sizeof(s_current_dir), "%s", FM_ROOT_FS);
    }
    else {
        *last_slash = '\0';
    }
}

static int compare_entries(const void *a, const void *b)
{
    const fm_entry_t *ea = (const fm_entry_t *)a;
    const fm_entry_t *eb = (const fm_entry_t *)b;

    if(ea->is_parent != eb->is_parent) return ea->is_parent ? -1 : 1;
    if(ea->is_dir != eb->is_dir) return ea->is_dir ? -1 : 1;

    return str_case_cmp(ea->name, eb->name);
}

static void add_parent_entry(void)
{
    if(is_root_dir(s_current_dir) || s_entry_count >= FM_MAX_ENTRIES) return;

    fm_entry_t *entry = &s_entries[s_entry_count++];
    memset(entry, 0, sizeof(*entry));
    snprintf(entry->name, sizeof(entry->name), "..");
    entry->is_dir = true;
    entry->is_parent = true;
}

static void scan_current_dir(void)
{
    s_entry_count = 0;
    add_parent_entry();

    DIR *dir = opendir(s_current_dir);
    if(dir == NULL) {
        ESP_LOGW(TAG, "opendir failed: %s", s_current_dir);
        return;
    }

    struct dirent *dir_entry = NULL;
    while((dir_entry = readdir(dir)) != NULL && s_entry_count < FM_MAX_ENTRIES) {
        const char *name = dir_entry->d_name;
        if(name[0] == '.') continue;

        fm_entry_t *entry = &s_entries[s_entry_count];
        memset(entry, 0, sizeof(*entry));
        snprintf(entry->name, sizeof(entry->name), "%s", name);
        join_path(entry->fs_path, sizeof(entry->fs_path), s_current_dir, name);
        fs_path_to_lvgl_path(entry->fs_path, entry->lvgl_path, sizeof(entry->lvgl_path));

        struct stat st = {0};
        if(stat(entry->fs_path, &st) == 0) {
            entry->is_dir = S_ISDIR(st.st_mode);
            entry->size = (uint32_t)st.st_size;
        }
        else {
            entry->is_dir = false;
            entry->size = 0;
        }

        entry->is_image = !entry->is_dir && is_image_path(entry->name);
        entry->is_jpeg = entry->is_image && is_jpeg_path(entry->name);
        s_entry_count++;
    }

    closedir(dir);
    qsort(s_entries, s_entry_count, sizeof(s_entries[0]), compare_entries);
}

static uint32_t calc_fit_scale(uint32_t img_w, uint32_t img_h, uint32_t max_w, uint32_t max_h)
{
    if(img_w == 0 || img_h == 0) return 256;

    uint32_t scale_w = (max_w * 256U) / img_w;
    uint32_t scale_h = (max_h * 256U) / img_h;
    uint32_t scale = scale_w < scale_h ? scale_w : scale_h;

    if(scale == 0) scale = 1;
    if(scale > 256) scale = 256;

    return scale;
}

static void close_image_view(void)
{
    if(s_view_root != NULL) {
        lv_obj_delete(s_view_root);
        s_view_root = NULL;
    }

    if(s_view_jpeg != NULL) {
        app_jpeg_image_free(s_view_jpeg);
        s_view_jpeg = NULL;
    }

    if(s_fm_root != NULL) lv_obj_remove_flag(s_fm_root, LV_OBJ_FLAG_HIDDEN);
}

static void close_view_event_cb(lv_event_t *e)
{
    if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        close_image_view();
    }
}

static void open_image_view(const fm_entry_t *entry)
{
    close_image_view();
    if(s_fm_root != NULL) lv_obj_add_flag(s_fm_root, LV_OBJ_FLAG_HIDDEN);

    s_view_root = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(s_view_root);
    lv_obj_set_size(s_view_root, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(s_view_root, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(s_view_root, LV_OPA_COVER, 0);
    lv_obj_clear_flag(s_view_root, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *img = lv_image_create(s_view_root);
    bool image_ready = false;

    if(entry->is_jpeg) {
        app_jpeg_image_t *jpeg = NULL;
        if(app_jpeg_image_load_rgb565(entry->fs_path, &jpeg) == ESP_OK && jpeg != NULL) {
            const lv_image_dsc_t *dsc = app_jpeg_image_get_dsc(jpeg);
            s_view_jpeg = jpeg;
            lv_image_set_src(img, dsc);
            lv_image_set_scale(img, calc_fit_scale(dsc->header.w, dsc->header.h, 460, 720));
            image_ready = true;
        }
    }

    if(!image_ready) {
        lv_image_set_src(img, entry->lvgl_path);
        image_ready = true;
    }

    lv_obj_center(img);

    lv_obj_t *back = lv_button_create(s_view_root);
    lv_obj_set_size(back, 92, 42);
    lv_obj_align(back, LV_ALIGN_TOP_LEFT, 8, 8);
    lv_obj_add_event_cb(back, close_view_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *back_label = lv_label_create(back);
    lv_label_set_text(back_label, "Back");
    lv_obj_center(back_label);
}

static void file_entry_event_cb(lv_event_t *e);

static void reload_file_list(void)
{
    scan_current_dir();

    lv_label_set_text(s_path_label, s_current_dir);
    lv_obj_clean(s_file_list);

    for(uint32_t i = 0; i < s_entry_count; i++) {
        const fm_entry_t *entry = &s_entries[i];
        const void *icon = LV_SYMBOL_FILE;
        char text[FM_NAME_MAX + 24];

        if(entry->is_parent) {
            icon = LV_SYMBOL_UP;
            snprintf(text, sizeof(text), "Up");
        }
        else if(entry->is_dir) {
            icon = LV_SYMBOL_DIRECTORY;
            snprintf(text, sizeof(text), "%s/", entry->name);
        }
        else if(entry->is_image) {
            icon = LV_SYMBOL_IMAGE;
            snprintf(text, sizeof(text), "%s", entry->name);
        }
        else {
            snprintf(text, sizeof(text), "%s", entry->name);
        }

        lv_obj_t *btn = lv_list_add_button(s_file_list, icon, text);
        lv_obj_add_event_cb(btn, file_entry_event_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)i);
    }

    lv_label_set_text_fmt(s_status_label, "%u items", (unsigned)s_entry_count);
}

static void file_entry_event_cb(lv_event_t *e)
{
    if(lv_event_get_code(e) != LV_EVENT_CLICKED) return;

    uint32_t index = (uint32_t)(uintptr_t)lv_event_get_user_data(e);
    if(index >= s_entry_count) return;

    fm_entry_t *entry = &s_entries[index];
    if(entry->is_parent) {
        set_parent_dir();
        reload_file_list();
    }
    else if(entry->is_dir) {
        snprintf(s_current_dir, sizeof(s_current_dir), "%s", entry->fs_path);
        reload_file_list();
    }
    else if(entry->is_image) {
        lv_label_set_text_fmt(s_status_label, "Open: %s", entry->name);
        open_image_view(entry);
    }
    else {
        lv_label_set_text_fmt(s_status_label, "%s, %u bytes", entry->name, (unsigned)entry->size);
    }
}

static void reload_event_cb(lv_event_t *e)
{
    if(lv_event_get_code(e) == LV_EVENT_CLICKED) reload_file_list();
}

static void root_event_cb(lv_event_t *e)
{
    if(lv_event_get_code(e) != LV_EVENT_CLICKED) return;

    snprintf(s_current_dir, sizeof(s_current_dir), "%s", FM_ROOT_FS);
    reload_file_list();
}

void lv_example_event_draw(void)
{
    snprintf(s_current_dir, sizeof(s_current_dir), "%s", FM_START_FS);

    DIR *dir = opendir(s_current_dir);
    if(dir != NULL) {
        closedir(dir);
    }
    else {
        snprintf(s_current_dir, sizeof(s_current_dir), "%s", FM_ROOT_FS);
    }

    s_fm_root = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(s_fm_root);
    lv_obj_set_size(s_fm_root, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(s_fm_root, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(s_fm_root, 8, 0);
    lv_obj_set_style_pad_gap(s_fm_root, 8, 0);
    lv_obj_set_style_bg_color(s_fm_root, lv_color_hex(0x18202a), 0);
    lv_obj_set_style_bg_opa(s_fm_root, LV_OPA_COVER, 0);

    lv_obj_t *toolbar = lv_obj_create(s_fm_root);
    lv_obj_remove_style_all(toolbar);
    lv_obj_set_size(toolbar, LV_PCT(100), 46);
    lv_obj_set_flex_flow(toolbar, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_gap(toolbar, 8, 0);

    lv_obj_t *root_btn = lv_button_create(toolbar);
    lv_obj_set_size(root_btn, 74, 42);
    lv_obj_add_event_cb(root_btn, root_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *root_label = lv_label_create(root_btn);
    lv_label_set_text(root_label, "Root");
    lv_obj_center(root_label);

    lv_obj_t *reload_btn = lv_button_create(toolbar);
    lv_obj_set_size(reload_btn, 92, 42);
    lv_obj_add_event_cb(reload_btn, reload_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *reload_label = lv_label_create(reload_btn);
    lv_label_set_text(reload_label, "Reload");
    lv_obj_center(reload_label);

    s_path_label = lv_label_create(toolbar);
    lv_obj_set_flex_grow(s_path_label, 1);
    lv_label_set_long_mode(s_path_label, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
    lv_obj_set_style_text_color(s_path_label, lv_color_white(), 0);

    s_file_list = lv_list_create(s_fm_root);
    lv_obj_set_width(s_file_list, LV_PCT(100));
    lv_obj_set_flex_grow(s_file_list, 1);

    s_status_label = lv_label_create(s_fm_root);
    lv_obj_set_width(s_status_label, LV_PCT(100));
    lv_obj_set_style_text_color(s_status_label, lv_color_hex(0xb8c7d9), 0);

    reload_file_list();
}

#endif
