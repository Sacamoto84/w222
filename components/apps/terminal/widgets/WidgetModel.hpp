#pragma once

// Модель данных консольных виджетов для ESP32-терминала.
//
// Это порт протокола `ui type=<widget> key=value ...` из Android-приложения
// (см. Android_Terminal/app/.../console/README.md и TimberWidget).
//
// Важно: этот заголовок НЕ зависит от LVGL и Arduino — только STL.
// Благодаря этому парсер можно собирать и юнит-тестировать на хосте.
// LVGL-рендер живёт отдельно в WidgetRender.{hpp,cpp}.

#include <cstdint>
#include <string>
#include <vector>

namespace timber {

// Полный список поддержанных типов виджетов (26 штук + Unknown).
enum class WidgetType : uint8_t {
    Unknown = 0,
    Badge,
    Dot,
    Image,
    Panel,
    Progress,
    TwoCol,
    Table,
    Switch,
    AlarmCard,
    Sparkline,
    BarGroup,
    Gauge,
    Battery,
    LedRow,
    StatsCard,
    KvGrid,
    PinBank,
    Timeline,
    LineChart,
    Bitfield,
    HexDump,
    RegisterTable,
    ModbusFrame,
    CanFrame,
    UartFrame,
    PacketFrame,
};

// Результат разбора одной завершённой строки потока.
enum class LineKind : uint8_t {
    Text,            // обычная текстовая строка терминала (не команда)
    Widget,          // ui/widget type=... — получился виджет
    ClearTerminal,   // clear-terminal [N]
    Beep,            // beep — служебная
    DemoWidgets,     // demo-widgets / demo — проиграть весь демо-набор
    Invalid,         // выглядело как команда ui, но не разобралось
};

// Одна пара key=value из команды.
struct WidgetAttr {
    std::string key;
    std::string value;
};

// Разобранная команда виджета: тип, канал, slot и набор атрибутов.
// Рендер читает атрибуты напрямую через хелперы ниже.
struct WidgetCommand {
    WidgetType type = WidgetType::Unknown;
    std::string typeRaw;                 // исходная строка type (для отладки)
    uint8_t channel = 0;                 // 0..3, из префикса @N или ключа
    int slot = -1;                       // slot/index/pos/position, -1 = нет
    std::vector<WidgetAttr> attrs;

    // Возвращает значение по первому совпавшему ключу, иначе nullptr.
    const char *find(std::initializer_list<const char *> keys) const;
    bool has(std::initializer_list<const char *> keys) const { return find(keys) != nullptr; }
};

// ------- Хелперы разбора значений (host-safe) -------

// Цвет в формате 0x00RRGGBB. Понимает #RRGGBB, #AARRGGBB (alpha отбрасывается),
// именованные цвета (black/white/red/...) и возвращает defaultRgb если не понял.
uint32_t parseColor(const char *value, uint32_t defaultRgb);

// Булево значение: on/off, true/false, 1/0, yes/no, active/...
bool parseBool(const char *value, bool defaultValue = false);

// Целое с клампом в [minVal, maxVal]; поддерживает 0x.. и десятичное.
int parseInt(const char *value, int defaultValue, int minVal, int maxVal);

// Число с плавающей точкой.
float parseFloat(const char *value, float defaultValue);

// Беззнаковое значение из hex (0x..), bin (0b..) или dec.
uint64_t parseUnsigned(const char *value, uint64_t defaultValue);

// Разбивает строку списка по любому из разделителей |,; (без пустых концов
// для висячих разделителей). Используется для values/labels/items/rows.
std::vector<std::string> splitList(const char *value, const char *separators = "|,;");

// Разбивает на строки таблицы по ';', каждую строку — по '|'.
std::vector<std::vector<std::string>> splitRows(const char *value);

// Парсит hex-строку байтов: "AA 55 10", "AA,55", или слитно "AA5510".
std::vector<uint8_t> parseHexBytes(const char *value);

}  // namespace timber
