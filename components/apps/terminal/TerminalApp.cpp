#include "terminal/TerminalApp.hpp"

#include <algorithm>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "driver/uart.h"
#include "esp_err.h"
#include "esp_log.h"

#include "widgets/WidgetParser.hpp"
#include "widgets/WidgetRender.hpp"

#include "src/indev/lv_indev_private.h"
#include "src/indev/lv_indev_gesture_private.h"

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
    lv_obj_set_size(button, 72, 64);
    lv_obj_set_style_radius(button, 12, 0);
    lv_obj_set_style_border_width(button, 0, 0);
    lv_obj_set_style_bg_color(button, lv_color_hex(kButtonBg), 0);
    lv_obj_set_style_bg_opa(button, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(button, lv_color_hex(0x364253), LV_STATE_PRESSED);

    lv_obj_t *label = lv_label_create(button);
    lv_label_set_text(label, symbol);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_28, 0);
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

static const lv_font_t *kTerminalFonts[] = {
    &lv_font_montserrat_12,
    &lv_font_montserrat_14,
    &lv_font_montserrat_16,
    &lv_font_montserrat_18,
    &lv_font_montserrat_20,
    &lv_font_montserrat_22,
    &lv_font_montserrat_24,
    &lv_font_montserrat_26,
    &lv_font_montserrat_28,
};
static constexpr size_t kTerminalFontCount = sizeof(kTerminalFonts) / sizeof(kTerminalFonts[0]);

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
    lv_obj_set_height(toolbar, 84);
    lv_obj_set_style_bg_color(toolbar, lv_color_hex(kToolbarBg), 0);
    lv_obj_set_style_bg_opa(toolbar, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_left(toolbar, 16, 0);
    lv_obj_set_style_pad_right(toolbar, 16, 0);
    lv_obj_set_style_pad_column(toolbar, 16, 0);
    lv_obj_set_flex_flow(toolbar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(toolbar, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *back_button = create_toolbar_button(toolbar, LV_SYMBOL_LEFT);
    lv_obj_add_event_cb(back_button, back_event_cb, LV_EVENT_CLICKED, this);

    status_label_ = lv_label_create(toolbar);
    lv_obj_set_flex_grow(status_label_, 1);
    lv_label_set_long_mode(status_label_, LV_LABEL_LONG_DOT);
    lv_obj_set_style_text_font(status_label_, &lv_font_montserrat_24, 0);
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
    lv_obj_add_flag(viewport_, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(viewport_, LV_OBJ_FLAG_SCROLL_MOMENTUM);
    lv_obj_add_event_cb(viewport_, scroll_event_cb, LV_EVENT_SCROLL, this);
    lv_obj_add_event_cb(viewport_, scroll_event_cb, LV_EVENT_SIZE_CHANGED, this);
    lv_obj_add_event_cb(viewport_, page_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_add_event_cb(viewport_, gesture_event_cb, LV_EVENT_GESTURE, this);

    spacer_ = lv_obj_create(viewport_);
    make_plain_container(spacer_);
    lv_obj_set_width(spacer_, 1);
    lv_obj_set_height(spacer_, 1);

    lv_obj_update_layout(root_);
    line_height_ = line_height_for_font(font_for_index(font_index_));

    // Настройка gesture recognizers: отключаем rotation (поднимаем порог до невозможного),
    // чтобы pinch распознавался корректно.
    lv_indev_t *indev = lv_indev_get_next(nullptr);
    while (indev != nullptr) {
        if (lv_indev_get_type(indev) == LV_INDEV_TYPE_POINTER) {
            // Уменьшаем порог pinch — срабатывать при 10% изменении расстояния
            lv_indev_set_pinch_up_threshold(indev, 1.1f);
            // Поднимаем порог rotation до 10 радиан (~573°) — фактически отключаем
            lv_indev_gesture_recognizer_t *rot = &indev->recognizers[LV_INDEV_GESTURE_ROTATE];
            if (rot->config == nullptr) {
                rot->config = (lv_indev_gesture_configuration_t *)lv_malloc_zeroed(sizeof(lv_indev_gesture_configuration_t));
            }
            if (rot->config != nullptr) {
                rot->config->rotation_angle_rad_threshold = 10.0f;
            }
            break;
        }
        indev = lv_indev_get_next(indev);
    }

    // Подгоняем высоту viewport под целое число строк:
    // высота viewport должна быть строго кратна line_height_.
    // Лишние/недостающие пиксели забираем/добавляем в toolbar (минимальным образом).
    const int32_t viewport_h = lv_obj_get_height(viewport_);
    const int32_t remainder = viewport_h % line_height_;
    if (remainder != 0) {
        const int32_t toolbar_h = lv_obj_get_height(toolbar);
        const int32_t to_remove = remainder;             // сколько пикселей забрать из viewport → добавить в toolbar
        const int32_t to_add = line_height_ - remainder; // сколько пикселей добавить в viewport → забрать из toolbar

        if (to_remove <= to_add && (toolbar_h + to_remove) >= 64) {
            lv_obj_set_height(toolbar, toolbar_h + to_remove);
        } else if ((toolbar_h - to_add) >= 64) {
            lv_obj_set_height(toolbar, toolbar_h - to_add);
        }
        lv_obj_update_layout(root_);
    }

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

    for (RowView &row : row_pool_) {
        if (row.draw_buf != nullptr) {
            lv_draw_buf_destroy(row.draw_buf);
            row.draw_buf = nullptr;
        }
    }
    row_pool_.clear();
    // Контейнеры виджетов — дети viewport_, они удалятся вместе с ним при
    // очистке родителя лаунчером; здесь просто сбрасываем указатели.
    widget_views_.clear();

    poll_timer_ = nullptr;
    root_ = nullptr;
    status_label_ = nullptr;
    follow_button_ = nullptr;
    viewport_ = nullptr;
    spacer_ = nullptr;
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

    // Встроенные виджеты — добавляем программно
    {
        WidgetDesc w;
        w.kind = WidgetDesc::Kind::ProgressBar;
        w.label = "CPU";
        w.value = 45;
        w.max_value = 100;
        w.bar_color = 0x00AA44;
        w.track_color = 0x1A1A1A;
        add_widget_line(w);

        w.label = "RAM";
        w.value = 72;
        w.bar_color = 0xCC8800;
        add_widget_line(w);

        w.label = "Flash";
        w.value = 12;
        w.bar_color = 0x0088CC;
        add_widget_line(w);

        WidgetDesc s;
        s.kind = WidgetDesc::Kind::StatusBlock;
        s.bar_color = 0xCC2222;
        s.text_color = 0xFFFFFF;
        s.label = "  ERROR: Motor fault  ";
        add_widget_line(s);

        s.bar_color = 0x22AA44;
        s.label = "  OK: System ready  ";
        add_widget_line(s);

        s.bar_color = 0xCCAA00;
        s.label = "  WARN: High temperature  ";
        add_widget_line(s);

        // Еще один прогрессбар
        w.label = "Battery";
        w.value = 89;
        w.bar_color = 0x00CC66;
        add_widget_line(w);
    }

    // Примеры виджетов через OSC-последовательности (как будто пришли по UART)
    static constexpr const char *kOscWidgets =
        "\x1B]WIDGET;PROGRESS;65;100;0x0088FF;Network\x07\r\n"
        "\x1B]WIDGET;STATUS;0;0;0x4444CC;  INFO: Data sync  \x07\r\n"
        "\x1B]WIDGET;PROGRESS;33;100;0xCC44CC;GPU\x07\r\n";
    queue_uart_bytes(kOscWidgets, std::strlen(kOscWidgets));

    // --- Полный каталог виджетов TimberWidget (`ui type=...`) ---
    // Это канонический демо-набор из TimberWidget (все 26 типов). Прогоняем его
    // как обычный UART-поток: каждая строка пройдёт через парсер и превратится
    // в виджет-элемент списка терминала.
    static const char *const kTimberDemo[] = {
        "ui type=badge text=\"READY\" st=ok\r\n",
        "ui type=badge text=\"WARN\" st=warn\r\n",
        "ui type=badge text=\"FAIL\" st=error\r\n",
        "ui type=dot color=#00E676 size=16 label=\"Link active\"\r\n",
        "ui type=image name=info size=40 desc=\"Info icon\"\r\n",
        "ui type=panel title=\"Motor 1\" value=READY subtitle=\"24.3V 1.8A\" accent=#36C36B icon=info\r\n",
        "ui type=progress label=\"Battery\" value=72 max=100 fill=#36C36B display=\"72%\"\r\n",
        "ui type=2col left=\"Voltage\" right=\"24.3V\"\r\n",
        "ui type=switch label=\"Pump enable\" state=on subtitle=\"Remote mode\"\r\n",
        "ui type=stats-card title=\"RPM\" value=1450 unit=\"rpm\" delta=\"+12\" subtitle=\"Motor 1\" accent=#36C36B\r\n",
        "ui type=alarm-card title=\"Overheat\" message=\"Motor 1 temperature reached 92C\" severity=critical time=\"12:41:03\" icon=warn2\r\n",
        "ui type=gauge label=\"CPU\" value=72 max=100 unit=\"%\" color=#36C36B\r\n",
        "ui type=battery label=\"Battery A\" value=78 max=100 charging=true voltage=4.08\r\n",
        "ui type=sparkline label=\"Temp\" values=\"21,22,22,23,24,23,25\" min=18 max=28 color=#36C36B display=\"25C\" points=on\r\n",
        "ui type=bar-group title=\"Motors\" labels=\"M1|M2|M3\" values=\"20|45|80\" max=100 colors=\"#36C36B|#4FC3F7|#FFB300\"\r\n",
        "ui type=line-chart title=\"Voltage\" values=\"24.1,24.2,24.0,24.3,24.4\" labels=\"T1|T2|T3|T4|T5\" min=23 max=25 color=#4FC3F7\r\n",
        "ui type=led-row title=\"Links\" items=\"NET:#00E676|MQTT:#00E676|ERR:#FF5252|GPS:off\"\r\n",
        "ui type=kv-grid title=\"Motor 1\" items=\"Voltage:24.3V|Current:1.8A|Temp:62C|State:READY\" columns=2\r\n",
        "ui type=pin-bank title=\"GPIO\" items=\"D1:on|D2:off|D3:warn|A0:adc|PWM1:pwm\"\r\n",
        "ui type=timeline title=\"Boot\" items=\"12:01 Boot|12:03 WiFi connected|12:05 MQTT online\"\r\n",
        "ui type=table headers=\"Name|State|Temp\" rows=\"M1|READY|24.3;M2|WAIT|22.9;M3|ALARM|91.8\"\r\n",
        "ui type=bitfield label=\"STATUS\" value=0xB38F bits=16\r\n",
        "ui type=hex-dump title=\"RX Buffer\" data=\"48 65 6C 6C 6F 20 57 6F 72 6C 64\" width=8 addr=0x1000 ascii=on\r\n",
        "ui type=register-table title=\"Holding Registers\" rows=\"0000|0x1234|Status;0001|0x00A5|Flags;0002|0x03E8|Speed\"\r\n",
        "ui type=modbus-frame direction=request preset=rtu data=\"01 03 00 10 00 02 C5 CE\"\r\n",
        "ui type=can-frame title=\"Motor CAN\" direction=rx id=0x18FF50E5 ext=true data=\"11 22 33 44 55 66 77 88\" channel=can0\r\n",
        "ui type=uart-frame title=\"UART RX\" direction=rx channel=UART1 baud=115200 data=\"AA 55 10 02 01 02 34\" fields=\"0-1|Sync|AA55|Preamble;2|Cmd|10|Command;3|Len|02|Payload length;4-5|Payload|0102|Data;6|CRC|34|Checksum\"\r\n",
        "ui type=packet-frame title=\"Binary Packet\" protocol=CUSTOM direction=tx data=\"7E A1 02 10 FF 55\" ascii=on\r\n",
    };
    for (const char *cmd : kTimberDemo) {
        queue_uart_bytes(cmd, std::strlen(cmd));
    }

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
        } else if (byte == ']') {
            osc_buffer_.clear();
            parser_state_ = ParserState::Osc;
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

    case ParserState::Osc:
        if (byte == 0x07 || byte == 0x1B) {
            if (byte == 0x1B) {
                // Ожидаем \ после ESC, но для простоты обрабатываем сразу
            }
            handle_osc(osc_buffer_);
            parser_state_ = ParserState::Normal;
            osc_buffer_.clear();
            return;
        }
        if (osc_buffer_.size() < 256) {
            osc_buffer_.push_back(static_cast<char>(byte));
        } else {
            parser_state_ = ParserState::Normal;
            osc_buffer_.clear();
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
    // Сначала пробуем распознать строку как команду виджета TimberWidget.
    // Если да — она превращается в виджет-элемент (или служебное действие),
    // и обычную текстовую строку не добавляем.
    if (try_consume_timber_line()) {
        return;
    }

    current_line_.sequence = next_sequence_++;
    lines_.push_back(std::move(current_line_));
    while (lines_.size() > static_cast<size_t>(clamp_positive(CONFIG_JC4880_TERMINAL_MAX_LINES, 5000))) {
        lines_.pop_front();
    }
    current_line_ = TerminalLine{};
}

void UartTerminalApp::add_widget_line(const WidgetDesc &widget)
{
    TerminalLine line;
    line.is_widget = true;
    line.widget = widget;
    line.sequence = next_sequence_++;
    lines_.push_back(std::move(line));
    while (lines_.size() > static_cast<size_t>(clamp_positive(CONFIG_JC4880_TERMINAL_MAX_LINES, 5000))) {
        lines_.pop_front();
    }
}

void UartTerminalApp::add_timber_widget(const timber::WidgetCommand &cmd)
{
    TerminalLine line;
    line.is_timber = true;
    line.timber_cmd = cmd;
    line.row_span = std::max(1, timber::measureWidgetRows(cmd, line_height_));
    line.sequence = next_sequence_++;
    lines_.push_back(std::move(line));
    while (lines_.size() > static_cast<size_t>(clamp_positive(CONFIG_JC4880_TERMINAL_MAX_LINES, 5000))) {
        lines_.pop_front();
    }
}

// Пытается распознать в накопленной строке команду TimberWidget (ui type=...,
// widget type=..., @N ..., clear-terminal, demo-widgets). Возвращает true,
// если строка была командой и обычную текстовую строку добавлять не нужно.
bool UartTerminalApp::try_consume_timber_line(void)
{
    // Собрать плоский текст текущей строки из ран.
    std::string text;
    for (const TextRun &run : current_line_.runs) {
        text += run.text;
    }
    if (text.empty()) {
        return false;
    }

    // Быстрый отсев: команды начинаются с @, "ui", "widget", "clear-terminal",
    // "demo". Иначе это обычный текст — не тратим время на полный парс.
    const char *p = text.c_str();
    while (*p == ' ') ++p;
    bool looksCmd = (*p == '@') ||
                    (std::strncmp(p, "ui ", 3) == 0) || (std::strcmp(p, "ui") == 0) ||
                    (std::strncmp(p, "widget ", 7) == 0) ||
                    (std::strncmp(p, "clear-terminal", 14) == 0) ||
                    (std::strncmp(p, "demo", 4) == 0) ||
                    (std::strcmp(p, "beep") == 0);
    if (!looksCmd) {
        return false;
    }

    timber::ParsedLine parsed = timber::parseLine(text);
    switch (parsed.kind) {
        case timber::LineKind::Widget:
            add_timber_widget(parsed.widget);
            current_line_ = TerminalLine{};
            return true;
        case timber::LineKind::ClearTerminal:
            clear_history(false);
            current_line_ = TerminalLine{};
            return true;
        case timber::LineKind::DemoWidgets:
            current_line_ = TerminalLine{};
            inject_demo_uart_data();
            return true;
        case timber::LineKind::Beep:
            // Звука нет — просто проглатываем служебную строку.
            current_line_ = TerminalLine{};
            return true;
        default:
            return false;  // Text/Invalid -> показать как обычную строку
    }
}

void UartTerminalApp::handle_osc(const std::string &osc)
{
    // Формат: WIDGET;KIND;value;max;color;label
    // Пример: WIDGET;PROGRESS;75;100;0x00FF00;CPU
    if (osc.rfind("WIDGET;", 0) != 0) {
        return;
    }

    std::vector<std::string> parts;
    size_t start = 7; // пропускаем "WIDGET;"
    while (start < osc.size()) {
        size_t end = osc.find(';', start);
        if (end == std::string::npos) {
            parts.push_back(osc.substr(start));
            break;
        }
        parts.push_back(osc.substr(start, end - start));
        start = end + 1;
    }

    if (parts.size() < 2) {
        return;
    }

    WidgetDesc w;
    if (parts[0] == "PROGRESS") {
        w.kind = WidgetDesc::Kind::ProgressBar;
    } else if (parts[0] == "STATUS") {
        w.kind = WidgetDesc::Kind::StatusBlock;
    } else {
        return;
    }

    if (parts.size() > 1) {
        w.value = std::clamp(std::atoi(parts[1].c_str()), 0, 100);
    }
    if (parts.size() > 2) {
        w.max_value = std::max(1, std::atoi(parts[2].c_str()));
    }
    if (parts.size() > 3) {
        w.bar_color = static_cast<uint32_t>(std::strtoul(parts[3].c_str(), nullptr, 0));
    }
    if (parts.size() > 4) {
        w.label = parts[4];
    }

    add_widget_line(w);
}

void UartTerminalApp::clear_history(bool reset_parser)
{
    lines_.clear();
    current_line_ = TerminalLine{};
    release_timber_views();   // удалить живые контейнеры виджетов
    if (reset_parser) {
        parser_state_ = ParserState::Normal;
        csi_buffer_.clear();
        osc_buffer_.clear();
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

int32_t UartTerminalApp::total_rows(void) const
{
    int32_t rows = 0;
    for (const TerminalLine &line : lines_) {
        rows += line.is_timber ? std::max(1, line.row_span) : 1;
    }
    if (current_line_.cells > 0) {
        rows += 1;  // незавершённая текстовая строка
    }
    return rows;
}

const UartTerminalApp::TerminalLine *
UartTerminalApp::element_at_row(int32_t row, int32_t *start_row) const
{
    if (row < 0) return nullptr;
    int32_t acc = 0;
    for (const TerminalLine &line : lines_) {
        const int32_t span = line.is_timber ? std::max(1, line.row_span) : 1;
        if (row < acc + span) {
            if (start_row) *start_row = acc;
            return &line;
        }
        acc += span;
    }
    if ((current_line_.cells > 0) && (row == acc)) {
        if (start_row) *start_row = acc;
        return &current_line_;
    }
    return nullptr;
}

int32_t UartTerminalApp::content_height(void) const
{
    const size_t rows = std::max<int32_t>(1, total_rows());
    const size_t pixels = rows * static_cast<size_t>(line_height_);
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
        if (row.canvas != nullptr) {
            if (row.draw_buf != nullptr) {
                lv_draw_buf_destroy(row.draw_buf);
                row.draw_buf = nullptr;
            }
            lv_obj_delete(row.canvas);
        }
    }
    row_pool_.clear();
    release_timber_views();   // виджеты пересоздадутся под новый размер

    lv_obj_update_layout(viewport_);
    const int32_t viewport_w = std::max<int32_t>(1, lv_obj_get_width(viewport_));
    const int32_t viewport_h = std::max<int32_t>(1, lv_obj_get_height(viewport_));
    const size_t row_count = static_cast<size_t>(viewport_h / line_height_) + 4;
    row_pool_.reserve(row_count);

    for (size_t i = 0; i < row_count; ++i) {
        RowView row;
        row.canvas = lv_canvas_create(viewport_);
        lv_obj_add_flag(row.canvas, LV_OBJ_FLAG_FLOATING);
        lv_obj_add_flag(row.canvas, LV_OBJ_FLAG_HIDDEN);
        row.draw_buf = lv_draw_buf_create(viewport_w, line_height_, LV_COLOR_FORMAT_RGB565, LV_STRIDE_AUTO);
        lv_canvas_set_draw_buf(row.canvas, row.draw_buf);
        row_pool_.push_back(row);
    }
}

void UartTerminalApp::refresh_visible_rows(void)
{
    if ((viewport_ == nullptr) || row_pool_.empty()) {
        return;
    }

    const int32_t scroll_y = std::max<int32_t>(0, lv_obj_get_scroll_y(viewport_));
    const int32_t first_row = scroll_y / line_height_;
    const int32_t offset_y = -(scroll_y % line_height_);

    // Каждый канвас-ряд пула отвечает за один абсолютный ряд экрана.
    // Для timber-виджетов канвас прячем — их рисует overlay (refresh_timber_widgets).
    for (size_t i = 0; i < row_pool_.size(); ++i) {
        const int32_t abs_row = first_row + static_cast<int32_t>(i);
        const int32_t y = offset_y + static_cast<int32_t>(i) * line_height_;
        render_line_to_row(row_pool_[i], static_cast<int>(abs_row), y);
    }

    refresh_timber_widgets();
}

// Создаёт/позиционирует живые контейнеры timber-виджетов поверх viewport.
// Каждый видимый timber-элемент получает свой контейнер высотой row_span строк.
void UartTerminalApp::refresh_timber_widgets(void)
{
    if (viewport_ == nullptr) {
        for (WidgetView &wv : widget_views_) {
            if (wv.container) lv_obj_add_flag(wv.container, LV_OBJ_FLAG_HIDDEN);
        }
        return;
    }

    const int32_t scroll_y = std::max<int32_t>(0, lv_obj_get_scroll_y(viewport_));
    const int32_t view_h = std::max<int32_t>(1, lv_obj_get_height(viewport_));
    const int32_t viewport_w = std::max<int32_t>(1, lv_obj_get_width(viewport_));
    const int32_t first_row = scroll_y / line_height_;
    const int32_t last_row = (scroll_y + view_h) / line_height_ + 1;

    // Собираем видимые timber-элементы и их позиции (в координатах контента).
    struct Visible { const TerminalLine *line; int element_index; int32_t start_row; };
    std::vector<Visible> visible;

    int32_t acc = 0;
    int element_index = 0;
    for (const TerminalLine &line : lines_) {
        const int32_t span = line.is_timber ? std::max(1, line.row_span) : 1;
        const int32_t end = acc + span;
        if (line.is_timber && end > first_row && acc < last_row) {
            visible.push_back({&line, element_index, acc});
        }
        acc += span;
        ++element_index;
    }

    // Прячем все контейнеры пула, затем переиспользуем под видимые виджеты.
    for (WidgetView &wv : widget_views_) {
        if (wv.container) lv_obj_add_flag(wv.container, LV_OBJ_FLAG_HIDDEN);
    }

    // Гарантируем достаточно контейнеров в пуле.
    while (widget_views_.size() < visible.size()) {
        WidgetView wv;
        wv.container = lv_obj_create(viewport_);
        lv_obj_remove_style_all(wv.container);
        lv_obj_add_flag(wv.container, LV_OBJ_FLAG_FLOATING);
        lv_obj_clear_flag(wv.container, LV_OBJ_FLAG_SCROLLABLE);
        // Контейнер виджета не должен перехватывать палец: и обычный скролл,
        // и постраничный клик по viewport_ должны работать поверх виджетов.
        lv_obj_clear_flag(wv.container, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_flag(wv.container, LV_OBJ_FLAG_EVENT_BUBBLE);
        lv_obj_add_flag(wv.container, LV_OBJ_FLAG_GESTURE_BUBBLE);
        lv_obj_set_style_pad_all(wv.container, 1, 0);
        lv_obj_add_flag(wv.container, LV_OBJ_FLAG_HIDDEN);
        widget_views_.push_back(wv);
    }

    for (size_t i = 0; i < visible.size(); ++i) {
        WidgetView &wv = widget_views_[i];
        const Visible &v = visible[i];
        const int32_t span = std::max(1, v.line->row_span);
        const int32_t h = span * line_height_;
        // Контейнеры — FLOATING, как и канвас-ряды: их координаты задаются
        // относительно вьюпорта и не сдвигаются скроллом автоматически.
        // Поэтому из абсолютной позиции элемента вычитаем scroll_y вручную.
        const int32_t y = v.start_row * line_height_ - scroll_y;

        lv_obj_set_size(wv.container, viewport_w, h);
        lv_obj_set_pos(wv.container, 0, y);
        lv_obj_clear_flag(wv.container, LV_OBJ_FLAG_HIDDEN);

        const bool same = (wv.element_index == v.element_index) &&
                          (wv.rendered_sequence == v.line->sequence);
        if (!same) {
            lv_obj_clean(wv.container);
            timber::renderWidget(wv.container, v.line->timber_cmd, line_height_);
            wv.element_index = v.element_index;
            wv.rendered_sequence = v.line->sequence;
        }
    }
}

void UartTerminalApp::release_timber_views(void)
{
    for (WidgetView &wv : widget_views_) {
        if (wv.container) {
            lv_obj_delete(wv.container);
        }
    }
    widget_views_.clear();
}

void UartTerminalApp::render_widget_to_row(RowView &row, const WidgetDesc &widget, int32_t viewport_w)
{
    lv_layer_t layer;
    lv_canvas_init_layer(row.canvas, &layer);

    const lv_font_t *font = font_for_index(font_index_);

    switch (widget.kind) {
    case WidgetDesc::Kind::ProgressBar: {
        const int32_t pad = 4;
        const int32_t bar_h = line_height_ - pad * 2;
        const int32_t bar_w = viewport_w - pad * 2 - 80;
        const int32_t fill_w = (bar_w * widget.value) / LV_MAX(1, widget.max_value);

        // Track background
        lv_area_t track = {pad, pad, pad + bar_w - 1, pad + bar_h - 1};
        lv_draw_fill_dsc_t track_dsc;
        lv_draw_fill_dsc_init(&track_dsc);
        track_dsc.color = lv_color_hex(widget.track_color);
        track_dsc.opa = LV_OPA_COVER;
        lv_draw_fill(&layer, &track_dsc, &track);

        // Fill
        if (fill_w > 0) {
            lv_area_t fill = {pad, pad, pad + fill_w - 1, pad + bar_h - 1};
            lv_draw_fill_dsc_t fill_dsc;
            lv_draw_fill_dsc_init(&fill_dsc);
            fill_dsc.color = lv_color_hex(widget.bar_color);
            fill_dsc.opa = LV_OPA_COVER;
            lv_draw_fill(&layer, &fill_dsc, &fill);
        }

        // Label
        char text[48];
        std::snprintf(text, sizeof(text), "%s %d%%",
                      widget.label.c_str(),
                      (widget.value * 100) / LV_MAX(1, widget.max_value));
        lv_draw_label_dsc_t lbl_dsc;
        lv_draw_label_dsc_init(&lbl_dsc);
        lbl_dsc.text = text;
        lbl_dsc.font = font;
        lbl_dsc.color = lv_color_hex(widget.text_color);
        lbl_dsc.opa = LV_OPA_COVER;
        lv_area_t lbl_area = {pad + bar_w + 8, 0, viewport_w - 1, line_height_ - 1};
        lv_draw_label(&layer, &lbl_dsc, &lbl_area);
        break;
    }

    case WidgetDesc::Kind::StatusBlock: {
        // Full-height colored block
        lv_area_t bg = {0, 0, viewport_w - 1, line_height_ - 1};
        lv_draw_fill_dsc_t bg_dsc;
        lv_draw_fill_dsc_init(&bg_dsc);
        bg_dsc.color = lv_color_hex(widget.bar_color);
        bg_dsc.opa = LV_OPA_COVER;
        lv_draw_fill(&layer, &bg_dsc, &bg);

        // Centered text
        lv_draw_label_dsc_t lbl_dsc;
        lv_draw_label_dsc_init(&lbl_dsc);
        lbl_dsc.text = widget.label.c_str();
        lbl_dsc.font = font;
        lbl_dsc.color = lv_color_hex(widget.text_color);
        lbl_dsc.opa = LV_OPA_COVER;
        lbl_dsc.align = LV_TEXT_ALIGN_CENTER;
        lv_area_t lbl_area = {0, 0, viewport_w - 1, line_height_ - 1};
        lv_draw_label(&layer, &lbl_dsc, &lbl_area);
        break;
    }
    }

    lv_canvas_finish_layer(row.canvas, &layer);
}

void UartTerminalApp::render_line_to_row(RowView &row, int line_index, int32_t y)
{
    if (row.canvas == nullptr) {
        return;
    }

    // line_index здесь — абсолютный ряд экрана. Находим элемент, который его
    // покрывает, с учётом многострочных timber-виджетов.
    int32_t start_row = 0;
    const TerminalLine *line = (line_index >= 0)
        ? element_at_row(static_cast<int32_t>(line_index), &start_row)
        : nullptr;
    if (line == nullptr) {
        lv_obj_add_flag(row.canvas, LV_OBJ_FLAG_HIDDEN);
        row.rendered_index = -1;
        row.rendered_sequence = 0;
        return;
    }

    // Ряды timber-виджета рисует overlay (refresh_timber_widgets) — канвас прячем.
    if (line->is_timber) {
        lv_obj_add_flag(row.canvas, LV_OBJ_FLAG_HIDDEN);
        row.rendered_index = -1;
        row.rendered_sequence = 0;
        return;
    }

    lv_obj_set_pos(row.canvas, 0, y);
    lv_obj_clear_flag(row.canvas, LV_OBJ_FLAG_HIDDEN);

    if ((row.rendered_index == line_index) && (row.rendered_sequence == line->sequence)) {
        return;
    }

    row.rendered_index = line_index;
    row.rendered_sequence = line->sequence;

    lv_canvas_fill_bg(row.canvas, lv_color_hex(kDefaultBg), LV_OPA_COVER);

    if (line->is_widget) {
        const int32_t viewport_w = lv_obj_get_width(viewport_);
        render_widget_to_row(row, line->widget, viewport_w);
        return;
    }

    lv_layer_t layer;
    lv_canvas_init_layer(row.canvas, &layer);

    const lv_font_t *font = font_for_index(font_index_);
    const int32_t font_h = lv_font_get_line_height(font);
    const int32_t ofs_y = (line_height_ - font_h) / 2;
    int32_t x = 6;

    for (const TextRun &run : line->runs) {
        if (run.text.empty()) {
            continue;
        }

        lv_point_t size;
        lv_text_get_size(&size, run.text.c_str(), font, 0, 0, INT32_MAX, LV_TEXT_FLAG_NONE);

        if (run.style.bg_enabled) {
            lv_draw_fill_dsc_t fill_dsc;
            lv_draw_fill_dsc_init(&fill_dsc);
            fill_dsc.color = lv_color_hex(run.style.bg);
            fill_dsc.opa = LV_OPA_COVER;
            lv_area_t bg_area;
            bg_area.x1 = x;
            bg_area.y1 = 0;
            bg_area.x2 = x + size.x - 1;
            bg_area.y2 = line_height_ - 1;
            lv_draw_fill(&layer, &fill_dsc, &bg_area);
        }

        lv_draw_label_dsc_t label_dsc;
        lv_draw_label_dsc_init(&label_dsc);
        label_dsc.text = run.text.c_str();
        label_dsc.font = font;
        label_dsc.color = lv_color_hex(run.style.fg);
        label_dsc.opa = LV_OPA_COVER;
        label_dsc.ofs_y = ofs_y;
        label_dsc.text_length = run.text.size();
        label_dsc.align = LV_TEXT_ALIGN_LEFT;

        lv_area_t txt_area;
        txt_area.x1 = x;
        txt_area.y1 = 0;
        txt_area.x2 = x + size.x - 1;
        txt_area.y2 = line_height_ - 1;
        lv_draw_label(&layer, &label_dsc, &txt_area);

        x += size.x;
    }

    lv_canvas_finish_layer(row.canvas, &layer);
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
        app->refresh_visible_rows();
        return;
    }

    if (code == LV_EVENT_SCROLL) {
        if (!app->suppress_scroll_event_) {
            const int32_t scroll_y = lv_obj_get_scroll_y(app->viewport_);
            const int32_t view_h = lv_obj_get_height(app->viewport_);
            int32_t max_scroll = app->content_height() - view_h;
            if (max_scroll < 0) {
                max_scroll = 0;
            }

            int32_t snapped = ((scroll_y + app->line_height_ / 2) / app->line_height_) * app->line_height_;
            if (snapped < 0) {
                snapped = 0;
            }
            if (snapped > max_scroll) {
                snapped = max_scroll;
            }

            if (snapped != scroll_y) {
                app->suppress_scroll_event_ = true;
                lv_obj_scroll_to_y(app->viewport_, snapped, LV_ANIM_OFF);
                app->suppress_scroll_event_ = false;
            }

            app->auto_follow_ = app->is_scroll_near_bottom();
            app->update_follow_button();
        }
        app->refresh_visible_rows();
    }
}

void UartTerminalApp::page_event_cb(lv_event_t *event)
{
    UartTerminalApp *app = static_cast<UartTerminalApp *>(lv_event_get_user_data(event));
    if (app == nullptr || app->viewport_ == nullptr) {
        return;
    }

    lv_indev_t *indev = lv_indev_get_act();
    if (indev == nullptr) {
        return;
    }

    lv_point_t vect;
    lv_indev_get_vect(indev, &vect);
    // Игнорируем событие, если было значительное движение (это скролл, а не клик).
    if (LV_ABS(vect.x) > 8 || LV_ABS(vect.y) > 8) {
        return;
    }

    lv_point_t click_pos;
    lv_indev_get_point(indev, &click_pos);

    lv_area_t vp_coords;
    lv_obj_get_coords(app->viewport_, &vp_coords);
    const int32_t center_y = (vp_coords.y1 + vp_coords.y2) / 2;
    const bool page_up = (click_pos.y < center_y);

    const int32_t view_h = lv_obj_get_height(app->viewport_);
    const int32_t page_lines = view_h / app->line_height_;
    const int32_t page_size = page_lines * app->line_height_;

    const int32_t total_h = app->content_height();
    const int32_t max_scroll = LV_MAX(0, total_h - view_h);

    int32_t scroll_y = lv_obj_get_scroll_y(app->viewport_);
    if (page_up) {
        scroll_y -= page_size;
    } else {
        scroll_y += page_size;
    }

    if (scroll_y < 0) {
        scroll_y = 0;
    }
    if (scroll_y > max_scroll) {
        scroll_y = max_scroll;
    }

    // Snap к границе строки.
    scroll_y = ((scroll_y + app->line_height_ / 2) / app->line_height_) * app->line_height_;
    if (scroll_y > max_scroll) {
        scroll_y = max_scroll;
    }

    app->suppress_scroll_event_ = true;
    lv_obj_scroll_to_y(app->viewport_, scroll_y, LV_ANIM_OFF);
    app->suppress_scroll_event_ = false;

    app->auto_follow_ = app->is_scroll_near_bottom();
    app->update_follow_button();
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

void UartTerminalApp::back_event_cb(lv_event_t *event)
{
    UartTerminalApp *app = static_cast<UartTerminalApp *>(lv_event_get_user_data(event));
    if (app != nullptr) {
        // Просим лаунчер закрыть приложение и вернуться на главный экран.
        app->request_close();
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

const lv_font_t *UartTerminalApp::font_for_index(size_t index)
{
    if (index < kTerminalFontCount) {
        return kTerminalFonts[index];
    }
    return &lv_font_montserrat_14;
}

int32_t UartTerminalApp::line_height_for_font(const lv_font_t *font)
{
    return lv_font_get_line_height(font) + 4;
}

void UartTerminalApp::apply_font_index(size_t index)
{
    if (index >= kTerminalFontCount) {
        return;
    }
    font_index_ = index;
    line_height_ = line_height_for_font(kTerminalFonts[index]);

    if (viewport_ != nullptr) {
        recreate_row_pool();
        update_content_height();
        if (auto_follow_) {
            scroll_to_bottom();
        }
        refresh_visible_rows();
    }
}

void UartTerminalApp::gesture_event_cb(lv_event_t *event)
{
    UartTerminalApp *app = static_cast<UartTerminalApp *>(lv_event_get_user_data(event));
    if (app == nullptr) {
        return;
    }

    lv_indev_gesture_type_t type = lv_event_get_gesture_type(event);
    ESP_LOGI("term_gesture", "gesture type=%d", (int)type);

    if (type != LV_INDEV_GESTURE_PINCH && type != LV_INDEV_GESTURE_ROTATE) {
        return;
    }

    lv_indev_gesture_state_t state = lv_event_get_gesture_state(event, LV_INDEV_GESTURE_PINCH);
    ESP_LOGI("term_gesture", "pinch state=%d", (int)state);

    if (state != LV_INDEV_GESTURE_STATE_ENDED) {
        return;
    }

    float scale = lv_event_get_pinch_scale(event);
    ESP_LOGI("term_gesture", "pinch scale=%.3f", scale);

    if (scale > 1.2f) {
        if (app->font_index_ + 1 < kTerminalFontCount) {
            app->apply_font_index(app->font_index_ + 1);
        }
    } else if (scale < 0.8f) {
        if (app->font_index_ > 0) {
            app->apply_font_index(app->font_index_ - 1);
        }
    }
}
