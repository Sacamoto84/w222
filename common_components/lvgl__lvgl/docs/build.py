#!/usr/bin/env python3
""" build.py -- Generate LVGL documentation using Doxygen and Sphinx + Breathe.

Data Flow
---------

.. code-block:: text

    Inputs              Generated Source Files             Output
    -----------         ----------------------       ----------------------
    ./docs/src/   \
    ./src/         >===> ./docs/intermediate/  ===>  ./docs/build/<format>/
    ./examples/   /

    Once ./docs/intermediate/ is built, you can use all the Sphinx output
    formats, e.g.

    - make html
    - make latex
    - make man
    - make htmlhelp
    - etc.


Description
-----------
    Copy source files to an intermediate directory and modify them there before
    doc generation occurs.  If a full rebuild is being done (e.g. after a `clean`)
    run Doxygen on LVGL's source files to generate intermediate API information
    in XML format.  Generate API documents for Breathe's consumption.  Add API
    links to end of some documents.  Generate example documents.  From there,
    Sphinx with Breathe extension uses the resulting set of intermediate files
    to generate the desired output.

    It is only during this first build that the `--skip-api` option has meaning.
    After the first build, no further actions is taken regarding API pages since
    they are not regenerated after the first build.

    The intermediate directory has a fixed location (overridable by
    `LVGL_DOC_BUILD_INTERMEDIATE_DIR` environment variable) and by default this
    script attempts to rebuild only those documents whose path, name or
    modification date has changed since the last build.

    The output directory also has a fixed location (overridable by
    `LVGL_DOC_BUILD_OUTPUT_DIR` environment variable).

    Caution:

    The document build meant for end-user consumption should ONLY be done after a
    `--clean` unless you know that no API documentation and no code examples have changed.

    A `sphinx-build` will do a full doc rebuild any time:

    - the intermediate directory doesn't exist or is empty (since the new files in
      the intermediate directory will have modification times after the generated
      HTML or Latex files, even if nothing changed),
    - the targeted output directory doesn't exist or is empty, or
    - Sphinx determines that a full rebuild is necessary.  This happens when:
        - intermediate directory path (Sphinx's source-file path) has changed,
        - any options on the `sphinx-build` command line have changed,
        - `conf.py` modification date has changed, or
        - `--fresh-env` argument is included (runs `sphinx-build` with -E option).

    Typical run time:

    Full build:  22.5 min
    --skip-api:   1.9 min  (applies to first build only)


Usage
-----
    usage: build.py [-h] [-s] [-E]
                    {html,latex,intermediate,clean,clean-intermediate,clean-html,clean-latex}
                    [{html,latex,intermediate,clean,clean-intermediate,clean-html,clean-latex} ...]

    Build LVGL documents

    positional arguments:
      {html,latex,intermediate,clean,clean-intermediate,clean-html,clean-latex}
                            output targets to generate; one or more of these;
                              `clean...` targets are completed first

    options:
      -h, --help            show this help message and exit
      -s, --skip-api        skip API-page generation
      -E, --fresh-env       rebuild Sphinx environment


Options
-------
    --help
        Print usage note and exit with status 0.

  Targets:

    html [ --skip-api ] [ --fresh-env ]
        Build HTML output.
        `--skip-api` only has effect on first build after a `clean` or `clean_intermediate`.

    latex [ --skip-api ] [ --fresh-env ]
        Build Latex/PDF output (on hold pending removal of non-ASCII characters from input files).
        `--skip-api` only has effect on first build after a `clean` or `clean_intermediate`.

    intermediate [ --skip-api ]
        Generate intermediate directory contents (ready to build output formats).
        If they already exist, they are removed and re-generated.
        Note:  "intermediate" can be abbreviated down to "int".

    clean
        Remove all generated files.

    clean-intermediate
        Remove intermediate directory.
        Note:  "clean_intermediate" can be abbreviated down to "clean_int".

    clean-html
        Remove HTML output directory.

    clean-latex
        Remove Latex output directory.

  Options with Output Targets:

    --skip-api (-s) (with `html` and/or `latex` and/or `intermediate` options)
        Skip API pages and links when intermediate directory contents are being generated
        (saving about 91% of build time).  Note: they are not thereafter regenerated unless
        requested by `intermediate` argument or the intermediate directory does not
        exist.  This is intended to be used only during doc development to speed up
        turn-around time between doc modifications and seeing final results.

    --fresh-env (-E) (with `html` and/or `latex` options)
        Run `sphinx-build` with -E command-line argument, which makes it regenerate its
        "environment" (memory of what was built previously, forcing a full rebuild).

    Unrecognized arguments print error message, usage note, and exit with status 1.


Python Package Requirements
---------------------------
    The list of Python package requirements are in `requirements.txt`.

    Install them by:

    $ pip install -r requirements.txt


History
-------
    The first version of this file (Apr 2021) discovered the name of
    the current branch (e.g. 'master', 'release/v8.4', etc.) to support
    different versions of the documentation by establishing the base URL
    (used in `conf.py` and in [Edit on GitHub] links), and then ran:

    - Doxygen (to generate LVGL API XML), then
    - Sphinx

    to generate the LVGL document tree.  Internally, Sphinx uses `breathe`
    (a Sphinx extension) to provide a bridge between Doxygen XML output and
    Sphinx documentation.  It also supported a command-line option `--clean`
    to remove generated files before starting (eliminates orphan files,
    for docs that have moved or changed).

    Since then its duties have grown to include:

    - Using environment variables to convey branch names to several more
      places where they are used in the docs-generating process (instead
      of re-writing `conf.py` and a `header.rst` each time docs were
      generated).  These are documented where they generated below.

    - Supporting additional command-line options.

    - Generating a `./docs/lv_conf.h` for Doxygen to use (config_builder.py).

    - Supporting multiple execution platforms (which then required tokenizing
      Doxygen's INPUT path in `Doxyfile` and re-writing portions that used
      `sed` to generate input or modify files).

    - Adding translation and API links (requiring generating docs in an
      intermediate directory so that the links could be programmatically
      added to each document before Sphinx was run).  Note:  translation links
      are now done manually since they are only on the main landing page.

    - Generating EXAMPLES page.  Specific example groups are added to individual
      documents by each document having an ``.. include:: /examples/.../index.rst``
      directive where the example(s) should go.

    - Building PDF via latex.

    - Shifting doc-generation paradigm to behave more like `make`.

"""

