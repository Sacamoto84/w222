#!/usr/bin/env python3

'''
Generates lv_conf_internal.h from lv_conf_template.h to provide default values
'''

import os
import sys
import re

SCRIPT_DIR = os.path.dirname(__file__)
LV_CONF_TEMPLATE = os.path.join(SCRIPT_DIR, "..", "lv_conf_template.h")
LV_CONF_INTERNAL = os.path.join(SCRIPT_DIR, "..", "src", "lv_conf_internal.h")

if sys.version_info < (3,6,0):
  print("Python >=3.6 is required", file=sys.stderr)
  exit(1)

def check_for_tabs(file_path):
    errors = []
    with open(file_path, 'r') as file:
        for line_number, line in enumerate(file, 1):
            if '\t' in line:
                errors.append(f" {file_path}:{line_number}")

    if errors:
        print(f"Tabs found in the following files:", file=sys.stderr)
        for error in errors:
            print(error, file=sys.stderr)
        print("Please replace tabs with spaces.", file=sys.stderr)
        exit(1)

check_for_tabs(LV_CONF_TEMPLATE)

fin = open(LV_CONF_TEMPLATE)
fout = open(LV_CONF_INTERNAL, "w", newline='')

fout.write(
'''/**
 * GENERATED FILE, DO NOT EDIT IT!
 * @file lv_conf_internal.h
 * This file ensures all defines of lv_conf.h have a default value.
 */

#ifndef LV_CONF_INTERNAL_H
#определить LV_CONF_INTERNAL_H
/* clang-format off */

/* Config options */
#определить LV_OS_NONE 0
#определить LV_OS_PTHREAD 1
#определить LV_OS_FREERTOS 2
#определить LV_OS_CMSIS_RTOS2 3
#определить LV_OS_RTTHREAD 4
#определить LV_OS_WINDOWS 5
#определить LV_OS_MQX 6
#определить LV_OS_SDL2 7
#определить LV_OS_CUSTOM 255

#определить LV_STDLIB_BUILTIN 0
#определить LV_STDLIB_CLIB 1
#определить LV_STDLIB_MICROPYTHON 2
#определить LV_STDLIB_RTTHREAD 3
#определить LV_STDLIB_CUSTOM 255

#определить LV_DRAW_SW_ASM_NONE 0
#определить LV_DRAW_SW_ASM_NEON 1
#определить LV_DRAW_SW_ASM_HELIUM 2
#определить LV_DRAW_SW_ASM_RISCV_V 3
#определить LV_DRAW_SW_ASM_CUSTOM 255

#определить LV_NEMA_LIB_NONE 0
#определить LV_NEMA_LIB_M33_REVC 1
#определить LV_NEMA_LIB_M33_NEMAPVG 2
#определить LV_NEMA_LIB_M55 3
#определить LV_NEMA_LIB_M7 4

#определить LV_NEMA_HAL_CUSTOM 0
#определить LV_NEMA_HAL_STM32 1

#определить LV_NANOVG_BACKEND_GL2 1
#определить LV_NANOVG_BACKEND_GL3 2
#определить LV_NANOVG_BACKEND_GLES2 3
#определить LV_NANOVG_BACKEND_GLES3 4

/** Handle special Kconfig options. */
#ifndef LV_KCONFIG_IGNORE
    #включить "lv_conf_kconfig.h"
    #если определено ( CONFIG_LV_CONF_SKIP ) && ! определено ( LV_CONF_SKIP )
        #определить LV_CONF_SKIP
    #конец
#конец

/* If "lv_conf.h" is available from here try to use it later. */
#ifdef __has_include
    #если__has_include("lv_conf.h")
        #ifndef LV_CONF_INCLUDE_SIMPLE
            #определить LV_CONF_INCLUDE_SIMPLE
        #конец
    #конец
#конец

/* If lv_conf.h is not skipped, include it. */
#если !определено( LV_CONF_SKIP ) || определено( LV_CONF_PATH )
    #ifdefLV_CONF_PATH/* Если дляlv_conf.hопределен путь, вскормите его */
        #включитьLV_CONF_PATH/* Примечание. Обязательно определите пользовательскийCONF_PATHкак символ */
    #elif определение(LV_CONF_INCLUDE_SIMPLE) /* Или простоlv_conf.h. */
        #включить "lv_conf.h"
    #еще
        #include "../../lv_conf.h" /* Иначе предположим, чтоlv_conf.hнаходится рядом с папкой lvgl. */
    #конец
    #if !define(LV_CONF_H) && !define(LV_CONF_SUPPRESS_DEFINE_CHECK)
        /* #include will sometimes silently fail when __has_include is used */
        /* https://gcc.gnu.org/bugzilla/show_bug.cgi?id=80753 */
        #pragma message("Возможно, не удалось включитьlv_conf.h. Если возникла ошибка, прочтите комментарий в этом файле")
    #конец
#конец

#ifdef CONFIG_LV_COLOR_DEPTH
    #определить LV_KCONFIG_PRESENT
#конец

/*----------------------------------
 * Start parsing lv_conf_template.h
 -----------------------------------*/
'''
)

