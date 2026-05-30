// Хостовый тест парсера (без LVGL). Компилировать:
//   g++ -std=c++17 -I. WidgetModel.cpp WidgetParser.cpp test_parser_host.cpp -o /tmp/twtest
// Не входит в прошивку: в CMake этот файл исключается из сборки прошивки.
#include <cassert>
#include <cstdio>
#include <string>
#include <vector>

#include "WidgetParser.hpp"

using namespace timber;

static int g_fail = 0;
#define CHECK(cond, msg) do { if (!(cond)) { printf("FAIL: %s\n", msg); ++g_fail; } } while (0)

static const char *kDemo[] = {
    "ui type=badge text=\"READY\" st=ok",
    "@3 ui type=progress label=\"Battery\" value=72 max=100 fill=#36C36B display=\"72%\" slot=0",
    "@3 ui type=progress label=\"Battery\" value=56 max=100 fill=#36C36B display=\"56%\" slot=0",
    "ui type=dot color=#00E676 size=16 label=\"Link active\"",
    "ui type=image name=info size=40 desc=\"Info icon\"",
    "ui type=panel title=\"Motor 1\" value=READY subtitle=\"24.3V 1.8A\" accent=#36C36B icon=info",
    "ui type=2col left=\"Voltage\" right=\"24.3V\"",
    "ui type=table headers=\"Name|State|Temp\" rows=\"M1|READY|24.3;M2|WAIT|22.9;M3|ALARM|91.8\"",
    "ui type=switch label=\"Pump enable\" state=on subtitle=\"Remote mode\"",
    "ui type=alarm-card title=\"Overheat\" message=\"Motor 1 temperature reached 92C\" severity=critical time=\"12:41:03\" icon=warn2",
    "ui type=sparkline label=\"Temp\" values=\"21,22,22,23,24,23,25\" min=18 max=28 color=#36C36B display=\"25C\" points=on",
    "ui type=bar-group title=\"Motors\" labels=\"M1|M2|M3\" values=\"20|45|80\" max=100 colors=\"#36C36B|#4FC3F7|#FFB300\"",
    "ui type=gauge label=\"CPU\" value=72 max=100 unit=\"%\" color=#36C36B",
    "ui type=battery label=\"Battery A\" value=78 max=100 charging=true voltage=4.08",
    "ui type=led-row title=\"Links\" items=\"NET:#00E676|MQTT:#00E676|ERR:#FF5252|GPS:off\"",
    "ui type=stats-card title=\"RPM\" value=1450 unit=\"rpm\" delta=\"+12\" subtitle=\"Motor 1\" accent=#36C36B",
    "ui type=kv-grid title=\"Motor 1\" items=\"Voltage:24.3V|Current:1.8A|Temp:62C|State:READY\" columns=2",
    "ui type=pin-bank title=\"GPIO\" items=\"D1:on|D2:off|D3:warn|A0:adc|PWM1:pwm\"",
    "ui type=timeline title=\"Boot\" items=\"12:01 Boot|12:03 WiFi connected|12:05 MQTT online\"",
    "ui type=line-chart title=\"Voltage\" values=\"24.1,24.2,24.0,24.3,24.4\" labels=\"T1|T2|T3|T4|T5\" min=23 max=25 color=#4FC3F7",
    "ui type=bitfield label=\"STATUS\" value=0xB38F bits=16",
    "ui type=hex-dump title=\"RX Buffer\" data=\"48 65 6C 6C 6F 20 57 6F 72 6C 64\" width=8 addr=0x1000 ascii=on",
    "ui type=register-table title=\"Holding Registers\" rows=\"0000|0x1234|Status;0001|0x00A5|Flags;0002|0x03E8|Speed\"",
    "ui type=modbus-frame direction=request preset=rtu data=\"01 03 00 10 00 02 C5 CE\"",
    "ui type=can-frame title=\"Motor CAN\" direction=rx id=0x18FF50E5 ext=true data=\"11 22 33 44 55 66 77 88\" channel=can0",
    "ui type=uart-frame title=\"UART RX\" direction=rx channel=UART1 baud=115200 data=\"AA 55 10 02 01 02 34\" fields=\"0-1|Sync|AA55|Preamble;2|Cmd|10|Command;3|Len|02|Payload length;4-5|Payload|0102|Data;6|CRC|34|Checksum\"",
    "ui type=packet-frame title=\"Binary Packet\" protocol=CUSTOM direction=tx data=\"7E A1 02 10 FF 55\" ascii=on",
};

static const WidgetType kExpect[] = {
    WidgetType::Badge, WidgetType::Progress, WidgetType::Progress, WidgetType::Dot,
    WidgetType::Image, WidgetType::Panel, WidgetType::TwoCol, WidgetType::Table,
    WidgetType::Switch, WidgetType::AlarmCard, WidgetType::Sparkline, WidgetType::BarGroup,
    WidgetType::Gauge, WidgetType::Battery, WidgetType::LedRow, WidgetType::StatsCard,
    WidgetType::KvGrid, WidgetType::PinBank, WidgetType::Timeline, WidgetType::LineChart,
    WidgetType::Bitfield, WidgetType::HexDump, WidgetType::RegisterTable, WidgetType::ModbusFrame,
    WidgetType::CanFrame, WidgetType::UartFrame, WidgetType::PacketFrame,
};

