#!/bin/bash

#Добавить охранник LVGL #if LV_USE_VG_LITE_DRIVER
#Использование
# найти -имя "*.c" | xargs ./add_lvgl_if.sh
# найти -name "t*.h" | xargs ./add_lvgl_if.sh

sed '0,/\*\/$/ {/\*\/$/ {n; s|^|\n#include "../../lv_conf_internal.h"\n#if LV_USE_VG_LITE_DRIVER\n|}}' $@ -i

sed -i -e '$a\
\
#endif /* LV_USE_VG_LITE_DRIVER */\
' $@ -i
