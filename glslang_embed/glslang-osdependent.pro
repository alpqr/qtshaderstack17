TEMPLATE = lib
CONFIG += staticlib
QT =
TARGET = $$qtLibraryTarget(QtSpirvGlslangOSDependent)
DESTDIR = $$PWD

GLSLANG_PATH=$$PWD/glslang

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
