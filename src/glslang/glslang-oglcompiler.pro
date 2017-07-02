TARGET = qtglslang-oglcompiler

CONFIG += \
    static \
    hide_symbols \
    exceptions_off rtti_off warn_off

load(qt_helper_lib)

GLSLANG_PATH=$$PWD/../3rdparty/glslang

win32: DEFINES += -DGLSLANG_OSINCLUDE_WIN32
unix: DEFINES += -DGLSLANG_OSINCLUDE_UNIX

SOURCES += \
    $$GLSLANG_PATH/OGLCompilersDLL/InitializeDLL.cpp
