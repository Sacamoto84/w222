# #########################################################################
# Файл конфигурации для конструктора документации Sphinx.
# Создано sphinx-quickstart в среду, 12 июня, 16:38:40 2019 г.
#
# Этот файл создан в форматеexecfile()с соответствующим каталогом.
# содержащий каталог.
#
# Все значения конфигурации имеют значение по умолчанию; включены некоторые значения
# ниже закомментированы, и некоторые из них служат для демонстрации
# по умолчанию.  Обратите внимание, что присутствуют не все возможные значения конфигурации.
#
# Полный список встроенных значений конфигурации см. в документации:
# https://www.sphinx-doc.org/en/master/usage/configuration.html
# Каждый из основных разделов ниже отражает основной раздел этой веб-страницы.
# и они упорядочены в одной последовательности, чтобы было понятно какой конфиг
# предметы сочетаются с чем.
#
# Если расширения (или модули для документирования с помощью autodoc) находятся в другом каталоге,
# добавьте эти каталоги в sys.path здесь. Если каталог находится относительно
# корень документации, воспользуйтесь os.path. abspath(), чтобы сделать его абсолютным, как показано здесь.
# #########################################################################
import os
import sys
from sphinx.builders.html import StandaloneHTMLBuilder

base_path = os.path.abspath(os.path.dirname(__file__))
# Добавьте путь для импортаlink_roles.pyи lv_example.py.
sys.path.insert(0, os.path.abspath('./_ext'))
# Добавьте путь для импорта lvgl_version.py.  помни, что это
# работает в среде `sphinx-build`, а не `build.py`.
sys.path.insert(0, base_path)
from lvgl_version import lvgl_version #NoQA

cfg_lv_version_file = 'lv_version.h'



# *************************************************************************
# Информация о проекте
# *************************************************************************

project = 'LVGL'
copyright = '2021-%Y, LVGL Kft'
author = 'LVGL Community'

if __name__ == '__main__':
    version_src_path = os.path.join(base_path, '../../lv_version.h')
else:
    version_src_path = os.path.join(base_path, cfg_lv_version_file)

if os.path.isfile(version_src_path):
    # У нас есть lv_version.h.  Используйте это.
    version = lvgl_version(version_src_path)
else:
    # Нам предстоит угадать.
    version = '9.3'

release = version
# Заметки о`version`здесь:
# ---------------------------
# Информация о версии проекта, который вы документируете, заменяет
# |версия| и |выпуск|, а также использовать в различных точках мира.
# построенные документы.
#
# Краткая версия X.Y извлекается из`lv_version.h`с использованием кроссплатформенного совместимого файла.
# Функция Python вlvgl_version.pyи применяется в командной строке`sphinx-build`.
#
# 22 апреля 2025 г., пока`-D version=...`в командной строке работает (пока кавычки
# не размещаются вокруг версии), если она добавлена после того, как `sphinx-build`
# выполнил этот скрипт недостаточно скоро, потому что нам нужна версия в каком-то
# ниже.  Итак, чтобы это сделать, нам нужно получить его здесь от `lv_version.h`.



# *************************************************************************
# Общая конфигурация
# *************************************************************************

# Если для вашей документации требуется минимальная версия Sphinx, укажите это здесь.
#
# needs_sphinx = '1.0'

# Добавьте сюда любые расширения расширений Sphinx в видеостроке. они могут быть
# расширения, поставляемые со Sphinx (с именем «sphinx.ext.*») или пользовательские расширения.
#
# По состоянию на 6 января 2025 г. `link_roles`закомментирован, поскольку он
# заменено ссылкой на перевод, установленной вручную в ./docs/index.rst.
extensions = [
    'sphinx.ext.autodoc',
    'sphinx.ext.extlinks',
    'sphinx.ext.intersphinx',
    'sphinx.ext.todo',
    'sphinx.ext.viewcode',      # Eye icon at top of page to view page source code on GitHub.
    'sphinx_copybutton',        # Copy-to-clipboard button in code blocks & code examples.
    'breathe',
    'sphinx_sitemap',
    'lv_example',
    'sphinx_design',
    # 'link_roles',
    'sphinxcontrib.mermaid',
]

needs_extensions = {
    'sphinxcontrib.mermaid': '0.9.2'
}

