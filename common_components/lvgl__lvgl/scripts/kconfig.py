#!/usr/bin/env python3

# Первоначально изменено из:
# https://github.com/zephyrproject-rtos/zephyr/blob/main/scripts/kconfig/kconfig.py

# SPDX-License-Identifier: ISC

# Записывает/обновляет файл конфигурации lvgl/.config посредством объединения конфигураций.
# файлы, передаваемые в качестве аргументов
#
# Если фрагменты не изменились, lvgl/.config одновременно является входным и
# вывод, который просто обновляет его. Это обрабатывается в файлах CMake.
#
# Также выполните различные проверки (большинство с помощью предупреждений Kconfiglib).

import argparse
import os
import re
import sys
import textwrap

# Lvgl не использует символы с состояниями. Их упоминают здесь только для того, чтобы
# сценарий немного более общий.
from kconfiglib import (
    Kconfig,
    split_expr,
    expr_value,
    expr_str,
    BOOL,
    TRISTATE,
    TRI_TO_STR,
    AND,
    OR,
)


def main():
    print(sys.argv)
    args = parse_args()

    print("Parsing " + args.kconfig_file)
    kconf = Kconfig(args.kconfig_file, warn_to_stderr=False, suppress_traceback=True)

    if args.handwritten_input_configs:
        # Предупреждать о присвоении неопределенных символов, но только для рукописных
        # фрагменты, чтобы избежать ошибок, ставших предупреждениями, при использовании старого
        # конфигурация файла вместе с обновленными файлами Kconfig
        kconf.warn_assign_undef = True

        # prj.conf может переопределить настройки конфигурации платы, поэтому
        # отключить предупреждения о присвоении символов более одного раза
        kconf.warn_assign_override = False
        kconf.warn_assign_redun = False

    if args.forced_input_configs:
        # Не предупреждать о избыточной конфигурации.
        # Причина в том, что обычный .config последует принудительный
        # конфиг, который при нормальных обстоятельствах должен быть идентичен
        # настроенная настройка.
        # Только если пользователь изменил значение, которое отменяется принудительным
        # выносится предупреждение.
        kconf.warn_assign_redun = False

    # Загрузить файлы
    print(kconf.load_config(args.configs_in[0]))
    for config in args.configs_in[1:]:
        # replace=False создает объединенную конфигурацию
        print(kconf.load_config(config, replace=False))

    if args.handwritten_input_configs:
        # Убедитесь, что нет назначений символам без подсказок, которые
        # не имеют никакого эффекта.
        #
        # Это имеет смысл только при загрузке рукописных фрагментов, а не при загрузке
        # загрузка lvgl/.config, поскольку lvgl/.config — это изменение
        # вывод, а также назначает символы без подсказок.
        check_no_promptless_assign(kconf)

        # Выводить предупреждения для символов, которым не присвоено значение. Только
        # сделайте это и для рукописного ввода, чтобы избежать возможных бесполезных предупреждений.
        # при использовании примерных конфигураций и обновленных файлов Kconfig.
        check_assigned_sym_values(kconf)
        check_assigned_choice_values(kconf)

    if kconf.syms.get("WARN_DEPRECATED", kconf.y).tri_value == 2:
        check_deprecated(kconf)

    if kconf.syms.get("WARN_EXPERIMENTAL", kconf.y).tri_value == 2:
        check_experimental(kconf)

    # Взлом: принудительно оценивать все символы, чтобы перехватывать генерируемые предупреждения.
    # во время оценки. Подождите до конца, чтобы записать фактические выходные файлы, поэтому
    # что мы не генерируем никаких результатов, если есть предупреждения, ставшие ошибками.
    #
    # Kconfiglib кэширует вычисленные значения символов внутри себя, так что это по-прежнему
    # быстро.
    kconf.write_config(os.devnull)

    warn_only = r"warning:.*set more than once."

    if kconf.warnings:
        if args.forced_input_configs:
            error_out = False
        else:
            error_out = True

        # Поместите пустую строку между предупреждениями, чтобы их было легче читать.
        for warning in kconf.warnings:
            print("\n" + warning, file=sys.stderr)

            if not error_out and not re.search(warn_only, warning):
                # Предупреждение неwarn_only, ошибка Kconfig.
                error_out = True

        # Превратите все предупреждения в ошибки, чтобы, например, присваивания неопределенному
        # Символы Kconfig становятся ошибками.
        #
        # Этот скрипт генерирует предупреждение всякий раз, когда символ получает
        # значение, отличное от того, которое ему было присвоено. Пусть это будет просто
        # предупреждение на данный момент.
        if error_out:
            err("Aborting due to Kconfig warnings")

    # Напишите объединенную конфигурацию и заголовок C.
    print(kconf.write_config(args.config_out))
    print(kconf.write_autoconf(args.header_out))

    # Записать список разобранных файлов Kconfig в файл
    write_kconfig_filenames(kconf, args.kconfig_list_out)


