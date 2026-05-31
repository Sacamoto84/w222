#include "terminal/TerminalApp.hpp"

#include <algorithm>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <fcntl.h>

#include "driver/uart.h"
#include "esp_err.h"
#include "esp_log.h"

#include "lwip/netdb.h"
#include "lwip/sockets.h"

#include "net_config.hpp"
#include "widgets/WidgetParser.hpp"
#include "widgets/WidgetRender.hpp"

#include "src/indev/lv_indev_private.h"
#include "src/indev/lv_indev_gesture_private.h"
#include "src/draw/snapshot/lv_snapshot.h"

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

// Heartbeat (UDP ping/pong) — совместимо с ESP32-C3 и андроид-референсом.
static constexpr uint32_t kHeartbeatTimeoutMs = 3000;   // нет pong дольше → связь потеряна
static constexpr uint32_t kHeartbeatIntervalMs = 700;   // период отправки ping
static constexpr const char *kHbPingPrefix = "tm3 hb ping";
static constexpr const char *kHbPongPrefix = "tm3 hb pong";

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
    lv_obj_set_size(button, 48, 42);   // высота уменьшена примерно на треть (64 -> 42)
    lv_obj_set_style_radius(button, 8, 0);
    lv_obj_set_style_border_width(button, 0, 0);
    lv_obj_set_style_bg_color(button, lv_color_hex(kButtonBg), 0);
    lv_obj_set_style_bg_opa(button, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(button, lv_color_hex(0x364253), LV_STATE_PRESSED);

    lv_obj_t *label = lv_label_create(button);
    lv_label_set_text(label, symbol);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_center(label);
    return button;
}

