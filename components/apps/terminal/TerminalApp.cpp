#include "terminal/TerminalApp.hpp"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "driver/uart.h"
#include "esp_err.h"
#include "esp_log.h"

#ifndef CONFIG_JC4880_TERMINAL_UART_PORT
#define CONFIG_JC4880_TERMINAL_UART_PORT 1
#endif

#ifndef CONFIG_JC4880_TERMINAL_UART_RX_PIN
#define CONFIG_JC4880_TERMINAL_UART_RX_PIN 35
#endif

#ifndef CONFIG_JC4880_TERMINAL_UART_TX_PIN
#define CONFIG_JC4880_TERMINAL_UART_TX_PIN -1
#endif

#ifndef CONFIG_JC4880_TERMINAL_UART_BAUD_RATE
#define CONFIG_JC4880_TERMINAL_UART_BAUD_RATE 115200
#endif

#ifndef CONFIG_JC4880_TERMINAL_MAX_LINES
#define CONFIG_JC4880_TERMINAL_MAX_LINES 5000
#endif

#ifndef CONFIG_JC4880_TERMINAL_MAX_COLUMNS
#define CONFIG_JC4880_TERMINAL_MAX_COLUMNS 240
#endif

#ifndef CONFIG_JC4880_TERMINAL_STREAM_BUFFER_SIZE
#define CONFIG_JC4880_TERMINAL_STREAM_BUFFER_SIZE 8192
#endif

