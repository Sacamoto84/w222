#include "WidgetRender.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace timber {

// ============================================================================
//  Палитра по умолчанию (повторяет цвета Android ConsoleWidgetSpec).
// ============================================================================
namespace {

constexpr uint32_t kCardBg       = 0x11171C;
constexpr uint32_t kCardBorder   = 0x23303A;
constexpr uint32_t kTitleColor   = 0xFFFFFF;
constexpr uint32_t kSubColor     = 0xB5C0C8;
constexpr uint32_t kMutedColor   = 0x8FA1AD;
constexpr uint32_t kValueColor   = 0xE3EEF5;
constexpr uint32_t kAccentGreen  = 0x36C36B;
constexpr uint32_t kTrackColor   = 0x1A242B;
constexpr uint32_t kGreenLed     = 0x00E676;
constexpr uint32_t kRedLed       = 0xFF5252;
constexpr uint32_t kAmberLed     = 0xFFB300;

inline lv_color_t C(uint32_t rgb) { return lv_color_hex(rgb); }

const lv_font_t *fontFor(int px) {
    // Доступные в sdkconfig: 12, 14, 16.
    if (px <= 12) return &lv_font_montserrat_12;
    if (px <= 14) return &lv_font_montserrat_14;
    return &lv_font_montserrat_16;
}

// Создаёт «голый» контейнер без стилей, прозрачный.
lv_obj_t *plain(lv_obj_t *parent) {
    lv_obj_t *o = lv_obj_create(parent);
    lv_obj_remove_style_all(o);
    lv_obj_set_style_bg_opa(o, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(o, 0, 0);
    lv_obj_set_style_pad_all(o, 0, 0);
    lv_obj_clear_flag(o, LV_OBJ_FLAG_SCROLLABLE);
    return o;
}

// Делает объект и всё его поддерево «прозрачным» для жестов пальца:
// снимает CLICKABLE/SCROLLABLE и включает всплытие событий и жестов вверх,
// чтобы прокрутку/клик всегда получал внешний viewport терминала, а не сам
// виджет. Без этого lv_chart/lv_arc перехватывают палец и срывают скролл.
void makeInertRecursive(lv_obj_t *obj) {
    if (obj == nullptr) return;
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE);
    uint32_t n = lv_obj_get_child_count(obj);
    for (uint32_t i = 0; i < n; ++i) {
        makeInertRecursive(lv_obj_get_child(obj, i));
    }
}

// Простая текстовая метка.
lv_obj_t *label(lv_obj_t *parent, const char *text, uint32_t color, int px = 14) {
    lv_obj_t *l = lv_label_create(parent);
    lv_label_set_text(l, text ? text : "");
    lv_obj_set_style_text_font(l, fontFor(px), 0);
    lv_obj_set_style_text_color(l, C(color), 0);
    lv_obj_set_style_pad_all(l, 0, 0);
    return l;
}

// Карточка-контейнер: фон, скруглённая рамка, внутренние отступы.
// Заполняет всю площадь parent. Дальнейшие объекты кладём внутрь.
lv_obj_t *card(lv_obj_t *parent, uint32_t bg, uint32_t border, uint32_t accent = 0) {
    lv_obj_t *c = lv_obj_create(parent);
    lv_obj_remove_style_all(c);
    lv_obj_set_size(c, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(c, C(bg), 0);
    lv_obj_set_style_bg_opa(c, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(c, 8, 0);
    lv_obj_set_style_border_width(c, 1, 0);
    lv_obj_set_style_border_color(c, C(border), 0);
    lv_obj_set_style_pad_all(c, 8, 0);
    lv_obj_clear_flag(c, LV_OBJ_FLAG_SCROLLABLE);
    if (accent != 0) {
        // Левая акцентная полоса.
        lv_obj_set_style_border_side(c, LV_BORDER_SIDE_FULL, 0);
        lv_obj_set_style_outline_width(c, 0, 0);
        lv_obj_t *strip = lv_obj_create(c);
        lv_obj_remove_style_all(strip);
        lv_obj_set_size(strip, 4, lv_pct(100));
        lv_obj_align(strip, LV_ALIGN_LEFT_MID, -4, 0);
        lv_obj_set_style_bg_color(strip, C(accent), 0);
        lv_obj_set_style_bg_opa(strip, LV_OPA_COVER, 0);
        lv_obj_set_style_radius(strip, 2, 0);
        lv_obj_clear_flag(strip, LV_OBJ_FLAG_SCROLLABLE);
    }
    return c;
}

// Перевести список строк значений в float.
std::vector<float> toFloats(const std::vector<std::string> &items) {
    std::vector<float> out;
    out.reserve(items.size());
    for (const std::string &s : items) {
        out.push_back(parseFloat(s.c_str(), 0.0f));
    }
    return out;
}

// Привести строку состояния led/pin к цвету индикатора.
uint32_t stateToColor(const std::string &stateRaw, uint32_t offColor) {
    // Если это уже цвет (#... / named) — взять как есть.
    uint32_t asColor = parseColor(stateRaw.c_str(), 0xFFFFFFFFu);
    if (asColor != 0xFFFFFFFFu &&
        (stateRaw.size() > 0 && (stateRaw[0] == '#' ||
         stateRaw == "red" || stateRaw == "green" || stateRaw == "blue"))) {
        return asColor;
    }
    std::string s;
    for (char c : stateRaw) s.push_back(static_cast<char>(std::tolower((unsigned char)c)));
    if (s == "on" || s == "true" || s == "1" || s == "active" || s == "ok" ||
        s == "high" || s == "enabled" || s == "out" || s == "output") return kGreenLed;
    if (s == "warn" || s == "warning") return kAmberLed;
    if (s == "error" || s == "err" || s == "alarm" || s == "critical") return kRedLed;
    if (s == "adc" || s == "analog") return 0x4FC3F7;
    if (s == "pwm") return 0xBA68C8;
    if (s == "in" || s == "input") return 0x64B5F6;
    return offColor;
}

// Цвета severity для alarm-card.
uint32_t severityColor(const char *sev) {
    if (sev == nullptr) return kAmberLed;
    std::string s;
    for (const char *p = sev; *p; ++p) s.push_back(static_cast<char>(std::tolower((unsigned char)*p)));
    if (s == "critical" || s == "alarm") return 0xFF1744;
    if (s == "error" || s == "fail" || s == "danger") return kRedLed;
    if (s == "warn" || s == "warning") return kAmberLed;
    return 0x4FC3F7;  // info
}

// Стиль пресета badge (фон, текст) по имени st=.
struct BadgePreset { uint32_t bg; uint32_t fg; bool ok; };
BadgePreset badgePreset(const char *st) {
    if (st == nullptr) return {0, 0, false};
    std::string s;
    for (const char *p = st; *p; ++p) s.push_back(static_cast<char>(std::tolower((unsigned char)*p)));
    if (s == "ok" || s == "ready" || s == "success" || s == "good" || s == "green")
        return {0x1F7A1F, 0xFFFFFF, true};
    if (s == "info" || s == "blue")    return {0x174A7A, 0xFFFFFF, true};
    if (s == "warn" || s == "warning" || s == "amber" || s == "yellow")
        return {0x7A5B12, 0xFFFFFF, true};
    if (s == "error" || s == "fail" || s == "danger" || s == "red")
        return {0x7A1F1F, 0xFFFFFF, true};
    if (s == "critical" || s == "alarm") return {0x5A1010, 0xFFE7E7, true};
    if (s == "neutral" || s == "default" || s == "gray" || s == "grey")
        return {0x33414D, 0xFFFFFF, true};
    if (s == "dark" || s == "muted")    return {0x1D252C, 0xE3EEF5, true};
    return {0, 0, false};
}

}  // namespace

// ============================================================================
//  Измерение высоты (в строках). Совпадает с реальной отрисовкой ниже.
// ============================================================================

int measureWidgetRows(const WidgetCommand &cmd, int lineHeight) {
    (void)lineHeight;
    switch (cmd.type) {
        case WidgetType::Badge:
        case WidgetType::Dot:
        case WidgetType::TwoCol:
            return 2;
        case WidgetType::Image:
            return 3;
        case WidgetType::Panel:
        case WidgetType::Progress:
        case WidgetType::Switch:
        case WidgetType::StatsCard:
            return 3;
        case WidgetType::Gauge:
        case WidgetType::Battery:
            return 4;
        case WidgetType::Sparkline:
            return 4;
        case WidgetType::LedRow:
            return 3;
        case WidgetType::Bitfield: {
            int bits = parseInt(cmd.find({"bits"}), 8, 1, 64);
            int perRow = 16;
            int rows = (bits + perRow - 1) / perRow;
            return 2 + rows;  // заголовок + строки битов
        }
        case WidgetType::AlarmCard:
            return 4;
        case WidgetType::KvGrid: {
            auto items = splitList(cmd.find({"items"}), "|");
            int cols = parseInt(cmd.find({"columns"}), 2, 1, 3);
            int rows = (static_cast<int>(items.size()) + cols - 1) / cols;
            return 2 + rows;
        }
        case WidgetType::PinBank: {
            auto items = splitList(cmd.find({"items"}), "|");
            int cols = parseInt(cmd.find({"columns"}), 3, 1, 4);
            int rows = (static_cast<int>(items.size()) + cols - 1) / cols;
            return 2 + rows;
        }
        case WidgetType::BarGroup:
            return 5;
        case WidgetType::Timeline: {
            auto items = splitList(cmd.find({"items"}), "|");
            return 2 + std::max<int>(1, static_cast<int>(items.size()));
        }
        case WidgetType::LineChart:
            return 6;
        case WidgetType::Table: {
            auto rows = splitRows(cmd.find({"rows"}));
            return 2 + static_cast<int>(rows.size());  // заголовок + строки
        }
        case WidgetType::HexDump: {
            auto bytes = parseHexBytes(cmd.find({"data", "bytes"}));
            int width = parseInt(cmd.find({"width"}), 8, 1, 32);
            int rows = (static_cast<int>(bytes.size()) + width - 1) / width;
            return 2 + std::max(1, rows);
        }
        case WidgetType::RegisterTable: {
            auto rows = splitRows(cmd.find({"rows", "registers"}));
            return 2 + 1 + static_cast<int>(rows.size());  // заголовок+шапка+строки
        }
        case WidgetType::ModbusFrame:
        case WidgetType::CanFrame:
        case WidgetType::UartFrame:
        case WidgetType::PacketFrame: {
            auto fields = splitRows(cmd.find({"fields"}));
            int base = 3;  // заголовок + строка байтов + meta
            return base + std::max<int>(0, static_cast<int>(fields.size()));
        }
        default:
            return 2;
    }
}

// Объявления функций рисования (определены во второй части файла).
void drawBadge(lv_obj_t *p, const WidgetCommand &c);
void drawDot(lv_obj_t *p, const WidgetCommand &c);
void drawImage(lv_obj_t *p, const WidgetCommand &c);
void drawPanel(lv_obj_t *p, const WidgetCommand &c);
void drawProgress(lv_obj_t *p, const WidgetCommand &c);
void drawTwoCol(lv_obj_t *p, const WidgetCommand &c);
void drawTable(lv_obj_t *p, const WidgetCommand &c);
void drawSwitch(lv_obj_t *p, const WidgetCommand &c);
void drawAlarmCard(lv_obj_t *p, const WidgetCommand &c);
void drawSparkline(lv_obj_t *p, const WidgetCommand &c);
void drawBarGroup(lv_obj_t *p, const WidgetCommand &c);
void drawGauge(lv_obj_t *p, const WidgetCommand &c);
void drawBattery(lv_obj_t *p, const WidgetCommand &c);
void drawLedRow(lv_obj_t *p, const WidgetCommand &c);
void drawStatsCard(lv_obj_t *p, const WidgetCommand &c);
void drawKvGrid(lv_obj_t *p, const WidgetCommand &c);
void drawPinBank(lv_obj_t *p, const WidgetCommand &c);
void drawTimeline(lv_obj_t *p, const WidgetCommand &c);
void drawLineChart(lv_obj_t *p, const WidgetCommand &c);
void drawBitfield(lv_obj_t *p, const WidgetCommand &c);
void drawHexDump(lv_obj_t *p, const WidgetCommand &c);
void drawRegisterTable(lv_obj_t *p, const WidgetCommand &c);
void drawFrame(lv_obj_t *p, const WidgetCommand &c, const char *defProto);

void renderWidget(lv_obj_t *parent, const WidgetCommand &cmd, int lineHeight) {
    (void)lineHeight;
    if (parent == nullptr) return;
    switch (cmd.type) {
        case WidgetType::Badge:        drawBadge(parent, cmd); break;
        case WidgetType::Dot:          drawDot(parent, cmd); break;
        case WidgetType::Image:        drawImage(parent, cmd); break;
        case WidgetType::Panel:        drawPanel(parent, cmd); break;
        case WidgetType::Progress:     drawProgress(parent, cmd); break;
        case WidgetType::TwoCol:       drawTwoCol(parent, cmd); break;
        case WidgetType::Table:        drawTable(parent, cmd); break;
        case WidgetType::Switch:       drawSwitch(parent, cmd); break;
        case WidgetType::AlarmCard:    drawAlarmCard(parent, cmd); break;
        case WidgetType::Sparkline:    drawSparkline(parent, cmd); break;
        case WidgetType::BarGroup:     drawBarGroup(parent, cmd); break;
        case WidgetType::Gauge:        drawGauge(parent, cmd); break;
        case WidgetType::Battery:      drawBattery(parent, cmd); break;
        case WidgetType::LedRow:       drawLedRow(parent, cmd); break;
        case WidgetType::StatsCard:    drawStatsCard(parent, cmd); break;
        case WidgetType::KvGrid:       drawKvGrid(parent, cmd); break;
        case WidgetType::PinBank:      drawPinBank(parent, cmd); break;
        case WidgetType::Timeline:     drawTimeline(parent, cmd); break;
        case WidgetType::LineChart:    drawLineChart(parent, cmd); break;
        case WidgetType::Bitfield:     drawBitfield(parent, cmd); break;
        case WidgetType::HexDump:      drawHexDump(parent, cmd); break;
        case WidgetType::RegisterTable:drawRegisterTable(parent, cmd); break;
        case WidgetType::ModbusFrame:  drawFrame(parent, cmd, "MODBUS"); break;
        case WidgetType::CanFrame:     drawFrame(parent, cmd, "CAN"); break;
        case WidgetType::UartFrame:    drawFrame(parent, cmd, "UART"); break;
        case WidgetType::PacketFrame:  drawFrame(parent, cmd, "CUSTOM"); break;
        default: {
            lv_obj_t *c = card(parent, kCardBg, kCardBorder);
            label(c, cmd.typeRaw.empty() ? "widget" : cmd.typeRaw.c_str(), kSubColor, 12);
            break;
        }
    }

    // Весь виджет должен пропускать жесты во внешний скролл-вьюпорт, иначе
    // chart/arc и прочие интерактивные объекты «ловят» палец и срывают скролл.
    uint32_t n = lv_obj_get_child_count(parent);
    for (uint32_t i = 0; i < n; ++i) {
        makeInertRecursive(lv_obj_get_child(parent, i));
    }
}

// ============================================================================
//  Рисование отдельных виджетов. Все используют хелперы из анонимного
//  namespace выше (тот же translation unit).
// ============================================================================

// --- Заголовок карточки: маленькая приглушённая подпись сверху ---
static lv_obj_t *cardTitle(lv_obj_t *c, const char *title) {
    if (title == nullptr || *title == '\0') return nullptr;
    lv_obj_t *t = label(c, title, kMutedColor, 12);
    lv_obj_align(t, LV_ALIGN_TOP_LEFT, 0, 0);
    return t;
}

void drawBadge(lv_obj_t *p, const WidgetCommand &c) {
    BadgePreset preset = badgePreset(c.find({"st", "style", "preset"}));
    uint32_t bg = parseColor(c.find({"bg", "background"}),
                             preset.ok ? preset.bg : 0x1F7A1F);
    uint32_t fg = parseColor(c.find({"fg", "textcolor", "color"}),
                             preset.ok ? preset.fg : 0xFFFFFF);
    int size = parseInt(c.find({"size", "font"}), 14, 10, 16);
    const char *text = c.find({"text", "label", "title"});

    lv_obj_t *root = plain(p);
    lv_obj_set_size(root, lv_pct(100), lv_pct(100));
    lv_obj_t *pill = lv_obj_create(root);
    lv_obj_remove_style_all(pill);
    lv_obj_set_style_bg_color(pill, C(bg), 0);
    lv_obj_set_style_bg_opa(pill, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(pill, 12, 0);
    lv_obj_set_style_pad_hor(pill, 12, 0);
    lv_obj_set_style_pad_ver(pill, 4, 0);
    lv_obj_set_height(pill, LV_SIZE_CONTENT);
    lv_obj_set_width(pill, LV_SIZE_CONTENT);
    lv_obj_align(pill, LV_ALIGN_LEFT_MID, 2, 0);
    lv_obj_clear_flag(pill, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t *l = label(pill, text ? text : "BADGE", fg, size);
    lv_obj_center(l);
}

void drawDot(lv_obj_t *p, const WidgetCommand &c) {
    uint32_t color = parseColor(c.find({"color", "fg"}), kGreenLed);
    int size = parseInt(c.find({"size", "diameter"}), 14, 6, 28);
    const char *lbl = c.find({"text", "label", "title"});
    uint32_t lblColor = parseColor(c.find({"labelcolor", "textcolor"}), kTitleColor);

    lv_obj_t *root = plain(p);
    lv_obj_set_size(root, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_flow(root, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(root, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(root, 8, 0);
    lv_obj_set_style_pad_left(root, 4, 0);

    lv_obj_t *dot = lv_obj_create(root);
    lv_obj_remove_style_all(dot);
    lv_obj_set_size(dot, size, size);
    lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(dot, C(color), 0);
    lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, 0);
    if (lbl) label(root, lbl, lblColor, 14);
}

void drawImage(lv_obj_t *p, const WidgetCommand &c) {
    // На устройстве нет общего реестра drawable по имени, поэтому показываем
    // имя ресурса как «иконку-заглушку» с подписью (имя + описание).
    const char *name = c.find({"name", "icon", "image", "drawable"});
    const char *desc = c.find({"desc", "description", "text"});

    lv_obj_t *cc = card(p, kCardBg, kCardBorder);
    lv_obj_set_flex_flow(cc, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cc, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(cc, 10, 0);

    lv_obj_t *box = lv_obj_create(cc);
    lv_obj_remove_style_all(box);
    lv_obj_set_size(box, 34, 34);
    lv_obj_set_style_radius(box, 8, 0);
    lv_obj_set_style_bg_color(box, C(0x223040), 0);
    lv_obj_set_style_bg_opa(box, LV_OPA_COVER, 0);
    lv_obj_clear_flag(box, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t *sym = label(box, LV_SYMBOL_IMAGE, kSubColor, 16);
    lv_obj_center(sym);

    lv_obj_t *col = plain(cc);
    lv_obj_set_flex_grow(col, 1);
    lv_obj_set_height(col, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(col, LV_FLEX_FLOW_COLUMN);
    label(col, name ? name : "image", kTitleColor, 14);
    if (desc) label(col, desc, kMutedColor, 12);
}

void drawPanel(lv_obj_t *p, const WidgetCommand &c) {
    uint32_t accent = parseColor(c.find({"accent", "accentcolor"}), 0x4CAF50);
    uint32_t bg = parseColor(c.find({"bg", "background"}), kCardBg);
    uint32_t border = parseColor(c.find({"border"}), kCardBorder);
    const char *title = c.find({"title", "text", "label"});
    const char *value = c.find({"value", "status"});
    const char *sub = c.find({"subtitle", "sub", "caption"});

    lv_obj_t *cc = card(p, bg, border, accent);
    // Левая колонка title/subtitle, справа value.
    lv_obj_t *col = plain(cc);
    lv_obj_set_flex_flow(col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_width(col, lv_pct(70));
    lv_obj_align(col, LV_ALIGN_LEFT_MID, 4, 0);
    label(col, title ? title : "", parseColor(c.find({"titlecolor", "fg"}), kTitleColor), 14);
    if (sub) label(col, sub, parseColor(c.find({"subtitlecolor"}), kSubColor), 12);

    if (value) {
        lv_obj_t *v = label(cc, value, parseColor(c.find({"valuecolor"}), kTitleColor), 16);
        lv_obj_align(v, LV_ALIGN_RIGHT_MID, 0, 0);
    }
}

void drawProgress(lv_obj_t *p, const WidgetCommand &c) {
    float value = parseFloat(c.find({"value"}), 0);
    float maxv = parseFloat(c.find({"max"}), 100);
    if (maxv <= 0) maxv = 100;
    uint32_t fill = parseColor(c.find({"fill"}), kAccentGreen);
    uint32_t track = parseColor(c.find({"track"}), kTrackColor);
    const char *lbl = c.find({"label"});
    const char *disp = c.find({"display"});

    lv_obj_t *cc = card(p, parseColor(c.find({"bg"}), kCardBg),
                        parseColor(c.find({"border"}), kCardBorder));
    if (lbl) {
        lv_obj_t *l = label(cc, lbl, parseColor(c.find({"labelcolor"}), kTitleColor), 13);
        lv_obj_align(l, LV_ALIGN_TOP_LEFT, 0, 0);
    }
    if (disp) {
        lv_obj_t *d = label(cc, disp, parseColor(c.find({"valuecolor"}), 0xBDEFCF), 13);
        lv_obj_align(d, LV_ALIGN_TOP_RIGHT, 0, 0);
    }
    lv_obj_t *bar = lv_bar_create(cc);
    lv_obj_set_size(bar, lv_pct(100), 8);
    lv_obj_align(bar, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_radius(bar, 4, 0);
    lv_obj_set_style_bg_color(bar, C(track), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(bar, C(fill), LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_radius(bar, 4, LV_PART_INDICATOR);
    lv_bar_set_range(bar, 0, static_cast<int32_t>(maxv));
    lv_bar_set_value(bar, static_cast<int32_t>(value), LV_ANIM_OFF);
}

void drawTwoCol(lv_obj_t *p, const WidgetCommand &c) {
    const char *left = c.find({"left"});
    const char *right = c.find({"right"});
    lv_obj_t *cc = card(p, parseColor(c.find({"bg"}), 0x10151A),
                        parseColor(c.find({"border"}), 0x202C35));
    lv_obj_t *l = label(cc, left ? left : "", parseColor(c.find({"leftcolor"}), kSubColor), 14);
    lv_obj_align(l, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_t *r = label(cc, right ? right : "", parseColor(c.find({"rightcolor"}), kTitleColor), 14);
    lv_obj_align(r, LV_ALIGN_RIGHT_MID, 0, 0);
}

void drawSwitch(lv_obj_t *p, const WidgetCommand &c) {
    bool on = parseBool(c.find({"state", "checked"}), false);
    uint32_t onColor = parseColor(c.find({"oncolor"}), 0x2ECC71);
    uint32_t offColor = parseColor(c.find({"offcolor"}), 0x54616C);
    const char *lbl = c.find({"label"});
    const char *sub = c.find({"subtitle"});

    lv_obj_t *cc = card(p, parseColor(c.find({"bg"}), kCardBg),
                        parseColor(c.find({"border"}), kCardBorder));
    lv_obj_t *col = plain(cc);
    lv_obj_set_flex_flow(col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_width(col, lv_pct(70));
    lv_obj_align(col, LV_ALIGN_LEFT_MID, 0, 0);
    if (lbl) label(col, lbl, parseColor(c.find({"labelcolor"}), kTitleColor), 14);
    if (sub) label(col, sub, parseColor(c.find({"subtitlecolor"}), kSubColor), 12);

    // Нарисованный «переключатель» (без интерактива).
    lv_obj_t *tr = lv_obj_create(cc);
    lv_obj_remove_style_all(tr);
    lv_obj_set_size(tr, 44, 22);
    lv_obj_set_style_radius(tr, 11, 0);
    lv_obj_set_style_bg_color(tr, C(on ? onColor : offColor), 0);
    lv_obj_set_style_bg_opa(tr, LV_OPA_COVER, 0);
    lv_obj_align(tr, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_clear_flag(tr, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t *knob = lv_obj_create(tr);
    lv_obj_remove_style_all(knob);
    lv_obj_set_size(knob, 18, 18);
    lv_obj_set_style_radius(knob, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(knob, C(parseColor(c.find({"thumb"}), 0xFFFFFF)), 0);
    lv_obj_set_style_bg_opa(knob, LV_OPA_COVER, 0);
    lv_obj_align(knob, on ? LV_ALIGN_RIGHT_MID : LV_ALIGN_LEFT_MID, on ? -2 : 2, 0);
}

void drawStatsCard(lv_obj_t *p, const WidgetCommand &c) {
    uint32_t accent = parseColor(c.find({"accent"}), kAccentGreen);
    const char *title = c.find({"title"});
    const char *value = c.find({"value"});
    const char *unit = c.find({"unit"});
    const char *delta = c.find({"delta"});
    const char *sub = c.find({"subtitle"});

    lv_obj_t *cc = card(p, parseColor(c.find({"bg"}), kCardBg),
                        parseColor(c.find({"border"}), kCardBorder), accent);
    lv_obj_t *col = plain(cc);
    lv_obj_set_flex_flow(col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_size(col, lv_pct(100), lv_pct(100));
    lv_obj_align(col, LV_ALIGN_LEFT_MID, 4, 0);
    if (title) label(col, title, parseColor(c.find({"titlecolor"}), kSubColor), 12);

    lv_obj_t *row = plain(col);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_END);
    lv_obj_set_style_pad_column(row, 6, 0);
    lv_obj_set_height(row, LV_SIZE_CONTENT);
    char big[64];
    std::snprintf(big, sizeof(big), "%s%s%s",
                  value ? value : "", unit ? " " : "", unit ? unit : "");
    label(row, big, parseColor(c.find({"valuecolor"}), kTitleColor), 16);
    if (delta) label(row, delta, parseColor(c.find({"deltacolor"}), accent), 12);
    if (sub) label(col, sub, parseColor(c.find({"subtitlecolor"}), kMutedColor), 12);
}

void drawAlarmCard(lv_obj_t *p, const WidgetCommand &c) {
    uint32_t accent = parseColor(c.find({"accent"}), severityColor(c.find({"severity"})));
    const char *title = c.find({"title"});
    const char *msg = c.find({"message"});
    const char *time = c.find({"time"});

    lv_obj_t *cc = card(p, parseColor(c.find({"bg"}), 0x1A1012),
                        parseColor(c.find({"border"}), 0x3A2326), accent);
    lv_obj_t *col = plain(cc);
    lv_obj_set_flex_flow(col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_size(col, lv_pct(100), lv_pct(100));
    lv_obj_align(col, LV_ALIGN_TOP_LEFT, 4, 0);
    lv_obj_set_style_pad_row(col, 2, 0);

    lv_obj_t *hr = plain(col);
    lv_obj_set_flex_flow(hr, LV_FLEX_FLOW_ROW);
    lv_obj_set_width(hr, lv_pct(100));
    lv_obj_set_height(hr, LV_SIZE_CONTENT);
    lv_obj_set_flex_align(hr, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    label(hr, title ? title : "ALARM", parseColor(c.find({"titlecolor"}), 0xFFFFFF), 14);
    if (time) label(hr, time, parseColor(c.find({"metacolor"}), kMutedColor), 12);

    if (msg) {
        lv_obj_t *m = label(col, msg, parseColor(c.find({"messagecolor"}), kSubColor), 12);
        lv_label_set_long_mode(m, LV_LABEL_LONG_MODE_WRAP);
        lv_obj_set_width(m, lv_pct(100));
    }
}

void drawLedRow(lv_obj_t *p, const WidgetCommand &c) {
    auto items = splitList(c.find({"items"}), "|");
    const char *title = c.find({"title"});
    uint32_t offColor = parseColor(c.find({"offcolor"}), 0x54616C);

    lv_obj_t *cc = card(p, parseColor(c.find({"bg"}), kCardBg),
                        parseColor(c.find({"border"}), kCardBorder));
    if (title) cardTitle(cc, title);
    lv_obj_t *row = plain(cc);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_width(row, lv_pct(100));
    lv_obj_set_height(row, LV_SIZE_CONTENT);
    lv_obj_align(row, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_set_style_pad_column(row, 12, 0);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    for (const std::string &it : items) {
        size_t colon = it.find(':');
        std::string name = colon == std::string::npos ? it : it.substr(0, colon);
        std::string state = colon == std::string::npos ? "" : it.substr(colon + 1);
        uint32_t col = stateToColor(state, offColor);

        lv_obj_t *cell = plain(row);
        lv_obj_set_flex_flow(cell, LV_FLEX_FLOW_ROW);
        lv_obj_set_height(cell, LV_SIZE_CONTENT);
        lv_obj_set_width(cell, LV_SIZE_CONTENT);
        lv_obj_set_style_pad_column(cell, 4, 0);
        lv_obj_set_flex_align(cell, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_t *led = lv_obj_create(cell);
        lv_obj_remove_style_all(led);
        lv_obj_set_size(led, 10, 10);
        lv_obj_set_style_radius(led, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(led, C(col), 0);
        lv_obj_set_style_bg_opa(led, LV_OPA_COVER, 0);
        label(cell, name.c_str(), parseColor(c.find({"labelcolor"}), kValueColor), 12);
    }
}

void drawKvGrid(lv_obj_t *p, const WidgetCommand &c) {
    auto items = splitList(c.find({"items"}), "|");
    int cols = parseInt(c.find({"columns"}), 2, 1, 3);
    const char *title = c.find({"title"});

    lv_obj_t *cc = card(p, parseColor(c.find({"bg"}), kCardBg),
                        parseColor(c.find({"border"}), kCardBorder));
    if (title) cardTitle(cc, title);
    lv_obj_t *grid = plain(cc);
    lv_obj_set_width(grid, lv_pct(100));
    lv_obj_set_height(grid, LV_SIZE_CONTENT);
    lv_obj_align(grid, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_set_flex_flow(grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_row(grid, 2, 0);
    lv_obj_set_style_pad_column(grid, 10, 0);

    for (const std::string &it : items) {
        size_t colon = it.find(':');
        std::string key = colon == std::string::npos ? it : it.substr(0, colon);
        std::string val = colon == std::string::npos ? "" : it.substr(colon + 1);
        lv_obj_t *cell = plain(grid);
        lv_obj_set_width(cell, lv_pct(100 / cols - 3));
        lv_obj_set_height(cell, LV_SIZE_CONTENT);
        lv_obj_set_flex_flow(cell, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(cell, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        label(cell, key.c_str(), parseColor(c.find({"keycolor"}), kMutedColor), 12);
        label(cell, val.c_str(), parseColor(c.find({"valuecolor"}), kTitleColor), 12);
    }
}

void drawPinBank(lv_obj_t *p, const WidgetCommand &c) {
    auto items = splitList(c.find({"items"}), "|");
    int cols = parseInt(c.find({"columns"}), 3, 1, 4);
    const char *title = c.find({"title"});

    lv_obj_t *cc = card(p, parseColor(c.find({"bg"}), kCardBg),
                        parseColor(c.find({"border"}), kCardBorder));
    if (title) cardTitle(cc, title);
    lv_obj_t *grid = plain(cc);
    lv_obj_set_width(grid, lv_pct(100));
    lv_obj_set_height(grid, LV_SIZE_CONTENT);
    lv_obj_align(grid, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_set_flex_flow(grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_row(grid, 2, 0);
    lv_obj_set_style_pad_column(grid, 8, 0);

    for (const std::string &it : items) {
        size_t colon = it.find(':');
        std::string pin = colon == std::string::npos ? it : it.substr(0, colon);
        std::string state = colon == std::string::npos ? "" : it.substr(colon + 1);
        uint32_t col = stateToColor(state, 0x54616C);
        lv_obj_t *cell = plain(grid);
        lv_obj_set_width(cell, lv_pct(100 / cols - 3));
        lv_obj_set_height(cell, LV_SIZE_CONTENT);
        lv_obj_set_flex_flow(cell, LV_FLEX_FLOW_ROW);
        lv_obj_set_style_pad_column(cell, 4, 0);
        lv_obj_set_flex_align(cell, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_t *led = lv_obj_create(cell);
        lv_obj_remove_style_all(led);
        lv_obj_set_size(led, 8, 8);
        lv_obj_set_style_radius(led, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(led, C(col), 0);
        lv_obj_set_style_bg_opa(led, LV_OPA_COVER, 0);
        label(cell, pin.c_str(), parseColor(c.find({"pincolor"}), kTitleColor), 12);
        if (!state.empty()) label(cell, state.c_str(), parseColor(c.find({"statecolor"}), kSubColor), 11);
    }
}

void drawTimeline(lv_obj_t *p, const WidgetCommand &c) {
    auto items = splitList(c.find({"items"}), "|");
    const char *title = c.find({"title"});
    uint32_t line = parseColor(c.find({"line", "color"}), kAccentGreen);

    lv_obj_t *cc = card(p, parseColor(c.find({"bg"}), kCardBg),
                        parseColor(c.find({"border"}), kCardBorder));
    if (title) cardTitle(cc, title);
    lv_obj_t *col = plain(cc);
    lv_obj_set_width(col, lv_pct(100));
    lv_obj_set_height(col, LV_SIZE_CONTENT);
    lv_obj_align(col, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_set_flex_flow(col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(col, 2, 0);

    for (const std::string &it : items) {
        // Формат "TIME TEXT" или расширенный "TIME~TEXT~SUB~COLOR".
        std::string time, text;
        if (it.find('~') != std::string::npos) {
            auto parts = splitList(it.c_str(), "~");
            time = parts.size() > 0 ? parts[0] : "";
            text = parts.size() > 1 ? parts[1] : "";
        } else {
            size_t sp = it.find(' ');
            time = sp == std::string::npos ? "" : it.substr(0, sp);
            text = sp == std::string::npos ? it : it.substr(sp + 1);
        }
        lv_obj_t *row = plain(col);
        lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
        lv_obj_set_width(row, lv_pct(100));
        lv_obj_set_height(row, LV_SIZE_CONTENT);
        lv_obj_set_style_pad_column(row, 6, 0);
        lv_obj_set_flex_align(row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_t *dot = lv_obj_create(row);
        lv_obj_remove_style_all(dot);
        lv_obj_set_size(dot, 7, 7);
        lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(dot, C(line), 0);
        lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, 0);
        if (!time.empty()) label(row, time.c_str(), parseColor(c.find({"timecolor"}), kMutedColor), 12);
        label(row, text.c_str(), parseColor(c.find({"textcolor"}), kTitleColor), 12);
    }
}

void drawTable(lv_obj_t *p, const WidgetCommand &c) {
    auto headers = splitList(c.find({"headers"}), "|");
    auto rows = splitRows(c.find({"rows"}));
    int ncol = static_cast<int>(headers.size());
    for (auto &r : rows) ncol = std::max(ncol, static_cast<int>(r.size()));
    if (ncol == 0) ncol = 1;

    lv_obj_t *cc = card(p, parseColor(c.find({"bg"}), 0x10151A),
                        parseColor(c.find({"border"}), 0x25323B));
    lv_obj_set_style_pad_all(cc, 4, 0);
    lv_obj_t *col = plain(cc);
    lv_obj_set_size(col, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_flow(col, LV_FLEX_FLOW_COLUMN);

    auto makeRow = [&](const std::vector<std::string> &cells, bool header) {
        lv_obj_t *row = plain(col);
        lv_obj_set_width(row, lv_pct(100));
        lv_obj_set_height(row, LV_SIZE_CONTENT);
        lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
        if (header) {
            lv_obj_set_style_bg_color(row, C(parseColor(c.find({"headerbg"}), 0x1A2630)), 0);
            lv_obj_set_style_bg_opa(row, LV_OPA_COVER, 0);
            lv_obj_set_style_radius(row, 4, 0);
        }
        lv_obj_set_style_pad_ver(row, 2, 0);
        for (int i = 0; i < ncol; ++i) {
            const char *txt = i < static_cast<int>(cells.size()) ? cells[i].c_str() : "";
            lv_obj_t *l = label(row, txt,
                header ? parseColor(c.find({"headercolor"}), kTitleColor)
                       : parseColor(c.find({"cellcolor"}), kValueColor), 12);
            lv_obj_set_width(l, lv_pct(100 / ncol));
            lv_obj_set_style_pad_left(l, 4, 0);
        }
    };
    if (!headers.empty()) makeRow(headers, true);
    for (const auto &r : rows) makeRow(r, false);
}

void drawBitfield(lv_obj_t *p, const WidgetCommand &c) {
    int bits = parseInt(c.find({"bits"}), 8, 1, 64);
    // kind=byte/short/word как альтернатива.
    if (const char *kind = c.find({"kind"})) {
        std::string k(kind);
        if (k == "byte") bits = 8; else if (k == "short") bits = 16; else if (k == "word") bits = 32;
    }
    uint64_t value = parseUnsigned(c.find({"value"}), 0);
    uint32_t setCol = parseColor(c.find({"setcolor"}), kAccentGreen);
    uint32_t clrCol = parseColor(c.find({"clearcolor"}), 0x202A31);
    const char *lbl = c.find({"label"});

    lv_obj_t *cc = card(p, parseColor(c.find({"bg"}), kCardBg),
                        parseColor(c.find({"border"}), kCardBorder));
    char head[64];
    std::snprintf(head, sizeof(head), "%s  0x%llX",
                  lbl ? lbl : "BITS", (unsigned long long)value);
    cardTitle(cc, head);

    lv_obj_t *grid = plain(cc);
    lv_obj_set_width(grid, lv_pct(100));
    lv_obj_set_height(grid, LV_SIZE_CONTENT);
    lv_obj_align(grid, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_set_flex_flow(grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_row(grid, 3, 0);
    lv_obj_set_style_pad_column(grid, 3, 0);

    for (int i = bits - 1; i >= 0; --i) {
        bool set = (value >> i) & 1ULL;
        lv_obj_t *cell = lv_obj_create(grid);
        lv_obj_remove_style_all(cell);
        lv_obj_set_size(cell, 16, 16);
        lv_obj_set_style_radius(cell, 3, 0);
        lv_obj_set_style_bg_color(cell, C(set ? setCol : clrCol), 0);
        lv_obj_set_style_bg_opa(cell, LV_OPA_COVER, 0);
        lv_obj_clear_flag(cell, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_t *t = label(cell, set ? "1" : "0",
                            set ? 0x05140A : parseColor(c.find({"indexcolor"}), kMutedColor), 12);
        lv_obj_center(t);
    }
}

void drawHexDump(lv_obj_t *p, const WidgetCommand &c) {
    auto bytes = parseHexBytes(c.find({"data", "bytes"}));
    int width = parseInt(c.find({"width"}), 8, 1, 32);
    int addr = parseInt(c.find({"addr", "address"}), 0, 0, 0x7FFFFFFF);
    bool ascii = parseBool(c.find({"ascii"}), true);
    const char *title = c.find({"title"});

    lv_obj_t *cc = card(p, parseColor(c.find({"bg"}), kCardBg),
                        parseColor(c.find({"border"}), kCardBorder));
    if (title) cardTitle(cc, title);
    lv_obj_t *col = plain(cc);
    lv_obj_set_width(col, lv_pct(100));
    lv_obj_set_height(col, LV_SIZE_CONTENT);
    lv_obj_align(col, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_set_flex_flow(col, LV_FLEX_FLOW_COLUMN);

    int n = static_cast<int>(bytes.size());
    for (int off = 0; off < n || off == 0; off += width) {
        std::string lineStr;
        char addrBuf[16];
        std::snprintf(addrBuf, sizeof(addrBuf), "%04X ", addr + off);
        lineStr += addrBuf;
        std::string asciiStr;
        for (int i = 0; i < width; ++i) {
            int idx = off + i;
            if (idx < n) {
                char b[4];
                std::snprintf(b, sizeof(b), "%02X ", bytes[idx]);
                lineStr += b;
                char ch = static_cast<char>(bytes[idx]);
                asciiStr += (ch >= 32 && ch < 127) ? ch : '.';
            } else {
                lineStr += "   ";
                asciiStr += ' ';
            }
        }
        if (ascii) { lineStr += " "; lineStr += asciiStr; }
        lv_obj_t *l = label(col, lineStr.c_str(), parseColor(c.find({"bytecolor"}), kValueColor), 12);
        lv_obj_set_style_text_font(l, &lv_font_montserrat_12, 0);
        if (off + width >= n) break;
    }
}

void drawRegisterTable(lv_obj_t *p, const WidgetCommand &c) {
    auto rows = splitRows(c.find({"rows", "registers"}));
    const char *title = c.find({"title"});

    lv_obj_t *cc = card(p, parseColor(c.find({"bg"}), kCardBg),
                        parseColor(c.find({"border"}), kCardBorder));
    if (title) cardTitle(cc, title);
    lv_obj_t *col = plain(cc);
    lv_obj_set_width(col, lv_pct(100));
    lv_obj_set_height(col, LV_SIZE_CONTENT);
    lv_obj_align(col, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_set_flex_flow(col, LV_FLEX_FLOW_COLUMN);

    // Шапка.
    {
        lv_obj_t *hr = plain(col);
        lv_obj_set_width(hr, lv_pct(100));
        lv_obj_set_height(hr, LV_SIZE_CONTENT);
        lv_obj_set_flex_flow(hr, LV_FLEX_FLOW_ROW);
        const char *hs[] = {"Addr", "Value", "Desc"};
        int w[] = {22, 28, 50};
        for (int i = 0; i < 3; ++i) {
            lv_obj_t *l = label(hr, hs[i], parseColor(c.find({"headercolor"}), kMutedColor), 12);
            lv_obj_set_width(l, lv_pct(w[i]));
        }
    }
    for (const auto &r : rows) {
        lv_obj_t *rr = plain(col);
        lv_obj_set_width(rr, lv_pct(100));
        lv_obj_set_height(rr, LV_SIZE_CONTENT);
        lv_obj_set_flex_flow(rr, LV_FLEX_FLOW_ROW);
        int w[] = {22, 28, 50};
        uint32_t cols[] = {parseColor(c.find({"addresscolor"}), kMutedColor),
                           parseColor(c.find({"valuecolor"}), kValueColor),
                           parseColor(c.find({"descriptioncolor"}), kSubColor)};
        for (int i = 0; i < 3; ++i) {
            const char *txt = i < static_cast<int>(r.size()) ? r[i].c_str() : "";
            lv_obj_t *l = label(rr, txt, cols[i], 12);
            lv_obj_set_width(l, lv_pct(w[i]));
        }
    }
}

// --- Графические виджеты через lv_chart / lv_arc ---

void drawSparkline(lv_obj_t *p, const WidgetCommand &c) {
    auto vals = toFloats(splitList(c.find({"values"}), "|,;"));
    const char *lbl = c.find({"label"});
    const char *disp = c.find({"display"});
    uint32_t color = parseColor(c.find({"color"}), kAccentGreen);

    lv_obj_t *cc = card(p, parseColor(c.find({"bg"}), kCardBg),
                        parseColor(c.find({"border"}), kCardBorder));
    if (lbl) {
        lv_obj_t *l = label(cc, lbl, parseColor(c.find({"labelcolor"}), kTitleColor), 12);
        lv_obj_align(l, LV_ALIGN_TOP_LEFT, 0, 0);
    }
    if (disp) {
        lv_obj_t *d = label(cc, disp, parseColor(c.find({"valuecolor"}), 0xBDEFCF), 12);
        lv_obj_align(d, LV_ALIGN_TOP_RIGHT, 0, 0);
    }
    lv_obj_t *chart = lv_chart_create(cc);
    lv_obj_set_size(chart, lv_pct(100), lv_pct(55));
    lv_obj_align(chart, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_opa(chart, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(chart, 0, 0);
    lv_obj_set_style_pad_all(chart, 0, 0);
    lv_chart_set_div_line_count(chart, 0, 0);
    lv_obj_set_style_size(chart, 0, 0, LV_PART_INDICATOR);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    int cnt = std::max<int>(2, static_cast<int>(vals.size()));
    lv_chart_set_point_count(chart, cnt);
    float mn = parseFloat(c.find({"min"}), 0), mx = parseFloat(c.find({"max"}), 0);
    if (c.find({"min"}) == nullptr || c.find({"max"}) == nullptr) {
        mn = 1e9f; mx = -1e9f;
        for (float v : vals) { mn = std::min(mn, v); mx = std::max(mx, v); }
        if (mn > mx) { mn = 0; mx = 1; }
        if (mn == mx) { mn -= 1; mx += 1; }
    }
    lv_chart_set_axis_range(chart, LV_CHART_AXIS_PRIMARY_Y, (int32_t)mn, (int32_t)mx);
    lv_chart_series_t *ser = lv_chart_add_series(chart, C(color), LV_CHART_AXIS_PRIMARY_Y);
    for (size_t i = 0; i < vals.size(); ++i) {
        lv_chart_set_next_value(chart, ser, (int32_t)vals[i]);
    }
}

void drawLineChart(lv_obj_t *p, const WidgetCommand &c) {
    auto vals = toFloats(splitList(c.find({"values"}), "|,;"));
    const char *title = c.find({"title"});
    uint32_t color = parseColor(c.find({"color"}), 0x4FC3F7);

    lv_obj_t *cc = card(p, parseColor(c.find({"bg"}), kCardBg),
                        parseColor(c.find({"border"}), kCardBorder));
    if (title) cardTitle(cc, title);
    lv_obj_t *chart = lv_chart_create(cc);
    lv_obj_set_size(chart, lv_pct(100), lv_pct(70));
    lv_obj_align(chart, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_opa(chart, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(chart, 0, 0);
    lv_obj_set_style_pad_all(chart, 0, 0);
    lv_obj_set_style_line_color(chart, C(parseColor(c.find({"axiscolor"}), 0x30404C)), LV_PART_MAIN);
    lv_chart_set_div_line_count(chart, 3, 4);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    int cnt = std::max<int>(2, static_cast<int>(vals.size()));
    lv_chart_set_point_count(chart, cnt);
    float mn = parseFloat(c.find({"min"}), 0), mx = parseFloat(c.find({"max"}), 0);
    if (c.find({"min"}) == nullptr || c.find({"max"}) == nullptr) {
        mn = 1e9f; mx = -1e9f;
        for (float v : vals) { mn = std::min(mn, v); mx = std::max(mx, v); }
        if (mn > mx) { mn = 0; mx = 1; }
        if (mn == mx) { mn -= 1; mx += 1; }
    }
    lv_chart_set_axis_range(chart, LV_CHART_AXIS_PRIMARY_Y, (int32_t)mn, (int32_t)mx);
    lv_chart_series_t *ser = lv_chart_add_series(chart, C(color), LV_CHART_AXIS_PRIMARY_Y);
    for (size_t i = 0; i < vals.size(); ++i) {
        lv_chart_set_next_value(chart, ser, (int32_t)vals[i]);
    }
}

void drawBarGroup(lv_obj_t *p, const WidgetCommand &c) {
    auto labels = splitList(c.find({"labels"}), "|");
    auto vals = toFloats(splitList(c.find({"values"}), "|,;"));
    auto colorStrs = splitList(c.find({"colors"}), "|");
    const char *title = c.find({"title"});
    float maxv = parseFloat(c.find({"max"}), 0);
    if (maxv <= 0) { for (float v : vals) maxv = std::max(maxv, v); if (maxv <= 0) maxv = 1; }
    uint32_t baseColor = parseColor(c.find({"color"}), kAmberLed);

    lv_obj_t *cc = card(p, parseColor(c.find({"bg"}), kCardBg),
                        parseColor(c.find({"border"}), kCardBorder));
    if (title) cardTitle(cc, title);
    lv_obj_t *row = plain(cc);
    lv_obj_set_size(row, lv_pct(100), lv_pct(70));
    lv_obj_align(row, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_END);

    for (size_t i = 0; i < vals.size(); ++i) {
        uint32_t col = baseColor;
        if (i < colorStrs.size()) col = parseColor(colorStrs[i].c_str(), baseColor);
        lv_obj_t *colBox = plain(row);
        lv_obj_set_flex_flow(colBox, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(colBox, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_END);
        lv_obj_set_height(colBox, lv_pct(100));
        lv_obj_set_width(colBox, LV_SIZE_CONTENT);
        lv_obj_set_style_pad_row(colBox, 2, 0);

        int pct = static_cast<int>((vals[i] / maxv) * 70.0f);
        if (pct < 4) pct = 4;
        if (pct > 100) pct = 100;
        lv_obj_t *bar = lv_obj_create(colBox);
        lv_obj_remove_style_all(bar);
        lv_obj_set_size(bar, 16, lv_pct(pct));
        lv_obj_set_style_bg_color(bar, C(col), 0);
        lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, 0);
        lv_obj_set_style_radius(bar, 3, 0);
        lv_obj_clear_flag(bar, LV_OBJ_FLAG_SCROLLABLE);
        if (i < labels.size())
            label(colBox, labels[i].c_str(), parseColor(c.find({"labelcolor"}), kSubColor), 11);
    }
}

void drawGauge(lv_obj_t *p, const WidgetCommand &c) {
    float value = parseFloat(c.find({"value"}), 0);
    float maxv = parseFloat(c.find({"max"}), 100);
    if (maxv <= 0) maxv = 100;
    uint32_t color = parseColor(c.find({"color"}), kAccentGreen);
    const char *lbl = c.find({"label"});
    const char *unit = c.find({"unit"});
    const char *disp = c.find({"display"});

    lv_obj_t *cc = card(p, parseColor(c.find({"bg"}), kCardBg),
                        parseColor(c.find({"border"}), kCardBorder));
    if (lbl) cardTitle(cc, lbl);

    lv_obj_t *arc = lv_arc_create(cc);
    lv_obj_set_size(arc, 60, 60);
    lv_obj_align(arc, LV_ALIGN_CENTER, 0, 6);
    lv_arc_set_rotation(arc, 135);
    lv_arc_set_bg_angles(arc, 0, 270);
    lv_arc_set_range(arc, 0, (int32_t)maxv);
    lv_arc_set_value(arc, (int32_t)value);
    lv_obj_remove_flag(arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_arc_width(arc, 6, LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc, C(parseColor(c.find({"track"}), kTrackColor)), LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc, 6, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc, C(color), LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(arc, LV_OPA_TRANSP, LV_PART_KNOB);
    lv_obj_set_style_pad_all(arc, 0, LV_PART_KNOB);

    char center[32];
    if (disp) std::snprintf(center, sizeof(center), "%s", disp);
    else std::snprintf(center, sizeof(center), "%d%s", (int)value, unit ? unit : "");
    lv_obj_t *ct = label(arc, center, parseColor(c.find({"valuecolor"}), kTitleColor), 14);
    lv_obj_center(ct);
}

void drawBattery(lv_obj_t *p, const WidgetCommand &c) {
    float value = parseFloat(c.find({"value"}), 0);
    float maxv = parseFloat(c.find({"max"}), 100);
    if (maxv <= 0) maxv = 100;
    int pct = static_cast<int>((value / maxv) * 100.0f);
    if (pct < 0) pct = 0;
    if (pct > 100) pct = 100;
    bool charging = parseBool(c.find({"charging"}), false);
    const char *lbl = c.find({"label"});
    const char *volt = c.find({"voltage"});
    uint32_t fill = parseColor(c.find({"fill"}), pct < 20 ? kRedLed : (pct < 50 ? kAmberLed : kAccentGreen));

    lv_obj_t *cc = card(p, parseColor(c.find({"bg"}), kCardBg),
                        parseColor(c.find({"border"}), kCardBorder));
    if (lbl) cardTitle(cc, lbl);

    // Корпус батареи.
    lv_obj_t *body = lv_obj_create(cc);
    lv_obj_remove_style_all(body);
    lv_obj_set_size(body, 70, 28);
    lv_obj_align(body, LV_ALIGN_LEFT_MID, 4, 6);
    lv_obj_set_style_radius(body, 4, 0);
    lv_obj_set_style_border_width(body, 2, 0);
    lv_obj_set_style_border_color(body, C(0x55636E), 0);
    lv_obj_set_style_pad_all(body, 2, 0);
    lv_obj_clear_flag(body, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t *lvl = lv_obj_create(body);
    lv_obj_remove_style_all(lvl);
    lv_obj_set_size(lvl, lv_pct(pct), lv_pct(100));
    lv_obj_align(lvl, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_bg_color(lvl, C(fill), 0);
    lv_obj_set_style_bg_opa(lvl, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(lvl, 2, 0);
    // «Пятачок» батареи.
    lv_obj_t *tip = lv_obj_create(cc);
    lv_obj_remove_style_all(tip);
    lv_obj_set_size(tip, 4, 12);
    lv_obj_align_to(tip, body, LV_ALIGN_OUT_RIGHT_MID, 1, 0);
    lv_obj_set_style_bg_color(tip, C(0x55636E), 0);
    lv_obj_set_style_bg_opa(tip, LV_OPA_COVER, 0);

    char txt[48];
    std::snprintf(txt, sizeof(txt), "%d%%%s%s%s", pct,
                  charging ? "  " LV_SYMBOL_CHARGE : "",
                  volt ? "  " : "", volt ? volt : "");
    lv_obj_t *t = label(cc, txt, parseColor(c.find({"valuecolor"}), 0xBDEFCF), 14);
    lv_obj_align(t, LV_ALIGN_RIGHT_MID, 0, 6);
}

void drawFrame(lv_obj_t *p, const WidgetCommand &c, const char *defProto) {
    auto bytes = parseHexBytes(c.find({"data", "bytes", "frame"}));
    auto fields = splitRows(c.find({"fields"}));
    const char *title = c.find({"title"});
    const char *dir = c.find({"direction"});
    const char *chan = c.find({"channel", "bus", "port"});
    const char *proto = c.find({"protocol"});
    const char *id = c.find({"id", "canid"});
    uint32_t accent = parseColor(c.find({"accent"}), 0x4FC3F7);

    lv_obj_t *cc = card(p, parseColor(c.find({"bg"}), kCardBg),
                        parseColor(c.find({"border"}), kCardBorder), accent);
    lv_obj_t *col = plain(cc);
    lv_obj_set_size(col, lv_pct(100), lv_pct(100));
    lv_obj_align(col, LV_ALIGN_TOP_LEFT, 4, 0);
    lv_obj_set_flex_flow(col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(col, 1, 0);

    // Заголовочная строка.
    char head[96];
    std::snprintf(head, sizeof(head), "%s%s%s%s",
                  title ? title : (proto ? proto : defProto),
                  dir ? "  " : "", dir ? dir : "",
                  "");
    lv_obj_t *hr = plain(col);
    lv_obj_set_flex_flow(hr, LV_FLEX_FLOW_ROW);
    lv_obj_set_width(hr, lv_pct(100));
    lv_obj_set_height(hr, LV_SIZE_CONTENT);
    lv_obj_set_flex_align(hr, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    label(hr, head, kTitleColor, 13);
    char meta[64] = {0};
    if (id) {
        std::snprintf(meta, sizeof(meta), "%s", id);
    } else if (chan) {
        std::snprintf(meta, sizeof(meta), "%s", chan);
    }
    if (meta[0]) label(hr, meta, parseColor(c.find({"metacolor"}), kMutedColor), 12);

    // Строка байтов.
    std::string bytesStr;
    char b[4];
    for (size_t i = 0; i < bytes.size() && i < 32; ++i) {
        std::snprintf(b, sizeof(b), "%02X ", bytes[i]);
        bytesStr += b;
    }
    if (!bytesStr.empty()) {
        lv_obj_t *bl = label(col, bytesStr.c_str(), parseColor(c.find({"bytecolor"}), kValueColor), 12);
        lv_obj_set_width(bl, lv_pct(100));
        lv_label_set_long_mode(bl, LV_LABEL_LONG_MODE_WRAP);
    }
    // meta-строка (channel/baud).
    const char *baud = c.find({"baud", "speed"});
    if (chan || baud) {
        char m[64];
        std::snprintf(m, sizeof(m), "%s%s%s", chan ? chan : "",
                      (chan && baud) ? " @ " : "", baud ? baud : "");
        label(col, m, kMutedColor, 11);
    }
    // Поля: RANGE|NAME|VALUE|DESC.
    for (const auto &f : fields) {
        std::string line;
        if (f.size() > 1) line = f[1];
        if (f.size() > 2 && !f[2].empty()) line += " = " + f[2];
        if (f.size() > 3 && !f[3].empty()) line += "  (" + f[3] + ")";
        lv_obj_t *fl = label(col, line.c_str(), parseColor(c.find({"fieldnamecolor"}), kSubColor), 11);
        lv_obj_set_width(fl, lv_pct(100));
    }
}

}  // namespace timber