# Если переменная среды «SPHINX_REREDIRECTS_STANDDOWN» существует и
# определили значение, не равное «0», не добавляйте «sphinx_reredirects»
# к расширениям.  Это дает возможность тестировать/редактировать/отлаживать документацию.
# создать возможность пропуска добавления редиректов в локальной среде
# при желании.
add_redirects = True
if 'SPHINX_REREDIRECTS_STANDDOWN' in os.environ:
    if os.environ.get('SPHINX_REREDIRECTS_STANDDOWN') != '0':
        print("sphinx_reredirects standing down as requested.")
        add_redirects = False

if add_redirects:
    extensions.append('sphinx_reredirects')

del add_redirects

# -------------------------------------------------------------------------
# Варианты выделения
# -------------------------------------------------------------------------
# Язык по умолчанию для выделения исходного кода. По умолчанию используется Python.
# Значение должно быть допустимым именем лексера Пигменты, см. раздел «Показ примера кода».
# для более подробной информации.
highlight_language = 'c'

# Имя используемого стиля Пигменты (подсветка синтаксиса).
pygments_style = 'github-light'
pygments_dark_style = 'github-dark'

# -------------------------------------------------------------------------
# Варианты интернационализации
# -------------------------------------------------------------------------
# Язык контента, автоматически создаваемый Sphinx. Обратитесь к документации
# для получения списка поддерживаемых языков.
#
# Это также используется, если вы осуществляете передачу контента через каталоги gettext.
# Обычно в таких случаях вы устанавливаете «язык» из командной строки.
language = 'en'

# -------------------------------------------------------------------------
# Варианты разметки
# -------------------------------------------------------------------------
default_role = 'literal'
# keep_warnings = False # True приводит к добавлению предупреждений Sphinx в документы.
primary_domain = 'c'      # Default:  'py'

# -------------------------------------------------------------------------
# Параметры исходных файлов
# -------------------------------------------------------------------------
# Список шаблонов в стиле glob относительно исходного каталога, который
# сопоставлять файлы и каталоги, которые следует игнорировать при поиске исходных файлов.
# Эти шаблоны также относятся кhtml_static_pathи html_extra_path.
exclude_patterns = ['build', 'doxygen', 'intermediate', 'doxygen_html', 'Thumbs.db', '.DS_Store',
                    'README.md', 'README_*', 'lv_examples', 'env', '_ext', 'examples']

# Главный документ toctree.  (Корень дереваTOC.)
master_doc = 'index'

# Суффикс(ы) имен исходных файлов.
# Вы можете указать несколько суффиксов в качестве словаря для каждого
# https://www.sphinx-doc.org/en/master/usage/configuration.html#confval-source_suffix
source_suffix = {'.rst': 'restructuredtext'}

# -------------------------------------------------------------------------
# Варианты смарт-кавычек
# -------------------------------------------------------------------------
# Включение действия смарт-кавычек для преобразования `--` в тире и `---` в em.
# тире.  Преобразование кавычек и многоточий выполняется NOT, потому что
# значение по умолчанию`smartquotes_action`'qDe' заменено на просто 'D' ниже, что
# выполняет преобразования тире по желанию.
#
# Список всех оригинальных украшенийsmartquotes_actionсм. в разделе:
# https://www.sphinx-doc.org/en/master/usage/configuration.html#confval-smartquotes_action
smartquotes = True
smartquotes_action = 'D'
smartquotes_excludes = {'builders': ['man', 'text', 'json', 'xml', 'latex']}

# -------------------------------------------------------------------------
# Варианты шаблонов
# -------------------------------------------------------------------------
# Список путей, содержащих дополнительные шаблоны (или шаблоны, которые перезаписывают
# встроенные/специфичные для темы шаблоны).  Относительные пути считаются относительными
# в каталог конфигурации.
templates_path = ['_templates']

# -------------------------------------------------------------------------
# Опции управления предупреждениями
# -------------------------------------------------------------------------
# suppress_warnings = ['прил. add_directive' и т. д. д.]



# *************************************************************************
# Параметры строителя
# *************************************************************************

# -------------------------------------------------------------------------
# Опции дляHTMLBuilder
# -------------------------------------------------------------------------
# Тема для выводаHTML.  См.  https://www.sphinx-doc.org/en/master/usage/theming.html.
html_theme = 'furo'

