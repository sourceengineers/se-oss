# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

import datetime
import subprocess
import os

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

def is_git_repo_dirty(path) -> bool:
    try:
        return subprocess.check_output(['git', 'status', '--porcelain'], cwd=path).decode('ascii', errors='ignore').strip() != ""
    except subprocess.CalledProcessError:
        return True
    
def get_git_revision_hash(path) -> str:
    if is_git_repo_dirty(path):
        return "unknown"
    try:
        return subprocess.check_output(['git', 'rev-parse', '--short', 'HEAD']).decode('ascii', errors='ignore').strip()
    except subprocess.CalledProcessError:
        return "unknown"
    
git_revision = get_git_revision_hash(path=os.path.join(os.path.dirname(os.path.realpath(__file__)), ".."))
build_time = datetime.datetime.now().strftime('%B %d, %Y, at %H:%M')

project = "se-oss Documentation"
copyright = f"<a href=\"https://creativecommons.org/licenses/by/4.0/\">CC BY 4.0</a> Source Engineers GmbH, Switzerland. Last update on {build_time} from git commit <code>{git_revision}</code>."
author = "Source Engineers GmbH"

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    'myst_parser',
]

templates_path = ["_templates"]
exclude_patterns = ["README.rst", "_build", ".venv", "Thumbs.db", ".DS_Store"]
numfig = True

source_suffix = {
    '.rst': 'restructuredtext',
    '.md': 'markdown',
}

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = "shibuya"
html_static_path = ["_static"]
html_theme_options = {
    "light_logo": "_static/sourceengineers_logo.svg",
    "dark_logo": "_static/sourceengineers_logo_inverted.svg",
    "accent_color": "indigo",
    "github_url": "https://github.com/sourceengineers/se-oss",
}
html_css_files = [
    'css/custom.css',
]

# -- LaTeX configuration --------------------------------------------------
# https://www.sphinx-doc.org/en/master/latex.html

# -- PlantUML configuration -----------------------------------------------
# https://sphinx-needs.readthedocs.io/en/latest/installation.html#plantuml-support

import os

on_rtd = os.environ.get('READTHEDOCS') == 'True'
if on_rtd:
    plantuml = 'java -Djava.awt.headless=true -jar /usr/share/plantuml/plantuml.jar'
else:
    plantuml = 'java -jar %s' % os.path.join(os.path.dirname(__file__), "utils", "plantuml.jar")

    plantuml_output_format = 'png'
