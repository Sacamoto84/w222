# Консольные виджеты терминала (ESP32-P4)

Подсистема рисует «карточки-виджеты» прямо в потоке UART-терминала. Микроконтроллер
(или любой источник на другом конце UART) присылает текстовую строку вида
`ui type=<widget> key=value ...`, парсер превращает её в виджет, а LVGL-рендер
рисует его как элемент построчного списка терминала.

Это порт протокола из Android-приложения **Android_Terminal** (символическая
ссылка в корне проекта) на нативный C++/LVGL. Здесь нет Compose и Android —
вместо живых Compose-карточек рисуются деревья LVGL-объектов, а скролл идёт
**кратно строке терминала**, чтобы оставаться быстрым.

## Референсы

Реализация повторяет поведение и набор полей из Android-стороны:

- Протокол и спецификация всех виджетов:
  [`Android_Terminal/app/.../console/README.md`](../../../../../Android_Terminal/app/src/main/java/com/example/terminalm3/console/README.md)
- C++ библиотека-отправитель (формирует те же строки на стороне МК):
  [`Android_Terminal/TimberWidget/README.md`](../../../../../Android_Terminal/TimberWidget/README.md)

То есть код, который шлёт виджеты на Android через `TimberWidget`, без изменений
работает и с этим терминалом — формат строк один и тот же.

## Архитектура модуля

| Файл | Назначение | Зависит от LVGL |
| --- | --- | --- |
| `WidgetModel.{hpp,cpp}` | Типы виджетов (`WidgetType`), модель команды (`WidgetCommand`) и хелперы разбора значений (цвета, bool, числа, списки, hex-байты) | Нет (чистый STL) |
| `WidgetParser.{hpp,cpp}` | Разбор строки `ui type=... key=value` в `WidgetCommand`: префикс канала, кавычки, алиасы типов | Нет (чистый STL) |
| `WidgetRender.{hpp,cpp}` | `measureWidgetRows()` — высота виджета в рядах; `renderWidget()` — отрисовка дерева LVGL-объектов | Да |
| `test_parser_host.cpp` | Хостовый юнит-тест парсера (в прошивку не входит) | Нет |

`WidgetModel` и `WidgetParser` намеренно не зависят от LVGL — их можно собрать и
протестировать на хосте:

```bash
g++ -std=c++17 -I. WidgetModel.cpp WidgetParser.cpp test_parser_host.cpp -o twtest && ./twtest
```

## Как это встроено в терминал

`TerminalApp` хранит список элементов (`TerminalLine`). Каждый элемент — это либо
обычная текстовая строка (1 ряд), либо виджет (N рядов). Когда из UART приходит
завершённая строка, `try_consume_timber_line()` пробует распознать её как команду
виджета. Если да — добавляется виджет-элемент с высотой `row_span = measureWidgetRows()`;
если нет — обычная цветная строка терминала.

Виртуализация ведётся по абсолютным рядам: текст и OSC-виджеты рисуются на пуле
канвасов, а timber-виджеты — отдельным пулом живых LVGL-контейнеров поверх
вьюпорта. Скролл шагает построчно (snap к границе строки).

## Формат команды

```text
[@N] ui type=<widget> key=value key="value with spaces" ...
```

Правила:

- Команда — одна завершённая строка (`\n` или `\r\n`).
- `widget` — полный алиас `ui`.
- Аргументы передаются как `key=value`; значения с пробелами — в кавычках.
  Поддерживаются три вида кавычек: двойные `"..."`, одинарные `'...'` и
  backtick `` `...` ``. Backtick особенно удобен, когда строку собирают в
  C++-коде: его не нужно экранировать (в отличие от `"` внутри строкового
  литерала). Внутри кавычек работает `\` для экранирования.

  ```cpp
  // Неудобно — двойные кавычки приходится экранировать:
  uart.print("ui type=badge text=\"READY\" st=ok");
  // Удобно — backtick без экранирования:
  uart.print("ui type=badge text=`READY` st=ok");
  ```
- Префикс `@N` в начале строки — номер канала консоли `0..3` (если нет — канал 0).
- Цвета: `#RRGGBB`, `#AARRGGBB` (альфа отбрасывается) или имена
  `black white red green blue yellow cyan magenta gray orange`.
