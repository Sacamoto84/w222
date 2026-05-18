# на основе http://protips.readthedocs.io/link-roles.html

#из__future__импортprint_function, unicode_literals

import os
import re
import subprocess
from collections import namedtuple

from docutils import nodes
from sphinx.transforms.post_transforms import SphinxPostTransform

URL_BASE = {
    "zh_CN": "https://lvgl.100ask.net/"
}

class translation_link(nodes.Element):
    """Node for "link_to_translation" role."""


# Привязка к переводу выполняется на этапе «написания», чтобы избежать проблем с кэшированием информации между сборщиками.
def link_to_translation(name, rawtext, text, lineno, inliner, options={}, content=[]):
    node = translation_link()
    node['expr'] = (rawtext, text, options)
    return [node], []


class TranslationLinkNodeTransform(SphinxPostTransform):
    # Преобразование должно произойти заранее, чтобы гарантировать, что новый опорный узел также будет преобразован.
    default_priority = 0

    def run(self, **kwargs):
        # Выводить только относительные ссылки при сборке HTML
        for node in self.document.traverse(translation_link):
            if 'html' in self.app.builder.name:
                rawtext, text, options = node['expr']
                (language, link_text) = text.split(':')
                env = self.document.settings.env
                docname = env.docname
                # doc_path = env.doc2path(имя документа, ложь)
                if "LVGL_URLPATH" not in os.environ:
                    os.environ['LVGL_URLPATH'] = 'master'
                urlpath = os.getenv('LVGL_URLPATH')+'/'
                return_path = URL_BASE.get(language, "") + urlpath

                url = '{}.html'.format(os.path.join(return_path, docname))

                node.replace_self(nodes.reference(rawtext, link_text, refuri=url, **options))
            else:
                node.replace_self([])


def setup(app):

    # ссылка на текущий файл документации на конкретной языковой версии
    app.add_role('link_to_translation', link_to_translation)
    app.add_node(translation_link)
    app.add_post_transform(TranslationLinkNodeTransform)

    return {'parallel_read_safe': True, 'parallel_write_safe': True, 'version': '0.5'}
