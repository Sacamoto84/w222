#pragma once

#include "lvgl.h"
#include "lite_app.h"

class Calculator: public LiteApp
{
public:
	Calculator();
	~Calculator() override;

    const char *title(void) const override { return "Calculator"; }
    const char *icon_text(void) const override { return "C"; }
    lv_color_t accent_color(void) const override { return lv_color_hex(0x2088FF); }
    bool open(lv_obj_t *parent) override;
    void close(void) override;

    bool isStartZero(void);
    bool isStartNum(void);
    bool isStartPercent(void);
    bool isLegalDot(void);
    double calculate(const char *input);

    int formula_len = 0;
    lv_obj_t *root = nullptr;
    lv_obj_t *keyboard = nullptr;
    lv_obj_t *history_label = nullptr;
    lv_obj_t *formula_label = nullptr;
    lv_obj_t *result_label = nullptr;
    uint16_t _height = 0;
    uint16_t _width = 0;

private:
    static void keyboard_event_cb(lv_event_t *e);
};
