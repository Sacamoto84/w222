#include "uart_setting/UartSetting.hpp"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "net_config.hpp"

namespace {

void style_screen(lv_obj_t *obj)
{
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x15171D), 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(obj, lv_color_hex(0xFFFFFF), 0);
}

lv_obj_t *create_label(lv_obj_t *parent, const char *text, const lv_font_t *font, uint32_t color)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(label, lv_pct(100));
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
    return label;
}

lv_obj_t *create_card(lv_obj_t *parent, const char *title)
{
    lv_obj_t *card = lv_obj_create(parent);
    lv_obj_set_width(card, lv_pct(100));
    lv_obj_set_height(card, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_color(card, lv_color_hex(0x20242C), 0);
    lv_obj_set_style_border_width(card, 1, 0);
    lv_obj_set_style_border_color(card, lv_color_hex(0x343B47), 0);
    lv_obj_set_style_radius(card, 8, 0);
    lv_obj_set_style_pad_all(card, 14, 0);
    lv_obj_set_style_pad_row(card, 10, 0);
    lv_obj_set_flex_flow(card, LV_FLEX_FLOW_COLUMN);

    create_label(card, title, &lv_font_montserrat_20, 0xFFFFFF);
    return card;
}

lv_obj_t *create_button(lv_obj_t *parent, const char *text)
{
    lv_obj_t *button = lv_button_create(parent);
    lv_obj_set_height(button, 44);
    lv_obj_set_width(button, lv_pct(100));
    lv_obj_set_style_radius(button, 8, 0);
    lv_obj_set_style_bg_color(button, lv_color_hex(0x2D7DFF), 0);

    lv_obj_t *label = lv_label_create(button);
    lv_label_set_text(label, text);
    lv_obj_center(label);
    return button;
}

// Поле ввода IPv4-адреса сервера: только цифры и точка.
lv_obj_t *create_host_textarea(lv_obj_t *parent)
{
    lv_obj_t *textarea = lv_textarea_create(parent);
    lv_textarea_set_one_line(textarea, true);
    lv_textarea_set_accepted_chars(textarea, "0123456789.");
    lv_textarea_set_max_length(textarea, 63);
    lv_obj_set_width(textarea, lv_pct(100));
    lv_obj_set_style_radius(textarea, 8, 0);
    lv_obj_set_style_bg_color(textarea, lv_color_hex(0x161A22), 0);
    lv_obj_set_style_border_width(textarea, 1, 0);
    lv_obj_set_style_border_color(textarea, lv_color_hex(0x3A4351), 0);
    lv_obj_set_style_text_color(textarea, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_pad_all(textarea, 10, 0);
    return textarea;
}

// Числовое поле для номера порта (до 5 цифр).
lv_obj_t *create_port_textarea(lv_obj_t *parent)
{
    lv_obj_t *textarea = lv_textarea_create(parent);
    lv_textarea_set_one_line(textarea, true);
    lv_textarea_set_accepted_chars(textarea, "0123456789");
    lv_textarea_set_max_length(textarea, 5);
    lv_obj_set_width(textarea, lv_pct(100));
    lv_obj_set_style_radius(textarea, 8, 0);
    lv_obj_set_style_bg_color(textarea, lv_color_hex(0x161A22), 0);
    lv_obj_set_style_border_width(textarea, 1, 0);
    lv_obj_set_style_border_color(textarea, lv_color_hex(0x3A4351), 0);
    lv_obj_set_style_text_color(textarea, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_pad_all(textarea, 10, 0);
    return textarea;
}

lv_obj_t *create_switch(lv_obj_t *parent)
{
    lv_obj_t *sw = lv_switch_create(parent);
    lv_obj_set_style_bg_color(sw, lv_color_hex(0x2D7DFF), LV_PART_INDICATOR | LV_STATE_CHECKED);
    return sw;
}

}  // namespace

bool UartSettingApp::open(lv_obj_t *parent)
{
    if (parent == nullptr) {
        return false;
    }

    style_screen(parent);

    lv_obj_t *root = lv_obj_create(parent);
    lv_obj_remove_style_all(root);
    lv_obj_set_size(root, lv_pct(100), lv_pct(100));
    lv_obj_set_style_pad_all(root, 12, 0);
    lv_obj_set_style_pad_row(root, 12, 0);
    lv_obj_set_flex_flow(root, LV_FLEX_FLOW_COLUMN);

    lv_obj_t *content = lv_obj_create(root);
    content_ = content;
    lv_obj_set_width(content, lv_pct(100));
    lv_obj_set_flex_grow(content, 1);
    lv_obj_set_style_bg_opa(content, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(content, 0, 0);
    lv_obj_set_style_pad_all(content, 0, 0);
    lv_obj_set_style_pad_row(content, 12, 0);
    lv_obj_set_flex_flow(content, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scrollbar_mode(content, LV_SCROLLBAR_MODE_AUTO);

    lv_obj_t *net_card = create_card(content, "Server (TCP)");

    create_label(net_card, "TCP client (connect to server)", &lv_font_montserrat_14, 0x96A2B3);
    tcp_switch_ = create_switch(net_card);

    create_label(net_card, "Server host (IP)", &lv_font_montserrat_14, 0x96A2B3);
    host_textarea_ = create_host_textarea(net_card);

    create_label(net_card, "TCP port", &lv_font_montserrat_14, 0x96A2B3);
    tcp_port_textarea_ = create_port_textarea(net_card);

    create_label(net_card, "UDP listener", &lv_font_montserrat_14, 0x96A2B3);
    udp_switch_ = create_switch(net_card);

    create_label(net_card, "UDP port", &lv_font_montserrat_14, 0x96A2B3);
    udp_port_textarea_ = create_port_textarea(net_card);

    lv_obj_t *net_save_button = create_button(net_card, "Save and apply");
    lv_obj_add_event_cb(net_save_button, save_net_event_cb, LV_EVENT_CLICKED, this);

    net_status_label_ = create_label(net_card, "", &lv_font_montserrat_14, 0x8FD3FF);

    // Экранная клавиатура поверх контента (скрыта; всплывает по тапу на поле).
    // FLOATING + явная геометрия — чтобы оверлеить низ независимо от layout
    // родителя и не участвовать в flex-потоке.
    keyboard_ = lv_keyboard_create(parent);
    lv_obj_add_flag(keyboard_, LV_OBJ_FLAG_FLOATING);
    lv_obj_set_size(keyboard_, lv_pct(100), lv_pct(45));
    lv_obj_align(keyboard_, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_flag(keyboard_, LV_OBJ_FLAG_HIDDEN);
    lv_keyboard_set_textarea(keyboard_, nullptr);
    lv_obj_add_event_cb(keyboard_, keyboard_event_cb, LV_EVENT_READY, this);
    lv_obj_add_event_cb(keyboard_, keyboard_event_cb, LV_EVENT_CANCEL, this);

    // Поля ввода — только цифровая клавиатура (IP, порты).
    attach_keyboard(host_textarea_, LV_KEYBOARD_MODE_NUMBER);
    attach_keyboard(tcp_port_textarea_, LV_KEYBOARD_MODE_NUMBER);
    attach_keyboard(udp_port_textarea_, LV_KEYBOARD_MODE_NUMBER);

    load_into_ui();

    // Состояние TCP меняется со временем — обновляем его на экране по таймеру.
    net_status_timer_ = lv_timer_create(net_status_timer_cb, 700, this);
    refresh_net_status();
    return true;
}

void UartSettingApp::attach_keyboard(lv_obj_t *textarea, int mode)
{
    if (textarea == nullptr) {
        return;
    }
    lv_obj_set_user_data(textarea, reinterpret_cast<void *>(static_cast<intptr_t>(mode)));
    lv_obj_add_event_cb(textarea, textarea_event_cb, LV_EVENT_CLICKED, this);
}

void UartSettingApp::close(void)
{
    if (net_status_timer_ != nullptr) {
        lv_timer_delete(net_status_timer_);
        net_status_timer_ = nullptr;
    }
    content_ = nullptr;
    tcp_switch_ = nullptr;
    host_textarea_ = nullptr;
    tcp_port_textarea_ = nullptr;
    udp_switch_ = nullptr;
    udp_port_textarea_ = nullptr;
    net_status_label_ = nullptr;
    keyboard_ = nullptr;
}

void UartSettingApp::load_into_ui(void)
{
    const netcfg::Config net = netcfg::load();

    if (tcp_switch_ != nullptr) {
        if (net.tcp_enabled) {
            lv_obj_add_state(tcp_switch_, LV_STATE_CHECKED);
        } else {
            lv_obj_remove_state(tcp_switch_, LV_STATE_CHECKED);
        }
    }
    if (host_textarea_ != nullptr) {
        lv_textarea_set_text(host_textarea_, net.host);
    }
    if (tcp_port_textarea_ != nullptr) {
        char buf[8];
        std::snprintf(buf, sizeof(buf), "%d", net.tcp_port);
        lv_textarea_set_text(tcp_port_textarea_, buf);
    }
    if (udp_switch_ != nullptr) {
        if (net.udp_enabled) {
            lv_obj_add_state(udp_switch_, LV_STATE_CHECKED);
        } else {
            lv_obj_remove_state(udp_switch_, LV_STATE_CHECKED);
        }
    }
    if (udp_port_textarea_ != nullptr) {
        char buf[8];
        std::snprintf(buf, sizeof(buf), "%d", net.udp_port);
        lv_textarea_set_text(udp_port_textarea_, buf);
    }
}

void UartSettingApp::save_net_from_ui(void)
{
    netcfg::Config cfg;

    cfg.tcp_enabled = (tcp_switch_ != nullptr) && lv_obj_has_state(tcp_switch_, LV_STATE_CHECKED);
    cfg.udp_enabled = (udp_switch_ != nullptr) && lv_obj_has_state(udp_switch_, LV_STATE_CHECKED);

    if (host_textarea_ != nullptr) {
        std::snprintf(cfg.host, sizeof(cfg.host), "%s", lv_textarea_get_text(host_textarea_));
    } else {
        cfg.host[0] = '\0';
    }

    cfg.tcp_port = (tcp_port_textarea_ != nullptr) ? std::atoi(lv_textarea_get_text(tcp_port_textarea_)) : 8888;
    cfg.udp_port = (udp_port_textarea_ != nullptr) ? std::atoi(lv_textarea_get_text(udp_port_textarea_)) : 8888;
    if (cfg.tcp_port < 1 || cfg.tcp_port > 65535) cfg.tcp_port = 8888;
    if (cfg.udp_port < 1 || cfg.udp_port > 65535) cfg.udp_port = 8888;

    netcfg::save(cfg);
    // Состояние подключения отразит таймер refresh_net_status().
}

void UartSettingApp::refresh_net_status(void)
{
    if (net_status_label_ == nullptr) {
        return;
    }
    const netcfg::Config cfg = netcfg::load();
    const netcfg::TcpState st = netcfg::tcp_state();
    char text[160];
    std::snprintf(text, sizeof(text), "TCP %s: %s\n%s:%d",
                  cfg.tcp_enabled ? "on" : "off",
                  netcfg::tcp_state_name(st),
                  cfg.host[0] ? cfg.host : "(no host)",
                  cfg.tcp_port);
    lv_label_set_text(net_status_label_, text);
}

void UartSettingApp::save_net_event_cb(lv_event_t *event)
{
    UartSettingApp *app = static_cast<UartSettingApp *>(lv_event_get_user_data(event));
    if (app != nullptr) {
        app->save_net_from_ui();
    }
}

void UartSettingApp::net_status_timer_cb(lv_timer_t *timer)
{
    UartSettingApp *app = static_cast<UartSettingApp *>(lv_timer_get_user_data(timer));
    if (app != nullptr) {
        app->refresh_net_status();
    }
}

void UartSettingApp::textarea_event_cb(lv_event_t *event)
{
    UartSettingApp *app = static_cast<UartSettingApp *>(lv_event_get_user_data(event));
    if ((app == nullptr) || (app->keyboard_ == nullptr)) {
        return;
    }
    lv_obj_t *textarea = static_cast<lv_obj_t *>(lv_event_get_target(event));
    const lv_keyboard_mode_t mode =
        static_cast<lv_keyboard_mode_t>(reinterpret_cast<intptr_t>(lv_obj_get_user_data(textarea)));

    lv_keyboard_set_mode(app->keyboard_, mode);
    lv_keyboard_set_textarea(app->keyboard_, textarea);
    lv_obj_clear_flag(app->keyboard_, LV_OBJ_FLAG_HIDDEN);

    // Запас прокрутки снизу = высота клавиатуры, чтобы поле можно было поднять
    // над ней, а не оставить перекрытым.
    if (app->content_ != nullptr) {
        lv_obj_update_layout(app->keyboard_);
        const int32_t kb_h = lv_obj_get_height(app->keyboard_);
        lv_obj_set_style_pad_bottom(app->content_, kb_h, 0);
        lv_obj_update_layout(app->content_);
    }
    lv_obj_scroll_to_view(textarea, LV_ANIM_ON);
}

void UartSettingApp::keyboard_event_cb(lv_event_t *event)
{
    UartSettingApp *app = static_cast<UartSettingApp *>(lv_event_get_user_data(event));
    if ((app == nullptr) || (app->keyboard_ == nullptr)) {
        return;
    }
    lv_obj_add_flag(app->keyboard_, LV_OBJ_FLAG_HIDDEN);
    lv_keyboard_set_textarea(app->keyboard_, nullptr);
    if (app->content_ != nullptr) {
        lv_obj_set_style_pad_bottom(app->content_, 0, 0);
    }
}
