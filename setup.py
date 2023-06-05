# Available at setup time due to pyproject.toml
from pybind11.setup_helpers import Pybind11Extension
from setuptools import setup

__version__ = "0.0.1"

ext_modules = [
    Pybind11Extension("color_mappyer",
        ["src/main.cpp"],
        # Example: passing in the version to the compiled code
        define_macros = [('VERSION_INFO', __version__)],
        cxx_std=14,
        ),
]

setup(
    name="color_mappyer",
    version=__version__,
    author="Utkarsh Ayachit",
    url="https://github.com/utkarshayachit/colormappyer.git",
    description="Python package to map NumPy scalar arrays to colors",
    long_description="",
    ext_modules=ext_modules,
    extras_require={"test": "pytest"},
    zip_safe=False,
    python_requires=">=3.7",
)
