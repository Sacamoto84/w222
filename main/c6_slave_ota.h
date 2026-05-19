#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t c6_slave_ota_from_sd(const char *firmware_path);

#ifdef __cplusplus
}
#endif
