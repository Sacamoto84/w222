#include "music_player/MusicPlayer.hpp"

#include "misc/lv_text.h"

namespace {

// Local LVGL styling/building helpers for the Winamp-like music player screen.
// They stay file-private because no playback logic should depend on UI shapes.

static void make_plain_container(lv_obj_t *obj) {
  lv_obj_remove_style_all(obj);
  lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(obj, 0, 0);
  lv_obj_set_style_pad_all(obj, 0, 0);
}

// Создать метку с консистентным стилем
static lv_obj_t *create_label(lv_obj_t *parent, const char *text, const lv_font_t *font, uint32_t color) {
  lv_obj_t *label = lv_label_create(parent);
  lv_label_set_text(label, text);
  lv_label_set_long_mode(label, LV_LABEL_LONG_DOT);
  lv_obj_set_style_text_font(label, font, 0);
  lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
  return label;
}

static void make_child_passthrough(lv_obj_t *obj) {
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_flag(obj, LV_OBJ_FLAG_EVENT_BUBBLE);
}

static lv_obj_t *create_icon_button(lv_obj_t *parent, const char *symbol,
                                    lv_coord_t width, uint32_t bg_color) {
  lv_obj_t *button = lv_button_create(parent);
  lv_obj_set_size(button, width, 42);
  lv_obj_set_ext_click_area(button, 10);
  lv_obj_add_flag(button, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_clear_flag(button, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_style_radius(button, 0, 0);
  lv_obj_set_style_bg_color(button, lv_color_hex(0xF0F8FA), 0);
  lv_obj_set_style_bg_grad_color(button, lv_color_hex(bg_color), 0);
  lv_obj_set_style_bg_grad_dir(button, LV_GRAD_DIR_VER, 0);
  lv_obj_set_style_bg_color(button, lv_color_hex(0x9AA5AF), LV_STATE_PRESSED);
  lv_obj_set_style_bg_opa(button, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(button, 2, 0);
  lv_obj_set_style_border_color(button, lv_color_hex(0x05060A), 0);
  lv_obj_set_style_shadow_width(button, 0, 0);

  lv_obj_t *label =
      create_label(button, symbol, &lv_font_montserrat_16, 0x20242A);
  lv_obj_center(label);
  make_child_passthrough(label);
  return button;
}

static void style_panel(lv_obj_t *panel) {
  lv_obj_set_style_radius(panel, 1, 0);
  lv_obj_set_style_bg_color(panel, lv_color_hex(0x38384A), 0);
  lv_obj_set_style_bg_grad_color(panel, lv_color_hex(0x1B1B27), 0);
  lv_obj_set_style_bg_grad_dir(panel, LV_GRAD_DIR_VER, 0);
  lv_obj_set_style_bg_opa(panel, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(panel, 2, 0);
  lv_obj_set_style_border_color(panel, lv_color_hex(0x07080C), 0);
  lv_obj_set_style_pad_all(panel, 8, 0);
}

static lv_obj_t *create_title_bar(lv_obj_t *parent, const char *title, const char *right_text) {
  lv_obj_t *bar = lv_obj_create(parent);
  make_plain_container(bar);
  lv_obj_set_width(bar, lv_pct(100));
  lv_obj_set_height(bar, 24);
  lv_obj_set_style_bg_color(bar, lv_color_hex(0x252536), 0);
  lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(bar, 1, 0);
  lv_obj_set_style_border_color(bar, lv_color_hex(0x08090D), 0);
  lv_obj_set_style_radius(bar, 0, 0);
  lv_obj_set_style_pad_left(bar, 6, 0);
  lv_obj_set_style_pad_right(bar, 5, 0);
  lv_obj_set_style_pad_column(bar, 7, 0);
  lv_obj_set_flex_flow(bar, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(bar, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);

  create_label(bar, "~", &lv_font_montserrat_16, 0xF4E46D);

  lv_obj_t *left_line = lv_obj_create(bar);
  make_plain_container(left_line);
  lv_obj_set_height(left_line, 8);
  lv_obj_set_width(left_line, 1);
  lv_obj_set_flex_grow(left_line, 1);
  lv_obj_set_style_bg_color(left_line, lv_color_hex(0xE8E58C), 0);
  lv_obj_set_style_bg_grad_color(left_line, lv_color_hex(0x595A30), 0);
  lv_obj_set_style_bg_grad_dir(left_line, LV_GRAD_DIR_VER, 0);
  lv_obj_set_style_bg_opa(left_line, LV_OPA_COVER, 0);

  create_label(bar, title, &lv_font_montserrat_14, 0xEEF2FA);

  lv_obj_t *right_line = lv_obj_create(bar);
  make_plain_container(right_line);
  lv_obj_set_height(right_line, 8);
  lv_obj_set_width(right_line, 1);
  lv_obj_set_flex_grow(right_line, 1);
  lv_obj_set_style_bg_color(right_line, lv_color_hex(0xE8E58C), 0);
  lv_obj_set_style_bg_grad_color(right_line, lv_color_hex(0x595A30), 0);
  lv_obj_set_style_bg_grad_dir(right_line, LV_GRAD_DIR_VER, 0);
  lv_obj_set_style_bg_opa(right_line, LV_OPA_COVER, 0);

  if ((right_text != nullptr) && (right_text[0] != '\0')) {
    create_label(bar, right_text, &lv_font_montserrat_14, 0xC8CCD8);
  }

  return bar;
}

static void style_lcd(lv_obj_t *lcd) {
  lv_obj_set_style_radius(lcd, 0, 0);
  lv_obj_set_style_bg_color(lcd, lv_color_hex(0x2B2B3A), 0);
  lv_obj_set_style_bg_grad_color(lcd, lv_color_hex(0x1D1D29), 0);
  lv_obj_set_style_bg_grad_dir(lcd, LV_GRAD_DIR_VER, 0);
  lv_obj_set_style_bg_opa(lcd, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(lcd, 2, 0);
  lv_obj_set_style_border_color(lcd, lv_color_hex(0x090A0E), 0);
  lv_obj_set_style_pad_all(lcd, 7, 0);
}

static void style_winamp_slider(lv_obj_t *slider) {
  lv_obj_set_style_radius(slider, 1, LV_PART_MAIN);
  lv_obj_set_style_bg_color(slider, lv_color_hex(0x14171D), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(slider, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_bg_color(slider, lv_color_hex(0xF0A020), LV_PART_INDICATOR);
  lv_obj_set_style_bg_grad_color(slider, lv_color_hex(0xFFE06C),
                                 LV_PART_INDICATOR);
  lv_obj_set_style_bg_grad_dir(slider, LV_GRAD_DIR_HOR, LV_PART_INDICATOR);
  lv_obj_set_style_bg_opa(slider, LV_OPA_COVER, LV_PART_INDICATOR);
  lv_obj_set_style_bg_color(slider, lv_color_hex(0xDDE2E8), LV_PART_KNOB);
  lv_obj_set_style_bg_opa(slider, LV_OPA_COVER, LV_PART_KNOB);
  lv_obj_set_style_radius(slider, 2, LV_PART_KNOB);
  lv_obj_set_style_pad_all(slider, 7, LV_PART_KNOB);
}

} // namespace

lv_obj_t *MusicPlayer::create_root_(lv_obj_t *parent) {
  lv_obj_set_style_bg_color(parent, lv_color_hex(0x07080C), 0);
  lv_obj_set_style_bg_opa(parent, LV_OPA_COVER, 0);

  lv_obj_t *root = lv_obj_create(parent);
  make_plain_container(root);
  lv_obj_set_size(root, lv_pct(100), lv_pct(100));
  lv_obj_set_style_bg_color(root, lv_color_hex(0x11111A), 0);
  lv_obj_set_style_bg_grad_color(root, lv_color_hex(0x050507), 0);
  lv_obj_set_style_bg_grad_dir(root, LV_GRAD_DIR_VER, 0);
  lv_obj_set_style_bg_opa(root, LV_OPA_COVER, 0);
  lv_obj_set_style_pad_all(root, 10, 0);
  lv_obj_set_style_pad_row(root, 10, 0);
  lv_obj_set_flex_flow(root, LV_FLEX_FLOW_COLUMN);
  return root;
}

lv_obj_t *MusicPlayer::create_deck_(lv_obj_t *parent) {
  
  lv_obj_t *deck = lv_obj_create(parent);
  style_panel(deck);
  lv_obj_set_width(deck, lv_pct(100));
  lv_obj_set_height(deck, LV_SIZE_CONTENT);
  lv_obj_set_style_pad_row(deck, 6, 0);
  lv_obj_set_flex_flow(deck, LV_FLEX_FLOW_COLUMN);
  lv_obj_clear_flag(deck, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *main_title = create_title_bar(deck, "", "");

  lv_obj_t *close_button = create_icon_button(main_title, LV_SYMBOL_LEFT, 30, 0xDDE2E8);
  lv_obj_set_size(close_button, 30, 20);
  lv_obj_set_ext_click_area(close_button, 10);
  lv_obj_add_event_cb(close_button, control_event_cb, LV_EVENT_CLICKED, &close_event_);

  lv_obj_t *lcd = lv_obj_create(deck);
  style_lcd(lcd);
  lv_obj_set_width(lcd, lv_pct(100));
  lv_obj_set_height(lcd, 156);
  lv_obj_set_style_pad_column(lcd, 9, 0);
  lv_obj_set_flex_flow(lcd, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(lcd, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);
  lv_obj_clear_flag(lcd, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *scope_box = lv_obj_create(lcd);
  make_plain_container(scope_box);
  lv_obj_set_size(scope_box, static_cast<lv_coord_t>(kScopeCanvasWidth),
                  static_cast<lv_coord_t>(kScopeCanvasHeight));
  lv_obj_set_style_bg_color(scope_box, lv_color_hex(0x061006), 0);
  lv_obj_set_style_bg_opa(scope_box, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(scope_box, 1, 0);
  lv_obj_set_style_border_color(scope_box, lv_color_hex(0x18351F), 0);
  lv_obj_clear_flag(scope_box, LV_OBJ_FLAG_SCROLLABLE);

  if (ensure_scope_canvas_buffer()) {
    scope_canvas_ = lv_canvas_create(scope_box);
    lv_canvas_set_buffer(scope_canvas_, scope_canvas_buffer_,
                         static_cast<int32_t>(kScopeCanvasWidth),
                         static_cast<int32_t>(kScopeCanvasHeight),
                         LV_COLOR_FORMAT_RGB565);
    lv_obj_center(scope_canvas_);
    render_scope();
  } else {
    lv_obj_t *label =
        create_label(scope_box, "SCOPE", &lv_font_montserrat_14, 0xF0A020);
    lv_obj_center(label);
  }

  scope_label_ = create_label(scope_box, "1x", &lv_font_montserrat_14, 0xD0D0D0);
  lv_obj_set_style_bg_color(scope_label_, lv_color_hex(0x061006), 0);
  lv_obj_set_style_bg_opa(scope_label_, LV_OPA_COVER, 0);
  lv_obj_set_style_pad_all(scope_label_, 1, 0);
  lv_obj_align(scope_label_, LV_ALIGN_TOP_RIGHT, -2, 2);
  lv_obj_add_event_cb(scope_box, scope_event_cb, LV_EVENT_CLICKED, this);

  lv_obj_t *lcd_text = lv_obj_create(lcd);
  make_plain_container(lcd_text);
  lv_obj_set_width(lcd_text, 1);
  lv_obj_set_height(lcd_text, lv_pct(100));
  lv_obj_set_flex_grow(lcd_text, 1);
  lv_obj_set_style_bg_color(lcd_text, lv_color_hex(0x000000), 0);
  lv_obj_set_style_bg_opa(lcd_text, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(lcd_text, 2, 0);
  lv_obj_set_style_border_color(lcd_text, lv_color_hex(0x545769), 0);
  lv_obj_set_style_pad_all(lcd_text, 7, 0);
  lv_obj_set_style_pad_row(lcd_text, 4, 0);
  lv_obj_set_flex_flow(lcd_text, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(lcd_text, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);

  track_label_ = create_label(lcd_text, "1. DEMO", &lv_font_montserrat_22, 0x37FF60);
  lv_obj_set_width(track_label_, lv_pct(100));
  file_label_ = create_label(lcd_text, "", &lv_font_montserrat_14, 0xF0A020);
  lv_obj_set_width(file_label_, lv_pct(100));
  state_label_ = create_label(lcd_text, "IDLE", &lv_font_montserrat_14, 0xC8CCD8);
  lv_obj_set_width(state_label_, lv_pct(100));

  lv_obj_t *seek_row = lv_obj_create(deck);
  make_plain_container(seek_row);
  lv_obj_set_width(seek_row, lv_pct(100));
  lv_obj_set_height(seek_row, 28);
  lv_obj_set_style_pad_column(seek_row, 8, 0);
  lv_obj_set_flex_flow(seek_row, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(seek_row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);

  seek_label_ = create_label(seek_row, "0:00/--:--", &lv_font_montserrat_14, 0x37FF60);
  lv_obj_set_width(seek_label_, 90);
  lv_obj_set_style_text_align(seek_label_, LV_TEXT_ALIGN_LEFT, 0);

  seek_slider_ = lv_slider_create(seek_row);
  lv_obj_set_width(seek_slider_, 1);
  lv_obj_set_height(seek_slider_, 16);
  lv_obj_set_flex_grow(seek_slider_, 1);
  lv_obj_set_ext_click_area(seek_slider_, 12);
  lv_slider_set_range(seek_slider_, 0, kSeekSliderMax);
  lv_slider_set_value(seek_slider_, 0, LV_ANIM_OFF);
  style_winamp_slider(seek_slider_);
  lv_obj_add_event_cb(seek_slider_, control_event_cb, LV_EVENT_VALUE_CHANGED,
                      &seek_event_);
  lv_obj_add_event_cb(seek_slider_, control_event_cb, LV_EVENT_RELEASED,
                      &seek_event_);
  lv_obj_add_event_cb(seek_slider_, control_event_cb, LV_EVENT_PRESS_LOST,
                      &seek_event_);

  lv_obj_t *controls = lv_obj_create(deck);
  make_plain_container(controls);
  lv_obj_set_width(controls, lv_pct(100));
  lv_obj_set_height(controls, 46);
  lv_obj_set_style_pad_column(controls, 8, 0);
  lv_obj_set_flex_flow(controls, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(controls, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);

  lv_obj_t *prev_button =
      create_icon_button(controls, LV_SYMBOL_PREV, 58, 0xA7ACB3);
  lv_obj_add_event_cb(prev_button, control_event_cb, LV_EVENT_CLICKED,
                      &previous_event_);

  lv_obj_t *play_button =
      create_icon_button(controls, LV_SYMBOL_PLAY, 72 * 2, 0xF0A020);
  play_label_ = lv_obj_get_child(play_button, 0);
  lv_obj_add_event_cb(play_button, control_event_cb, LV_EVENT_CLICKED,
                      &play_event_);

  lv_obj_t *stop_button =
      create_icon_button(controls, LV_SYMBOL_STOP, 58 * 2, 0xA7ACB3);
  lv_obj_add_event_cb(stop_button, control_event_cb, LV_EVENT_CLICKED,
                      &stop_event_);

  lv_obj_t *next_button =
      create_icon_button(controls, LV_SYMBOL_NEXT, 58, 0xA7ACB3);
  lv_obj_add_event_cb(next_button, control_event_cb, LV_EVENT_CLICKED,
                      &next_event_);

  lv_obj_t *volume_row = lv_obj_create(deck);
  make_plain_container(volume_row);
  lv_obj_set_width(volume_row, lv_pct(100));
  lv_obj_set_height(volume_row, 34);
  lv_obj_set_style_pad_column(volume_row, 8, 0);
  lv_obj_set_flex_flow(volume_row, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(volume_row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);

  create_label(volume_row, "VOLUME", &lv_font_montserrat_14, 0xD7DAE5);

  volume_slider_ = lv_slider_create(volume_row);
  lv_obj_set_width(volume_slider_, 1);
  lv_obj_set_height(volume_slider_, 18);
  lv_obj_set_flex_grow(volume_slider_, 1);
  lv_obj_set_ext_click_area(volume_slider_, 12);
  lv_slider_set_range(volume_slider_, 0, 100);
  lv_slider_set_value(volume_slider_, volume_percent_, LV_ANIM_OFF);
  style_winamp_slider(volume_slider_);
  lv_obj_add_event_cb(volume_slider_, control_event_cb, LV_EVENT_VALUE_CHANGED,
                      &volume_event_);

  volume_label_ = create_label(volume_row, "", &lv_font_montserrat_14, 0x37FF60);
  lv_obj_set_width(volume_label_, 48);
  lv_obj_set_style_text_align(volume_label_, LV_TEXT_ALIGN_RIGHT, 0);

  status_label_ = create_label(deck, runtime_status_, &lv_font_montserrat_14, 0xC8CCD8);
  lv_obj_set_width(status_label_, lv_pct(100));

  return deck;
}

lv_obj_t *MusicPlayer::create_playlist_(lv_obj_t *parent) {
  lv_obj_t *playlist = lv_obj_create(parent);
  style_panel(playlist);
  lv_obj_set_width(playlist, lv_pct(100));
  lv_obj_set_flex_grow(playlist, 1);
  lv_obj_set_style_pad_row(playlist, 6, 0);
  lv_obj_set_flex_flow(playlist, LV_FLEX_FLOW_COLUMN);

  create_title_bar(playlist, "PLAYLIST", "SD:/music");

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

  return playlist;
}