# ****************************************************************************
# IMPORTANT: Если вы внесли ошибкуPDF-lexer, например, проверьте
# для дополнительных строк в конце файла. Только одна пустая строка
# разрешено!!! Спросите меня, сколько времени мне понадобилось, чтобы это понять.
# -- @kdschlosser
# ****************************************************************************

# Библиотека Python
import sys
import os
import subprocess
import shutil
import dirsync
import argparse
from datetime import datetime

# LVGL Пользовательский
import example_list
import api_doc_builder
import config_builder
from src.lvgl_version import lvgl_version
from announce import *

# В настоящее время не используется
# (Код сохраняется на случай, если мы захотим его повторно реализовать позже.)
# импорт add_translation

# -------------------------------------------------------------------------
# Конфигурация
# -------------------------------------------------------------------------
# Это соответствующие пути из каталога ./docs/.
cfg_project_dir = '..'
cfg_lvgl_src_dir = 'src'
cfg_doc_src_dir = 'src'
cfg_examples_dir = 'examples'
cfg_default_intermediate_dir = 'intermediate'
cfg_default_output_dir = 'build'
cfg_static_dir = '_static'
cfg_downloads_dir = 'downloads'
cfg_lv_conf_filename = 'lv_conf.h'
cfg_lv_version_filename = 'lv_version.h'
cfg_doxyfile_filename = 'Doxyfile'
cfg_top_index_filename = 'index.rst'
cfg_default_branch = 'master'
cfg_target_html = 'html'
cfg_target_latex = 'latex'
cfg_target_intermediate = 'intermediate'
cfg_target_clean_all = 'clean'
cfg_target_clean_intermediate = 'clean-intermediate'
cfg_target_clean_html = 'clean-html'
cfg_target_clean_latex = 'clean-latex'

# Имя файла, сгенерированное в`latex_output_dir`и скопированное в`pdf_output_dir`.
cfg_pdf_filename = 'LVGL.pdf'


