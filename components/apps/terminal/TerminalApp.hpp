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
#include "uart_config.hpp"
#include "widgets/WidgetParser.hpp"

class UartTerminalApp : public LiteApp {
public:
    UartTerminalApp();
    ~UartTerminalApp() override;

    const char *title(void) const override { return "UART Terminal"; }
    const char *icon_text(void) const override { return ">_"; }
    lv_color_t accent_color(void) const override { return lv_color_hex(0x16A085); }

    // Терминал сам рисует панель с кнопками (в т.ч. "назад"), поэтому
    // верхний заголовок лаунчера с названием "UART Terminal" не нужен.
    bool use_launcher_header(void) const override { return false; }

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

    struct WidgetDesc {
        enum Kind {
            ProgressBar,
            StatusBlock,
        } kind = ProgressBar;

        int32_t value = 0;
        int32_t max_value = 100;
        uint32_t bar_color = 0x00AA44;
        uint32_t track_color = 0x1A1A1A;
        uint32_t text_color = 0xFFFFFF;
        std::string label;
    };

    struct TerminalLine {
        std::vector<TextRun> runs;
        size_t cells = 0;
        uint32_t sequence = 0;
        bool is_widget = false;          // встроенный OSC-виджет (canvas, 1 ряд)
        WidgetDesc widget;

        // Виджет протокола TimberWidget (`ui type=...`): рисуется живым деревом
        // LVGL-объектов и занимает row_span рядов терминала.
        bool is_timber = false;
        timber::WidgetCommand timber_cmd;
        int row_span = 1;                // высота элемента в рядах (>=1)

        // Канал строки (0..3) из транспортного префикса @N или ключа channel=
        // у виджетов. Используется фильтром каналов в тулбаре.
        int8_t channel = 0;
    };

    // Кэшированная в PSRAM картинка одного элемента списка. Рендерится один раз
    // (текст — рисованием ранов, timber-виджет — через lv_snapshot), при скролле
    // только перемещается; при выходе за окно overscan освобождается.
    struct ElementTile {
        lv_obj_t *canvas = nullptr;     // FLOATING-канвас поверх viewport
        lv_draw_buf_t *buf = nullptr;   // RGB565, width × (alloc_span*kGridCellPx)
        int alloc_span = 0;             // высота буфера в ячейках (для реаллокации)
        int element_index = -1;         // индекс в lines_ (==lines_.size() → current_line_); -1 = свободен
        uint32_t sequence = 0;          // ключ валидности кэша
        int span = 0;                   // фактическая высота элемента в ячейках
        bool rendered = false;          // false → показан фон-плейсхолдер, нужен рендер
    };

    enum class ParserState {
        Normal,
        Escape,
        Csi,
        Osc,
    };

    bool init_uart(void);
    void reconfigure_uart(void);   // применить новую конфигурацию из NVS (в UART-задаче)
    void uart_task(void);
    void queue_uart_bytes(const char *data, size_t len);
    void inject_demo_uart_data(void);
    void drain_uart_stream(void);

    void parse_byte(uint8_t byte);
    void append_printable(char ch);
    void append_spaces(size_t count);
    void finish_line(void);
    void add_widget_line(const WidgetDesc &widget);
    void add_timber_widget(const timber::WidgetCommand &cmd);  // ui type=... виджет
    void push_committed_line(TerminalLine &&line);             // +учёт committed_rows_
    bool try_consume_timber_line(void);                        // распознать команду в current_line_
    void clear_history(bool reset_parser);
    void reset_style(void);
    void handle_csi_final(char final);
    void handle_osc(const std::string &osc);
    void apply_sgr(const std::vector<int> &params);

    size_t virtual_line_count(void) const;
    const TerminalLine *line_at(size_t index) const;

    // Учёт переменной высоты: суммарное число рядов и поиск элемента по ряду.
    // total_rows() — O(1) за счёт инкрементального кэша committed_rows_.
    int32_t total_rows(void) const;
    // Возвращает элемент, покрывающий абсолютный ряд row, и его стартовый ряд.
    // Линейный поиск; на горячем пути скролла не используется — там идёт
    // инкрементальный обход через локацию по курсору (locate_row).
    const TerminalLine *element_at_row(int32_t row, int32_t *start_row) const;

    // Учёт высоты одного элемента в ячейках сетки (по kGridCellPx px) с учётом
    // активного фильтра каналов. Если элемент отфильтрован (канал не совпадает и
    // фильтр не "All"), возвращает 0 — элемент пропускается при разметке/отрисовке.
    //   timber-виджет → row_span (целые ячейки, не зависит от zoom);
    //   OSC-виджет     → 1 ячейка (фиксированно);
    //   текст          → text_zoom_span_ (1..3 → 20/40/60px).
    int32_t element_span(const TerminalLine &line) const {
        if ((active_channel_ >= 0) && (line.channel != active_channel_)) {
            return 0;
        }
        if (line.is_timber) {
            return line.row_span > 1 ? line.row_span : 1;
        }
        if (line.is_widget) {
            return 1;
        }
        return text_zoom_span_;
    }
    // Пересобрать кэш суммарной высоты после массовых изменений lines_.
    void recompute_committed_rows(void);
    // Найти индекс элемента в lines_, покрывающего абсолютный ряд row, и его
    // стартовый ряд. Использует курсор-кэш предыдущего поиска, поэтому при
    // скролле работает почти за O(1). Возвращает индекс или lines_.size()
    // (тогда ряд относится к current_line_ или за пределами).
    size_t locate_row(int32_t row, int32_t *start_row) const;

