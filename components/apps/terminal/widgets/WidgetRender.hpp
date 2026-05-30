#pragma once

// LVGL-рендер консольных виджетов для ESP32-терминала.
//
// Каждый виджет рисуется как «карточка» в переданный родительский контейнер.
// Высота карточки кратна высоте строки терминала (lineHeight): так виджет
// занимает целое число строк в общем построчном списке, и скролл остаётся
// построчным, как у текста.
//
// Этот модуль ЗАВИСИТ от LVGL (в отличие от WidgetModel/WidgetParser).

#include "lvgl.h"
#include "WidgetModel.hpp"

namespace timber {

// Сколько строк терминала займёт виджет данной команды.
// Чистая функция от спека: не создаёт LVGL-объектов.
int measureWidgetRows(const WidgetCommand &cmd, int lineHeight);

// Нарисовать виджет в parent. parent уже имеет нужный размер
// (ширина = ширине вьюпорта, высота = measureWidgetRows * lineHeight).
// Рендер создаёт внутри parent дочерние объекты с нуля.
void renderWidget(lv_obj_t *parent, const WidgetCommand &cmd, int lineHeight);

}  // namespace timber