// Узкая кнопка-переключатель канала ("All" / "0".."3").
static lv_obj_t *create_channel_button(lv_obj_t *parent, const char *text)
{
    lv_obj_t *button = lv_button_create(parent);
    lv_obj_set_size(button, 44, 42);
    lv_obj_set_style_radius(button, 8, 0);
    lv_obj_set_style_border_width(button, 0, 0);
    lv_obj_set_style_bg_color(button, lv_color_hex(kButtonBg), 0);
    lv_obj_set_style_bg_opa(button, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(button, lv_color_hex(0x364253), LV_STATE_PRESSED);

    lv_obj_t *label = lv_label_create(button);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_center(label);
    return button;
}

static uart_parity_t to_uart_parity(uartcfg::Parity parity)
{
    switch (parity) {
    case uartcfg::Parity::Even: return UART_PARITY_EVEN;
    case uartcfg::Parity::Odd:  return UART_PARITY_ODD;
    case uartcfg::Parity::None:
    default:                    return UART_PARITY_DISABLE;
    }
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
    if (rx_mutex_ == nullptr) {
        rx_mutex_ = xSemaphoreCreateMutex();
    }
    // Встроенный UART отключён — терминал работает полностью от сети.
    // Поток приёма создаём здесь (раньше его создавал init_uart()).
    if (rx_stream_ == nullptr) {
        rx_stream_ = xStreamBufferCreate(CONFIG_JC4880_TERMINAL_STREAM_BUFFER_SIZE, 1);
    }
    init_network();
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
    lv_obj_set_height(toolbar, 56);   // под кнопки высотой 42 (раньше 84 под кнопки 64)
    lv_obj_set_style_bg_color(toolbar, lv_color_hex(kToolbarBg), 0);
    lv_obj_set_style_bg_opa(toolbar, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_left(toolbar, 16, 0);
    lv_obj_set_style_pad_right(toolbar, 16, 0);
    lv_obj_set_style_pad_column(toolbar, 12, 0);
    lv_obj_set_flex_flow(toolbar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(toolbar, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    // Кнопок больше, чем влезает по ширине — делаем тулбар прокручиваемым по
    // горизонтали (без вертикальной прокрутки и без инерции).
    lv_obj_set_scroll_dir(toolbar, LV_DIR_HOR);
    lv_obj_set_scrollbar_mode(toolbar, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(toolbar, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(toolbar, LV_OBJ_FLAG_SCROLL_MOMENTUM);

    // Светодиод состояния связи — первым в ряду (зелёный/жёлтый/красный).
    net_dot_ = lv_obj_create(toolbar);
    lv_obj_remove_style_all(net_dot_);
    lv_obj_set_size(net_dot_, 16, 16);
    lv_obj_set_style_radius(net_dot_, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_opa(net_dot_, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(net_dot_, lv_color_hex(0x555B66), 0);
    lv_obj_clear_flag(net_dot_, LV_OBJ_FLAG_CLICKABLE);

    // Сразу за светодиодом — счётчик строк (только число).
    status_label_ = lv_label_create(toolbar);
    lv_label_set_long_mode(status_label_, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_text_font(status_label_, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(status_label_, lv_color_hex(0xAFC2D2), 0);

    lv_obj_t *back_button = create_toolbar_button(toolbar, LV_SYMBOL_LEFT);
    lv_obj_add_event_cb(back_button, back_event_cb, LV_EVENT_CLICKED, this);

    // Переключатель каналов: All / 0 / 1 / 2 / 3. Канал кодируется в user_data
    // кнопки (All = -1). Активный канал подсвечивается в update_channel_buttons().
    static const char *kChannelLabels[5] = {"All", "0", "1", "2", "3"};
    for (int i = 0; i < 5; ++i) {
        lv_obj_t *button = create_channel_button(toolbar, kChannelLabels[i]);
        const int channel = (i == 0) ? -1 : (i - 1);
        lv_obj_set_user_data(button, reinterpret_cast<void *>(static_cast<intptr_t>(channel)));
        lv_obj_add_event_cb(button, channel_event_cb, LV_EVENT_CLICKED, this);
        channel_buttons_[i] = button;
    }

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

    // Высота viewport должна быть кратна базовой ячейке сетки (kGridCellPx):
    // лишние пиксели снизу отдаём toolbar, чтобы низ списка попадал ровно на сетку.
    const int32_t viewport_h = lv_obj_get_height(viewport_);
    const int32_t remainder = viewport_h % kGridCellPx;
    if (remainder != 0) {
        const int32_t toolbar_h = lv_obj_get_height(toolbar);
        lv_obj_set_height(toolbar, toolbar_h + remainder);   // забрать остаток у viewport
        lv_obj_update_layout(root_);
    }

    rebuild_tile_pool();
    update_content_height();
    update_status_label(true);
    update_follow_button();
    update_channel_buttons();
    scroll_to_bottom();
    refresh_visible_elements();

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

    free_all_tiles();

    poll_timer_ = nullptr;
    root_ = nullptr;
    status_label_ = nullptr;
    follow_button_ = nullptr;
    for (lv_obj_t *&btn : channel_buttons_) {
        btn = nullptr;
    }
    net_dot_ = nullptr;
    viewport_ = nullptr;
    spacer_ = nullptr;
}

bool UartTerminalApp::init_uart(void)
{
    if (uart_installed_) {
        return true;
    }

    // Конфигурация берётся из NVS (или значений по умолчанию из Kconfig).
    uart_cfg_ = uartcfg::load();

    if (rx_stream_ == nullptr) {
        rx_stream_ = xStreamBufferCreate(CONFIG_JC4880_TERMINAL_STREAM_BUFFER_SIZE, 1);
        if (rx_stream_ == nullptr) {
            std::snprintf(uart_status_, sizeof(uart_status_), "UART: no stream buffer memory");
            ESP_LOGE(TAG, "%s", uart_status_);
            return false;
        }
    }

    if (uart_cfg_.rx_pin < 0) {
        std::snprintf(uart_status_, sizeof(uart_status_), "UART: RX pin is not configured");
        ESP_LOGW(TAG, "%s", uart_status_);
        return false;
    }

    const uart_port_t port = static_cast<uart_port_t>(uart_cfg_.port);
    uart_config_t uart_config = {};
    uart_config.baud_rate = uart_cfg_.baud;
    uart_config.data_bits = UART_DATA_8_BITS;
    uart_config.parity = to_uart_parity(uart_cfg_.parity);
    uart_config.stop_bits = UART_STOP_BITS_1;
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    uart_config.source_clk = UART_SCLK_DEFAULT;

    esp_err_t err = uart_driver_install(port, 4096, 0, 0, nullptr, 0);
    if ((err != ESP_OK) && (err != ESP_ERR_INVALID_STATE)) {
        std::snprintf(uart_status_, sizeof(uart_status_), "UART%d driver: %s", uart_cfg_.port, esp_err_to_name(err));
        ESP_LOGE(TAG, "%s", uart_status_);
        return false;
    }

    err = uart_param_config(port, &uart_config);
    if (err != ESP_OK) {
        std::snprintf(uart_status_, sizeof(uart_status_), "UART%d config: %s", uart_cfg_.port, esp_err_to_name(err));
        ESP_LOGE(TAG, "%s", uart_status_);
        return false;
    }

    err = uart_set_pin(port,
                       uart_cfg_.tx_pin >= 0 ? uart_cfg_.tx_pin : UART_PIN_NO_CHANGE,
                       uart_cfg_.rx_pin >= 0 ? uart_cfg_.rx_pin : UART_PIN_NO_CHANGE,
                       UART_PIN_NO_CHANGE,
                       UART_PIN_NO_CHANGE);
    if (err != ESP_OK) {
        std::snprintf(uart_status_, sizeof(uart_status_), "UART%d pins: %s", uart_cfg_.port, esp_err_to_name(err));
        ESP_LOGE(TAG, "%s", uart_status_);
        return false;
    }

    uart_flush_input(port);
    uart_installed_ = true;
    std::snprintf(uart_status_, sizeof(uart_status_), "UART%d RX GPIO%d @ %d",
                  uart_cfg_.port, uart_cfg_.rx_pin, uart_cfg_.baud);

    if (!uart_task_started_) {
        BaseType_t task_ret = xTaskCreatePinnedToCore(uart_task_entry,
                                                      "uart_terminal_rx",
                                                      4096,
                                                      this,
                                                      5,
                                                      &uart_task_,
                                                      1);
        if (task_ret != pdPASS) {
            std::snprintf(uart_status_, sizeof(uart_status_), "UART%d task: no memory", uart_cfg_.port);
            ESP_LOGE(TAG, "%s", uart_status_);
            return false;
        }
        uart_task_started_ = true;
    }

    ESP_LOGI(TAG, "%s", uart_status_);
    return true;
}

// Применяет изменённую (через приложение "Настройки UART") конфигурацию к живому
// драйверу. Вызывается ТОЛЬКО из uart_task(), поэтому безопасно трогает драйвер,
// который эта же задача читает в uart_read_bytes.
void UartTerminalApp::reconfigure_uart(void)
{
    const uartcfg::Config old_cfg = uart_cfg_;
    const uartcfg::Config cfg = uartcfg::load();
    uart_cfg_ = cfg;

    // Смена номера порта: переустанавливаем драйвер на новый контроллер.
    if (uart_installed_ && (cfg.port != old_cfg.port)) {
        uart_driver_delete(static_cast<uart_port_t>(old_cfg.port));
        uart_installed_ = false;
    }

    const uart_port_t port = static_cast<uart_port_t>(cfg.port);

    if (cfg.rx_pin < 0) {
        if (uart_installed_) {
            uart_driver_delete(port);
            uart_installed_ = false;
        }
        std::snprintf(uart_status_, sizeof(uart_status_), "UART: RX pin is not configured");
        ESP_LOGW(TAG, "%s", uart_status_);
        return;
    }

    uart_config_t uart_config = {};
    uart_config.baud_rate = cfg.baud;
    uart_config.data_bits = UART_DATA_8_BITS;
    uart_config.parity = to_uart_parity(cfg.parity);
    uart_config.stop_bits = UART_STOP_BITS_1;
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    uart_config.source_clk = UART_SCLK_DEFAULT;

    if (!uart_installed_) {
        esp_err_t err = uart_driver_install(port, 4096, 0, 0, nullptr, 0);
        if ((err != ESP_OK) && (err != ESP_ERR_INVALID_STATE)) {
            std::snprintf(uart_status_, sizeof(uart_status_), "UART%d driver: %s", cfg.port, esp_err_to_name(err));
            ESP_LOGE(TAG, "%s", uart_status_);
            return;
        }
        uart_installed_ = true;
    }

    uart_param_config(port, &uart_config);
    uart_set_pin(port,
                 cfg.tx_pin >= 0 ? cfg.tx_pin : UART_PIN_NO_CHANGE,
                 cfg.rx_pin >= 0 ? cfg.rx_pin : UART_PIN_NO_CHANGE,
                 UART_PIN_NO_CHANGE,
                 UART_PIN_NO_CHANGE);
    uart_flush_input(port);

    std::snprintf(uart_status_, sizeof(uart_status_), "UART%d RX GPIO%d @ %d",
                  cfg.port, cfg.rx_pin, cfg.baud);
    ESP_LOGI(TAG, "reconfigured: %s", uart_status_);
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

    for (;;) {
        // Применяем изменения конфигурации, сделанные в приложении "Настройки
        // UART". Делаем это в своей задаче, чтобы не было гонки с драйвером.
        if (uartcfg::consume_dirty()) {
            reconfigure_uart();
        }

        if (!uart_installed_) {
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        const uart_port_t port = static_cast<uart_port_t>(uart_cfg_.port);
        const int read_len = uart_read_bytes(port, buffer, sizeof(buffer), pdMS_TO_TICKS(50));
        if (read_len <= 0) {
            continue;
        }

        enqueue_rx(reinterpret_cast<const char *>(buffer), static_cast<size_t>(read_len));
    }
}

// Низкоуровневая отправка в rx_stream_ под мьютексом (без учёта счётчиков).
// Несколько писателей (UART/TCP/UDP) — StreamBuffer допускает одного писателя,
// поэтому сериализуем их мьютексом. Читатель один (LVGL poll), ему мьютекс не нужен.
size_t UartTerminalApp::rx_send_locked(const char *data, size_t len)
{
    if ((data == nullptr) || (len == 0) || (rx_stream_ == nullptr)) {
        return 0;
    }
    if (rx_mutex_ != nullptr) {
        xSemaphoreTake(rx_mutex_, portMAX_DELAY);
    }
    const size_t sent = xStreamBufferSend(rx_stream_, data, len, 0);
    if (rx_mutex_ != nullptr) {
        xSemaphoreGive(rx_mutex_);
    }
    return sent;
}

// Best-effort запись из источника (UART/TCP/UDP-задач): что не влезло — теряем.
// Дренаж потока делает только LVGL-поток (poll-таймер), поэтому здесь его не зовём.
void UartTerminalApp::enqueue_rx(const char *data, size_t len)
{
    if ((data == nullptr) || (len == 0)) {
        return;
    }
    received_bytes_ += static_cast<uint32_t>(len);
    const size_t sent = rx_send_locked(data, len);
    if (sent < len) {
        dropped_bytes_ += static_cast<uint32_t>(len - sent);
    }
}

void UartTerminalApp::init_network(void)
{
    if (net_started_) {
        return;
    }
    net_cfg_ = netcfg::load();
    netcfg::set_tcp_state(net_cfg_.tcp_enabled ? netcfg::TcpState::Idle
                                               : netcfg::TcpState::Disabled);

    // Долгоживущие задачи: каждая сама проверяет свой флаг включения и версию
    // конфигурации, поэтому пересоздавать их при смене настроек не нужно.
    xTaskCreatePinnedToCore(tcp_task_entry, "term_tcp_rx", 4096, this, 5, &tcp_task_, 1);
    xTaskCreatePinnedToCore(udp_task_entry, "term_udp_rx", 4096, this, 5, &udp_task_, 1);
    xTaskCreatePinnedToCore(hb_task_entry, "term_hb", 4096, this, 5, &hb_task_, 1);
    net_started_ = true;
}

void UartTerminalApp::tcp_task_entry(void *arg)
{
    UartTerminalApp *app = static_cast<UartTerminalApp *>(arg);
    if (app != nullptr) {
        app->tcp_task();
    }
    vTaskDelete(nullptr);
}

void UartTerminalApp::udp_task_entry(void *arg)
{
    UartTerminalApp *app = static_cast<UartTerminalApp *>(arg);
    if (app != nullptr) {
        app->udp_task();
    }
    vTaskDelete(nullptr);
}

void UartTerminalApp::hb_task_entry(void *arg)
{
    UartTerminalApp *app = static_cast<UartTerminalApp *>(arg);
    if (app != nullptr) {
        app->hb_task();
    }
    vTaskDelete(nullptr);
}

// TCP-клиент: подключается к host:port (как андроид-референс), читает сырой
// поток и пишет его в общий rx_stream_. Переподключается при разрыве и при
// изменении настроек (по netcfg::version()).
void UartTerminalApp::tcp_task(void)
{
    char buffer[512];
    int sock = -1;
    unsigned cfg_ver = netcfg::version();

    auto close_sock = [&](void) {
        if (sock >= 0) {
            lwip_close(sock);
            sock = -1;
        }
    };

    for (;;) {
        // Применяем изменения настроек: рвём соединение и перечитываем конфиг.
        const unsigned ver = netcfg::version();
        if (ver != cfg_ver) {
            cfg_ver = ver;
            net_cfg_ = netcfg::load();
            close_sock();
        }

        if (!net_cfg_.tcp_enabled || (net_cfg_.host[0] == '\0')) {
            close_sock();
            netcfg::set_tcp_state(net_cfg_.tcp_enabled ? netcfg::TcpState::Idle
                                                       : netcfg::TcpState::Disabled);
            vTaskDelay(pdMS_TO_TICKS(500));
            continue;
        }

        if (sock < 0) {
            netcfg::set_tcp_state(netcfg::TcpState::Connecting);

            struct addrinfo hints = {};
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_STREAM;
            char port_str[8];
            std::snprintf(port_str, sizeof(port_str), "%d", net_cfg_.tcp_port);

            struct addrinfo *res = nullptr;
            if ((getaddrinfo(net_cfg_.host, port_str, &hints, &res) != 0) || (res == nullptr)) {
                if (res != nullptr) {
                    freeaddrinfo(res);
                }
                netcfg::set_tcp_state(netcfg::TcpState::Error);
                vTaskDelay(pdMS_TO_TICKS(1500));
                continue;
            }

            int s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
            if (s < 0) {
                freeaddrinfo(res);
                netcfg::set_tcp_state(netcfg::TcpState::Error);
                vTaskDelay(pdMS_TO_TICKS(1500));
                continue;
            }

            // Неблокирующий connect с таймаутом ~1.5 c (как в андроид-референсе),
            // чтобы недоступный сервер не вешал задачу надолго.
            fcntl(s, F_SETFL, O_NONBLOCK);
            int cr = connect(s, res->ai_addr, res->ai_addrlen);
            bool ok = (cr == 0);
            if (!ok && (errno == EINPROGRESS)) {
                fd_set wset;
                FD_ZERO(&wset);
                FD_SET(s, &wset);
                struct timeval tv;
                tv.tv_sec = 1;
                tv.tv_usec = 500000;
                if (select(s + 1, nullptr, &wset, nullptr, &tv) > 0) {
                    int soerr = 0;
                    socklen_t slen = sizeof(soerr);
                    getsockopt(s, SOL_SOCKET, SO_ERROR, &soerr, &slen);
                    ok = (soerr == 0);
                }
            }
            freeaddrinfo(res);

            if (!ok) {
                lwip_close(s);
                netcfg::set_tcp_state(netcfg::TcpState::Error);
                vTaskDelay(pdMS_TO_TICKS(1500));
                continue;
            }

            // Возвращаем блокирующий режим + таймаут чтения (чтобы периодически
            // проверять флаг включения/версию конфигурации).
            fcntl(s, F_SETFL, 0);
            int one = 1;
            setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, &one, sizeof(one));
            struct timeval rtv;
            rtv.tv_sec = 1;
            rtv.tv_usec = 0;
            setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &rtv, sizeof(rtv));

            sock = s;
            // Праймим "признак жизни" и сбрасываем флаг heartbeat для нового
            // соединения (pong ещё не приходил).
            last_pong_ms_ = esp_log_timestamp();
            hb_seen_ = false;
            netcfg::set_tcp_state(netcfg::TcpState::Connected);
            ESP_LOGI(TAG, "TCP connected to %s:%d", net_cfg_.host, net_cfg_.tcp_port);
        }

        const int r = recv(sock, buffer, sizeof(buffer), 0);
        if (r > 0) {
            // Данные = сервер жив (даже если heartbeat не поддерживается).
            last_pong_ms_ = esp_log_timestamp();
            enqueue_rx(buffer, static_cast<size_t>(r));
        } else if (r == 0) {
            ESP_LOGW(TAG, "TCP peer closed");
            close_sock();
            netcfg::set_tcp_state(netcfg::TcpState::Error);
            vTaskDelay(pdMS_TO_TICKS(800));
        } else {
            // r < 0: таймаут чтения — норма. Но если давно нет heartbeat-pong,
            // считаем, что сервер пропал (например, выключили питание, FIN не
            // пришёл): рвём сокет и уходим в реконнект (он сам поднимется, когда
            // сервер вернётся в сеть).
            if ((errno == EWOULDBLOCK) || (errno == EAGAIN) || (errno == 0)) {
                // Сервер считаем пропавшим только если heartbeat уже работал на
                // этом соединении, а затем pong'и пропали (питание сняли, FIN не
                // пришёл). Сервер без heartbeat живое соединение не теряет.
                if (hb_seen_ && ((esp_log_timestamp() - last_pong_ms_) > kHeartbeatTimeoutMs)) {
                    ESP_LOGW(TAG, "heartbeat stale — assuming server gone, reconnecting");
                    close_sock();
                    netcfg::set_tcp_state(netcfg::TcpState::Error);
                    vTaskDelay(pdMS_TO_TICKS(500));
                }
                continue;
            }
            ESP_LOGW(TAG, "TCP recv error: %d", errno);
            close_sock();
            netcfg::set_tcp_state(netcfg::TcpState::Error);
            vTaskDelay(pdMS_TO_TICKS(800));
        }
    }
}

// UDP heartbeat: периодически шлёт `tm3 hb ping <ts>` на host:tcp_port и ждёт
// `tm3 hb pong`. Обновляет last_pong_ms_/RTT. Совместимо с ESP32-C3
// (handleHeartbeatUdp) и андроид-референсом. Позволяет обнаружить пропажу
// сервера даже при "висящем" TCP-сокете.
void UartTerminalApp::hb_task(void)
{
    int sock = -1;
    unsigned cfg_ver = static_cast<unsigned>(-1);
    netcfg::Config cfg = netcfg::defaults();
    struct sockaddr_in dest = {};
    bool dest_ok = false;

    auto close_sock = [&](void) {
        if (sock >= 0) {
            lwip_close(sock);
            sock = -1;
        }
    };

    for (;;) {
        const unsigned ver = netcfg::version();
        if (ver != cfg_ver) {
            cfg_ver = ver;
            cfg = netcfg::load();
            close_sock();
            dest_ok = false;
        }

        if (!cfg.tcp_enabled || (cfg.host[0] == '\0')) {
            close_sock();
            vTaskDelay(pdMS_TO_TICKS(500));
            continue;
        }

        if (sock < 0) {
            sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            if (sock < 0) {
                vTaskDelay(pdMS_TO_TICKS(1000));
                continue;
            }
            struct timeval rtv;
            rtv.tv_sec = 0;
            rtv.tv_usec = 400000;   // 400 мс на ожидание pong
            setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &rtv, sizeof(rtv));

            // Резолвим адрес сервера (heartbeat-порт = tcp_port, как у C3).
            dest_ok = false;
            struct addrinfo hints = {};
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_DGRAM;
            char port_str[8];
            std::snprintf(port_str, sizeof(port_str), "%d", cfg.tcp_port);
            struct addrinfo *res = nullptr;
            if ((getaddrinfo(cfg.host, port_str, &hints, &res) == 0) && (res != nullptr)) {
                dest = *reinterpret_cast<struct sockaddr_in *>(res->ai_addr);
                dest_ok = true;
            }
            if (res != nullptr) {
                freeaddrinfo(res);
            }
        }

        if (!dest_ok) {
            close_sock();
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        // Отправляем ping с меткой времени, чтобы посчитать RTT по эху.
        const uint32_t t0 = esp_log_timestamp();
        char ping[48];
        const int plen = std::snprintf(ping, sizeof(ping), "%s %lu",
                                       kHbPingPrefix, static_cast<unsigned long>(t0));
        sendto(sock, ping, static_cast<size_t>(plen), 0,
               reinterpret_cast<struct sockaddr *>(&dest), sizeof(dest));

        // Ждём pong (несколько датаграмм в пределах таймаута сокета).
        char rx[64];
        const int n = recv(sock, rx, sizeof(rx) - 1, 0);
        if (n > 0) {
            rx[n] = '\0';
            if (std::strncmp(rx, kHbPongPrefix, std::strlen(kHbPongPrefix)) == 0) {
                last_pong_ms_ = esp_log_timestamp();
                last_hb_rtt_ms_ = static_cast<int32_t>(last_pong_ms_ - t0);
                hb_seen_ = true;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(kHeartbeatIntervalMs));
    }
}

// UDP-слушатель (опционально): принимает датаграммы текста на udp_port и пишет
// их в общий rx_stream_. Полезно для broadcast-логов.
void UartTerminalApp::udp_task(void)
{
    char buffer[1500];
    int sock = -1;
    int bound_port = -1;
    bool enabled = false;
    unsigned cfg_ver = static_cast<unsigned>(-1);   // форсируем загрузку на старте

    auto close_sock = [&](void) {
        if (sock >= 0) {
            lwip_close(sock);
            sock = -1;
        }
        bound_port = -1;
    };

    for (;;) {
        const unsigned ver = netcfg::version();
        if (ver != cfg_ver) {
            cfg_ver = ver;
            const netcfg::Config cfg = netcfg::load();
            enabled = cfg.udp_enabled;
            if (!enabled || (cfg.udp_port != bound_port)) {
                close_sock();
            }
            if (enabled && (sock < 0)) {
                int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                if (s >= 0) {
                    struct sockaddr_in addr = {};
                    addr.sin_family = AF_INET;
                    addr.sin_addr.s_addr = htonl(INADDR_ANY);
                    addr.sin_port = htons(static_cast<uint16_t>(cfg.udp_port));
                    if (bind(s, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) == 0) {
                        struct timeval rtv;
                        rtv.tv_sec = 1;
                        rtv.tv_usec = 0;
                        setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &rtv, sizeof(rtv));
                        sock = s;
                        bound_port = cfg.udp_port;
                        ESP_LOGI(TAG, "UDP listening on %d", cfg.udp_port);
                    } else {
                        lwip_close(s);
                    }
                }
            }
        }

        if (!enabled || (sock < 0)) {
            vTaskDelay(pdMS_TO_TICKS(500));
            continue;
        }

        const int r = recv(sock, buffer, sizeof(buffer), 0);
        if (r > 0) {
            enqueue_rx(buffer, static_cast<size_t>(r));
        } else {
            // таймаут/ошибка — просто продолжаем (проверим версию конфига)
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}

void UartTerminalApp::queue_uart_bytes(const char *data, size_t len)
{
    if ((data == nullptr) || (len == 0) || (rx_stream_ == nullptr)) {
        return;
    }

    received_bytes_ += static_cast<uint32_t>(len);

    // Вызывается из LVGL-потока (демо-инъекция), поэтому при заполнении потока
    // можно безопасно дренировать его прямо здесь и повторить отправку.
    size_t offset = 0;
    while (offset < len) {
        const size_t sent = rx_send_locked(data + offset, len - offset);
        if (sent > 0) {
            offset += sent;
            continue;
        }

        drain_uart_stream();
        const size_t retry_sent = rx_send_locked(data + offset, len - offset);
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
        refresh_visible_elements();
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

    // Транспортный префикс @N задаёт канал и удаляется из видимого текста.
    current_line_.channel = extract_text_channel(current_line_);
    current_line_.sequence = next_sequence_++;
    push_committed_line(std::move(current_line_));
    current_line_ = TerminalLine{};
}

void UartTerminalApp::add_widget_line(const WidgetDesc &widget)
{
    TerminalLine line;
    line.is_widget = true;
    line.widget = widget;
    line.sequence = next_sequence_++;
    push_committed_line(std::move(line));
}

void UartTerminalApp::add_timber_widget(const timber::WidgetCommand &cmd)
{
    TerminalLine line;
    line.is_timber = true;
    line.timber_cmd = cmd;
    line.channel = static_cast<int8_t>(cmd.channel);
    line.row_span = std::max(1, timber::measureWidgetRows(cmd, kGridCellPx));
    line.sequence = next_sequence_++;
    push_committed_line(std::move(line));
}

void UartTerminalApp::push_committed_line(TerminalLine &&line)
{
    committed_rows_ += element_span(line);
    lines_.push_back(std::move(line));

    const size_t limit = static_cast<size_t>(clamp_positive(CONFIG_JC4880_TERMINAL_MAX_LINES, 5000));
    bool popped = false;
    while (lines_.size() > limit) {
        committed_rows_ -= element_span(lines_.front());
        lines_.pop_front();
        popped = true;
    }
    if (popped) {
        // Индексы сдвинулись — курсор-кэш больше не валиден.
        locate_cursor_index_ = 0;
        locate_cursor_start_ = 0;
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
    committed_rows_ = 0;          // сброс инкрементального счётчика рядов
    locate_cursor_index_ = 0;
    locate_cursor_start_ = 0;
    current_line_ = TerminalLine{};
    // Тайлы не уничтожаем — пустое окно при следующем refresh само отвяжет и
    // спрячет все тайлы (их картинки переиспользуются под новый контент).
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
    refresh_visible_elements();
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

void UartTerminalApp::recompute_committed_rows(void)
{
    int32_t rows = 0;
    for (const TerminalLine &line : lines_) {
        rows += element_span(line);
    }
    committed_rows_ = rows;
    // Курсор мог стать невалидным — сбрасываем на начало.
    locate_cursor_index_ = 0;
    locate_cursor_start_ = 0;
}

int32_t UartTerminalApp::total_rows(void) const
{
    // O(1): committed_rows_ поддерживается инкрементально. Живой хвост
    // (current_line_) занимает text_zoom_span_ ячеек.
    return committed_rows_ + (current_line_.cells > 0 ? text_zoom_span_ : 0);
}

size_t UartTerminalApp::locate_row(int32_t row, int32_t *start_row) const
{
    const size_t n = lines_.size();
    if (row < 0 || n == 0) {
        if (start_row) *start_row = 0;
        return n;
    }

    // Стартуем с закэшированного курсора, если он ещё валиден.
    size_t idx = locate_cursor_index_;
    int32_t acc = locate_cursor_start_;
    if (idx >= n) { idx = 0; acc = 0; }

    // Если искомый ряд левее курсора — шагаем назад.
    while (idx > 0 && acc > row) {
        --idx;
        acc -= element_span(lines_[idx]);
    }
    // Шагаем вперёд, пока ряд не попадёт в текущий элемент.
    while (idx < n) {
        const int32_t span = element_span(lines_[idx]);
        if (row < acc + span) {
            locate_cursor_index_ = idx;
            locate_cursor_start_ = acc;
            if (start_row) *start_row = acc;
            return idx;
        }
        acc += span;
        ++idx;
    }

    // Ряд за пределами lines_ — относится к current_line_ или к пустоте.
    if (start_row) *start_row = acc;
    return n;
}

const UartTerminalApp::TerminalLine *
UartTerminalApp::element_at_row(int32_t row, int32_t *start_row) const
{
    int32_t start = 0;
    const size_t idx = locate_row(row, &start);
    if (idx < lines_.size()) {
        if (start_row) *start_row = start;
        return &lines_[idx];
    }
    if ((current_line_.cells > 0) && (row == start)) {
        if (start_row) *start_row = start;
        return &current_line_;
    }
    return nullptr;
}

int32_t UartTerminalApp::content_height(void) const
{
    const size_t rows = std::max<int32_t>(1, total_rows());
    const size_t pixels = rows * static_cast<size_t>(kGridCellPx);
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
    return (total_h <= view_h) || ((scroll_y + view_h) >= (total_h - (kGridCellPx * 2)));
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

void UartTerminalApp::rebuild_tile_pool(void)
{
    if (viewport_ == nullptr) {
        return;
    }

    free_all_tiles();

    lv_obj_update_layout(viewport_);
    const int32_t viewport_h = std::max<int32_t>(1, lv_obj_get_height(viewport_));
    // Верхняя оценка числа одновременно нужных тайлов: окно видимых ячеек +
    // overscan сверху/снизу (если бы все элементы были по одной ячейке).
    const int32_t visible_cells = viewport_h / kGridCellPx;
    const size_t pool_size = static_cast<size_t>(visible_cells + 2 * kOverscanCells + 2);
    tiles_.reserve(pool_size);

    for (size_t i = 0; i < pool_size; ++i) {
        ElementTile tile;
        tile.canvas = lv_canvas_create(viewport_);
        lv_obj_add_flag(tile.canvas, LV_OBJ_FLAG_FLOATING);
        lv_obj_add_flag(tile.canvas, LV_OBJ_FLAG_HIDDEN);
        // Картинка не перехватывает палец: скролл/постраничный клик/жесты
        // должны работать поверх неё.
        lv_obj_clear_flag(tile.canvas, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_flag(tile.canvas, LV_OBJ_FLAG_EVENT_BUBBLE);
        lv_obj_add_flag(tile.canvas, LV_OBJ_FLAG_GESTURE_BUBBLE);
        tiles_.push_back(tile);
    }
    tiles_dirty_ = false;
}

UartTerminalApp::ElementTile *UartTerminalApp::tile_for_sequence(uint32_t sequence)
{
    if (sequence == 0) {
        return nullptr;
    }
    for (ElementTile &t : tiles_) {
        if (t.sequence == sequence) {
            return &t;
        }
    }
    return nullptr;
}

UartTerminalApp::ElementTile *UartTerminalApp::acquire_free_tile(void)
{
    for (ElementTile &t : tiles_) {
        if (t.sequence == 0) {
            return &t;
        }
    }
    return nullptr;
}

void UartTerminalApp::ensure_tile_buf(ElementTile &tile, int span)
{
    if (span < 1) {
        span = 1;
    }
    const int32_t w = std::max<int32_t>(1, lv_obj_get_width(viewport_));
    const int32_t h = span * kGridCellPx;

    if ((tile.buf == nullptr) || (tile.alloc_span != span)) {
        if (tile.buf != nullptr) {
            lv_draw_buf_destroy(tile.buf);
            tile.buf = nullptr;
        }
        tile.buf = lv_draw_buf_create(w, h, LV_COLOR_FORMAT_RGB565, LV_STRIDE_AUTO);
        tile.alloc_span = span;
        lv_canvas_set_draw_buf(tile.canvas, tile.buf);
    }
    lv_obj_set_size(tile.canvas, w, h);
    // Чистый фон как плейсхолдер, пока элемент не отрисован.
    lv_canvas_fill_bg(tile.canvas, lv_color_hex(kDefaultBg), LV_OPA_COVER);
}

void UartTerminalApp::free_tile(ElementTile &tile)
{
    if (tile.buf != nullptr) {
        lv_draw_buf_destroy(tile.buf);
        tile.buf = nullptr;
    }
    if (tile.canvas != nullptr) {
        lv_obj_delete(tile.canvas);
        tile.canvas = nullptr;
    }
    tile.alloc_span = 0;
    tile.element_index = -1;
    tile.sequence = 0;
    tile.span = 0;
    tile.rendered = false;
}

void UartTerminalApp::free_all_tiles(void)
{
    for (ElementTile &t : tiles_) {
        free_tile(t);
    }
    tiles_.clear();
    tiles_dirty_ = false;
}

void UartTerminalApp::refresh_visible_elements(void)
{
    if ((viewport_ == nullptr) || tiles_.empty()) {
        return;
    }

    const int32_t view_h = std::max<int32_t>(1, lv_obj_get_height(viewport_));
    const int32_t scroll_y = std::max<int32_t>(0, lv_obj_get_scroll_y(viewport_));
    const int32_t first_row = scroll_y / kGridCellPx;
    const int32_t last_row = (scroll_y + view_h - 1) / kGridCellPx;
    const int32_t win_first = first_row - kOverscanCells;
    const int32_t win_last = last_row + kOverscanCells;

    // 1) Собрать элементы окна [win_first, win_last] инкрементально через locate_row.
    struct WinElem {
        const TerminalLine *line;
        int element_index;
        int32_t start_row;
        int span;
        ElementTile *tile;
        bool in_view;
    };
    std::vector<WinElem> win;

    const size_t n = lines_.size();
    int32_t acc = 0;
    size_t idx = locate_row(win_first < 0 ? 0 : win_first, &acc);
    while (idx < n) {
        const TerminalLine &line = lines_[idx];
        const int32_t span = element_span(line);
        if (span > 0) {
            if (acc > win_last) {
                break;
            }
            if ((acc + span) > win_first) {
                const bool in_view = (acc <= last_row) && ((acc + span) > first_row);
                win.push_back({&line, static_cast<int>(idx), acc, span, nullptr, in_view});
            }
        }
        acc += span;
        ++idx;
    }
    // current_line_ — «живой хвост», элемент с индексом n, высотой text_zoom_span_.
    if (current_line_.cells > 0) {
        const int32_t span = text_zoom_span_;
        if ((acc <= win_last) && ((acc + span) > win_first)) {
            const bool in_view = (acc <= last_row) && ((acc + span) > first_row);
            win.push_back({&current_line_, static_cast<int>(n), acc, span, nullptr, in_view});
        }
    }

    // 2) Освободить тайлы, чьих элементов больше нет в окне (ключ — sequence).
    for (ElementTile &t : tiles_) {
        if (t.sequence == 0) {
            continue;
        }
        bool still_visible = false;
        for (const WinElem &w : win) {
            if (w.line->sequence == t.sequence) {
                still_visible = true;
                break;
            }
        }
        if (!still_visible) {
            t.sequence = 0;
            t.element_index = -1;
            t.rendered = false;
            if (t.canvas) {
                lv_obj_add_flag(t.canvas, LV_OBJ_FLAG_HIDDEN);
            }
        }
    }

    // 3) Привязать тайл каждому элементу окна и спозиционировать.
    for (WinElem &w : win) {
        ElementTile *tile = tile_for_sequence(w.line->sequence);
        if (tile == nullptr) {
            tile = acquire_free_tile();
            if (tile == nullptr) {
                continue;   // пул переполнен (не должно происходить при верной ёмкости)
            }
            tile->sequence = w.line->sequence;
            tile->span = w.span;
            tile->rendered = false;
            ensure_tile_buf(*tile, w.span);
        } else if (tile->span != w.span) {
            // Высота элемента изменилась (например, zoom для текста) — перерисуем.
            tile->span = w.span;
            tile->rendered = false;
            ensure_tile_buf(*tile, w.span);
        }
        tile->element_index = w.element_index;
        w.tile = tile;

        const int32_t y = w.start_row * kGridCellPx - scroll_y;
        lv_obj_set_pos(tile->canvas, 0, y);
        lv_obj_clear_flag(tile->canvas, LV_OBJ_FLAG_HIDDEN);
    }

    // Направление скролла → ведущий край предзагрузки (куда едем — то и грузим
    // первым). При delta>=0 (вниз/стоим) ведущий край снизу.
    const bool leading_below = (scroll_y - last_scroll_y_) >= 0;
    last_scroll_y_ = scroll_y;

    // 4a) Видимые тайлы — приоритет (анти-фриз: ≤ kMaxRendersPerFrame за кадр).
    int budget = kMaxRendersPerFrame;
    for (WinElem &w : win) {
        if (budget <= 0) {
            break;
        }
        if ((w.tile != nullptr) && !w.tile->rendered && w.in_view) {
            render_tile(*w.tile, *w.line);
            --budget;
        }
    }

    // 4b) Предзагрузка overscan в обе стороны. Бюджет = неиспользованный остаток
    //     видимого бюджета + гарантированный минимум: в простое overscan
    //     заполняется быстро, при активном скролле — минимум kPreloadPerFrame за
    //     кадр (margin растёт даже когда видимые съели весь бюджет 4a).
    //     Сначала ведущий край (по направлению скролла), затем обратный.
    int preload = (budget > 0 ? budget : 0) + kPreloadPerFrame;
    auto preload_band = [&](bool below) {
        for (WinElem &w : win) {
            if (preload <= 0) {
                break;
            }
            if ((w.tile == nullptr) || w.tile->rendered || w.in_view) {
                continue;
            }
            const bool is_below = (w.start_row > last_row);
            if (is_below != below) {
                continue;
            }
            render_tile(*w.tile, *w.line);
            --preload;
        }
    };
    preload_band(leading_below);
    preload_band(!leading_below);

    // Если что-то ещё не отрисовано (видимое или overscan) — добираем в
    // следующих кадрах (флаг подхватывает poll-таймер).
    bool remaining = false;
    for (const WinElem &w : win) {
        if ((w.tile != nullptr) && !w.tile->rendered) {
            remaining = true;
            break;
        }
    }
    tiles_dirty_ = remaining;
}

void UartTerminalApp::render_tile(ElementTile &tile, const TerminalLine &line)
{
    if ((tile.canvas == nullptr) || (tile.buf == nullptr)) {
        return;
    }

    if (line.is_timber) {
        render_timber(tile, line.timber_cmd);
    } else if (line.is_widget) {
        lv_canvas_fill_bg(tile.canvas, lv_color_hex(kDefaultBg), LV_OPA_COVER);
        render_osc_widget(tile, line.widget);
    } else {
        lv_canvas_fill_bg(tile.canvas, lv_color_hex(kDefaultBg), LV_OPA_COVER);
        render_text(tile, line);
    }
    tile.rendered = true;
}

void UartTerminalApp::render_text(ElementTile &tile, const TerminalLine &line)
{
    const int32_t h = tile.span * kGridCellPx;

    lv_layer_t layer;
    lv_canvas_init_layer(tile.canvas, &layer);

    const lv_font_t *font = font_for_zoom(tile.span);
    const int32_t font_h = lv_font_get_line_height(font);
    const int32_t ofs_y = (h - font_h) / 2;   // центрируем строку по высоте ячейки
    int32_t x = 6;

    for (const TextRun &run : line.runs) {
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
            lv_area_t bg_area = {x, 0, x + size.x - 1, h - 1};
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

        lv_area_t txt_area = {x, 0, x + size.x - 1, h - 1};
        lv_draw_label(&layer, &label_dsc, &txt_area);

        x += size.x;
    }

    lv_canvas_finish_layer(tile.canvas, &layer);
}

void UartTerminalApp::render_osc_widget(ElementTile &tile, const WidgetDesc &widget)
{
    const int32_t viewport_w = lv_obj_get_width(tile.canvas);
    const int32_t cell_h = kGridCellPx;

    lv_layer_t layer;
    lv_canvas_init_layer(tile.canvas, &layer);

    const lv_font_t *font = &lv_font_montserrat_16;

    switch (widget.kind) {
    case WidgetDesc::Kind::ProgressBar: {
        const int32_t pad = 4;
        const int32_t bar_h = cell_h - pad * 2;
        const int32_t bar_w = viewport_w - pad * 2 - 80;
        const int32_t fill_w = (bar_w * widget.value) / LV_MAX(1, widget.max_value);

        lv_area_t track = {pad, pad, pad + bar_w - 1, pad + bar_h - 1};
        lv_draw_fill_dsc_t track_dsc;
        lv_draw_fill_dsc_init(&track_dsc);
        track_dsc.color = lv_color_hex(widget.track_color);
        track_dsc.opa = LV_OPA_COVER;
        lv_draw_fill(&layer, &track_dsc, &track);

        if (fill_w > 0) {
            lv_area_t fill = {pad, pad, pad + fill_w - 1, pad + bar_h - 1};
            lv_draw_fill_dsc_t fill_dsc;
            lv_draw_fill_dsc_init(&fill_dsc);
            fill_dsc.color = lv_color_hex(widget.bar_color);
            fill_dsc.opa = LV_OPA_COVER;
            lv_draw_fill(&layer, &fill_dsc, &fill);
        }

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
        lv_area_t lbl_area = {pad + bar_w + 8, 0, viewport_w - 1, cell_h - 1};
        lv_draw_label(&layer, &lbl_dsc, &lbl_area);
        break;
    }

    case WidgetDesc::Kind::StatusBlock: {
        lv_area_t bg = {0, 0, viewport_w - 1, cell_h - 1};
        lv_draw_fill_dsc_t bg_dsc;
        lv_draw_fill_dsc_init(&bg_dsc);
        bg_dsc.color = lv_color_hex(widget.bar_color);
        bg_dsc.opa = LV_OPA_COVER;
        lv_draw_fill(&layer, &bg_dsc, &bg);

        lv_draw_label_dsc_t lbl_dsc;
        lv_draw_label_dsc_init(&lbl_dsc);
        lbl_dsc.text = widget.label.c_str();
        lbl_dsc.font = font;
        lbl_dsc.color = lv_color_hex(widget.text_color);
        lbl_dsc.opa = LV_OPA_COVER;
        lbl_dsc.align = LV_TEXT_ALIGN_CENTER;
        lv_area_t lbl_area = {0, 0, viewport_w - 1, cell_h - 1};
        lv_draw_label(&layer, &lbl_dsc, &lbl_area);
        break;
    }
    }

    lv_canvas_finish_layer(tile.canvas, &layer);
}

// Растеризует дерево timber-виджета в PSRAM-буфер тайла через lv_snapshot.
// Дерево строится во временном offscreen-контейнере и сразу уничтожается —
// в кэше остаётся только статичная картинка (виджеты в логе неинтерактивны).
void UartTerminalApp::render_timber(ElementTile &tile, const timber::WidgetCommand &cmd)
{
    if ((tile.buf == nullptr) || (viewport_ == nullptr)) {
        return;
    }

    const int32_t w = lv_obj_get_width(tile.canvas);
    const int32_t h = tile.span * kGridCellPx;

    // Временный контейнер строится как FLOATING-ребёнок viewport и удаляется в
    // этом же синхронном вызове (LVGL не успеет его отрисовать на экране).
    // HIDDEN не ставим — снапшот скрытого объекта может выйти пустым.
    lv_obj_t *tmp = lv_obj_create(viewport_);
    lv_obj_remove_style_all(tmp);
    lv_obj_add_flag(tmp, LV_OBJ_FLAG_FLOATING);
    lv_obj_clear_flag(tmp, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(tmp, lv_color_hex(kDefaultBg), 0);
    lv_obj_set_style_bg_opa(tmp, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(tmp, 1, 0);
    lv_obj_set_size(tmp, w, h);
    lv_obj_set_pos(tmp, 0, 0);

    timber::renderWidget(tmp, cmd, kGridCellPx);
    lv_obj_update_layout(tmp);

    lv_snapshot_take_to_draw_buf(tmp, LV_COLOR_FORMAT_RGB565, tile.buf);
    // Перепривязываем буфер к канвасу — это и обновляет геометрию, и инвалидирует
    // канвас, чтобы LVGL перерисовал его новым содержимым снапшота.
    lv_canvas_set_draw_buf(tile.canvas, tile.buf);

    lv_obj_delete(tmp);
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

    // Счётчик строк — только число (состояние связи показывает светодиод слева).
    char text[24] = {};
    std::snprintf(text, sizeof(text), "%u",
                  static_cast<unsigned>(virtual_line_count()));
    lv_label_set_text(status_label_, text);

    update_net_indicator();
}

// Цвет кружка-индикатора по состоянию TCP-подключения:
//  зелёный — подключено, жёлтый — подключение/ожидание, красный — ошибка.
void UartTerminalApp::update_net_indicator(void)
{
    if (net_dot_ == nullptr) {
        return;
    }
    uint32_t color;
    switch (netcfg::tcp_state()) {
    case netcfg::TcpState::Connected:  color = 0x22C55E; break;  // зелёный
    case netcfg::TcpState::Connecting:
    case netcfg::TcpState::Idle:       color = 0xF59E0B; break;  // жёлтый
    case netcfg::TcpState::Error:      color = 0xEF4444; break;  // красный
    case netcfg::TcpState::Disabled:
    default:                           color = 0x555B66; break;  // серый (выкл)
    }
    lv_obj_set_style_bg_color(net_dot_, lv_color_hex(color), 0);
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

int8_t UartTerminalApp::extract_text_channel(TerminalLine &line)
{
    // Префикс @N в самом начале строки (обычно одним раном без особого стиля).
    if (line.runs.empty()) {
        return 0;
    }
    std::string &head = line.runs.front().text;
    if (head.empty() || head[0] != '@') {
        return 0;
    }

    size_t p = 1;
    int ch = 0;
    bool any = false;
    while ((p < head.size()) && (head[p] >= '0') && (head[p] <= '9')) {
        ch = ch * 10 + (head[p] - '0');
        ++p;
        any = true;
    }
    // Префикс валиден только если за числом идёт пробел: "@3 ...".
    if (!any || (p >= head.size()) || (head[p] != ' ')) {
        return 0;
    }

    if (ch < 0) ch = 0;
    if (ch > 3) ch = 3;

    // Удаляем "@N " из видимого текста и корректируем счётчик ячеек строки.
    const size_t consumed = p + 1;
    head.erase(0, consumed);
    if (line.cells >= consumed) {
        line.cells -= consumed;
    } else {
        line.cells = 0;
    }
    return static_cast<int8_t>(ch);
}

void UartTerminalApp::set_active_channel(int channel)
{
    if (channel < -1) channel = -1;
    if (channel > 3) channel = 3;
    if (channel == active_channel_) {
        return;
    }
    active_channel_ = channel;

    // Высота видимого контента зависит от фильтра — пересобираем кэш рядов и
    // сбрасываем курсор поиска. Тайлы отфильтрованных элементов сами отвяжутся
    // при следующем refresh (их sequence уйдёт из окна).
    recompute_committed_rows();

    update_channel_buttons();
    update_content_height();

    // При смене канала прижимаемся к низу — показываем свежие строки канала.
    auto_follow_ = true;
    scroll_to_bottom();
    refresh_visible_elements();
    update_status_label(true);
}

void UartTerminalApp::update_channel_buttons(void)
{
    for (int i = 0; i < 5; ++i) {
        lv_obj_t *button = channel_buttons_[i];
        if (button == nullptr) {
            continue;
        }
        const int channel = (i == 0) ? -1 : (i - 1);
        const bool active = (channel == active_channel_);
        lv_obj_set_style_bg_color(button,
                                  lv_color_hex(active ? kButtonActiveBg : kButtonBg),
                                  0);
    }
}

void UartTerminalApp::channel_event_cb(lv_event_t *event)
{
    UartTerminalApp *app = static_cast<UartTerminalApp *>(lv_event_get_user_data(event));
    if (app == nullptr) {
        return;
    }
    lv_obj_t *button = static_cast<lv_obj_t *>(lv_event_get_target(event));
    const int channel = static_cast<int>(reinterpret_cast<intptr_t>(lv_obj_get_user_data(button)));
    app->set_active_channel(channel);
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

    // Добор отложенных рендеров тайлов (анти-фриз: ≤K за кадр). Пока остались
    // нерендеренные тайлы в окне — продолжаем дорисовывать на следующих тиках.
    if (app->tiles_dirty_) {
        app->refresh_visible_elements();
    }
}

void UartTerminalApp::scroll_event_cb(lv_event_t *event)
{
    UartTerminalApp *app = static_cast<UartTerminalApp *>(lv_event_get_user_data(event));
    if (app == nullptr) {
        return;
    }

    const lv_event_code_t code = lv_event_get_code(event);
    if (code == LV_EVENT_SIZE_CHANGED) {
        app->rebuild_tile_pool();
        app->update_content_height();
        app->refresh_visible_elements();
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

            int32_t snapped = ((scroll_y + kGridCellPx / 2) / kGridCellPx) * kGridCellPx;
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
        app->refresh_visible_elements();
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
    const int32_t page_lines = view_h / kGridCellPx;
    const int32_t page_size = page_lines * kGridCellPx;

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

    // Snap к границе ячейки сетки.
    scroll_y = ((scroll_y + kGridCellPx / 2) / kGridCellPx) * kGridCellPx;
    if (scroll_y > max_scroll) {
        scroll_y = max_scroll;
    }

    app->suppress_scroll_event_ = true;
    lv_obj_scroll_to_y(app->viewport_, scroll_y, LV_ANIM_OFF);
    app->suppress_scroll_event_ = false;

    app->auto_follow_ = app->is_scroll_near_bottom();
    app->update_follow_button();
    app->refresh_visible_elements();
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
    app->refresh_visible_elements();
}

const lv_font_t *UartTerminalApp::font_for_zoom(int span)
{
    switch (span) {
    case 1:  return &lv_font_montserrat_16;   // 20px ячейка
    case 2:  return &lv_font_montserrat_28;   // 40px
    case 3:  return &lv_font_montserrat_40;   // 60px
    default: return &lv_font_montserrat_16;
    }
}

void UartTerminalApp::apply_zoom(int span)
{
    if (span < 1) span = 1;
    if (span > kMaxTextZoomSpan) span = kMaxTextZoomSpan;
    if (span == text_zoom_span_) {
        return;
    }
    text_zoom_span_ = span;

    // Меняется только высота текстовых строк; высота виджетов фиксирована
    // (kGridCellPx-сетка). Пересчитываем суммарную высоту и пересоздаём тайлы
    // (у текстовых тайлов поменялся размер буфера).
    recompute_committed_rows();

    if (viewport_ != nullptr) {
        free_all_tiles();
        rebuild_tile_pool();
        update_content_height();
        if (auto_follow_) {
            scroll_to_bottom();
        }
        refresh_visible_elements();
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
        if (app->text_zoom_span_ < kMaxTextZoomSpan) {
            app->apply_zoom(app->text_zoom_span_ + 1);
        }
    } else if (scale < 0.8f) {
        if (app->text_zoom_span_ > 1) {
            app->apply_zoom(app->text_zoom_span_ - 1);
        }
    }
}