def check_no_promptless_assign(kconf):
    # Проверяет, что не назначены символы без подсказок.

    for sym in kconf.unique_defined_syms:
        if sym.user_value is not None and promptless(sym):
            err(
                f"""\
{sym.name_and_loc} is assigned in a configuration file, but is not directly
user-configurable (has no prompt). It gets its value indirectly from other
symbols. """
                + SYM_INFO_HINT.format(sym)
            )


def check_assigned_sym_values(kconf):
    # Проверяет, что значения, присвоенные символам, «приняли» (соответствуют значению
    # символы действительно попали), в противном случае печатаются предупреждения. Символы выбора
    # проверяются отдельно, в check_assigned_choice_values().

    for sym in kconf.unique_defined_syms:
        if sym.choice:
            continue

        user_value = sym.user_value
        if user_value is None:
            continue

        # Значения трех состояний представлены как 0, 1, 2. Имея их как «n», «m»,
        # «y» здесь удобнее, так что конвертируйте.
        if sym.type in (BOOL, TRISTATE):
            user_value = TRI_TO_STR[user_value]

        if user_value != sym.str_value:
            msg = (
                f"{sym.name_and_loc} was assigned the value '{user_value}'"
                f" but got the value '{sym.str_value}'. "
            )

            # Перечислите все неудовлетворенные зависимости «зависит от» в предупреждении.
            mdeps = missing_deps(sym)
            if mdeps:
                expr_strs = []
                for expr in mdeps:
                    estr = expr_str(expr)
                    if isinstance(expr, tuple):
                        # Add () о зависимостях, которые не являются простыми символами.
                        # Возвращает '( FOO || BAR ) (=n)' вместо
                        # ' FOO ||  BAR (=n)', что могло бы быть более понятным.
                        estr = f"({estr})"
                    expr_strs.append(f"{estr} " f"(={TRI_TO_STR[expr_value(expr)]})")

                msg += (
                    "Check these unsatisfied dependencies: "
                    + ", ".join(expr_strs)
                    + ". "
                )

            warn(msg + SYM_INFO_HINT.format(sym))


def missing_deps(sym):
    # check_assigned_sym_values () помощник для устранения неудовлетворенных зависимостей.
    #
    # Учитывая прямые зависимости
    #
    # зависит от <выражение> && <выражение> && ... && <выражение>
    #
    # для 'sym' (который также может превращаться, например, из окружающего 'if'), возвращает
    # список всех <выражений> со значением, которое приводится, чем вводное значение 'sym'
    # («меньше» вместо «не равно», просто чтобы быть общим и обрабатывать три состояния,
    # хотя лвгл их не использует).
    #
    # Для символов string/int/hex просто ищется <expr> = n.
    #
    # Обратите внимание, что <expr> может быть чем-то более сложным, чем простой символ.
    # например ' FOO ||  BAR ' или ' FOO = "строка"'.

    deps = split_expr(sym.direct_dep, AND)

    if sym.type in (BOOL, TRISTATE):
        return [dep for dep in deps if expr_value(dep) < sym.user_value]
    # string/int/hex
    return [dep for dep in deps if expr_value(dep) == 0]


