#include "WidgetParser.hpp"

#include <cctype>
#include <cstring>

namespace timber {

namespace {

std::string toLower(const std::string &in) {
    std::string out(in.size(), '\0');
    for (size_t i = 0; i < in.size(); ++i) {
        out[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(in[i])));
    }
    return out;
}

std::string trim(const std::string &in) {
    size_t a = 0, b = in.size();
    while (a < b && std::isspace(static_cast<unsigned char>(in[a]))) ++a;
    while (b > a && std::isspace(static_cast<unsigned char>(in[b - 1]))) --b;
    return in.substr(a, b - a);
}

// Токенизирует "key=value key=\"two words\" type=badge" в список key=value,
// уважая одинарные и двойные кавычки. Возвращает токены как сырые строки
// (с уже снятыми кавычками у значения).
struct Token {
    std::string key;
    std::string value;
    bool hasEquals = false;
};

std::vector<Token> tokenize(const std::string &body) {
    std::vector<Token> tokens;
    size_t i = 0;
    const size_t n = body.size();

    while (i < n) {
        // Пропустить пробелы между токенами.
        while (i < n && std::isspace(static_cast<unsigned char>(body[i]))) ++i;
        if (i >= n) break;

        std::string key;
        std::string value;
        bool hasEquals = false;

        // Читаем ключ до '=' или пробела.
        while (i < n && !std::isspace(static_cast<unsigned char>(body[i])) && body[i] != '=') {
            key.push_back(body[i]);
            ++i;
        }

        if (i < n && body[i] == '=') {
            hasEquals = true;
            ++i;  // съесть '='
            if (i < n && (body[i] == '"' || body[i] == '\'')) {
                char quote = body[i++];
                while (i < n && body[i] != quote) {
                    if (body[i] == '\\' && i + 1 < n) {
                        // escape: \" \\ -> символ
                        ++i;
                        value.push_back(body[i]);
                    } else {
                        value.push_back(body[i]);
                    }
                    ++i;
                }
                if (i < n) ++i;  // съесть закрывающую кавычку
            } else {
                // Значение без кавычек — до следующего пробела.
                while (i < n && !std::isspace(static_cast<unsigned char>(body[i]))) {
                    value.push_back(body[i]);
                    ++i;
                }
            }
        }

        Token t;
        t.key = key;
        t.value = value;
        t.hasEquals = hasEquals;
        tokens.push_back(std::move(t));
    }
    return tokens;
}

}  // namespace

WidgetType widgetTypeFromString(const std::string &t) {
    struct Entry { const char *name; WidgetType type; };
    static const Entry kTable[] = {
        {"badge", WidgetType::Badge},
        {"dot", WidgetType::Dot}, {"circle", WidgetType::Dot},
        {"image", WidgetType::Image}, {"icon", WidgetType::Image},
        {"panel", WidgetType::Panel}, {"card", WidgetType::Panel},
        {"progress", WidgetType::Progress}, {"bar", WidgetType::Progress},
        {"2col", WidgetType::TwoCol}, {"twocol", WidgetType::TwoCol}, {"pair", WidgetType::TwoCol},
        {"table", WidgetType::Table}, {"grid", WidgetType::Table},
        {"switch", WidgetType::Switch}, {"toggle", WidgetType::Switch},
        {"alarm-card", WidgetType::AlarmCard}, {"alarm", WidgetType::AlarmCard}, {"alert", WidgetType::AlarmCard},
        {"sparkline", WidgetType::Sparkline}, {"trend", WidgetType::Sparkline},
        {"bar-group", WidgetType::BarGroup}, {"bars", WidgetType::BarGroup}, {"columns", WidgetType::BarGroup},
        {"gauge", WidgetType::Gauge}, {"dial", WidgetType::Gauge},
        {"battery", WidgetType::Battery}, {"cell", WidgetType::Battery},
        {"led-row", WidgetType::LedRow}, {"leds", WidgetType::LedRow}, {"status-row", WidgetType::LedRow},
        {"stats-card", WidgetType::StatsCard}, {"stat", WidgetType::StatsCard}, {"metric-card", WidgetType::StatsCard},
        {"kv-grid", WidgetType::KvGrid}, {"kv", WidgetType::KvGrid}, {"facts", WidgetType::KvGrid},
        {"pin-bank", WidgetType::PinBank}, {"pins", WidgetType::PinBank}, {"gpio", WidgetType::PinBank},
        {"timeline", WidgetType::Timeline}, {"events", WidgetType::Timeline}, {"log", WidgetType::Timeline},
        {"line-chart", WidgetType::LineChart}, {"chart", WidgetType::LineChart}, {"plot", WidgetType::LineChart},
        {"bitfield", WidgetType::Bitfield}, {"bits", WidgetType::Bitfield}, {"register", WidgetType::Bitfield},
        {"hex-dump", WidgetType::HexDump}, {"hexdump", WidgetType::HexDump}, {"dump", WidgetType::HexDump},
        {"register-table", WidgetType::RegisterTable}, {"registers", WidgetType::RegisterTable}, {"reg-table", WidgetType::RegisterTable},
        {"modbus-frame", WidgetType::ModbusFrame}, {"modbus", WidgetType::ModbusFrame}, {"frame", WidgetType::ModbusFrame},
        {"can-frame", WidgetType::CanFrame}, {"can", WidgetType::CanFrame},
        {"uart-frame", WidgetType::UartFrame}, {"uart", WidgetType::UartFrame},
        {"packet-frame", WidgetType::PacketFrame}, {"packet", WidgetType::PacketFrame},
    };
    for (const Entry &e : kTable) {
        if (t == e.name) return e.type;
    }
    return WidgetType::Unknown;
}