def remove_dir(tgt_dir):
    """Remove directory `tgt_dir`."""
    if os.path.isdir(tgt_dir):
        announce(__file__, f'Removing {tgt_dir}...')
        shutil.rmtree(tgt_dir)
    else:
        announce(__file__, f'{tgt_dir} already removed...')


def cmd(cmd_str, start_dir=None, exit_on_error=True):
    """Run external command and abort build on error."""
    saved_dir = None

    if start_dir is not None:
        saved_dir = os.getcwd()
        os.chdir(start_dir)

    announce(__file__, f'Running [{cmd_str}] in [{os.getcwd()}]...')
    return_code = os.system(cmd_str)

    if saved_dir is not None:
        os.chdir(saved_dir)

    if return_code != 0 and exit_on_error:
        announce(__file__, "Exiting build due to previous error.")
        sys.exit(1)


def intermediate_dir_contents_exists(intermediate_dir):
    """Provide answer to question:  Can we have reasonable confidence that
    the contents of `intermediate_directory` already exists?
    """
    result = False
    c1 = os.path.isdir(intermediate_dir)

    if c1:
        temp_path = os.path.join(intermediate_dir, 'CHANGELOG.rst')
        c2 = os.path.exists(temp_path)
        temp_path = os.path.join(intermediate_dir, '_ext')
        c3 = os.path.isdir(temp_path)
        temp_path = os.path.join(intermediate_dir, '_static')
        c4 = os.path.isdir(temp_path)
        temp_path = os.path.join(intermediate_dir, 'debugging')
        c5 = os.path.isdir(temp_path)
        temp_path = os.path.join(intermediate_dir, 'introduction')
        c6 = os.path.isdir(temp_path)
        temp_path = os.path.join(intermediate_dir, 'contributing')
        c7 = os.path.isdir(temp_path)
        temp_path = os.path.join(intermediate_dir, cfg_examples_dir)
        c8 = os.path.isdir(temp_path)
        result = c2 and c3 and c4 and c5 and c6 and c7 and c8

    return result