    int32_t content_height(void) const;
    bool is_scroll_near_bottom(void) const;
    void update_content_height(void);
    void scroll_to_bottom(void);

    // --- Кэш-картинки элементов (PSRAM) ---
    void rebuild_tile_pool(void);            // (пере)создать пул тайлов под размер viewport
    void refresh_visible_elements(void);     // раскладка + рендер видимого окна (горячий путь)
    ElementTile *tile_for_sequence(uint32_t sequence);  // тайл, связанный с элементом, или nullptr
    ElementTile *acquire_free_tile(void);    // взять свободный тайл из пула (или nullptr)
    void ensure_tile_buf(ElementTile &tile, int span); // (ре)аллокация буфера под span ячеек
    void render_tile(ElementTile &tile, const TerminalLine &line);   // диспатч по типу элемента
    void render_text(ElementTile &tile, const TerminalLine &line);
    void render_osc_widget(ElementTile &tile, const WidgetDesc &widget);
    void render_timber(ElementTile &tile, const timber::WidgetCommand &cmd);
    void free_tile(ElementTile &tile);
    void free_all_tiles(void);

    void update_status_label(bool force);
    void update_follow_button(void);

    // Фильтр каналов: -1 = All (показывать всё), 0..3 = только этот канал.
    void set_active_channel(int channel);
    void update_channel_buttons(void);
    // Считать префикс @N в начале готовой текстовой строки, выставить её канал
    // и удалить префикс из видимого текста. Возвращает номер канала (0..3).
    static int8_t extract_text_channel(TerminalLine &line);

    static bool same_style(const TextStyle &a, const TextStyle &b);
    static uint32_t xterm256_to_rgb(int index);
    static void poll_timer_cb(lv_timer_t *timer);
    static void scroll_event_cb(lv_event_t *event);
    static void page_event_cb(lv_event_t *event);
    static void gesture_event_cb(lv_event_t *event);
    static void demo_event_cb(lv_event_t *event);
    static void clear_event_cb(lv_event_t *event);
    static void follow_event_cb(lv_event_t *event);
    static void channel_event_cb(lv_event_t *event);
    static void back_event_cb(lv_event_t *event);
    static void uart_task_entry(void *arg);

    void apply_zoom(int span);                       // 1..kMaxTextZoomSpan (20/40/60px)
    static const lv_font_t *font_for_zoom(int span);

    // Базовая ячейка сетки и параметры кэша/анти-фриза.
    static constexpr int kGridCellPx = 20;
    static constexpr int kMaxTextZoomSpan = 3;       // 20/40/60px
    static constexpr int kOverscanCells = 8;         // запас тайлов сверху/снизу окна
    static constexpr int kMaxRendersPerFrame = 4;    // лимит рендеров видимых тайлов за кадр
    static constexpr int kPreloadPerFrame = 2;       // отдельный бюджет предзагрузки overscan за кадр

    bool uart_installed_ = false;
    bool uart_task_started_ = false;
    TaskHandle_t uart_task_ = nullptr;
    StreamBufferHandle_t rx_stream_ = nullptr;
    char uart_status_[128] = {};
    uartcfg::Config uart_cfg_ = uartcfg::defaults();

    volatile uint32_t received_bytes_ = 0;
    volatile uint32_t dropped_bytes_ = 0;

    std::deque<TerminalLine> lines_;
    TerminalLine current_line_;

    // Кэш суммарной высоты завершённых элементов (lines_) в рядах.
    // Поддерживается инкрементально: += при добавлении, -= при pop_front,
    // сброс при clear. Делает total_rows()/content_height() константными.
    int32_t committed_rows_ = 0;

    // Курсор-кэш для locate_row(): индекс последнего найденного элемента и
    // его стартовый ряд. Скролл двигает строй на 1-2 ряда, поэтому следующий
    // поиск стартует рядом и почти всегда O(1). mutable — locate_row const.
    mutable size_t locate_cursor_index_ = 0;
    mutable int32_t locate_cursor_start_ = 0;

    TextStyle current_style_;
    ParserState parser_state_ = ParserState::Normal;
    std::string csi_buffer_;
    std::string osc_buffer_;
    bool previous_was_cr_ = false;
    uint32_t next_sequence_ = 1;

    lv_obj_t *root_ = nullptr;
    lv_obj_t *status_label_ = nullptr;
    lv_obj_t *follow_button_ = nullptr;
    lv_obj_t *channel_buttons_[5] = {};   // [0]=All, [1..4]=каналы 0..3
    int active_channel_ = -1;             // -1 = All
    lv_obj_t *viewport_ = nullptr;
    lv_obj_t *spacer_ = nullptr;
    lv_timer_t *poll_timer_ = nullptr;
    std::vector<ElementTile> tiles_;   // пул кэш-картинок элементов (PSRAM)
    bool tiles_dirty_ = false;         // остались нерендеренные тайлы → добор по кадрам
    int32_t last_scroll_y_ = 0;        // прошлый scroll_y → направление для приоритета предзагрузки
    int text_zoom_span_ = 1;           // высота текстовой строки в ячейках (1..3 → 20/40/60px)
    bool auto_follow_ = true;
    bool demo_seeded_ = false;
    bool suppress_scroll_event_ = false;
    uint32_t last_status_update_ms_ = 0;
};