ParsedLine parseLine(const std::string &rawLine) {
    ParsedLine result;
    std::string line = rawLine;

    // 1) Транспортный префикс @N в начале строки.
    if (!line.empty() && line[0] == '@') {
        size_t p = 1;
        int ch = 0;
        bool any = false;
        while (p < line.size() && std::isdigit(static_cast<unsigned char>(line[p]))) {
            ch = ch * 10 + (line[p] - '0');
            ++p;
            any = true;
        }
        if (any && p < line.size() && line[p] == ' ') {
            if (ch < 0) ch = 0;
            if (ch > 3) ch = 3;
            result.channel = static_cast<uint8_t>(ch);
            line = line.substr(p + 1);
        }
    }

    std::string trimmed = trim(line);
    std::string lower = toLower(trimmed);

    // 2) Служебные команды.
    if (lower == "beep") {
        result.kind = LineKind::Beep;
        return result;
    }
    if (lower == "demo-widgets" || lower == "demo") {
        result.kind = LineKind::DemoWidgets;
        return result;
    }
    if (lower == "clear-terminal" || lower.rfind("clear-terminal ", 0) == 0) {
        result.kind = LineKind::ClearTerminal;
        // Возможный номер канала после команды: "clear-terminal 3".
        std::string rest = trim(trimmed.substr(std::strlen("clear-terminal")));
        if (!rest.empty()) {
            result.clearChannel = parseInt(rest.c_str(), -1, 0, 3);
        }
        return result;
    }

    // 3) Команда виджета: "ui ..." или "widget ...".
    bool isUi = (lower.rfind("ui ", 0) == 0) || lower == "ui";
    bool isWidget = (lower.rfind("widget ", 0) == 0) || lower == "widget";
    if (!isUi && !isWidget) {
        result.kind = LineKind::Text;
        result.text = trimmed;
        return result;
    }

    // Тело после имени команды.
    size_t cmdLen = isUi ? 2 : 6;  // "ui" / "widget"
    std::string body = (trimmed.size() > cmdLen) ? trimmed.substr(cmdLen) : std::string();

    std::vector<Token> tokens = tokenize(body);

    WidgetCommand cmd;
    cmd.channel = result.channel;
    for (const Token &tok : tokens) {
        if (!tok.hasEquals || tok.key.empty()) continue;
        std::string keyLower = toLower(tok.key);

        if (keyLower == "type") {
            cmd.typeRaw = tok.value;
            cmd.type = widgetTypeFromString(toLower(tok.value));
            continue;
        }
        if (keyLower == "channel" || keyLower == "terminal" ||
            keyLower == "term" || keyLower == "ch") {
            // У frame-виджетов channel может быть именем шины (can0/UART1).
            // Берём как канал только если это чистое число 0..3.
            bool numeric = !tok.value.empty();
            for (char c : tok.value) {
                if (!std::isdigit(static_cast<unsigned char>(c))) { numeric = false; break; }
            }
            if (numeric && (keyLower != "channel" || cmd.type == WidgetType::Unknown)) {
                cmd.channel = static_cast<uint8_t>(parseInt(tok.value.c_str(), 0, 0, 3));
            }
            // channel всё равно кладём в атрибуты (frame-виджетам нужно имя шины).
        }
        if (keyLower == "slot" || keyLower == "index" ||
            keyLower == "pos" || keyLower == "position") {
            cmd.slot = parseInt(tok.value.c_str(), -1, 0, 100000);
            continue;
        }

        WidgetAttr attr;
        attr.key = keyLower;
        attr.value = tok.value;
        cmd.attrs.push_back(std::move(attr));
    }

    result.channel = cmd.channel;

    if (cmd.type == WidgetType::Unknown) {
        result.kind = LineKind::Invalid;
        result.text = trimmed;
        return result;
    }

    result.kind = LineKind::Widget;
    result.widget = std::move(cmd);
    return result;
}

}  // namespace timber