def check_assigned_choice_values(kconf):
    # Проверяет, что все выбранные символы выбора (устанавливая для них значение
    # y) оказался выбранным, в противном случае печатались предупреждения.
    #
    # Мы проверяем символы выбора отдельно, чтобы избежать предупреждений, когда два разных
    # символы выбора в рамках одного и того же выбора устанавливаются на y. Это может произойти, если
    # выбор выбора платы defconfig переопределяется в prj.conf,
    # например. Последний символ выбора, установленный на y, становится выбором (и
    # все остальные символы выбора получают значение n).
    #
    # Без специального выбора регистра мы бы обнаружили, что первый символ имеет значение
    # y оказался n и напечатал ложное предупреждение.

    for choice in kconf.unique_choices:
        if choice.user_selection and choice.user_selection is not choice.selection:

            warn(
                f"""\
The choice symbol {choice.user_selection.name_and_loc} was selected (set =y),
but {choice.selection.name_and_loc if choice.selection else "no symbol"} ended
up as the choice selection. """
                + SYM_INFO_HINT.format(choice.user_selection)
            )


# Подсказка о том, где найти информацию о символах. Используется как
# SYM_INFO_HINT .format(sym).
SYM_INFO_HINT = """\
See https://docs.lvgl.io/master/integration/overview/configuration.html
look up {0.name} in the menuconfig/guiconfig interface. The Application
Development Primer, Setting Configuration Values, and Kconfig - Tips and Best
Practices sections of the manual might be helpful too.\
"""


def check_deprecated(kconf):
    deprecated = kconf.syms.get("DEPRECATED")
    dep_expr = kconf.n if deprecated is None else deprecated.rev_dep

    if dep_expr is not kconf.n:
        selectors = [s for s in split_expr(dep_expr, OR) if expr_value(s) == 2]
        for selector in selectors:
            selector_name = split_expr(selector, AND)[0].name
            warn(f"Deprecated symbol {selector_name} is enabled.")


def check_experimental(kconf):
    experimental = kconf.syms.get("EXPERIMENTAL")
    dep_expr = kconf.n if experimental is None else experimental.rev_dep

    if dep_expr is not kconf.n:
        selectors = [s for s in split_expr(dep_expr, OR) if expr_value(s) == 2]
        for selector in selectors:
            selector_name = split_expr(selector, AND)[0].name
            warn(f"Experimental symbol {selector_name} is enabled.")


def promptless(sym):
    # Возвращает True, если у «sym» нет подсказок. as может быть символом в
    # несколько мест, нам нужно проверить все местоположения.

    return not any(node.prompt for node in sym.nodes)


def write_kconfig_filenames(kconf, kconfig_list_path):
    # Записывает отсортированный список с абсолютными путями всех проанализированных файлов Kconfig.
    # на «kconfig_list_path». Пути имеют вид realpath(), дубликаты
    # удален. Этот файл использует CMake для поиска измененных файлов Kconfig. Это
    # должен быть детерминистическим.

    with open(kconfig_list_path, "w") as out:
        for path in sorted(
            {
                os.path.realpath(os.path.join(kconf.srctree, path))
                for path in kconf.kconfig_filenames
            }
        ):
            print(path, file=out)


def parse_args():
    parser = argparse.ArgumentParser(allow_abbrev=False)

    parser.add_argument(
        "--handwritten-input-configs",
        action="store_true",
        help="Assume the input configuration fragments are "
        "handwritten fragments and do additional checks "
        "on them, like no promptless symbols being "
        "assigned",
    )
    parser.add_argument(
        "--forced-input-configs",
        action="store_true",
        help="Indicate the input configuration files are "
        "followed by an forced configuration file."
        "The forced configuration is used to forcefully "
        "set specific configuration settings to a "
        "pre-defined value and thereby remove any user "
        " adjustments.",
    )
    parser.add_argument("kconfig_file", help="Top-level Kconfig file")
    parser.add_argument("config_out", help="Output configuration file")
    parser.add_argument("header_out", help="Output header file")
    parser.add_argument(
        "kconfig_list_out", help="Output file for list of parsed Kconfig files"
    )
    parser.add_argument(
        "configs_in",
        nargs="+",
        help="Input configuration fragments. Will be merged " "together.",
    )

    return parser.parse_args()


def warn(msg):
    # Используйте больший вариантfill(), чтобы избежать разрывов символьных строк.
    # ссылочную ссылку и добавьте несколько дополнительных строк новой строки, чтобы отключить сообщение от
    # окружающий текст (обычно он печатается как часть спама CMake).
    # выход)
    print("\n" + textwrap.fill("warning: " + msg, 100) + "\n", file=sys.stderr)


def err(msg):
    sys.exit("\n" + textwrap.fill("error: " + msg, 100) + "\n")


if __name__ == "__main__":
    main()