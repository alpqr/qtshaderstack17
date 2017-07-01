TEMPLATE = lib
CONFIG += staticlib
QT =
TARGET = $$qtLibraryTarget(QtSpirvGlslangOGLCompiler)
DESTDIR = $$PWD

GLSLANG_PATH=$$PWD/glslang

win32: DEFINES += -DGLSLANG_OSINCLUDE_WIN32
unix: DEFINES += -DGLSLANG_OSINCLUDE_UNIX

SOURCES += \
    $$GLSLANG_PATH/OGLCompilersDLL/InitializeDLL.cpp
