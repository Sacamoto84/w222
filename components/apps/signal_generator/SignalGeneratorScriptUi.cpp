#include "signal_generator/SignalGenerator.hpp"

#include <string.h>

namespace {

static constexpr uint32_t kScriptTickMs = 10;

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

} // namespace
void SignalGenerator::create_script_view(lv_obj_t *parent)
{
    script_root_ = lv_obj_create(parent);
    lv_obj_set_width(script_root_, lv_pct(100));
    lv_obj_set_flex_grow(script_root_, 1);
    lv_obj_set_style_radius(script_root_, 8, 0);
    lv_obj_set_style_bg_color(script_root_, lv_color_hex(0x171E28), 0);
    lv_obj_set_style_bg_opa(script_root_, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(script_root_, 1, 0);
    lv_obj_set_style_border_color(script_root_, lv_color_hex(0x2B3645), 0);
    lv_obj_set_style_pad_all(script_root_, 8, 0);
    lv_obj_set_style_pad_row(script_root_, 6, 0);
    lv_obj_set_flex_flow(script_root_, LV_FLEX_FLOW_COLUMN);
    lv_obj_clear_flag(script_root_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(script_root_, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t *header = lv_obj_create(script_root_);
    make_plain_container(header);
    lv_obj_set_width(header, lv_pct(100));
    lv_obj_set_height(header, 24);
    lv_obj_set_style_pad_column(header, 8, 0);
    lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    create_text_label(header, "Script", &lv_font_montserrat_16, 0xFFB36B);
    script_name_label_ = create_text_label(header, "", &lv_font_montserrat_14, 0xEEF4FA);
    lv_obj_set_width(script_name_label_, 180);
    script_pc_label_ = create_text_label(header, "", &lv_font_montserrat_14, 0x9FB0C2);
    lv_obj_set_width(script_pc_label_, 92);
    script_status_label_ = create_text_label(header, "", &lv_font_montserrat_14, 0x9FB0C2);
    lv_obj_set_flex_grow(script_status_label_, 1);
    lv_obj_set_width(script_status_label_, 1);

    lv_obj_t *body = lv_obj_create(script_root_);
    make_plain_container(body);
    lv_obj_set_width(body, lv_pct(100));
    lv_obj_set_flex_grow(body, 1);
    lv_obj_set_style_pad_column(body, 8, 0);
    lv_obj_set_flex_flow(body, LV_FLEX_FLOW_ROW);

    script_list_ = lv_obj_create(body);
    make_plain_container(script_list_);
    lv_obj_set_width(script_list_, 1);
    lv_obj_set_flex_grow(script_list_, 1);
    lv_obj_set_height(script_list_, lv_pct(100));
    lv_obj_set_style_pad_row(script_list_, 1, 0);
    lv_obj_set_flex_flow(script_list_, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_flag(script_list_, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(script_list_, LV_OBJ_FLAG_SCROLLABLE);
    attach_event(script_list_, Control::ScriptListGesture, 0, LV_EVENT_GESTURE);

    for (size_t i = 0; i < kScriptVisibleLines; i++) {
        create_script_line(script_list_, i);
    }

    lv_obj_t *side = lv_obj_create(body);
    make_plain_container(side);
    lv_obj_set_width(side, 122);
    lv_obj_set_height(side, lv_pct(100));
    lv_obj_set_style_pad_row(side, 5, 0);
    lv_obj_set_flex_flow(side, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(side, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_add_flag(side, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(side, LV_SCROLLBAR_MODE_AUTO);

    create_script_side_button(side, "Run", Control::ScriptRunStop);
    script_run_label_ = lv_obj_get_child(lv_obj_get_child(side, 0), 0);
    create_script_side_button(side, "New", Control::ScriptNew);
    create_script_side_button(side, "Load", Control::ScriptLoad);
    create_script_side_button(side, "Save", Control::ScriptSave);
    create_script_side_button(side, "Save As", Control::ScriptSaveAs);
    create_script_side_button(side, "Add", Control::ScriptAdd);
    create_script_side_button(side, "Add END", Control::ScriptAddEnd);
    create_script_side_button(side, "Delete", Control::ScriptDelete);
    create_script_side_button(side, "Up", Control::ScriptUp);
    create_script_side_button(side, "Down", Control::ScriptDown);
    create_script_side_button(side, "P Save", Control::ScriptPresetSave);
    create_script_side_button(side, "P Load", Control::ScriptPresetLoad);

    script_keyboard_ = lv_obj_create(script_root_);
    make_plain_container(script_keyboard_);
    lv_obj_set_width(script_keyboard_, lv_pct(100));
    lv_obj_set_height(script_keyboard_, 160);
    lv_obj_set_style_pad_column(script_keyboard_, 6, 0);
    lv_obj_set_style_pad_row(script_keyboard_, 6, 0);
    lv_obj_set_flex_flow(script_keyboard_, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(script_keyboard_, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    for (uint8_t i = 0; i < kScriptKeyCount; i++) {
        create_script_keyboard_button(script_keyboard_, i);
    }

    if (script_timer_ != nullptr) {
        lv_timer_delete(script_timer_);
    }
    script_timer_ = lv_timer_create(script_timer_cb, kScriptTickMs, this);
}

void SignalGenerator::create_script_line(lv_obj_t *parent, size_t visible_index)
{
    if ((parent == nullptr) || (visible_index >= kScriptVisibleLines)) {
        return;
    }

    lv_obj_t *button = lv_button_create(parent);
    script_line_button_[visible_index] = button;
    lv_obj_set_width(button, lv_pct(100));
    lv_obj_set_height(button, 27);
    lv_obj_set_ext_click_area(button, 4);
    style_control_box(button);
    lv_obj_set_style_bg_color(button, lv_color_hex(0x202834), 0);
    lv_obj_set_style_bg_color(button, lv_color_hex(0x31445C), LV_STATE_PRESSED);
    attach_event(button, Control::ScriptSelectLine, static_cast<uint8_t>(visible_index), LV_EVENT_CLICKED);

    lv_obj_t *label = create_text_label(button, "", &lv_font_montserrat_14, 0xEEF4FA);
    script_line_label_[visible_index] = label;
    lv_obj_set_width(label, lv_pct(100));
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 8, 0);
    make_child_passthrough(label);
}

void SignalGenerator::create_script_side_button(lv_obj_t *parent, const char *text, Control control)
{
    lv_obj_t *button = create_button(parent, text, 116);
    lv_obj_set_height(button, 45);
    attach_event(button, control, 0, LV_EVENT_CLICKED);
}

void SignalGenerator::create_script_keyboard_button(lv_obj_t *parent, uint8_t key_index)
{
    if ((parent == nullptr) || (key_index >= kScriptKeyCount)) {
        return;
    }

    lv_obj_t *button = create_button(parent, "", 70);
    script_key_button_[key_index] = button;
    script_key_label_[key_index] = lv_obj_get_child(button, 0);
    lv_obj_set_width(button, lv_pct(23));
    lv_obj_set_height(button, 34);
    attach_event(button, Control::ScriptTemplate, key_index, LV_EVENT_CLICKED);
}

void SignalGenerator::refresh_script_ui(void)
{
    if (script_root_ == nullptr) {
        return;
    }

    if (script_line_count_ == 0) {
        script_load_default();
    }

    if (script_selected_line_ >= script_line_count_) {
        script_selected_line_ = script_line_count_ > 0 ? script_line_count_ - 1 : 0;
    }
    if (script_pc_ >= script_line_count_) {
        script_pc_ = script_line_count_ > 0 ? script_line_count_ - 1 : 0;
    }
    if (script_name_label_ != nullptr) {
        lv_label_set_text_fmt(script_name_label_, "%s.sk", script_name_);
    }
    if (script_pc_label_ != nullptr) {
        lv_label_set_text_fmt(script_pc_label_, "PC:%u",
                              static_cast<unsigned>(script_pc_));
    }
    if (script_status_label_ != nullptr) {
        lv_label_set_text_fmt(script_status_label_, "%s%s  F1=%.1f",
                              script_running_ ? "RUN" : "EDIT",
                              script_dirty_ ? " *" : "",
                              static_cast<double>(script_registers_[1]));
    }
    if (script_run_label_ != nullptr) {
        lv_label_set_text(script_run_label_, script_running_ ? "Stop" : "Run");
    }

    if (script_selected_line_ < script_scroll_offset_) {
        script_scroll_offset_ = script_selected_line_;
    }
    if (script_selected_line_ >= (script_scroll_offset_ + kScriptVisibleLines)) {
        script_scroll_offset_ = script_selected_line_ - kScriptVisibleLines + 1;
    }
    if ((script_line_count_ > kScriptVisibleLines) &&
        (script_scroll_offset_ > (script_line_count_ - kScriptVisibleLines))) {
        script_scroll_offset_ = script_line_count_ - kScriptVisibleLines;
    }

    for (size_t i = 0; i < kScriptVisibleLines; i++) {
        lv_obj_t *button = script_line_button_[i];
        lv_obj_t *label = script_line_label_[i];
        if ((button == nullptr) || (label == nullptr)) {
            continue;
        }

        const size_t line_index = script_scroll_offset_ + i;
        if (line_index >= script_line_count_) {
            lv_obj_add_flag(button, LV_OBJ_FLAG_HIDDEN);
            continue;
        }

        lv_obj_clear_flag(button, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text_fmt(label, "%03u  %s",
                              static_cast<unsigned>(line_index),
                              script_lines_[line_index].text);

        const bool selected = line_index == script_selected_line_;
        const bool pc = script_running_ && (line_index == script_pc_);
        lv_obj_set_style_bg_color(button,
                                  lv_color_hex(selected ? 0x305B82 : (pc ? 0x245138 : 0x202834)),
                                  0);
        lv_obj_set_style_border_color(button,
                                      lv_color_hex(selected ? 0x61D1FF : (pc ? 0x7CFF9B : 0x354151)),
                                      0);
        lv_obj_set_style_text_color(label,
                                    lv_color_hex(selected || pc ? 0xFFFFFF : 0xD5E0EC),
                                    0);
    }

    refresh_script_keyboard();
}

void SignalGenerator::refresh_script_keyboard(void)
{
    if (script_keyboard_ == nullptr) {
        return;
    }

    const char *labels[kScriptKeyCount] = {};
    const char *home_labels[kScriptKeyCount] = {
        "CH1",   "CR1",   "AM1",   "FM1",
        "CH2",   "CR2",   "AM2",   "FM2",
        "GOTO",  "IF",    "ELSE",  "PLUS",
        "DELAY", "ENDIF", "LOAD",  "MINUS",
    };
    const char *number_labels[kScriptKeyCount] = {
        "1", "2", "3", "DEL",
        "4", "5", "6", "<-",
        "7", "8", "9", "",
        ".", "0", "",  "DONE",
    };
    const char *register_labels[kScriptKeyCount] = {
        "F1", "F2", "F3", "<-",
        "F4", "F5", "F6", "",
        "F7", "F8", "F9", "",
        "",   "F0", "",   "",
    };
    const char *comparison_labels[kScriptKeyCount] = {
        "<",  ">",  "", "",
        "<=", ">=", "", "<-",
        "==", "!=", "", "",
        "",   "",   "", "",
    };
    const char *onoff_labels[kScriptKeyCount] = {
        "ON", "OFF", "", "<-",
        "",   "",    "", "",
        "",   "",    "", "",
        "",   "",    "", "",
    };
    const char *cramfm_labels[kScriptKeyCount] = {
        "CR", "AM", "FM", "<-",
        "",   "",   "",   "",
        "",   "",   "",   "",
        "",   "",   "",   "",
    };
    const char *cram_value_labels[kScriptKeyCount] = {
        "FR Fx",   "FR xx.x", "", "<-",
        "MOD",     "",        "", "",
        "",        "",        "", "",
        "",        "",        "", "",
    };
    const char *fm_value_labels[kScriptKeyCount] = {
        "FR Fx",   "FR xx",   "", "<-",
        "MOD",     "",        "", "",
        "BASE Fx", "BASE xx", "", "",
        "DEV Fx",  "DEV xx",  "", "",
    };
    const char *if_value_labels[kScriptKeyCount] = {
        "Fx",     "xxxx.x", "", "<-",
        "",       "",       "", "",
        "",       "",       "", "",
        "",       "",       "", "",
    };

    switch (script_keyboard_route_) {
    case ScriptKeyboardRoute::Home:
        memcpy(labels, home_labels, sizeof(labels));
        break;
    case ScriptKeyboardRoute::Number:
        memcpy(labels, number_labels, sizeof(labels));
        break;
    case ScriptKeyboardRoute::Register:
        memcpy(labels, register_labels, sizeof(labels));
        break;
    case ScriptKeyboardRoute::OnOff:
        memcpy(labels, onoff_labels, sizeof(labels));
        break;
    case ScriptKeyboardRoute::CramFm:
        memcpy(labels, cramfm_labels, sizeof(labels));
        break;
    case ScriptKeyboardRoute::CramValue:
        memcpy(labels, cram_value_labels, sizeof(labels));
        break;
    case ScriptKeyboardRoute::FmValue:
        memcpy(labels, fm_value_labels, sizeof(labels));
        break;
    case ScriptKeyboardRoute::Comparison:
        memcpy(labels, comparison_labels, sizeof(labels));
        break;
    case ScriptKeyboardRoute::IfValue:
        memcpy(labels, if_value_labels, sizeof(labels));
        break;
    case ScriptKeyboardRoute::ModCarrier:
    case ScriptKeyboardRoute::ModAm:
    case ScriptKeyboardRoute::ModFm: {
        labels[3] = "<-";
        const Waveform *items = script_keyboard_route_ == ScriptKeyboardRoute::ModCarrier ? carrier_waves_ : mod_waves_;
        const size_t count = script_keyboard_route_ == ScriptKeyboardRoute::ModCarrier ? carrier_wave_count_ : mod_wave_count_;
        size_t wave_index = 0;
        for (size_t i = 0; i < kScriptKeyCount && wave_index < count; i++) {
            if (i == 3) {
                continue;
            }
            labels[i] = items[wave_index].name;
            wave_index++;
        }
        break;
    }
    }

    for (size_t i = 0; i < kScriptKeyCount; i++) {
        lv_obj_t *button = script_key_button_[i];
        lv_obj_t *label = script_key_label_[i];
        if ((button == nullptr) || (label == nullptr)) {
            continue;
        }

        const bool enabled = (labels[i] != nullptr) && (labels[i][0] != '\0');
        lv_label_set_text(label, enabled ? labels[i] : "");
        if (enabled) {
            lv_obj_remove_state(button, LV_STATE_DISABLED);
            lv_obj_set_style_bg_color(button, lv_color_hex(0x273444), 0);
            lv_obj_set_style_border_color(button, lv_color_hex(0x354151), 0);
        } else {
            lv_obj_add_state(button, LV_STATE_DISABLED);
            lv_obj_set_style_bg_color(button, lv_color_hex(0x171E28), 0);
            lv_obj_set_style_border_color(button, lv_color_hex(0x171E28), 0);
        }
    }
}
