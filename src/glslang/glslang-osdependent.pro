TARGET = qtglslang-osdependent

CONFIG += \
    static \
    hide_symbols \
    exceptions_off rtti_off warn_off

load(qt_helper_lib)

GLSLANG_PATH=$$PWD/../3rdparty/glslang

win32 {
    DEFINES += -DGLSLANG_OSINCLUDE_WIN32
    GLSLANG_OSDEP_PATH=$$GLSLANG_PATH/glslang/OSDependent/Windows
}

unix {
    DEFINES += -DGLSLANG_OSINCLUDE_UNIX
    GLSLANG_OSDEP_PATH=$$GLSLANG_PATH/glslang/OSDependent/Unix
}

SOURCES += \
    $$GLSLANG_OSDEP_PATH/ossource.cpp