namespace {

static const char *TAG = "uart_terminal";

static constexpr uint32_t kDefaultFg = 0xD6DEE8;
static constexpr uint32_t kDefaultBg = 0x05070A;
static constexpr uint32_t kToolbarBg = 0x111820;
static constexpr uint32_t kButtonBg = 0x27313D;
static constexpr uint32_t kButtonActiveBg = 0x177D5B;
static constexpr size_t kReadChunkSize = 256;

static void make_plain_container(lv_obj_t *obj)
{
    lv_obj_remove_style_all(obj);
    lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_pad_all(obj, 0, 0);
}

static lv_obj_t *create_toolbar_button(lv_obj_t *parent, const char *symbol)
{
    lv_obj_t *button = lv_button_create(parent);
    lv_obj_set_size(button, 36, 32);
    lv_obj_set_style_radius(button, 6, 0);
    lv_obj_set_style_border_width(button, 0, 0);
    lv_obj_set_style_bg_color(button, lv_color_hex(kButtonBg), 0);
    lv_obj_set_style_bg_opa(button, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(button, lv_color_hex(0x364253), LV_STATE_PRESSED);

    lv_obj_t *label = lv_label_create(button);
    lv_label_set_text(label, symbol);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_center(label);
    return button;
}

static int configured_uart_port(void)
{
    return CONFIG_JC4880_TERMINAL_UART_PORT;
}

static int configured_rx_pin(void)
{
    return CONFIG_JC4880_TERMINAL_UART_RX_PIN;
}

static int configured_tx_pin(void)
{
    return CONFIG_JC4880_TERMINAL_UART_TX_PIN;
}

static int configured_baud_rate(void)
{
    return CONFIG_JC4880_TERMINAL_UART_BAUD_RATE;
}

static int clamp_positive(int value, int fallback)
{
    return value > 0 ? value : fallback;
}

} // namespace

UartTerminalApp::UartTerminalApp()
{
    reset_style();
    std::snprintf(uart_status_, sizeof(uart_status_), "UART: not started");
}

UartTerminalApp::~UartTerminalApp()
{
}

bool UartTerminalApp::init(void)
{
    init_uart();
    return true;
}

bool UartTerminalApp::open(lv_obj_t *parent)
{
    if (parent == nullptr) {
        return false;
    }

    auto_follow_ = true;
    lv_obj_set_style_bg_color(parent, lv_color_hex(kDefaultBg), 0);
    lv_obj_set_style_bg_opa(parent, LV_OPA_COVER, 0);

    root_ = lv_obj_create(parent);
    make_plain_container(root_);
    lv_obj_set_size(root_, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(root_, lv_color_hex(kDefaultBg), 0);
    lv_obj_set_style_bg_opa(root_, LV_OPA_COVER, 0);
    lv_obj_set_flex_flow(root_, LV_FLEX_FLOW_COLUMN);

    lv_obj_t *toolbar = lv_obj_create(root_);
    make_plain_container(toolbar);
    lv_obj_set_width(toolbar, lv_pct(100));
    lv_obj_set_height(toolbar, 42);
    lv_obj_set_style_bg_color(toolbar, lv_color_hex(kToolbarBg), 0);
    lv_obj_set_style_bg_opa(toolbar, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_left(toolbar, 8, 0);
    lv_obj_set_style_pad_right(toolbar, 8, 0);
    lv_obj_set_style_pad_column(toolbar, 8, 0);
    lv_obj_set_flex_flow(toolbar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(toolbar, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    status_label_ = lv_label_create(toolbar);
    lv_obj_set_flex_grow(status_label_, 1);
    lv_label_set_long_mode(status_label_, LV_LABEL_LONG_DOT);
    lv_obj_set_style_text_font(status_label_, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(status_label_, lv_color_hex(0xAFC2D2), 0);

    lv_obj_t *demo_button = create_toolbar_button(toolbar, LV_SYMBOL_TINT);
    lv_obj_add_event_cb(demo_button, demo_event_cb, LV_EVENT_CLICKED, this);

    follow_button_ = create_toolbar_button(toolbar, LV_SYMBOL_DOWN);
    lv_obj_add_event_cb(follow_button_, follow_event_cb, LV_EVENT_CLICKED, this);

    lv_obj_t *clear_button = create_toolbar_button(toolbar, LV_SYMBOL_TRASH);
    lv_obj_add_event_cb(clear_button, clear_event_cb, LV_EVENT_CLICKED, this);

    viewport_ = lv_obj_create(root_);
    make_plain_container(viewport_);
    lv_obj_set_width(viewport_, lv_pct(100));
    lv_obj_set_flex_grow(viewport_, 1);
    lv_obj_set_style_bg_color(viewport_, lv_color_hex(kDefaultBg), 0);
    lv_obj_set_style_bg_opa(viewport_, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(viewport_, 0, 0);
    lv_obj_set_scroll_dir(viewport_, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(viewport_, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_add_flag(viewport_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(viewport_, scroll_event_cb, LV_EVENT_SCROLL, this);
    lv_obj_add_event_cb(viewport_, scroll_event_cb, LV_EVENT_SIZE_CHANGED, this);

    spacer_ = lv_obj_create(viewport_);
    make_plain_container(spacer_);
    lv_obj_set_width(spacer_, 1);
    lv_obj_set_height(spacer_, 1);

    lv_obj_update_layout(root_);
    recreate_row_pool();
    update_content_height();
    update_status_label(true);
    update_follow_button();
    scroll_to_bottom();
    refresh_visible_rows();

    poll_timer_ = lv_timer_create(poll_timer_cb, 30, this);
    if (!demo_seeded_ && (received_bytes_ == 0) && (virtual_line_count() == 0)) {
        inject_demo_uart_data();
        demo_seeded_ = true;
    }
    return true;
}

void UartTerminalApp::close(void)
{
    if (poll_timer_ != nullptr) {
        lv_timer_delete(poll_timer_);
    }

    poll_timer_ = nullptr;
    root_ = nullptr;
    status_label_ = nullptr;
    follow_button_ = nullptr;
    viewport_ = nullptr;
    spacer_ = nullptr;
    row_pool_.clear();
}

bool UartTerminalApp::init_uart(void)
{
    if (uart_installed_) {
        return true;
    }

    if (rx_stream_ == nullptr) {
        rx_stream_ = xStreamBufferCreate(CONFIG_JC4880_TERMINAL_STREAM_BUFFER_SIZE, 1);
        if (rx_stream_ == nullptr) {
            std::snprintf(uart_status_, sizeof(uart_status_), "UART: no stream buffer memory");
            ESP_LOGE(TAG, "%s", uart_status_);
            return false;
        }
    }

    const int rx_pin = configured_rx_pin();
    if (rx_pin < 0) {
        std::snprintf(uart_status_, sizeof(uart_status_), "UART: RX pin is not configured");
        ESP_LOGW(TAG, "%s", uart_status_);
        return false;
    }

    const uart_port_t port = static_cast<uart_port_t>(configured_uart_port());
    uart_config_t uart_config = {};
    uart_config.baud_rate = configured_baud_rate();
    uart_config.data_bits = UART_DATA_8_BITS;
    uart_config.parity = UART_PARITY_DISABLE;
    uart_config.stop_bits = UART_STOP_BITS_1;
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    uart_config.source_clk = UART_SCLK_DEFAULT;

    esp_err_t err = uart_driver_install(port, 4096, 0, 0, nullptr, 0);
    if ((err != ESP_OK) && (err != ESP_ERR_INVALID_STATE)) {
        std::snprintf(uart_status_, sizeof(uart_status_), "UART%d driver: %s", configured_uart_port(), esp_err_to_name(err));
        ESP_LOGE(TAG, "%s", uart_status_);
        return false;
    }

    err = uart_param_config(port, &uart_config);
    if (err != ESP_OK) {
        std::snprintf(uart_status_, sizeof(uart_status_), "UART%d config: %s", configured_uart_port(), esp_err_to_name(err));
        ESP_LOGE(TAG, "%s", uart_status_);
        return false;
    }

    const int tx_pin = configured_tx_pin();
    err = uart_set_pin(port,
                       tx_pin >= 0 ? tx_pin : UART_PIN_NO_CHANGE,
                       rx_pin >= 0 ? rx_pin : UART_PIN_NO_CHANGE,
                       UART_PIN_NO_CHANGE,
                       UART_PIN_NO_CHANGE);
    if (err != ESP_OK) {
        std::snprintf(uart_status_, sizeof(uart_status_), "UART%d pins: %s", configured_uart_port(), esp_err_to_name(err));
        ESP_LOGE(TAG, "%s", uart_status_);
        return false;
    }

    uart_flush_input(port);
    uart_installed_ = true;
    std::snprintf(uart_status_, sizeof(uart_status_), "UART%d RX GPIO%d @ %d",
                  configured_uart_port(), rx_pin, configured_baud_rate());

    if (!uart_task_started_) {
        BaseType_t task_ret = xTaskCreatePinnedToCore(uart_task_entry,
                                                      "uart_terminal_rx",
                                                      4096,
                                                      this,
                                                      5,
                                                      &uart_task_,
                                                      1);
        if (task_ret != pdPASS) {
            std::snprintf(uart_status_, sizeof(uart_status_), "UART%d task: no memory", configured_uart_port());
            ESP_LOGE(TAG, "%s", uart_status_);
            return false;
        }
        uart_task_started_ = true;
    }

    ESP_LOGI(TAG, "%s", uart_status_);
    return true;
}

void UartTerminalApp::uart_task_entry(void *arg)
{
    UartTerminalApp *app = static_cast<UartTerminalApp *>(arg);
    if (app != nullptr) {
        app->uart_task();
    }
    vTaskDelete(nullptr);
}

void UartTerminalApp::uart_task(void)
{
    uint8_t buffer[kReadChunkSize];
    const uart_port_t port = static_cast<uart_port_t>(configured_uart_port());

    for (;;) {
        const int read_len = uart_read_bytes(port, buffer, sizeof(buffer), pdMS_TO_TICKS(50));
        if (read_len <= 0) {
            continue;
        }

        received_bytes_ += static_cast<uint32_t>(read_len);
        if (rx_stream_ == nullptr) {
            dropped_bytes_ += static_cast<uint32_t>(read_len);
            continue;
        }

        const size_t sent = xStreamBufferSend(rx_stream_, buffer, static_cast<size_t>(read_len), 0);
        if (sent < static_cast<size_t>(read_len)) {
            dropped_bytes_ += static_cast<uint32_t>(static_cast<size_t>(read_len) - sent);
        }
    }
}

void UartTerminalApp::queue_uart_bytes(const char *data, size_t len)
{
    if ((data == nullptr) || (len == 0) || (rx_stream_ == nullptr)) {
        return;
    }

    received_bytes_ += static_cast<uint32_t>(len);

    size_t offset = 0;
    while (offset < len) {
        const size_t sent = xStreamBufferSend(rx_stream_, data + offset, len - offset, 0);
        if (sent > 0) {
            offset += sent;
            continue;
        }

        drain_uart_stream();
        const size_t retry_sent = xStreamBufferSend(rx_stream_, data + offset, len - offset, 0);
        if (retry_sent == 0) {
            dropped_bytes_ += static_cast<uint32_t>(len - offset);
            return;
        }
        offset += retry_sent;
    }
}

void UartTerminalApp::inject_demo_uart_data(void)
{
    static constexpr const char *kDemoHeader =
        "\x1B[38;5;45m[UART]\x1B[0m synthetic controller stream, xterm256 test\r\n"
        "\x1B[38;5;82m[OK]\x1B[0m boot complete  "
        "\x1B[38;5;226m[WARN]\x1B[0m adc noise high  "
        "\x1B[38;5;196m[ERR]\x1B[0m limit switch open\r\n"
        "\x1B[48;5;22;38;5;231m SAFE \x1B[0m "
        "\x1B[48;5;94;38;5;231m BUSY \x1B[0m "
        "\x1B[48;5;52;38;5;231m FAULT \x1B[0m "
        "\x1B[48;5;17;38;5;159m DEBUG \x1B[0m background color blocks\r\n"
        "\x1B[38;5;16m016 \x1B[38;5;21m021 \x1B[38;5;46m046 \x1B[38;5;51m051 "
        "\x1B[38;5;93m093 \x1B[38;5;129m129 \x1B[38;5;160m160 \x1B[38;5;196m196 "
        "\x1B[38;5;202m202 \x1B[38;5;226m226 \x1B[38;5;231m231 \x1B[0m xterm color cube samples\r\n"
        "\x1B[38;5;232m232 \x1B[38;5;236m236 \x1B[38;5;240m240 \x1B[38;5;244m244 "
        "\x1B[38;5;248m248 \x1B[38;5;252m252 \x1B[38;5;255m255 \x1B[0m grayscale ramp\r\n";

    queue_uart_bytes(kDemoHeader, std::strlen(kDemoHeader));

    char line[256];
    static constexpr const char *kLevels[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FAULT"};
    static constexpr int kLevelFg[] = {244, 45, 82, 226, 196, 231};
    static constexpr int kLevelBg[] = {235, 17, 22, 58, 52, 88};

    for (int i = 0; i < 320; ++i) {
        const int level_index = i % 6;
        const int channel = i % 8;
        const int fg = 16 + ((i * 7) % 216);
        const int bg = 232 + (i % 24);
        const int value = (i * 37) % 4096;
        const int temp = 24 + (i % 19);
        const int voltage_mv = 3150 + ((i * 11) % 700);

        const int len = std::snprintf(
            line,
            sizeof(line),
            "\x1B[38;5;%dm%04d\x1B[0m "
            "\x1B[48;5;%d;38;5;%dm %-5s \x1B[0m "
            "ch=\x1B[38;5;%dm%d\x1B[0m adc=%04d temp=%02dC vbat=%dmV "
            "\x1B[48;5;%d;38;5;15m pwm=%03d \x1B[0m\r\n",
            fg,
            i,
            kLevelBg[level_index],
            kLevelFg[level_index],
            kLevels[level_index],
            33 + channel,
            channel,
            value,
            temp,
            voltage_mv,
            bg,
            (i * 3) % 256);

        if (len > 0) {
            queue_uart_bytes(line, static_cast<size_t>(std::min<int>(len, sizeof(line) - 1)));
        }
    }

    static constexpr const char *kDemoTail =
        "\x1B[38;5;118m[UART]\x1B[0m synthetic burst finished; scroll upward to verify virtualization\r\n";
    queue_uart_bytes(kDemoTail, std::strlen(kDemoTail));

    drain_uart_stream();
    update_status_label(true);
}

void UartTerminalApp::drain_uart_stream(void)
{
    if (rx_stream_ == nullptr) {
        return;
    }

    uint8_t buffer[kReadChunkSize];
    bool changed = false;
    for (int batch = 0; batch < 12; ++batch) {
        const size_t read_len = xStreamBufferReceive(rx_stream_, buffer, sizeof(buffer), 0);
        if (read_len == 0) {
            break;
        }

        changed = true;
        for (size_t i = 0; i < read_len; ++i) {
            parse_byte(buffer[i]);
        }
    }

    if (changed) {
        update_content_height();
        if (auto_follow_) {
            scroll_to_bottom();
        }
        refresh_visible_rows();
    }
}

void UartTerminalApp::parse_byte(uint8_t byte)
{
    switch (parser_state_) {
    case ParserState::Normal:
        if (byte == 0x1B) {
            parser_state_ = ParserState::Escape;
            return;
        }
        if (byte == '\r') {
            finish_line();
            previous_was_cr_ = true;
            return;
        }
        if (byte == '\n') {
            if (!previous_was_cr_) {
                finish_line();
            }
            previous_was_cr_ = false;
            return;
        }
        previous_was_cr_ = false;
        if (byte == '\t') {
            const size_t spaces = 4 - (current_line_.cells % 4);
            append_spaces(spaces);
            return;
        }
        if ((byte >= 0x20) || (byte >= 0x80)) {
            append_printable(static_cast<char>(byte));
        }
        return;

    case ParserState::Escape:
        if (byte == '[') {
            csi_buffer_.clear();
            parser_state_ = ParserState::Csi;
        } else {
            parser_state_ = ParserState::Normal;
        }
        return;

    case ParserState::Csi:
        if ((byte >= 0x40) && (byte <= 0x7E)) {
            handle_csi_final(static_cast<char>(byte));
            parser_state_ = ParserState::Normal;
            csi_buffer_.clear();
            return;
        }
        if (csi_buffer_.size() < 64) {
            csi_buffer_.push_back(static_cast<char>(byte));
        } else {
            parser_state_ = ParserState::Normal;
            csi_buffer_.clear();
        }
        return;
    }
}

void UartTerminalApp::append_printable(char ch)
{
    if (current_line_.cells >= static_cast<size_t>(clamp_positive(CONFIG_JC4880_TERMINAL_MAX_COLUMNS, 240))) {
        finish_line();
    }

    if (current_line_.runs.empty() || !same_style(current_line_.runs.back().style, current_style_)) {
        TextRun run;
        run.style = current_style_;
        current_line_.runs.push_back(std::move(run));
    }

    current_line_.runs.back().text.push_back(ch);
    current_line_.cells++;
    current_line_.sequence = next_sequence_++;
}

void UartTerminalApp::append_spaces(size_t count)
{
    for (size_t i = 0; i < count; ++i) {
        append_printable(' ');
    }
}

void UartTerminalApp::finish_line(void)
{
    current_line_.sequence = next_sequence_++;
    lines_.push_back(std::move(current_line_));
    while (lines_.size() > static_cast<size_t>(clamp_positive(CONFIG_JC4880_TERMINAL_MAX_LINES, 5000))) {
        lines_.pop_front();
    }
    current_line_ = TerminalLine{};
}

void UartTerminalApp::clear_history(bool reset_parser)
{
    lines_.clear();
    current_line_ = TerminalLine{};
    if (reset_parser) {
        parser_state_ = ParserState::Normal;
        csi_buffer_.clear();
        previous_was_cr_ = false;
        reset_style();
    }
    next_sequence_++;
    update_content_height();
    scroll_to_bottom();
    refresh_visible_rows();
}

void UartTerminalApp::reset_style(void)
{
    current_style_.fg = kDefaultFg;
    current_style_.bg = kDefaultBg;
    current_style_.bg_enabled = false;
    current_style_.bold = false;
}

void UartTerminalApp::handle_csi_final(char final)
{
    if (final == 'm') {
        std::vector<int> params;
        if (csi_buffer_.empty()) {
            params.push_back(0);
        } else {
            size_t start = 0;
            while (start <= csi_buffer_.size()) {
                const size_t end = csi_buffer_.find(';', start);
                const size_t token_end = (end == std::string::npos) ? csi_buffer_.size() : end;
                if (token_end == start) {
                    params.push_back(0);
                } else {
                    bool numeric = true;
                    for (size_t i = start; i < token_end; ++i) {
                        if ((csi_buffer_[i] < '0') || (csi_buffer_[i] > '9')) {
                            numeric = false;
                            break;
                        }
                    }
                    params.push_back(numeric ? std::atoi(csi_buffer_.substr(start, token_end - start).c_str()) : -1);
                }
                if (end == std::string::npos) {
                    break;
                }
                start = end + 1;
            }
        }
        apply_sgr(params);
        return;
    }

    if (final == 'J') {
        if ((csi_buffer_ == "2") || (csi_buffer_ == "3")) {
            clear_history(false);
        }
        return;
    }

    if (final == 'K') {
        current_line_ = TerminalLine{};
        current_line_.sequence = next_sequence_++;
    }
}

void UartTerminalApp::apply_sgr(const std::vector<int> &params)
{
    for (size_t i = 0; i < params.size(); ++i) {
        const int p = params[i];
        if (p == 0) {
            reset_style();
        } else if (p == 1) {
            current_style_.bold = true;
        } else if (p == 22) {
            current_style_.bold = false;
        } else if ((p >= 30) && (p <= 37)) {
            current_style_.fg = xterm256_to_rgb(p - 30);
        } else if (p == 39) {
            current_style_.fg = kDefaultFg;
        } else if ((p >= 40) && (p <= 47)) {
            current_style_.bg = xterm256_to_rgb(p - 40);
            current_style_.bg_enabled = true;
        } else if (p == 49) {
            current_style_.bg = kDefaultBg;
            current_style_.bg_enabled = false;
        } else if ((p >= 90) && (p <= 97)) {
            current_style_.fg = xterm256_to_rgb((p - 90) + 8);
        } else if ((p >= 100) && (p <= 107)) {
            current_style_.bg = xterm256_to_rgb((p - 100) + 8);
            current_style_.bg_enabled = true;
        } else if ((p == 38 || p == 48) && (i + 2 < params.size()) && (params[i + 1] == 5)) {
            const uint32_t rgb = xterm256_to_rgb(params[i + 2]);
            if (p == 38) {
                current_style_.fg = rgb;
            } else {
                current_style_.bg = rgb;
                current_style_.bg_enabled = true;
            }
            i += 2;
        } else if ((p == 38 || p == 48) && (i + 4 < params.size()) && (params[i + 1] == 2)) {
            const int r = std::max(0, std::min(255, params[i + 2]));
            const int g = std::max(0, std::min(255, params[i + 3]));
            const int b = std::max(0, std::min(255, params[i + 4]));
            const uint32_t rgb = (static_cast<uint32_t>(r) << 16) |
                                 (static_cast<uint32_t>(g) << 8) |
                                 static_cast<uint32_t>(b);
            if (p == 38) {
                current_style_.fg = rgb;
            } else {
                current_style_.bg = rgb;
                current_style_.bg_enabled = true;
            }
            i += 4;
        }
    }
}

size_t UartTerminalApp::virtual_line_count(void) const
{
    return lines_.size() + (current_line_.cells > 0 ? 1U : 0U);
}

const UartTerminalApp::TerminalLine *UartTerminalApp::line_at(size_t index) const
{
    if (index < lines_.size()) {
        return &lines_[index];
    }

    if ((index == lines_.size()) && (current_line_.cells > 0)) {
        return &current_line_;
    }

    return nullptr;
}

int32_t UartTerminalApp::content_height(void) const
{
    const size_t line_count = virtual_line_count();
    const size_t pixels = std::max<size_t>(1, line_count) * static_cast<size_t>(line_height_);
    return static_cast<int32_t>(std::min<size_t>(pixels, 0x7FFFFFFF));
}

bool UartTerminalApp::is_scroll_near_bottom(void) const
{
    if (viewport_ == nullptr) {
        return true;
    }

    const int32_t view_h = std::max<int32_t>(1, lv_obj_get_height(viewport_));
    const int32_t scroll_y = lv_obj_get_scroll_y(viewport_);
    const int32_t total_h = content_height();
    return (total_h <= view_h) || ((scroll_y + view_h) >= (total_h - (line_height_ * 2)));
}

void UartTerminalApp::update_content_height(void)
{
    if (spacer_ != nullptr) {
        lv_obj_set_height(spacer_, content_height());
    }
}

void UartTerminalApp::scroll_to_bottom(void)
{
    if (viewport_ == nullptr) {
        return;
    }

    suppress_scroll_event_ = true;
    lv_obj_update_layout(viewport_);
    lv_obj_scroll_to_y(viewport_, content_height(), LV_ANIM_OFF);
    suppress_scroll_event_ = false;
    auto_follow_ = true;
    update_follow_button();
}

void UartTerminalApp::recreate_row_pool(void)
{
    if (viewport_ == nullptr) {
        return;
    }

    for (RowView &row : row_pool_) {
        if (row.container != nullptr) {
            lv_obj_delete(row.container);
        }
    }
    row_pool_.clear();

    lv_obj_update_layout(viewport_);
    const int32_t viewport_h = std::max<int32_t>(1, lv_obj_get_height(viewport_));
    const size_t row_count = static_cast<size_t>(viewport_h / line_height_) + 4;
    row_pool_.reserve(row_count);

    for (size_t i = 0; i < row_count; ++i) {
        RowView row;
        row.container = lv_obj_create(viewport_);
        make_plain_container(row.container);
        lv_obj_add_flag(row.container, LV_OBJ_FLAG_FLOATING);
        lv_obj_clear_flag(row.container, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_size(row.container, lv_pct(100), line_height_);
        lv_obj_set_style_pad_left(row.container, 6, 0);
        lv_obj_set_style_pad_right(row.container, 6, 0);
        lv_obj_set_style_pad_column(row.container, 0, 0);
        lv_obj_set_flex_flow(row.container, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(row.container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_add_flag(row.container, LV_OBJ_FLAG_HIDDEN);
        row_pool_.push_back(row);
    }
}

void UartTerminalApp::refresh_visible_rows(void)
{
    if ((viewport_ == nullptr) || row_pool_.empty()) {
        return;
    }

    const int32_t scroll_y = std::max<int32_t>(0, lv_obj_get_scroll_y(viewport_));
    const int first_index = static_cast<int>(scroll_y / line_height_);
    const int32_t offset_y = -(scroll_y % line_height_);

    for (size_t i = 0; i < row_pool_.size(); ++i) {
        const int line_index = first_index + static_cast<int>(i);
        const int32_t y = offset_y + static_cast<int32_t>(i) * line_height_;
        render_line_to_row(row_pool_[i], line_index, y);
    }
}

void UartTerminalApp::render_line_to_row(RowView &row, int line_index, int32_t y)
{
    if (row.container == nullptr) {
        return;
    }

    const TerminalLine *line = (line_index >= 0) ? line_at(static_cast<size_t>(line_index)) : nullptr;
    if (line == nullptr) {
        lv_obj_add_flag(row.container, LV_OBJ_FLAG_HIDDEN);
        row.rendered_index = -1;
        row.rendered_sequence = 0;
        return;
    }

    lv_obj_set_pos(row.container, 0, y);
    lv_obj_clear_flag(row.container, LV_OBJ_FLAG_HIDDEN);

    if ((row.rendered_index == line_index) && (row.rendered_sequence == line->sequence)) {
        return;
    }

    lv_obj_clean(row.container);
    row.rendered_index = line_index;
    row.rendered_sequence = line->sequence;

    for (const TextRun &run : line->runs) {
        if (run.text.empty()) {
            continue;
        }

        lv_obj_t *label = lv_label_create(row.container);
        lv_label_set_text(label, run.text.c_str());
        lv_label_set_long_mode(label, LV_LABEL_LONG_MODE_CLIP);
        lv_obj_set_height(label, line_height_);
        lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(label, lv_color_hex(run.style.fg), 0);
        lv_obj_set_style_pad_top(label, 1, 0);
        lv_obj_set_style_pad_bottom(label, 0, 0);
        lv_obj_set_style_pad_left(label, 0, 0);
        lv_obj_set_style_pad_right(label, 0, 0);
        if (run.style.bg_enabled) {
            lv_obj_set_style_bg_color(label, lv_color_hex(run.style.bg), 0);
            lv_obj_set_style_bg_opa(label, LV_OPA_COVER, 0);
        } else {
            lv_obj_set_style_bg_opa(label, LV_OPA_TRANSP, 0);
        }
    }
}

void UartTerminalApp::update_status_label(bool force)
{
    if (status_label_ == nullptr) {
        return;
    }

    const uint32_t now = esp_log_timestamp();
    if (!force && ((now - last_status_update_ms_) < 500)) {
        return;
    }
    last_status_update_ms_ = now;

    char text[192] = {};
    std::snprintf(text,
                  sizeof(text),
                  "%s  lines:%u  bytes:%lu  drop:%lu",
                  uart_status_,
                  static_cast<unsigned>(virtual_line_count()),
                  static_cast<unsigned long>(received_bytes_),
                  static_cast<unsigned long>(dropped_bytes_));
    lv_label_set_text(status_label_, text);
}

void UartTerminalApp::update_follow_button(void)
{
    if (follow_button_ == nullptr) {
        return;
    }

    lv_obj_set_style_bg_color(follow_button_,
                              lv_color_hex(auto_follow_ ? kButtonActiveBg : kButtonBg),
                              0);
}

bool UartTerminalApp::same_style(const TextStyle &a, const TextStyle &b)
{
    return (a.fg == b.fg) &&
           (a.bg == b.bg) &&
           (a.bg_enabled == b.bg_enabled) &&
           (a.bold == b.bold);
}

uint32_t UartTerminalApp::xterm256_to_rgb(int index)
{
    static constexpr uint32_t kAnsi16[16] = {
        0x000000, 0xCD3131, 0x0DBC79, 0xE5E510,
        0x2472C8, 0xBC3FBC, 0x11A8CD, 0xE5E5E5,
        0x666666, 0xF14C4C, 0x23D18B, 0xF5F543,
        0x3B8EEA, 0xD670D6, 0x29B8DB, 0xFFFFFF,
    };

    if (index < 0) {
        return kDefaultFg;
    }
    if (index < 16) {
        return kAnsi16[index];
    }
    if (index < 232) {
        const int color = index - 16;
        const int r = color / 36;
        const int g = (color / 6) % 6;
        const int b = color % 6;
        const auto component = [](int value) -> uint32_t {
            return value == 0 ? 0U : static_cast<uint32_t>(55 + (value * 40));
        };
        return (component(r) << 16) | (component(g) << 8) | component(b);
    }
    if (index < 256) {
        const uint32_t gray = static_cast<uint32_t>(8 + ((index - 232) * 10));
        return (gray << 16) | (gray << 8) | gray;
    }
    return kDefaultFg;
}

void UartTerminalApp::poll_timer_cb(lv_timer_t *timer)
{
    UartTerminalApp *app = static_cast<UartTerminalApp *>(lv_timer_get_user_data(timer));
    if (app == nullptr) {
        return;
    }

    app->drain_uart_stream();
    app->update_status_label(false);
}

void UartTerminalApp::scroll_event_cb(lv_event_t *event)
{
    UartTerminalApp *app = static_cast<UartTerminalApp *>(lv_event_get_user_data(event));
    if (app == nullptr) {
        return;
    }

    const lv_event_code_t code = lv_event_get_code(event);
    if (code == LV_EVENT_SIZE_CHANGED) {
        app->recreate_row_pool();
        app->update_content_height();
    }

    if (!app->suppress_scroll_event_) {
        app->auto_follow_ = app->is_scroll_near_bottom();
        app->update_follow_button();
    }
    app->refresh_visible_rows();
}

void UartTerminalApp::demo_event_cb(lv_event_t *event)
{
    UartTerminalApp *app = static_cast<UartTerminalApp *>(lv_event_get_user_data(event));
    if (app != nullptr) {
        app->inject_demo_uart_data();
    }
}

void UartTerminalApp::clear_event_cb(lv_event_t *event)
{
    UartTerminalApp *app = static_cast<UartTerminalApp *>(lv_event_get_user_data(event));
    if (app != nullptr) {
        app->clear_history(true);
        app->update_status_label(true);
    }
}

void UartTerminalApp::follow_event_cb(lv_event_t *event)
{
    UartTerminalApp *app = static_cast<UartTerminalApp *>(lv_event_get_user_data(event));
    if (app == nullptr) {
        return;
    }

    app->auto_follow_ = !app->auto_follow_;
    if (app->auto_follow_) {
        app->scroll_to_bottom();
    }
    app->update_follow_button();
    app->refresh_visible_rows();
}
