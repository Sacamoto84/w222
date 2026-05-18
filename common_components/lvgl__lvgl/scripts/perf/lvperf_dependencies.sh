#!/bin/sh

# Этот скрипт устанавливает дополнительные зависимости для образов Docker lvperf, включаемые в конвейерCI.
# Образы основаны на Alpine Linux и включают установку зависимостей во время выполнения.
# позволяющий расширить функциональность без пересборки образа.
#
# Инструкции по этому поводу обычно приводятся в зависимости от см. в Докерфайлах:
# - https://github.com/smartobjectoriented/so3/blob/main/docker/Dockerfile.lvperf_32b
# - https://github.com/smartobjectoriented/so3/blob/main/docker/Dockerfile.lvperf_64b
