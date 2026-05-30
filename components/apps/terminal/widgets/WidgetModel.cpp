#include "WidgetModel.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
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

bool isHexDigit(char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

int hexVal(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return 0;
}

}  // namespace

const char *WidgetCommand::find(std::initializer_list<const char *> keys) const {
    for (const char *key : keys) {
        for (const WidgetAttr &attr : attrs) {
            if (attr.key == key) {
                return attr.value.c_str();
            }
        }
    }
    return nullptr;
}

uint32_t parseColor(const char *value, uint32_t defaultRgb) {
    if (value == nullptr || *value == '\0') {
        return defaultRgb;
    }

    std::string v = toLower(value);
    // Обрезать пробелы по краям.
    while (!v.empty() && std::isspace(static_cast<unsigned char>(v.front()))) v.erase(v.begin());
    while (!v.empty() && std::isspace(static_cast<unsigned char>(v.back()))) v.pop_back();

    // Именованные цвета (как в Android namedColors).
    struct Named { const char *name; uint32_t rgb; };
    static const Named kNamed[] = {
        {"black", 0x000000},   {"white", 0xFFFFFF}, {"red", 0xFF0000},
        {"green", 0x00FF00},   {"blue", 0x0000FF},  {"yellow", 0xFFFF00},
        {"cyan", 0x00FFFF},    {"magenta", 0xFF00FF}, {"gray", 0x888888},
        {"grey", 0x888888},    {"orange", 0xFF9800},
    };
    for (const Named &n : kNamed) {
        if (v == n.name) return n.rgb;
    }

    // Hex-формат: #RGB? нет, поддерживаем #RRGGBB и #AARRGGBB.
    size_t start = 0;
    if (v[0] == '#') start = 1;
    else if (v.size() > 2 && v[0] == '0' && v[1] == 'x') start = 2;

    std::string hex = v.substr(start);
    for (char c : hex) {
        if (!isHexDigit(c)) return defaultRgb;
    }

    if (hex.size() == 6) {
        return static_cast<uint32_t>(std::strtoul(hex.c_str(), nullptr, 16)) & 0x00FFFFFFu;
    }
    if (hex.size() == 8) {
        // AARRGGBB — отбрасываем альфу.
        return static_cast<uint32_t>(std::strtoul(hex.c_str() + 2, nullptr, 16)) & 0x00FFFFFFu;
    }
    return defaultRgb;
}

bool parseBool(const char *value, bool defaultValue) {
    if (value == nullptr || *value == '\0') return defaultValue;
    std::string v = toLower(value);
    if (v == "on" || v == "true" || v == "1" || v == "yes" ||
        v == "active" || v == "ok" || v == "high" || v == "enabled") {
        return true;
    }
    if (v == "off" || v == "false" || v == "0" || v == "no" ||
        v == "inactive" || v == "disabled" || v == "low") {
        return false;
    }
    return defaultValue;
}

int parseInt(const char *value, int defaultValue, int minVal, int maxVal) {
    if (value == nullptr || *value == '\0') return defaultValue;
    char *end = nullptr;
    long parsed = std::strtol(value, &end, 0);  // base 0 -> понимает 0x..
    if (end == value) return defaultValue;
    if (parsed < minVal) parsed = minVal;
    if (parsed > maxVal) parsed = maxVal;
    return static_cast<int>(parsed);
}

float parseFloat(const char *value, float defaultValue) {
    if (value == nullptr || *value == '\0') return defaultValue;
    char *end = nullptr;
    float parsed = std::strtof(value, &end);
    if (end == value) return defaultValue;
    return parsed;
}

uint64_t parseUnsigned(const char *value, uint64_t defaultValue) {
    if (value == nullptr || *value == '\0') return defaultValue;
    std::string v = toLower(value);
    if (v.size() > 2 && v[0] == '0' && v[1] == 'b') {
        uint64_t acc = 0;
        for (size_t i = 2; i < v.size(); ++i) {
            if (v[i] != '0' && v[i] != '1') return defaultValue;
            acc = (acc << 1) | static_cast<uint64_t>(v[i] - '0');
        }
        return acc;
    }
    char *end = nullptr;
    unsigned long long parsed = std::strtoull(value, &end, 0);  // 0x.. или dec
    if (end == value) return defaultValue;
    return static_cast<uint64_t>(parsed);
}

std::vector<std::string> splitList(const char *value, const char *separators) {
    std::vector<std::string> out;
    if (value == nullptr) return out;

    std::string token;
    for (const char *p = value; *p; ++p) {
        if (std::strchr(separators, *p) != nullptr) {
            out.push_back(token);
            token.clear();
        } else {
            token.push_back(*p);
        }
    }
    out.push_back(token);

    // Убрать пробелы по краям токенов.
    for (std::string &t : out) {
        while (!t.empty() && std::isspace(static_cast<unsigned char>(t.front()))) t.erase(t.begin());
        while (!t.empty() && std::isspace(static_cast<unsigned char>(t.back()))) t.pop_back();
    }
    // Убрать единственный пустой хвост от висячего разделителя.
    if (out.size() > 1 && out.back().empty()) {
        out.pop_back();
    }
    return out;
}

std::vector<std::vector<std::string>> splitRows(const char *value) {
    std::vector<std::vector<std::string>> rows;
    if (value == nullptr) return rows;

    std::vector<std::string> rawRows = splitList(value, ";");
    for (const std::string &row : rawRows) {
        if (row.empty()) continue;
        rows.push_back(splitList(row.c_str(), "|"));
    }
    return rows;
}

std::vector<uint8_t> parseHexBytes(const char *value) {
    std::vector<uint8_t> out;
    if (value == nullptr) return out;

    // Сначала собираем только hex-символы, учитывая разделители.
    // Поддержка: "AA 55 10", "AA,55", слитно "AA5510".
    std::string cleaned;
    for (const char *p = value; *p; ++p) {
        char c = *p;
        if (isHexDigit(c)) {
            cleaned.push_back(c);
        } else if (c == ' ' || c == ',' || c == ';' || c == '|' || c == ':' || c == '-') {
            cleaned.push_back(' ');
        }
        // прочее игнорируем
    }

    // Если есть пробелы — токенизируем по ним; иначе берём парами.
    bool hasSep = cleaned.find(' ') != std::string::npos;
    if (hasSep) {
        std::string token;
        auto flush = [&]() {
            if (token.empty()) return;
            unsigned long b = std::strtoul(token.c_str(), nullptr, 16);
            out.push_back(static_cast<uint8_t>(b & 0xFF));
            token.clear();
        };
        for (char c : cleaned) {
            if (c == ' ') flush();
            else token.push_back(c);
        }
        flush();
    } else {
        for (size_t i = 0; i + 1 < cleaned.size(); i += 2) {
            int hi = hexVal(cleaned[i]);
            int lo = hexVal(cleaned[i + 1]);
            out.push_back(static_cast<uint8_t>((hi << 4) | lo));
        }
    }
    return out;
}

}  // namespace timber
