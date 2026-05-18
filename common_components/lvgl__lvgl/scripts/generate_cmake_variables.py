#!/usr/bin/env python3

#
# Сгенерируйте переменные cmakeCONFIG_LV_USE_* илиCONFIG_LV_BUILD_* из
# предварительно обработанный lv_conf_internal.h
#
# Автор: ДэвидTRUAN(david.truan@edgemtech.ch)
# Автор: Эрик Тагиров (erik.tagiros@edgemtech.ch)
#

import os
import argparse
import re

def fatal(msg):
    print()
    print("ERROR! " + msg)
    exit(1)

def get_args():
    parser = argparse.ArgumentParser(formatter_class=argparse.RawTextHelpFormatter, description=""
                                     "Convert the expanded lv_conf_internal.h to cmake variables."
                                     "It converts all LV_USE_*, LV_BUILD_* configurations."
                                    )

    parser.add_argument('--input', type=str, required=True, nargs='?',
                        help='Path of the macro expanded lv_conf_internal.h, which should be generated during a cmake build')

    parser.add_argument('--output', type=str, required=True, nargs='?',
                        help='Path of the output file, where the cmake variables declaration will be written (ex: build/lv_conf.cmake)')

    parser.add_argument("--kconfig", action="store_true", help="Enable kconfig flag")

    parser.add_argument("--debug", action="store_true", required=False, help="Show unhandled expressions")

    parser.add_argument("--parentscope", action="store_true", required=False, help="Additionally set the variables in the parent scope")

    args = parser.parse_args()

    # Входные данные должны существовать
    if not os.path.exists(args.input):
        fatal(f"Input {args.input} not found")

    return args

def write_set_cmd(fout, expr, is_parent_scope):

    fout.write(f'set({expr})\n')

    # Это делает переменную доступной для использования из каталога верхнего уровня.
    if is_parent_scope == True:
        fout.write(f'set({expr} PARENT_SCOPE)\n')

def generate_cmake_variables(path_input: str, path_output: str, kconfig: bool, debug: bool, is_parent_scope: bool):
    fin = open(path_input)
    fout = open(path_output, "w", newline='')

    # Если мы используем Kconfig, мы должны проверитьCONFIG_LV_USE_* и 
    # CONFIG_LV_BUILD_ * определяет
    if kconfig:
        CONFIG_PATTERN="^#define +(CONFIG_LV_USE|CONFIG_LV_BUILD|CONFIG_LV_[0-9A-Z_]+_USE)"
        CONFIG_PREFIX=""
    # В противном случае проверьте, что LV_USE_ * и LV_BUILD_ * определяют
    else:
        CONFIG_PATTERN="^#define +(LV_USE|LV_BUILD|LV_[0-9A-Z_]+_USE)"
        CONFIG_PREFIX="CONFIG_"


    # использовать расширенный lv_conf_internal, мы не имеем дело с регулярными выражениями,
    # поскольку все#defineбудут выровнены слева с одним пробелом перед значением
    for line in fin.read().splitlines():

        # К конфигам LV_USE_STDLIB_* относимся особым образом, как нам нужно
        # чтобы преобразовать определение в полную конфигурацию с 1 значением, когда оно включено
        if re.search(f'{CONFIG_PATTERN}_STDLIB', line):

            parts = line.split()
            if len(parts) < 3:
                continue

            name = parts[1]
            value = parts[2].strip()

            type = value.split("LV_STDLIB_")[1]

            name = name.replace("STDLIB", type)

            write_set_cmd(fout, f'{CONFIG_PREFIX}{name} 1', is_parent_scope)

        # Отнеситесь к конфигу LV_USE_OS по-особому, так как нам нужно
        # чтобы преобразовать определение в полную конфигурацию с 1 значением, когда оно включено
        if re.search(f'{CONFIG_PATTERN}_OS', line):

            parts = line.split()
            if len(parts) < 3:
                continue

            name = parts[1]
            value = parts[2].strip()

            type = value.split("LV_OS")[1]

            name += type

            write_set_cmd(fout, f'{CONFIG_PREFIX}{name} 1', is_parent_scope)

        # Для остальных конфигов просто добавьте CONFIG_ и напишите имя дефайна
        # все конфиги LV_USE_* или LV_BUILD_*, где значение 0 или 1,
        # поскольку это те, которые нужны в cmake
        # Чтобы обнаружить конфигурацию LVGL для выполнения условной компиляции/связывания
        elif re.search(f'{CONFIG_PATTERN}.* +[01] *$', line):

            parts = line.split()
            if len(parts) < 3:
                continue

            name = parts[1]
            value = parts[2].strip()

            write_set_cmd(fout, f'{CONFIG_PREFIX}{name} {value}', is_parent_scope)

        else:
            # Полезно для отладки необработанных выражений.
            # если сценарий дает сбой «неожиданным образом»
            if debug == True:
                print(f"DBG: Skipping expression: '{line} - not handled'")


if __name__ == '__main__':
    args = get_args()

    generate_cmake_variables(args.input, args.output, args.kconfig, args.debug, args.parentscope)
