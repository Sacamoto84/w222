#!/bin/sh

# Установите необходимые пакеты Linux, необходимые для разработки LVGL.
# и тестирование. Некоторые менее распространенные пакеты разработки не включены.
# здесь, например, пакеты симуляторов MicroPython и PC.
#
# Примечание. Этот сценарий запускается рабочими процессами CI.
SCRIPT_PATH=$(readlink -f $0)
SCRIPT_DIR=$(dirname $SCRIPT_PATH)

sudo dpkg --add-architecture i386
sudo apt update

cat $SCRIPT_DIR/prerequisites-apt.txt | xargs sudo apt install -y
pip3 install --user -r $SCRIPT_DIR/prerequisites-pip.txt