- Булевы: `on/off`, `true/false`, `1/0`, `yes/no`.
- Списки: разделители `|`, `,` или `;` (зависит от поля).
- Числа: десятичные и `0x..` (а для bitfield ещё `0b..`).

Служебные команды:

```text
clear-terminal        # очистить терминал (можно с @N или "clear-terminal 3")
demo-widgets          # проиграть весь демо-набор виджетов
beep                  # служебная (звука на МК нет, строка проглатывается)
```

## Поддержанные виджеты

Реализованы все 26 типов протокола. Колонка «рядов» — высота в строках терминала
(её считает `measureWidgetRows`; для табличных/списковых виджетов высота зависит
от числа элементов).

### Базовые и статусные

| Тип (и алиасы) | Рядов | Пример |
| --- | --- | --- |
| `badge` | 2 | `ui type=badge text="READY" st=ok` |
| `dot` `circle` | 2 | `ui type=dot color=#00E676 size=16 label="Link active"` |
| `image` `icon` | 3 | `ui type=image name=info size=40 desc="Info icon"` |
| `panel` `card` | 3 | `ui type=panel title="Motor 1" value=READY subtitle="24.3V 1.8A" accent=#36C36B icon=info` |
| `progress` `bar` | 3 | `ui type=progress label="Battery" value=72 max=100 fill=#36C36B display="72%"` |
| `2col` `twocol` `pair` | 2 | `ui type=2col left="Voltage" right="24.3V"` |
| `switch` `toggle` | 3 | `ui type=switch label="Pump enable" state=on subtitle="Remote mode"` |
| `stats-card` `stat` `metric-card` | 3 | `ui type=stats-card title="RPM" value=1450 unit="rpm" delta="+12" subtitle="Motor 1" accent=#36C36B` |
| `alarm-card` `alarm` `alert` | 4 | `ui type=alarm-card title="Overheat" message="Motor 1 reached 92C" severity=critical time="12:41:03"` |

Пресеты стиля `badge` (`st=`): `ok info warn error critical neutral dark`
(с алиасами `ready/success/good/green`, `blue`, `warning/amber/yellow`,
`fail/danger/red`, `alarm`, `default/gray/grey`, `muted`).

### Телеметрия и графики

| Тип (и алиасы) | Рядов | Пример |
| --- | --- | --- |
| `gauge` `dial` | 4 | `ui type=gauge label="CPU" value=72 max=100 unit="%" color=#36C36B` |
| `battery` `cell` | 4 | `ui type=battery label="Battery A" value=78 max=100 charging=true voltage=4.08` |
| `sparkline` `trend` | 4 | `ui type=sparkline label="Temp" values="21,22,23,24,25" min=18 max=28 color=#36C36B display="25C"` |
| `bar-group` `bars` `columns` | 5 | `ui type=bar-group title="Motors" labels="M1|M2|M3" values="20|45|80" max=100 colors="#36C36B|#4FC3F7|#FFB300"` |
| `line-chart` `chart` `plot` | 6 | `ui type=line-chart title="Voltage" values="24.1,24.2,24.0,24.3" labels="T1|T2|T3|T4" color=#4FC3F7` |
| `led-row` `leds` `status-row` | 3 | `ui type=led-row title="Links" items="NET:#00E676|MQTT:#00E676|ERR:#FF5252|GPS:off"` |
| `kv-grid` `kv` `facts` | 2 + строки | `ui type=kv-grid title="Motor 1" items="Voltage:24.3V|Current:1.8A|Temp:62C" columns=2` |
| `pin-bank` `pins` `gpio` | 2 + строки | `ui type=pin-bank title="GPIO" items="D1:on|D2:off|D3:warn|A0:adc|PWM1:pwm"` |
| `timeline` `events` `log` | 2 + события | `ui type=timeline title="Boot" items="12:01 Boot|12:03 WiFi connected|12:05 MQTT online"` |

Графики (`sparkline`, `line-chart`) рисуются через `lv_chart`, `gauge` — через
`lv_arc`, `progress`/`battery` — через `lv_bar`/нарисованную ячейку.

