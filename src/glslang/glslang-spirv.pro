TARGET = qtglslang-spirv

CONFIG += \
    static \
    hide_symbols \
    exceptions_off rtti_off warn_off

load(qt_helper_lib)

GLSLANG_PATH=$$PWD/../3rdparty/glslang

win32: DEFINES += GLSLANG_OSINCLUDE_WIN32
unix: DEFINES += GLSLANG_OSINCLUDE_UNIX

SOURCES += \
    $$GLSLANG_PATH/SPIRV/GlslangToSpv.cpp \
    $$GLSLANG_PATH/SPIRV/InReadableOrder.cpp \
    $$GLSLANG_PATH/SPIRV/Logger.cpp \
    $$GLSLANG_PATH/SPIRV/SpvBuilder.cpp \
    $$GLSLANG_PATH/SPIRV/doc.cpp \
    $$GLSLANG_PATH/SPIRV/disassemble.cpp \
    $$GLSLANG_PATH/SPIRV/SPVRemapper.cpp \
