#pragma once

#include <cstddef>
#include <cstdint>
#include <deque>
#include <string>
#include <vector>

#include "freertos/FreeRTOS.h"
#include "freertos/stream_buffer.h"
#include "freertos/task.h"
#include "lite_app.h"
#include "lvgl.h"

class UartTerminalApp : public LiteApp {
public:
    UartTerminalApp();
    ~UartTerminalApp() override;

    const char *title(void) const override { return "UART Terminal"; }
    const char *icon_text(void) const override { return ">_"; }
    lv_color_t accent_color(void) const override { return lv_color_hex(0x16A085); }

    bool init(void) override;
    bool open(lv_obj_t *parent) override;
    void close(void) override;

private:
    struct TextStyle {
        uint32_t fg = 0;
        uint32_t bg = 0;
        bool bg_enabled = false;
        bool bold = false;
    };

    struct TextRun {
        TextStyle style;
        std::string text;
    };

    struct TerminalLine {
        std::vector<TextRun> runs;
        size_t cells = 0;
        uint32_t sequence = 0;
    };

    struct RowView {
        lv_obj_t *container = nullptr;
        int rendered_index = -1;
        uint32_t rendered_sequence = 0;
    };

    enum class ParserState {
        Normal,
        Escape,
        Csi,
    };

    bool init_uart(void);
    void uart_task(void);
    void drain_uart_stream(void);

    void parse_byte(uint8_t byte);
    void append_printable(char ch);
    void append_spaces(size_t count);
    void finish_line(void);
    void clear_history(bool reset_parser);
    void reset_style(void);
    void handle_csi_final(char final);
    void apply_sgr(const std::vector<int> &params);

    size_t virtual_line_count(void) const;
    const TerminalLine *line_at(size_t index) const;
    int32_t content_height(void) const;
    bool is_scroll_near_bottom(void) const;
    void update_content_height(void);
    void scroll_to_bottom(void);
    void recreate_row_pool(void);
    void refresh_visible_rows(void);
    void render_line_to_row(RowView &row, int line_index, int32_t y);
    void update_status_label(bool force);
    void update_follow_button(void);

    static bool same_style(const TextStyle &a, const TextStyle &b);
    static uint32_t xterm256_to_rgb(int index);
    static void poll_timer_cb(lv_timer_t *timer);
    static void scroll_event_cb(lv_event_t *event);
    static void clear_event_cb(lv_event_t *event);
    static void follow_event_cb(lv_event_t *event);
    static void uart_task_entry(void *arg);

    bool uart_installed_ = false;
    bool uart_task_started_ = false;
    TaskHandle_t uart_task_ = nullptr;
    StreamBufferHandle_t rx_stream_ = nullptr;
    char uart_status_[128] = {};

    volatile uint32_t received_bytes_ = 0;
    volatile uint32_t dropped_bytes_ = 0;

    std::deque<TerminalLine> lines_;
    TerminalLine current_line_;
    TextStyle current_style_;
    ParserState parser_state_ = ParserState::Normal;
    std::string csi_buffer_;
    bool previous_was_cr_ = false;
    uint32_t next_sequence_ = 1;

    lv_obj_t *root_ = nullptr;
    lv_obj_t *status_label_ = nullptr;
    lv_obj_t *follow_button_ = nullptr;
    lv_obj_t *viewport_ = nullptr;
    lv_obj_t *spacer_ = nullptr;
    lv_timer_t *poll_timer_ = nullptr;
    std::vector<RowView> row_pool_;
    int32_t line_height_ = 20;
    bool auto_follow_ = true;
    bool suppress_scroll_event_ = false;
    uint32_t last_status_update_ms_ = 0;
};
