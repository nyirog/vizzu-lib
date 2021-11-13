from setuptools import setup
from Cython.Build import cythonize

setup(
    name='Vizzu wrapper',
    ext_modules=cythonize("vizzu.pyx"),
    zip_safe=False,
)

