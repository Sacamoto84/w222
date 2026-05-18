#pragma once

#include <stdint.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char *root_path;
    const char *user;
    const char *password;
    uint16_t control_port;
    uint16_t passive_port_min;
    uint16_t passive_port_max;
    uint32_t task_stack;
    int task_priority;
} ftp_server_config_t;

#define FTP_SERVER_DEFAULT_CONFIG(root) { \
    .root_path = root,                    \
    .user = "esp32",                      \
    .password = "esp32",                  \
    .control_port = 21,                   \
    .passive_port_min = 50000,            \
    .passive_port_max = 50010,            \
    .task_stack = 12288,                  \
    .task_priority = 4,                   \
}

esp_err_t ftp_server_start(const ftp_server_config_t *config);

#ifdef __cplusplus
}
#endif