started = 0

for line in fin.read().splitlines():
  if not started:
    if '#define LV_CONF_H' in line:
      started = 1
      continue
    else:
      continue

  if '/*--END OF LV_CONF_H--*/' in line: break

  #Is there a #define in this line?
  r = re.search(r'^([\s]*)#[\s]*(undef|define)[\s]+([^\s]+).*$', line)   # \s means any white space character

  if r:
    indent = r[1]

    name = r[3]
    name = re.sub(r'\(.*?\)', '', name, count=1)    #remove parentheses from macros. E.g. MY_FUNC(5) -> MY_FUNC

    line = re.sub(r'[\s]*', '', line, count=1)

    #Если значение должно быть 1 (включено), по умолчанию используйте более сложную модель для тестов Kconfig, поскольку
    #если не определенное значение CONFIG_ ... должно интерпретироваться как 0, а не как значение по умолчанию LVGL
    is_one = re.search(r'#[\s]*define[\s]*[A-Z0-9_]+[\s]+1([\s]*$|[\s]+)', line)
    if is_one:
      #1. Используйте значение, если оно уже установлено, изlv_conf.hили чего-либо еще (т.е. ничего не делайте).
      #2. В среде Kconfig викор значениеCONFIG_..., если оно установлено, в противном случае викор 0.
      #3. В окружающей среде, отлично от Kconfig, воспользуйтесь значением по умолчаниюLVGL.

      fout.write(
        f'{indent}#ifndef {name}\n'
        f'{indent}    #ifdef LV_KCONFIG_PRESENT\n'
        f'{indent}        #ifdef CONFIG_{name.upper()}\n'
        f'{indent}            #define {name} CONFIG_{name.upper()}\n'
        f'{indent}        #else\n'
        f'{indent}            #define {name} 0\n'
        f'{indent}        #endif\n'
        f'{indent}    #else\n'
        f'{indent}        {line}\n'
        f'{indent}    #endif\n'
        f'{indent}#endif\n'
      )
    else:
      #1. Используйте значение, если оно уже установлено, изlv_conf.hили чего-либо еще (т.е. ничего не делайте).
      #2. Используйте значение Kconfig, если оно установлено.
      #3. Используйте значение по умолчанию LVGL.

      fout.write(
        f'{indent}#ifndef {name}\n'
        f'{indent}    #ifdef CONFIG_{name.upper()}\n'
        f'{indent}        #define {name} CONFIG_{name.upper()}\n'
        f'{indent}    #else\n'
        f'{indent}        {line}\n'
        f'{indent}    #endif\n'
        f'{indent}#endif\n'
      )

  elif re.search('^ *typedef .*;.*$', line):
    continue   #ignore typedefs to avoid redeclaration
  else:
    fout.write(f'{line}\n')

