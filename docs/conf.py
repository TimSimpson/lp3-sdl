# -*- coding: utf-8 -*-
#
# Configuration file for the Sphinx documentation builder.
project = 'Lp3-Sdl'
copyright = '2020, Tim Simpson'
author = 'Tim Simpson'

# The short X.Y version
version = ''
# The full version, including alpha/beta/rc tags
release = ''

extensions = [
]

templates_path = ['_templates']
source_suffix = ['.rst', '.md', '.mrst']
master_doc = 'index'
language = None
exclude_patterns = []
pygments_style = 'monokai'
html_theme = 'traditional'
html_static_path = ['_static']
htmlhelp_basename = 'Lp3-Sdldoc'
latex_elements = {
}
latex_documents = [
    (master_doc, 'Lp3-Sdl.tex', 'Lp3-Sdl Documentation',
     'Tim Simpson', 'manual'),
]
man_pages = [
    (master_doc, 'lp3-sdl', 'Lp3-Sdl Documentation',
     [author], 1)
]
texinfo_documents = [
    (master_doc, 'Lp3-Sdl', 'Lp3-Sdl Documentation',
     author, 'Lp3-Sdl', 'One line description of project.',
     'Miscellaneous'),
]