# Параметры темы зависят от темы и позволяют настроить внешний вид
# тема дальше.  Список опций, доступных для каждой темы, см.
# документация темы.
#
# Различные другие сборщики основаны на выводе HTML и также используют
# из этих вариантов.
html_theme_options = {
    "sidebar_hide_name": True,      # True when the logo carries project name
    "light_logo": "images/logo-light.svg",
    "dark_logo": "images/logo-dark.svg",
    "top_of_page_buttons": ["view"],
    # Приведенные ниже 3 направляют «top_of_page_buttons» на github для кнопок просмотра и редактирования.
    "source_repository": "https://github.com/lvgl/lvgl/",
    "source_branch": "master",
    "source_directory": "docs/src/",
    # "announcement": "<em>Полупостоянное объявление</em> от <code>conf.py</code>.",
}

html_sidebars = {
    "**": [
        "sidebar/brand.html",
        "sidebar/version-selector.html",
        "sidebar/search.html",
        "sidebar/scroll-start.html",
        "sidebar/navigation.html",
        "sidebar/ethical-ads.html",
        "sidebar/scroll-end.html",
        "sidebar/variant-selector.html"
    ]
}

# Для создания карты сайта
if "LVGL_URLPATH" not in os.environ:
    os.environ['LVGL_URLPATH'] = 'master'

_branch = os.getenv('LVGL_URLPATH')
html_baseurl = f"https://docs.lvgl.io/{_branch}/"

#lvgl_github_url = е" https://github.com/lvgl/lvgl/blob/{os.environ['LVGL_GITCOMMIT']}/docs"
#extlinks = {'github_link_base': (github_url+ '%s',github_url)}

if "LVGL_GITCOMMIT" not in os.environ:
    os.environ['LVGL_GITCOMMIT'] = 'master'

_git_commit_ref = os.getenv('LVGL_GITCOMMIT')

# Эти ключи используются «голыми» в качестве переменных шаблона в:
# - Шаблон темыsphinx_rtd_theme: breadcrumbs.html.
# - шаблон темы фуро: edit-this-page.html
# - шаблон темы фуро: view-this-page.html
html_context = {
    'display_github': True,
    'github_user': 'lvgl',
    'github_repo': 'lvgl',
    'github_version': _git_commit_ref,
    'conf_py_path': '/docs/src/'
}

html_logo = ''
html_favicon = '_static/images/favicon.png'

html_css_files = [
    'css/fontawesome.min.css',
    'css/solid.min.css',
    'css/brands.min.css',
    'css/custom.css'
]

html_js_files = [
    'js/custom.js',
    'js/include_html.js'
]

html_static_path = ['_static']
html_last_updated_fmt = ''          # Empty string uses default format:  '%b %d, %Y'
html_last_updated_use_utc = False   # False = use generating system's local date, not GMT.
html_permalinks = True              # Default = True, add link anchor for each heading and description environment.

# 10.03.2025 16:21 -- прокомментировал тему Фуро.
# html_sidebars = {
#     '**': [
# 'relations.html', # требуется 'show_related': для отображения истинной темы
# 'searchbox.html',
#     ]
# }

# html_domain_indices
# html_use_index = Истина # По умолчанию = Истина
# html_split_index
# html_copy_source = Истина # По умолчанию = Истина; Если это правда, источники reStructuredText включаются в сборкуHTMLкак _sources/docname.
# html_show_sourcelink = Истина # По умолчанию = Истина
# html_sourcelink_suffix = '.txt' # По умолчанию = '.txt'
# html_use_opensearch = ''
# html_file_suffix = '.html' # По умолчанию = '.html'
# html_link_suffix = html_file_suffix
html_show_copyright = True         # Default = True; shows copyright notice in footer.
# html_show_search_summary = Истина # По умолчанию = Истина
html_show_sphinx = False          # Default = True; adds "Created using Sphinx" to footer.
# html_output_encoding = 'utf-8' # По умолчанию = 'utf-8'
# html_compact_lists = Истина # По умолчанию = Истина
# html_secnumber_suffix = '. '# По умолчанию = '. '; установите значение '', чтобы поставить ни малейшего знака в обращении разделов.
# html_search_language = язык # По умолчанию = язык; язык, который будет использоваться для создания индекса полнотекстового поискаHTML.
# html_search_options

# Свяжите изображения, размер которых был изменен, с помощью параметра масштаба (масштаб, ширина или высота).
# в исходное изображение в полном разрешении.  Это не приведет к перезаписи какой-либо указанной ссылки.
# по целевой опции директивы изображения, если она присутствует.
#
# Совет
#
# Чтобы отключить эту функцию для каждого изображения, страницы класса no-scaled-link в директиве изображения:
#
# .. изображение:: сфинкс.png
# :масштаб: 50%
# :class: ссылка без масштабирования
# html_scaled_image_link