Состояния в `led-row`/`pin-bank`: `on/true/1/active/ok/high` (зелёный),
`off/0/...` (выкл), `warn` (жёлтый), `error/err/alarm/critical` (красный),
`adc`, `pwm`, `in`, `out`; либо прямой цвет `NAME:#RRGGBB`.

### Таблицы, биты, память

| Тип (и алиасы) | Рядов | Пример |
| --- | --- | --- |
| `table` `grid` | 2 + строки | `ui type=table headers="Name|State|Temp" rows="M1|READY|24.3;M2|WAIT|22.9"` |
| `bitfield` `bits` `register` | 2 + bits/16 | `ui type=bitfield label="STATUS" value=0xB38F bits=16` |
| `hex-dump` `hexdump` `dump` | 2 + строки | `ui type=hex-dump title="RX Buffer" data="48 65 6C 6C 6F" width=8 addr=0x1000 ascii=on` |
| `register-table` `registers` `reg-table` | 3 + строки | `ui type=register-table title="Holding" rows="0000|0x1234|Status;0001|0x00A5|Flags"` |

Формат строк таблиц: строки через `;`, ячейки через `|`.
`bitfield` понимает `value` в hex/bin/dec и `kind=byte|short|word` как альтернативу `bits`.
`hex-dump` принимает `data` через пробел/запятую или слитной hex-строкой.

### Кадры и пакеты

| Тип (и алиасы) | Рядов | Пример |
| --- | --- | --- |
| `modbus-frame` `modbus` `frame` | 3 + поля | `ui type=modbus-frame direction=request preset=rtu data="01 03 00 10 00 02 C5 CE"` |
| `can-frame` `can` | 3 + поля | `ui type=can-frame title="Motor CAN" direction=rx id=0x18FF50E5 ext=true data="11 22 33 44" channel=can0` |
| `uart-frame` `uart` | 3 + поля | `ui type=uart-frame title="UART RX" direction=rx channel=UART1 baud=115200 data="AA 55 10 02"` |
| `packet-frame` `packet` | 3 + поля | `ui type=packet-frame title="Binary" protocol=CUSTOM direction=tx data="7E A1 02 10" ascii=on` |

Расшифровка полей `fields`: записи через `;`, колонки `RANGE|NAME|VALUE|DESC` через `|`.
У frame-виджетов `channel` — это имя шины/порта (`can0`, `UART1`), а не канал
консоли: для канала консоли используйте префикс `@N` или ключи `terminal/term/ch`.

## Каналы и slot

Каналы консоли `0..3` (как на Android). Рекомендуемый способ маршрутизации —
префикс `@N` в начале строки. Внутри `ui`/`widget` также понимаются ключи
`channel`/`terminal`/`term`/`ch` (только числовое значение 0..3).

Ключи `slot`/`index`/`pos`/`position` разбираются (`WidgetCommand.slot`) для
совместимости с протоколом; обновление виджета «на месте» по slot на стороне
терминала пока не реализовано — каждый виджет добавляется как новый элемент.

## Демо

В терминале есть кнопка демо (иконка-капля в тулбаре) и команда `demo-widgets`.
Демо прогоняет канонический набор из всех 26 типов виджетов (тот же, что в
`TimberWidget::sendDemoCommands`), плюс примеры цветного xterm-256 текста и
встроенных OSC-виджетов. Реализация — в `UartTerminalApp::inject_demo_uart_data()`.

## Что НЕ переносилось

- Интерактивность виджетов (нажатия, тулбары каналов) — терминал только
  отображает; виджеты «прозрачны» для жестов, чтобы не мешать скроллу.
- `image`/`icon` рисуется заглушкой (рамка + имя ресурса): на МК нет общего
  реестра drawable по имени.
- Обновление по `slot` «на месте» (см. выше).

## Как добавить новый виджет

1. Добавить значение в `enum WidgetType` (`WidgetModel.hpp`).
2. Прописать имя и алиасы в `widgetTypeFromString()` (`WidgetParser.cpp`).
3. Добавить ветку в `measureWidgetRows()` (высота в рядах) и в `renderWidget()`
   плюс саму функцию `drawXxx()` (`WidgetRender.cpp`).
4. По желанию — строку в демо-набор `inject_demo_uart_data()` и тест в
   `test_parser_host.cpp`.