int main() {
    const int count = sizeof(kDemo) / sizeof(kDemo[0]);
    for (int i = 0; i < count; ++i) {
        ParsedLine p = parseLine(kDemo[i]);
        char msg[128];
        snprintf(msg, sizeof(msg), "[%d] kind==Widget for: %s", i, kDemo[i]);
        CHECK(p.kind == LineKind::Widget, msg);
        snprintf(msg, sizeof(msg), "[%d] type matches (got %d want %d)",
                 i, (int)p.widget.type, (int)kExpect[i]);
        CHECK(p.widget.type == kExpect[i], msg);
    }

    // Канал из @3.
    {
        ParsedLine p = parseLine(kDemo[1]);
        CHECK(p.channel == 3, "channel @3");
        CHECK(p.widget.slot == 0, "slot=0 parsed");
    }
    // Badge st=ok -> атрибут st присутствует.
    {
        ParsedLine p = parseLine(kDemo[0]);
        CHECK(p.widget.find({"st"}) != nullptr, "badge has st");
        CHECK(std::string(p.widget.find({"text"})) == "READY", "badge text READY");
    }
    // panel value=READY (без кавычек).
    {
        ParsedLine p = parseLine(kDemo[5]);
        CHECK(std::string(p.widget.find({"value"})) == "READY", "panel value READY unquoted");
        CHECK(std::string(p.widget.find({"subtitle"})) == "24.3V 1.8A", "panel subtitle with spaces");
    }
    // table rows split.
    {
        ParsedLine p = parseLine(kDemo[7]);
        auto rows = splitRows(p.widget.find({"rows"}));
        CHECK(rows.size() == 3, "table 3 rows");
        CHECK(rows[0].size() == 3, "table row has 3 cells");
        CHECK(rows[2][2] == "91.8", "table cell value");
    }
    // colors helpers.
    CHECK(parseColor("#36C36B", 0) == 0x36C36B, "hex color");
    CHECK(parseColor("red", 0) == 0xFF0000, "named color");
    CHECK(parseColor("#FF36C36B", 0) == 0x36C36B, "argb drops alpha");
    CHECK(parseColor("garbage", 0x123456) == 0x123456, "bad color -> default");
    // bool.
    CHECK(parseBool("on") == true, "bool on");
    CHECK(parseBool("off") == false, "bool off");
    CHECK(parseBool("true") == true, "bool true");
    // unsigned.
    CHECK(parseUnsigned("0xB38F", 0) == 0xB38F, "hex unsigned");
    CHECK(parseUnsigned("0b1010", 0) == 10, "bin unsigned");
    CHECK(parseUnsigned("165", 0) == 165, "dec unsigned");
    // hex bytes.
    {
        auto b = parseHexBytes("48 65 6C 6C 6F");
        CHECK(b.size() == 5 && b[0] == 0x48 && b[4] == 0x6F, "hexbytes spaced");
        auto b2 = parseHexBytes("48656C6C6F");
        CHECK(b2.size() == 5 && b2[0] == 0x48, "hexbytes packed");
        auto b3 = parseHexBytes("48,65,6C");
        CHECK(b3.size() == 3 && b3[2] == 0x6C, "hexbytes comma");
    }
    // clear-terminal + beep + demo.
    CHECK(parseLine("@2 clear-terminal").kind == LineKind::ClearTerminal, "clear-terminal");
    CHECK(parseLine("@2 clear-terminal").channel == 2, "clear-terminal channel");
    CHECK(parseLine("clear-terminal 3").clearChannel == 3, "clear-terminal explicit");
    CHECK(parseLine("beep").kind == LineKind::Beep, "beep");
    CHECK(parseLine("demo-widgets").kind == LineKind::DemoWidgets, "demo-widgets");
    CHECK(parseLine("just a log line").kind == LineKind::Text, "plain text");
    CHECK(parseLine("widget type=badge text=hi").widget.type == WidgetType::Badge, "widget alias");
    CHECK(parseLine("ui type=nonsense").kind == LineKind::Invalid, "unknown type -> invalid");
    // can-frame channel=can0 must NOT be treated as console channel, stays 0.
    {
        ParsedLine p = parseLine(kDemo[24]);
        CHECK(p.channel == 0, "can-frame channel=can0 not console channel");
        CHECK(std::string(p.widget.find({"channel"})) == "can0", "can channel attr kept");
    }

    if (g_fail == 0) {
        printf("ALL PARSER TESTS PASSED (%d demo commands)\n", count);
        return 0;
    }
    printf("%d FAILURES\n", g_fail);
    return 1;
}
