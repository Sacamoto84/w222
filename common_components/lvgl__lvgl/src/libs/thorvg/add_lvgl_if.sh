#!/bin/bash

#Добавить охранник LVGL #if LV_USE_THORVG_INTERNAL
#Использование
# найти -имя "*.cpp" | xargs ./add_lvgl_if.sh
# найти -имя "t*.h" | xargs ./add_lvgl_if.sh

sed '0,/\*\/$/ {/\*\/$/ {n; s|^|\n#include "../../lv_conf_internal.h"\n#if LV_USE_THORVG_INTERNAL\n|}}' $@ -i

sed -i -e '$a\
\
#endif /* LV_USE_THORVG_INTERNAL */\
' $@ -i
