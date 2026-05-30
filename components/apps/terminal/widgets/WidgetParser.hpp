#pragma once

// Парсер протокола консольных виджетов.
// Host-safe (только STL), чтобы юнит-тестировать без LVGL/ESP.

#include <string>

#include "WidgetModel.hpp"

namespace timber {

// Результат разбора одной завершённой строки.
struct ParsedLine {
    LineKind kind = LineKind::Text;
    uint8_t channel = 0;        // канал из префикса @N (для всех видов)
    WidgetCommand widget;       // заполнено только при kind == Widget
    int clearChannel = -1;      // для ClearTerminal: явный номер или -1
    std::string text;           // для Text: исходная строка (без @N префикса)
};

// Главная точка входа: разобрать одну строку (без завершающего \n).
ParsedLine parseLine(const std::string &line);

// Сопоставление строки type (с алиасами) -> WidgetType.
WidgetType widgetTypeFromString(const std::string &typeLower);

}  // namespace timber