# html_math_renderer = 'mathjax' # По умолчанию = 'mathjax'

# Изменить последовательность поиска изображений: поиск файлов изображений осуществляется в том порядке, в котором они здесь появляются.
# См.  https://www.sphinx-doc.org/en/master/usage/builders/index.html#sphinx.builders.html.StandaloneHTMLBuilder.supported_image_types.
StandaloneHTMLBuilder.supported_image_types = [
    'image/svg+xml',
    'image/gif',  #prefer gif over png
    'image/png',
    'image/jpeg'
]

# -------------------------------------------------------------------------
# Параметры HTMLHelp Builder
# -------------------------------------------------------------------------
# Базовое имя выходного файла для конструктора справки HTML.
htmlhelp_basename = 'LVGLdoc'

# -------------------------------------------------------------------------
# Опции для LaTeX Builder
# -------------------------------------------------------------------------
latex_engine = 'xelatex'
latex_use_xindy = False
latex_elements = {
    # Размер бумаги («Letterpaper» или «A4paper»).
    #
    # 'размер бумаги': 'почтовая бумага',

    # Размер шрифта («10pt», «11pt» или «12pt»).
    #
    # 'размер в точках': '10pt',

    # Дополнительные материалы для преамбулы LaTeX.
    #
    # 'преамбула': '',

    # Выравнивание латексной фигуры (поплавок)
    #
    # 'figure_align': 'htbp',

    'inputenc': '',
    'utf8extra': '',
    'classoptions': ',openany,oneside',
    'babel': '\\usepackage{babel}',
    'passoptionstopackages': r'''
\PassOptionsToPackage{bookmarksdepth=5}{hyperref}% depth of pdf bookmarks
''',
    'preamble': r'''
\usepackage{fontspec}
\setmonofont{DejaVu Sans Mono}
\usepackage{silence}
\WarningsOff*
''',
}

# Группировка деревянных документов в файлы LaTeX. Список кортежей
# (исходный стартовый файл, целевое имя, заголовок,
# автор, класс документа [инструкция, руководство или собственный класс]).
latex_documents = [
    (master_doc, 'LVGL.tex', 'LVGL v' + version,
     author, 'manual'),
]

# -------------------------------------------------------------------------
# Параметры ручного конструктора страниц
# -------------------------------------------------------------------------
# Одна запись на страницу руководства. Список кортежей
# (исходный файл, название, описание, авторы, раздел руководства).
man_pages = [
    (master_doc, 'lvgl', 'LVGL v' + version,
     [author], 3)
]

# -------------------------------------------------------------------------
# Опции для Texinfo Builder
# -------------------------------------------------------------------------
# Группировка деревянных документов в файлы Texinfo. Список кортежей
# (исходный стартовый файл, целевое имя, название, автор,
# пункт меню dir, описание, категория)
texinfo_documents = [
    (master_doc, 'LVGL', 'LVGL v' + version,
     author, 'Contributors of LVGL', 'One line description of project.',
     'Miscellaneous'),
]



# *************************************************************************
# Параметры домена
# *************************************************************************

# -------------------------------------------------------------------------
# Опции для домена C
# -------------------------------------------------------------------------

# -------------------------------------------------------------------------
# Опции для домена CPP
# -------------------------------------------------------------------------



# *************************************************************************
# Конфигурация расширений Sphinx
# *************************************************************************

# -------------------------------------------------------------------------
# Расширение опций sphinx_rtd_dark_mode
# -------------------------------------------------------------------------
default_dark_mode = False

# -------------------------------------------------------------------------
# Расширение опций lv_example
# -------------------------------------------------------------------------
repo_commit_hash = _git_commit_ref

# -------------------------------------------------------------------------
# Расширение параметров sphinx.ext.todo
# -------------------------------------------------------------------------
# Если это правда, директивы`todo`и`todoList`выдают данные на выходные, в противном случае они ничего не производят.
# См.  https://www.sphinx-doc.org/en/master/usage/extensions/todo.html.
todo_include_todos = True

# -------------------------------------------------------------------------
# Расширение опций sphinx_sitemap
# -------------------------------------------------------------------------
# См.  https://sphinx-sitemap.readthedocs.io/en/latest/index.html.
sitemap_url_scheme = "{link}"
# Улучшите вывод, используя 4 пробела для отступа.
sitemap_indent = 4

