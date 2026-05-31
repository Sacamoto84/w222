#pragma once

#include "lite_app.h"
#include "lvgl.h"

// Приложение настройки порта UART для терминала.
//
// Показывает и позволяет менять параметры UART (порт, скорость, чётность,
// пины RX/TX). Изменения сохраняются в NVS через модуль uartcfg и применяются
// приложением терминала к живому драйверу.
class UartSettingApp : public LiteApp {
public:
    UartSettingApp() = default;
    ~UartSettingApp() override = default;

    const char *title(void) const override { return "UART Settings"; }
    const char *icon_text(void) const override { return LV_SYMBOL_USB; }
    lv_color_t accent_color(void) const override { return lv_color_hex(0xC77F0A); }

    bool open(lv_obj_t *parent) override;
    void close(void) override;

private:
    lv_obj_t *port_dropdown_ = nullptr;
    lv_obj_t *baud_dropdown_ = nullptr;
    lv_obj_t *parity_dropdown_ = nullptr;
    lv_obj_t *rx_textarea_ = nullptr;
    lv_obj_t *tx_textarea_ = nullptr;
    lv_obj_t *status_label_ = nullptr;

    void load_into_ui(void);
    void save_from_ui(void);

    static void save_event_cb(lv_event_t *event);
};
