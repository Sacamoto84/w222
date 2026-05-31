#pragma once

// Общая конфигурация сетевого приёма данных терминала.
//
// Терминал может получать тот же текстовый поток (строки, префикс @N, виджеты
// `ui ...`), что и по UART, по сети WiFi:
//  - TCP-клиент: подключается к host:port внешнего сервера (например, ESP32-C3,
//    который читает UART и отдаёт поток по TCP на 8888) и читает байты;
//  - UDP-слушатель (опционально): принимает датаграммы текста на заданном порту.
//
// Значения хранятся в NVS; и приложение терминала, и приложение настроек I/O
// работают с этой же конфигурацией. После сохранения выставляется флаг "грязно",
// который TCP-задача терминала вычитывает и переподключается.

#include <cstddef>

namespace netcfg {

struct Config {
    bool tcp_enabled = false;
    char host[64] = {0};     // IP или hostname сервера (для TCP-клиента)
    int tcp_port = 8888;
    bool udp_enabled = false;
    int udp_port = 8888;
};

// Состояние TCP-клиента — терминал публикует, экран настроек читает.
enum class TcpState : int {
    Disabled = 0,
    Idle,        // включён, но нет адреса/ждёт
    Connecting,
    Connected,
    Error,
};

Config defaults();
Config load();                 // из NVS или defaults()
bool save(const Config &cfg);  // в NVS + инкремент version()

// Монотонный счётчик версии конфигурации. Несколько потребителей (TCP- и
// UDP-задачи) запоминают последнее значение и перечитывают конфиг при изменении
// (single-shot флаг не подошёл бы — его увидел бы только один потребитель).
unsigned version();

// Публикация/чтение состояния TCP-соединения (потокобезопасно, atomic).
void set_tcp_state(TcpState state);
TcpState tcp_state();
const char *tcp_state_name(TcpState state);

}  // namespace netcfg