fout.write(
r'''

/*----------------------------------
 * End of parsing lv_conf_template.h
 -----------------------------------*/

/*Fix inconsistent name*/
#определить LV_USE_ANIMIMAGE LV_USE_ANIMIMG

#ifndef __ASSEMBLY__
LV_EXPORT_CONST_INT(LV_DPI_DEF);
LV_EXPORT_CONST_INT(LV_DRAW_BUF_STRIDE_ALIGN);
LV_EXPORT_CONST_INT(LV_DRAW_BUF_ALIGN);
#конец

#undef LV_KCONFIG_PRESENT

/* Disable VGLite drivers if VGLite drawing is disabled */
#ifndef LV_USE_VG_LITE_DRIVER
    #определить LV_USE_VG_LITE_DRIVER 0
#конец

#ifndef LV_USE_VG_LITE_THORVG
    #определить LV_USE_VG_LITE_THORVG 0
#конец

/* Set some defines if a dependency is disabled. */
#if LV_USE_LOG == 0
    #определить LV_LOG_LEVEL LV_LOG_LEVEL_NONE
    #определить LV_LOG_TRACE_MEM 0
    #определить LV_LOG_TRACE_TIMER 0
    #определить LV_LOG_TRACE_INDEV 0
    #определить LV_LOG_TRACE_DISP_REFR 0
    #определить LV_LOG_TRACE_EVENT 0
    #определить LV_LOG_TRACE_OBJ_CREATE 0
    #определить LV_LOG_TRACE_LAYOUT 0
    #определить LV_LOG_TRACE_ANIM 0
#endif /*LV_USE_LOG*/

#if LV_USE_WAYLAND
    /*Automatically detect wayland backend*/
    #if LV_USE_OPENGLES
        #определить LV_WAYLAND_USE_EGL 1
        #определить LV_WAYLAND_USE_G2D 0
        #определить LV_WAYLAND_USE_SHM 0
    #Элиф LV_USE_G2D
        #определить LV_WAYLAND_USE_EGL 0
        #определить LV_WAYLAND_USE_G2D 1
        #определить LV_WAYLAND_USE_SHM 0
    #еще
        #определить LV_WAYLAND_USE_EGL 0
        #определить LV_WAYLAND_USE_G2D 0
        #определить LV_WAYLAND_USE_SHM 1
    #конец
#еще
    #определить LV_WAYLAND_USE_G2D 0
    #определить LV_WAYLAND_USE_SHM 0
    #определить LV_WAYLAND_USE_EGL 0
#конец

#if LV_USE_LINUX_DRM
    #if LV_USE_OPENGLES
        #определить LV_LINUX_DRM_USE_EGL 1
    #еще
        #определить LV_LINUX_DRM_USE_EGL 0
    #endif /* LV_USE_OPENGLES */
#еще
    #определить LV_LINUX_DRM_USE_EGL 0
#endif /*LV_USE_LINUX_DRM*/

#if LV_USE_SYSMON == 0
    #определить LV_USE_PERF_MONITOR 0
    #определить LV_USE_MEM_MONITOR 0
    #определить LV_SYSMON_PROC_IDLE_AVAILABLE 0
#endif /*LV_USE_SYSMON*/

#if LV_USE_PERF_MONITOR == 0
    #определить LV_USE_PERF_MONITOR_LOG_MODE 0
#endif /*LV_USE_PERF_MONITOR*/

#if LV_BUILD_DEMOS == 0
    #определить LV_USE_DEMO_WIDGETS 0
    #определить LV_USE_DEMO_KEYPAD_AND_ENCODER 0
    #определить LV_USE_DEMO_BENCHMARK 0
    #определить LV_USE_DEMO_RENDER 0
    #определить LV_USE_DEMO_STRESS 0
    #определить LV_USE_DEMO_MUSIC 0
    #определить LV_USE_DEMO_VECTOR_GRAPHIC 0
    #определить LV_USE_DEMO_FLEX_LAYOUT 0
    #определить LV_USE_DEMO_MULTILANG 0
    #определить LV_USE_DEMO_EBIKE 0
    #определить LV_USE_DEMO_HIGH_RES 0
    #определить LV_USE_DEMO_SMARTWATCH 0
    #определить LV_USE_DEMO_GLTF 0
#endif /* LV_BUILD_DEMOS */

#ifndef LV_USE_LZ4
    #if (LV_USE_LZ4_INTERNAL || LV_USE_LZ4_EXTERNAL)
        #определить LV_USE_LZ4 1
    #еще
        #определить LV_USE_LZ4 0
    #конец
#конец

#ifndef LV_USE_THORVG
    #if (LV_USE_THORVG_INTERNAL || LV_USE_THORVG_EXTERNAL)
        #определить LV_USE_THORVG 1
    #еще
        #определить LV_USE_THORVG 0
    #конец
#конец

#if LV_USE_SDL && LV_USE_OPENGLES && (LV_USE_DRAW_OPENGLES || LV_USE_DRAW_NANOVG)
    #определить LV_SDL_USE_EGL 1
#еще
    #определить LV_SDL_USE_EGL 0
#конец

#ifndef LV_USE_EGL
    #if LV_LINUX_DRM_USE_EGL || LV_WAYLAND_USE_EGL || LV_SDL_USE_EGL
        #определить LV_USE_EGL 1
    #еще
        #определить LV_USE_EGL 0
    #конец
#endif /* LV_USE_EGL */


#if LV_USE_OS
    #if (LV_USE_FREETYPE || LV_USE_THORVG) && LV_DRAW_THREAD_STACK_SIZE < (32 * 1024)
        #ошибка «Увеличьте размерLV_DRAW_THREAD_STACK_SIZEкак минимум до 32 КБ для FreeType или ThorVG».
    #конец

    #если определено ( LV_DRAW_THREAD_STACKSIZE ) && ! определено ( LV_DRAW_THREAD_STACK_SIZE )
        #предупреждение «LV_DRAW_THREAD_STACKSIZEбыл переименован вLV_DRAW_THREAD_STACK_SIZE. Пожалуйста, обновитеlv_conf.hили запустите меню настройки еще раз».
        #определить LV_DRAW_THREAD_STACK_SIZE LV_DRAW_THREAD_STACKSIZE
    #конец
#конец

/*Allow only upper case letters and '/'  ('/' is a special case for backward compatibility)*/
#определить LV_FS_IS_VALID_LETTER (l) ((l) == '/' || ((l) >= 'A' && (l) <= 'Z'))

/* If running without lv_conf.h, add typedefs with default value. */
#ifdef LV_CONF_SKIP
    #если определено(_MSC_VER) && !define(_CRT_SECURE_NO_WARNINGS) /*отключить преобразование для Visual Studio*/
        #определить _CRT_SECURE_NO_WARNINGS
    #конец
#endif /* определение(LV_CONF_SKIP)*/

#endif /*LV_CONF_INTERNAL_H*/
'''
)

fin.close()
fout.close()

check_for_tabs(LV_CONF_INTERNAL)