def run():
    """Perform doc-build function(s) requested."""

    def print_setting(setting_name, val):
        """Print one setting; used for debugging."""
        announce(__file__, f'{setting_name:18} = [{val}]')

    def print_settings(args, and_exit: bool):
        """Print all settings and optionally exit; used for debugging.

        This routine has proven useful for verification and validation,
        and is being kept for future debugging purposes.
        """
        # Цели
        print_setting("build_html", cfg_target_html in args.targets)
        print_setting("build_latex", cfg_target_latex in args.targets)
        print_setting("build_intermediate", cfg_target_intermediate in args.targets)
        print_setting("clean_all", cfg_target_clean_all in args.targets)
        print_setting("clean_intermediate", cfg_target_clean_intermediate in args.targets)
        print_setting("clean_html", cfg_target_clean_html in args.targets)
        print_setting("clean_latex", cfg_target_clean_latex in args.targets)
        # Опции
        print_setting("skip_api", args.skip_api)
        print_setting("fresh_sphinx_env", args.fresh_sphinx_env)

        if and_exit:
            exit(0)

    # ---------------------------------------------------------------------
    # Обработка аргументов командной строки.
    # ---------------------------------------------------------------------
    ap = argparse.ArgumentParser(description='Build LVGL documents')
    ap.add_argument('targets', nargs='+', choices=[
            cfg_target_html,
            cfg_target_latex,
            cfg_target_intermediate,
            cfg_target_clean_all,
            cfg_target_clean_intermediate,
            cfg_target_clean_html,
            cfg_target_clean_latex,
            ],
            help='output targets to generate; one or more of these;\n`clean...` targets are completed first')

    ap.add_argument('-s', '--skip-api' , action='store_true', dest='skip_api',
                help='skip API-page generation'     )
    ap.add_argument('-E', '--fresh-env', action='store_true', dest='fresh_sphinx_env',
                help='rebuild Sphinx environment'   )
    args = ap.parse_args()

    if cfg_target_clean_all in args.targets:
        if cfg_target_clean_intermediate not in args.targets:
            args.targets.append(cfg_target_clean_intermediate)
        if cfg_target_clean_html not in args.targets:
            args.targets.append(cfg_target_clean_html)
        if cfg_target_clean_latex not in args.targets:
            args.targets.append(cfg_target_clean_latex)

    # Опция '-E' заставляет Сфинкса перестроить свою среду, чтобы все документы были
    # полностью регенерируется, даже если не изменяется.
    # Примечание. Sphinx работает в ./docs/, но для ввода используется`intermediate_dir`.
    if args.fresh_sphinx_env:
        announce(__file__, "Force-regenerating all files...")
        env_opt = '-E'
    else:
        env_opt = ''

    # ---------------------------------------------------------------------
    # Начни.
    # ---------------------------------------------------------------------
    t0 = datetime.now()

    # ---------------------------------------------------------------------
    # Настройте пути.
    #
    # Суффиксы переменных:
    # _filename = имя файла без пути
    # _path = путь, ведущий к файлу или каталогу (абсолютный или относительный)
    # _file = путь, ведущий к файлу (абсолютный или относительный)
    # _dir = путь, ведущий к каталогу (абсолютный или относительный)
    # ---------------------------------------------------------------------
    base_dir = os.path.abspath(os.path.dirname(__file__))
    project_dir = os.path.abspath(os.path.join(base_dir, cfg_project_dir))
    examples_dir = os.path.join(project_dir, cfg_examples_dir)
    lvgl_src_dir = os.path.join(project_dir, cfg_lvgl_src_dir)

    # Установите промежуточный каталог.  Наличие переменной окружения
    # `LVGL_DOC_BUILD_INTERMEDIATE_DIR` переопределяет значение по умолчанию в`cfg_default_intermediate_dir`.
    if 'LVGL_DOC_BUILD_INTERMEDIATE_DIR' in os.environ:
        intermediate_dir = os.environ['LVGL_DOC_BUILD_INTERMEDIATE_DIR']
    else:
        intermediate_dir = os.path.join(base_dir, cfg_default_intermediate_dir)

    lv_conf_file = os.path.join(intermediate_dir, cfg_lv_conf_filename)
    lv_temp_conf_file_for_doxygen = os.path.join(lvgl_src_dir, cfg_lv_conf_filename)
    version_dst_file = os.path.join(intermediate_dir, cfg_lv_version_filename)
    top_index_file = os.path.join(intermediate_dir, cfg_top_index_filename)
    doxyfile_src_file = os.path.join(base_dir, cfg_doxyfile_filename)
    doxyfile_dst_file = os.path.join(intermediate_dir, cfg_doxyfile_filename)
    pdf_intermediate_dst_dir = os.path.join(intermediate_dir, cfg_static_dir, cfg_downloads_dir)
    pdf_intermediate_dst_file = os.path.join(pdf_intermediate_dst_dir, cfg_pdf_filename)
    sphinx_path_sep = '/'
    pdf_relative_file = cfg_static_dir + sphinx_path_sep + cfg_downloads_dir + sphinx_path_sep + cfg_pdf_filename
    pdf_link_ref_str = f'PDF Version: :download:`{cfg_pdf_filename} <{pdf_relative_file}>`'

    # Установите каталог сборки.  Наличие переменной окружения
    # `LVGL_DOC_BUILD_OUTPUT_DIR` переопределяет значение по умолчанию в`cfg_default_output_dir`.
    if 'LVGL_DOC_BUILD_OUTPUT_DIR' in os.environ:
        output_dir = os.environ['LVGL_DOC_BUILD_OUTPUT_DIR']
    else:
        output_dir = os.path.join(base_dir, cfg_default_output_dir)

    html_output_dir = os.path.join(output_dir, 'html')
    latex_output_dir = os.path.join(output_dir, 'latex')
    pdf_output_dir = os.path.join(output_dir, 'pdf')
    pdf_src_file = os.path.join(latex_output_dir, cfg_pdf_filename)
    pdf_dst_file = os.path.join(pdf_output_dir, cfg_pdf_filename)
    version_src_file = os.path.join(project_dir, cfg_lv_version_filename)

    # Специальные материалы для выравнивания по правому краю ссылки для скачивания PDF.
    # Примечание: это необходимо встроить в тег <div>, потому что
    # Роль Sphinx`:download:`приводит к появлению ссылки в теге <p>.
    # а в HTML5 теги <p> не могут быть вложенными!
    cfg_right_just_para_text = """.. raw:: html

    <div style="text-align: right;">"""
    cfg_end_right_just_para_text = """.. raw:: html

    </div>"""
    # Пустые строки необходимы в соответствии с директивами.
    cfg_pdf_link_ref_block_str = \
        cfg_right_just_para_text + os.linesep \
        + os.linesep \
        + pdf_link_ref_str + os.linesep + \
        os.linesep \
        + cfg_end_right_just_para_text + os.linesep \
        + os.linesep

    # ---------------------------------------------------------------------
    # Перейдите в каталог сценария для обеспечения согласованной среды выполнения.
    # ---------------------------------------------------------------------
    os.chdir(base_dir)
    announce(__file__, f'Intermediate dir:  [{intermediate_dir}]')
    announce(__file__, f'Output dir      :  [{output_dir}]')
    announce(__file__, f'Running from    :  [{base_dir}]')

    # ---------------------------------------------------------------------
    # Clean?  If so, clean (like `make clean`), but do not exit.
    # ---------------------------------------------------------------------
    some_cleaning_to_be_done = cfg_target_clean_intermediate in args.targets \
            or cfg_target_clean_html in args.targets \
            or cfg_target_clean_latex in args.targets \
            or cfg_target_clean_all in args.targets \
            or ( \
                os.path.isdir(intermediate_dir) \
                and cfg_target_intermediate in args.targets \
            )

    if some_cleaning_to_be_done:
        announce(__file__, "Cleaning...", box=True)

        if cfg_target_clean_intermediate in args.targets:
            remove_dir(intermediate_dir)

        if cfg_target_clean_html in args.targets:
            remove_dir(html_output_dir)

        if cfg_target_clean_latex in args.targets:
            remove_dir(latex_output_dir)

        if cfg_target_clean_all in args.targets:
            remove_dir(output_dir)

        if os.path.isdir(intermediate_dir) and cfg_target_intermediate in args.targets:
            remove_dir(intermediate_dir)

    # ---------------------------------------------------------------------
    # Заполните переменные среды LVGL_URLPATH и LVGL_GITCOMMIT:
    # - LVGL_URLPATH <= «главный» или «8,4», «9,2» и т. д.
    # - LVGL_GITCOMMIT <= то же самое (см. примечание от 3 октября 2024 г. ниже).
    #
    # Эти данные вводятся позже в процессе создания документа следующим образом:
    #
    # LVGL_URLPATH используется:
    # - `conf.py` для сборки`html_baseurl`для Sphinx для
    # - сгенерированный индекс
    # - создано окно поиска
    # - создание канонической страницы для поисковых систем
    # - `link_roles.py` для создания ссылок на перевод
    # - `doxygen_xml.py` для создания ссылок на страницы API.
    #
    # LVGL_GITCOMMIT используется:
    # - `conf.py` =>html_context['github_version'] для
    # Тема Sphinx Read-the-Docs для добавления в ссылки [Редактировать на GitHub]
    # - `conf.py` =>repo_commit_hashдля сгенерированных страницEXAMPLESдля:
    # - Кнопки [Просмотреть на GitHub] (просмотреть пример кода C)
    # - Кнопки [Просмотреть на GitHub] (просмотреть пример кода Python)
    # ---------------------------------------------------------------------
    # 03 октября 2024 г.: Габор попросил превратить LVGL_GITCOMMIT в ветку.
    # имя, поскольку оно всегда будет актуальным и исправит большую
    # количество неработающих ссылок на сайте документации, поскольку это зафиксировано
    # сгенерированные документы иногда могут исчезнуть.  Это используется в:
    # - Ссылки [Edit on GitHub] на страницы документации (через тему Sphinx) и
    # - Ссылки [View on GitHub] на примеры страниц (через `example_list.py`
    # и `lv_example.py`).
    # Исходный код:
    # статус, br = subprocess.getstatusoutput("git Branch --show-current")
    # _, gitcommit = subprocess.getstatusoutput("git rev-parse HEAD")
    # br = re.sub(r'\* ', '', br)
    # 're' ранее использовался для удаления начального '*' из текущей ветки
    # строку, когда мы анализировали вывод чистого вывода`git branch`.
    # Этого больше не требуется, поскольку теперь используется опция `--show-current`.
    # ---------------------------------------------------------------------
    status, branch = subprocess.getstatusoutput("git branch --show-current")

    # Если добиться не удалось (т. е.`branch`недействителен), по умолчанию используется «главный».
    if status != 0:
        branch = cfg_default_branch
    elif branch == cfg_default_branch:
        # Ожидаемо в большинстве случаев.  Ничего не изменить.
        pass
    else:
        # `branch` действителен.  Запишите версию выпуска, если она находится в ветке Release/.
        if branch.startswith('release/'):
            branch = branch[8:]
        else:
            # По умолчанию «мастер».
            branch = cfg_default_branch

    os.environ['LVGL_URLPATH'] = branch
    os.environ['LVGL_GITCOMMIT'] = branch

    # ---------------------------------------------------------------------
    # Подготовьте `intermediate_dir`, чтобы он стал исходным каталогом `sphinx-build`.
    # ---------------------------------------------------------------------
    # dirsync`exclude_list`= список шаблонов регулярных выражений, которые требуют аварийной ситуации.
    intermediate_re = r'^' + cfg_default_intermediate_dir + r'.*'
    output_re = r'^' + cfg_default_output_dir + r'.*'
    exclude_list = [r'lv_conf\.h', r'^__pycache__.*', intermediate_re, output_re]

    if intermediate_dir_contents_exists(intermediate_dir):
        # Мы просто обновляем криптовалюту intermediate_dir.
        announce(__file__, "Updating intermediate directory...", box=True)

        exclude_list.append(r'examples.*')
        options = {
            'verbose': True,   # Report files copied.
            'create': True,    # Create directories if they don't exist.
            'twoway': False,   # False means data flow only src => tgt.
            'purge': False,    # False means DO NOT remove orphan files/dirs in tgt dir (preserving examples/ dir).
            'exclude': exclude_list
        }
        # action == 'sync' означает копирование файлов, даже если их еще нет в каталоге tgt.
        # action == «обновление» означает, чтоDONOT копирует файлы, если они еще не существуют в каталоге tgt.
        dirsync.sync(cfg_doc_src_dir, intermediate_dir, 'sync', **options)
        dirsync.sync(examples_dir, os.path.join(intermediate_dir, cfg_examples_dir), 'sync', **options)
    elif cfg_target_intermediate in args.targets or cfg_target_html in args.targets or cfg_target_latex in args.targets:
        # Нам нужно создать атрибутыintermediate_dirпутем копирования.
        announce(__file__, "Building intermediate directory...", box=True)

        t1 = datetime.now()
        copy_method = 1

        # Оба эти метода работают.
        if copy_method == 0:
            # --------- Способ 0:
            ignore_func = shutil.ignore_patterns('tmp*', 'output*')
            announce(__file__, 'Copying docs...')
            shutil.copytree(cfg_doc_src_dir, intermediate_dir, ignore=ignore_func, dirs_exist_ok=True)
            announce(__file__, 'Copying examples...')
            shutil.copytree(examples_dir, os.path.join(intermediate_dir, cfg_examples_dir), dirs_exist_ok=True)
        else:
            # --------- Способ 1:
            options = {
                'create': True,    # Create directories if they don't exist.
                'exclude': exclude_list
            }
            # action == 'sync' означает копирование файлов, даже если их еще нет в каталоге tgt.
            # action == «обновление» означает, чтоDONOT копирует файлы, если они еще не существуют в каталоге tgt.
            announce(__file__, 'Copying docs...')
            dirsync.sync(cfg_doc_src_dir, intermediate_dir, 'sync', **options)
            announce(__file__, 'Copying examples...')
            dirsync.sync(examples_dir, os.path.join(intermediate_dir, cfg_examples_dir), 'sync', **options)

        # -----------------------------------------------------------------
        # Поставьте <intermediate_dir>/lv_conf.hизlv_conf_template.h.
        # -----------------------------------------------------------------
        config_builder.run(lv_conf_file)
        # Создайте временную версию этого файла в ../src/, чтобы Doxygen мог ее увидеть.
        # Причина: входные данные Doxygen — это главный каталог`lvgl/src/`,
        # не промежуточный каталог.  Этот файл будет удален позже.
        config_builder.run(lv_temp_conf_file_for_doxygen)

        # -----------------------------------------------------------------
        # Скопируйте`lv_version.h`в промежуточный каталог.
        # -----------------------------------------------------------------
        shutil.copyfile(version_src_file, version_dst_file)

        # -----------------------------------------------------------------
        # Создайте страницы с примерами.  Включить подстраницы, которые будут включены
        # в отдельных документах, где это применимо.
        # -----------------------------------------------------------------
        announce(__file__, "Generating examples...")
        example_list.make_warnings_into_errors()
        example_list.exec(intermediate_dir)

        # -----------------------------------------------------------------
        # Добавьте ссылки на перевод.
        # Это пропускается в пользу размещения вручную
        # ссылка на перевод вверху`./docs/index.rst`.
        # -----------------------------------------------------------------
        # Исходный код:
        # если правда:
        # анонс(__file__, "Пропуск добавления ссылок на перевод.")
        # еще:
        # анонс(__file__, "Добавление ссылок на перевод...")
        # add_translation .exec(intermediate_dir)

        if args.skip_api:
            announce(__file__, "Skipping API generation as requested.")
        else:
            # -------------------------------------------------------------
            # Генерируйте страницы API и ссылки на них.
            # -------------------------------------------------------------
            announce(__file__, "API page and link processing...")
            api_doc_builder.EMIT_WARNINGS = False

            # api_doc_builder . run()=>doxy_xml_parser. DoxygenXml()сейчас:
            # - подготавливает и запускает Doxygen, генераторXML,
            # - загружается сгенерированный XML .
            # Тогда api_doc_builder. run():
            # - создает. файлы RST для страниц API и
            # - добавляет гиперссылки API в файлы . RST файлы в каталогах переданного массива.
            api_doc_builder.build_api_docs(lvgl_src_dir,
                                           intermediate_dir,
                                           doxyfile_src_file,
                                           'auxiliary-modules',
                                           'common-widget-features',
                                           'contributing',
                                           'debugging',
                                           'getting_started',
                                           'guides',
                                           'integration',
                                           'introduction',
                                           'libs',
                                           'main-modules',
                                           'widgets',
                                           'xml',
                                           )

        # Теперь, когда Doxygen запущен, этот файл больше не нужен.
        # Выполняйте очистку сейчас, а не позже, чтобы сохранить логику в чистоте.
        os.remove(lv_temp_conf_file_for_doxygen)

        # Отметьте время завершения этого этапа.
        t2 = datetime.now()
        announce(__file__, 'Example/API run time:  ' + str(t2 - t1))

    # ---------------------------------------------------------------------
    # Сборка PDF
    # ---------------------------------------------------------------------
    if cfg_target_latex not in args.targets:
        announce(__file__, "Skipping Latex build.")
    else:
        t1 = datetime.now()
        announce(__file__, "Building Latex output...", box=True)

        # Если ссылкаPDFнаходится в верхней частиindex.rst, удалите ее, чтобы PDF
        # не имеет ссылки на себя.
        with open(top_index_file, 'rb') as f:
            index_data = f.read().decode('utf-8')

        if pdf_link_ref_str in index_data:
            index_data = index_data.replace(pdf_link_ref_str, '')

            with open(top_index_file, 'wb') as f:
                f.write(index_data.encode('utf-8'))

        src = intermediate_dir
        dst = output_dir
        cpu = os.cpu_count()

        # Опция -D правильно заменяет (переопределяет) атрибут конфигурации.
        # значения в модуле `conf.py`.  поскольку`conf.py`теперь правильно
        # вычислите свое собственное значение `version`, нам не нужно его здесь переопределять
        # с опцией -D.  Если его понадобится использовать в будущем,
        # значение после '=" MUST NOT заключено в кавычки
        # или это не сработает.  Правильное использование: f'-D version={ver}' .
        cmd_line = f'sphinx-build -M latex "{src}" "{dst}" -j {cpu} --fail-on-warning --keep-going'
        cmd(cmd_line)

        # Сгенерируйте PDF .
        announce(__file__, "Building PDF...", box=True)
        cmd_line = 'latexmk -pdf "LVGL.tex"'
        cmd(cmd_line, latex_output_dir, False)

        # Переместите полученный PDF в выходной каталог.
        if not os.path.exists(pdf_output_dir):
            os.makedirs(pdf_output_dir)

        shutil.move(pdf_src_file, pdf_dst_file)
        t2 = datetime.now()
        announce(__file__, 'PDF           :  ' + pdf_dst_file)
        announce(__file__, 'Latex gen time:  ' + str(t2 - t1))

    # ---------------------------------------------------------------------
    # Сборка HTML
    # ---------------------------------------------------------------------
    if cfg_target_html not in args.targets:
        announce(__file__, "Skipping HTML build.")
    else:
        t1 = datetime.now()
        announce(__file__, "Building HTML output...", box=True)

        # Если PDF присутствует в каталоге сборки, скопируйте его в
        # промежуточный каталог для использования сборкой HTML.
        # (Sphinx копирует его на выходеHTML, поэтому он завершается.
        # на веб-сервере, где его можно скачать).
        if os.path.isfile(pdf_dst_file):
            # При необходимости создайте каталог _static/download/.
            if not os.path.exists(pdf_intermediate_dst_dir):
                os.makedirs(pdf_intermediate_dst_dir)

            shutil.copyfile(pdf_dst_file, pdf_intermediate_dst_file)

        # Если PDF присутствует, убедитесь, что ссылка на него находится вверху.
        # index.rst, поэтому он будет в сборке HTML.
        # Поддерживает платформы Windows и Linux с помощью `os.linesep`.
        if os.path.isfile(pdf_intermediate_dst_file):
            with open(top_index_file, 'rb') as f:
                index_data = f.read().decode('utf-8')

            if pdf_link_ref_str not in index_data:
                index_data = cfg_pdf_link_ref_block_str + index_data

                with open(top_index_file, 'wb') as f:
                    f.write(index_data.encode('utf-8'))

        # Примечание. Хотя это можно сделать (например, если нужно установить точку остановки).
        # в коде Sphinx для целей разработки) рекомендуется использоватьNOT.
        # запустить Sphinx из скрипта как
        # из sphinx.cmd.build импортируйте основной как sphinx_build
        # sphinx_args = [...]
        # sphinx_build (sphinx_args)
        # потому что его запуск занимает примерно в 10 раз больше времени, чем исполняемый файл`sphinx_build`,
        # буквально >3 часа.

        ver = lvgl_version(version_src_file)
        src = intermediate_dir
        dst = output_dir
        cpu = os.cpu_count()

        debugging_breathe = 0
        if debugging_breathe:
            from sphinx.cmd.build import main as sphinx_build
            # Не разрешать параллельную обработку во время отладки (аргумент -j удален).
            sphinx_args = ['-M', 'html', f'{src}', f'{dst}']

            if len(env_opt) > 0:
                sphinx_args.append(f'{env_opt}')

            sphinx_build(sphinx_args)
        else:
            # Опция -D правильно заменяет (переопределяет) атрибут конфигурации.
            # значения в модуле `conf.py`.  поскольку`conf.py`теперь правильно
            # вычислите свое собственное значение `version`, нам не нужно его здесь переопределять
            # с опцией -D.  Если его понадобится использовать в будущем,
            # значение после '=" MUST NOT заключено в кавычки
            # или это не сработает.  Правильное использование: f'-D version={ver}' .
            cmd_line = f'sphinx-build -M html "{src}" "{dst}" -j {cpu} {env_opt} --fail-on-warning --keep-going'
            cmd(cmd_line)

        t2 = datetime.now()
        announce(__file__, 'HTML gen time :  ' + str(t2 - t1))

    # ---------------------------------------------------------------------
    # Укажите результаты.
    # ---------------------------------------------------------------------
    t_end = datetime.now()
    announce(__file__, 'Total run time:  ' + str(t_end - t0))
    announce(__file__, 'Done.')


if __name__ == '__main__':
    """Make module importable as well as run-able."""
    run()