# -------------------------------------------------------------------------
# Варианты расширения дыхания
# -------------------------------------------------------------------------
# См.  https://breathe.readthedocs.io/en/latest/.
breathe_projects = {
    "lvgl": "xml/",
}

breathe_default_project = "lvgl"
# breathe_debug_trace_directives = Истина

# -------------------------------------------------------------------------
# Опции для sphinx_reredirects
# -------------------------------------------------------------------------
# Ниже генерируется . HTML перенаправляет страницы, которые были перемещены.
# Браузеры перенаправляются через `<meta http-equiv="refresh" content="0; url=new_url">`.
redirects = {
    "CONTRIBUTING":                               "contributing/index.html"                                 ,
    "get-started/index":                          "../getting_started.html"                                 ,
    "get-started/os/nuttx":                       "../../integration/rtos/nuttx.html"                       ,
    "get-started/platforms/arduino":              "../../integration/framework/arduino.html"                ,
    "get-started/platforms/espressif":            "../../integration/chip/espressif.html"                   ,
    "get-started/platforms/pc-simulator":         "../../integration/ide/pc-simulator.html"                 ,
    "get-started/quick-overview":                 "../getting_started.html"                                 ,
    "integration/framework/arduino":              "../../integration/frameworks/arduino.html"               ,
    "integration/framework/index":                "../../integration/frameworks/index.html"                 ,
    "integration/framework/platformio":           "../../integration/frameworks/platformio.html"            ,
    "integration/framework/tasmota-berry":        "../../integration/frameworks/tasmota-berry.html"         ,
    "integration/ide/pc-simulator":               "../../integration/ide/pc-simulator.html"                 ,
    "integration/os/rt-thread":                   "../../integration/os/rt-thread.html"                     ,
    "intro/getting_started/index":                "../../getting_started/index.html"                        ,
    "layouts/flex":                               "../common-widget-features/layouts/flex.html"             ,
    "layouts/grid":                               "../common-widget-features/layouts/grid.html"             ,
    "layouts/index":                              "../common-widget-features/layouts/index.html"            ,
    "others/file_explorer":                       "../auxiliary-modules/file_explorer.html"                 ,
    "others/font_manager":                        "../auxiliary-modules/font_manager.html"                  ,
    "others/fragment":                            "../auxiliary-modules/fragment.html"                      ,
    "others/gridnav":                             "../auxiliary-modules/gridnav.html"                       ,
    "others/ime_pinyin":                          "../auxiliary-modules/ime_pinyin.html"                    ,
    "others/imgfont":                             "../auxiliary-modules/imgfont.html"                       ,
    "others/index":                               "../auxiliary-modules/index.html"                         ,
    "others/monkey":                              "../auxiliary-modules/monkey.html"                        ,
    "others/obj_id":                              "../auxiliary-modules/obj_id.html"                        ,
    "others/obj_property":                        "../auxiliary-modules/obj_property.html"                  ,
    "others/observer":                            "../main-modules/observer.html"                           ,
    "others/snapshot":                            "../auxiliary-modules/snapshot.html"                      ,
    "overview/animations":                        "../main-modules/animation.html"                          ,
    "overview/color":                             "../main-modules/color.html"                              ,
    "overview/coord":                             "../common-widget-features/coord.html"                    ,
    "overview/debugging/gdb_plugin":              "../../debugging/gdb_plugin.html"                         ,
    "overview/debugging/index":                   "../../debugging/index.html"                              ,
    "overview/debugging/log":                     "../../debugging/log.html"                                ,
    "overview/debugging/profiler":                "../../debugging/profiler.html"                           ,
    "overview/debugging/vg_lite_tvg":             "../../debugging/vg_lite_tvg.html"                        ,
    "overview/display":                           "../main-modules/display/index.html"                      ,
    "overview/event":                             "../common-widget-features/event.html"                    ,
    "overview/font":                              "../main-modules/font.html"                               ,
    "overview/fs":                                "../main-modules/fs.html"                                 ,
    "overview/image":                             "../main-modules/image.html"                              ,
    "overview/indev":                             "../main-modules/indev.html"                              ,
    "overview/index":                             "../main-modules/index.html"                              ,
    "overview/layer":                             "../common-widget-features/layer.html"                    ,
    "overview/new_widget":                        "../widgets/new_widget.html"                              ,
    "overview/obj":                               "../common-widget-features/obj.html"                      ,
    "overview/renderers/arm2d":                   "../../integration/renderers/arm2d.html"                  ,
    "overview/renderers/index":                   "../../integration/renderers/index.html"                  ,
    "overview/renderers/nema_gfx":                "../../integration/renderers/nema_gfx.html"               ,
    "overview/renderers/pxp":                     "../../integration/renderers/nxp_pxp.html"                ,
    "overview/renderers/sdl":                     "../../integration/renderers/sdl.html"                    ,
    "overview/renderers/stm32_dma2d":             "../../integration/renderers/stm32_dma2d.html"            ,
    "overview/renderers/sw":                      "../../integration/renderers/sw.html"                     ,
    "overview/renderers/vg_lite":                 "../../integration/renderers/vg_lite.html"                ,
    "overview/renderers/vglite":                  "../../integration/renderers/nxp_vglite_gpu.html"         ,
    "overview/scroll":                            "../common-widget-features/scroll.html"                   ,
    "overview/style":                             "../common-widget-features/styles/styles.html"            ,
    "overview/style-props":                       "../common-widget-features/styles/style-properties.html"  ,
    "overview/timer":                             "../main-modules/timer.html"                              ,
    "porting/display":                            "../main-modules/display/index.html"                      ,
    "porting/draw":                               "../main-modules/draw.html"                               ,
    "porting/indev":                              "../main-modules/indev.html"                              ,
    "porting/index":                              "../integration/adding-lvgl-to-your-project/index.html"   ,
    "porting/os":                                 "../integration/adding-lvgl-to-your-project/threading.html",
    "porting/project":                            "../integration/adding-lvgl-to-your-project/connecting_lvgl.html",
    "porting/sleep":                              "../integration/adding-lvgl-to-your-project/threading.html",
    "porting/tick":                               "../integration/adding-lvgl-to-your-project/connecting_lvgl.html",
    "porting/timer_handler":                      "../integration/adding-lvgl-to-your-project/timer_handler.html",
    "widgets/obj":                                "../widgets/base_widget.html"                             ,
    # После 27 ноября 2025 г.
    "intro/getting_started/index":                   "../../getting_started/index.html"                     ,
    "details/auxiliary-modules/observer/index":      "../../../main-modules/observer/index.html"            ,
    "details/common-widget-features/layouts/flex":   "../../../common-widget-features/layouts/flex.html"    ,
    "details/common-widget-features/layouts/grid":   "../../../common-widget-features/layouts/grid.html"    ,
    "details/common-widget-features/layouts/index":  "../../../common-widget-features/layouts/index.html"   ,
    "details/common-widget-features/styles/index":   "../../../common-widget-features/styles/index.html"    ,
    "details/integration/framework/arduino":         "../../../integration/frameworks/arduino.html"         ,
    "details/integration/os/nuttx":                  "../../../integration/rtos/nuttx.html"                 ,
    "details/integration/os/rt-thread":              "../../../integration/rtos/rt-thread.html"             ,
    "details/integration/os/zephyr":                 "../../../integration/rtos/zephyr.html"                ,
    "details/libs/gltf":                             "../../libs/gltf.html"                                 ,
    "details/main-modules/display/overview":         "../../../main-modules/display/overview.html"          ,
    "details/main-modules/indev":                    "../../main-modules/indev/index.html"                  ,
    "details/widgets/index":                         "../../widgets/index.html"                             ,
    "details/xml/editor/index":                      "../../../xml/editor/index.html"                       ,
    "details/xml/index":                             "../../xml/index.html"                                 ,
    "details/xml/integration/renesas-dev-tools":     "../../../xml/integration/renesas-dev-tools.html"      ,
    "details/xml/tools/cli":                         "../../../xml/tools/cli.html"                          ,
    "details/xml/tools/figma":                       "../../../xml/tools/figma.html"                        ,
    "details/xml/tools/online_share":                "../../../xml/tools/online_share.html"                 ,
    "details/xml/xml/index":                         "../../../xml/xml/index.html"                          ,
    "details/xml/features/index":                    "../../../xml/features/index.html"                     ,
}


# Пример конфигурации для intersphinx: обратитесь к библиотеке библиотеки Python.

def setup(app):
    pass
    # приложение.  add_config_value('recommonmark_config', {
    # 'enable_eval_rst': Верно,
    # 'enable_auto_toc_tree': 'Верно',
    # }, Правда)
    # приложение. add_transform(автоструктурирование)
    # приложение. add_css_file('css/custom.css')
    # приложение. add_css_file('css/fontawesome.min.css')


