from setuptools import setup, Extension
from Cython.Build import cythonize

setup(
    name='Vizzu wrapper',
    ext_modules=cythonize(
        Extension(
            "vizzu",
            sources=[
                "vizzu.pyx",
                "chart/rendering/painter/adaptivepainter.cpp",
                "apps/weblib/jscriptcanvas.cpp",
                # "apps/qutils/canvas.cpp",
            ],
            language="c++",
            # include_dirs = [
                # '/usr/include/qt',
                # '/usr/include/qt/QtCore',
                # '/usr/include/qt/QtGui',
            # ],
        )
    ),
    zip_safe=False,
)

