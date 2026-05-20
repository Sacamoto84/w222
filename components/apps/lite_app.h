#pragma once

#include "lvgl.h"

class LiteApp {
public:
    using CloseRequestCallback = void (*)(LiteApp *app, void *user_ctx);

    virtual ~LiteApp() = default;

    virtual const char *title(void) const = 0;
    virtual const char *icon_text(void) const = 0;
    virtual lv_color_t accent_color(void) const = 0;

    virtual bool init(void) { return true; }
    virtual bool open(lv_obj_t *parent) = 0;
    virtual bool back(void) { return false; }
    virtual void close(void) {}

    void set_close_request_callback(CloseRequestCallback callback, void *user_ctx)
    {
        close_request_callback_ = callback;
        close_request_user_ctx_ = user_ctx;
    }

protected:
    void request_close(void)
    {
        if (close_request_callback_ != nullptr) {
            close_request_callback_(this, close_request_user_ctx_);
        }
    }

private:
    CloseRequestCallback close_request_callback_ = nullptr;
    void *close_request_user_ctx_ = nullptr;
};
