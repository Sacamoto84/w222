#!/usr/bin/env python3

import argparse
from typing import List, Tuple
import sys
import os

try:
    from fontTools.ttLib import TTFont
except ImportError:
    print("Need fonttools package, do `pip3 install fonttools`")
    sys.exit(1)

# Шрифты, исключенные из проверки лицензии
# Добавляйте только те шрифты, которые, как известно, являются общедоступными или имеют совместимую лицензию.
_EXCLUDED_FONTS = {
    "OpenTypeTest GPOS One",
}

# Сопоставление имен шрифтов для удаления любого суффикса стиля.
_FONT_NAME_MAP = {
    "Montserrat Medium": "Montserrat",
    "Montserrat SemiBold": "Montserrat",
    "Montserrat Bold": "Montserrat",
    "Montserrat Regular": "Montserrat",
    "Source Han Sans SC Normal": "Source Han Sans SC",
    "Font Awesome 5 Free Solid": "Font Awesome",
    "Lato Regular": "Lato",
    "Noto Sans SC Regular": "Noto Sans SC",
}


def get_font_full_name(font_path: str) -> str:
    font = TTFont(font_path)
    name_records = font["name"].names
    fallback = None
    for record in name_records:
        if record.nameID == 1:  # ID 1 corresponds to the font family name and will be used if full name doesn't exists
            fallback = record.toStr()
        if record.nameID == 4:  # ID 4 corresponds to the full font name
            return record.toStr()
    return fallback


def list_intree_fonts(path: str) -> List[Tuple[str, str]]:
    fonts = []
    for root, _, files in os.walk(path):
        for file in files:
            if file.lower().endswith((".ttf", ".otf", ".woff", ".woff2")):
                font_path = os.path.join(root, file)
                font_name = get_font_full_name(font_path).strip()
                if font_name:
                    # Добавьте нестрогий режим, чтобы игнорировать отсутствующие файлы лицензий.
                    fonts.append((font_path, font_name))
    return fonts


def has_intree_license(license_root_folder: str, font_name: str) -> bool:
    if font_name in _EXCLUDED_FONTS:
        return True

    # Подготовьте имена папок-кандидатов, игнорируя регистр.
    candidates = {
        font_name.lower(),
        font_name.replace(" ", "_").lower(),
        font_name.replace(" ", "").lower(),
    }

    if font_name in _FONT_NAME_MAP:
        candidates.add(_FONT_NAME_MAP[font_name].lower())
        candidates.add(_FONT_NAME_MAP[font_name].replace(" ", "_").lower())
        candidates.add(_FONT_NAME_MAP[font_name].replace(" ", "").lower())

    # Список всех каталогов в license_root_folder
    for entry in os.listdir(license_root_folder):
        entry_path = os.path.join(license_root_folder, entry)
        if os.path.isdir(entry_path):
            # Сравните имя каталога в нижнем регистре с кандидатами
            if entry.lower() in candidates:
                # Проверьте, содержит ли каталог хотя бы один файл (игнорируя подкаталоги)
                for item in os.listdir(entry_path):
                    item_path = os.path.join(entry_path, item)
                    if os.path.isfile(item_path):
                        return True
    return False


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Verify font licenses")
    parser.add_argument(
        "--no-strict",
        action="store_true",
        help="Ignore missing license files",
        default=False,
    )
    args = parser.parse_args()

    SCRIPT_DIR = os.path.dirname(__file__)
    # Список файлов шрифтов в дереве
    fonts = list_intree_fonts(os.path.join(SCRIPT_DIR, ".."))
    has_font_without_license = False

    print("Verifying license for fonts:")
    for path, name in fonts:
        if has_intree_license(
            os.path.join(SCRIPT_DIR, "built_in_font", "font_license"), name
        ):
            print(f"  [OK] '{name}'")
        else:
            print(f"  [MISSING] '{name}' - {path}")
            has_font_without_license = True

    if has_font_without_license and not args.no_strict:
        sys.exit(1)
