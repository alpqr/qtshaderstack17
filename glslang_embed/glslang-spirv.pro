TEMPLATE = lib
CONFIG += staticlib
QT =
TARGET = $$qtLibraryTarget(QtSpirvGlslangSPIRV)
DESTDIR = $$PWD

GLSLANG_PATH=$$PWD/glslang

win32: DEFINES += -DGLSLANG_OSINCLUDE_WIN32
unix: DEFINES += -DGLSLANG_OSINCLUDE_UNIX

SOURCES += \
    $$GLSLANG_PATH/SPIRV/GlslangToSpv.cpp \
    $$GLSLANG_PATH/SPIRV/InReadableOrder.cpp \
    $$GLSLANG_PATH/SPIRV/Logger.cpp \
    $$GLSLANG_PATH/SPIRV/SpvBuilder.cpp \
    $$GLSLANG_PATH/SPIRV/doc.cpp \
    $$GLSLANG_PATH/SPIRV/disassemble.cpp \
    $$GLSLANG_PATH/SPIRV/SPVRemapper.cpp \
