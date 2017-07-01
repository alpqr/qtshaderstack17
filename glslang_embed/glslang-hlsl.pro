TEMPLATE = lib
CONFIG += staticlib
QT =
TARGET = $$qtLibraryTarget(QtSpirvGlslangHLSL)
DESTDIR = $$PWD

GLSLANG_PATH=$$PWD/glslang

win32: DEFINES += -DGLSLANG_OSINCLUDE_WIN32
unix: DEFINES += -DGLSLANG_OSINCLUDE_UNIX

SOURCES += \
    $$GLSLANG_PATH/hlsl/hlslAttributes.cpp \
    $$GLSLANG_PATH/hlsl/hlslParseHelper.cpp \
    $$GLSLANG_PATH/hlsl/hlslScanContext.cpp \
    $$GLSLANG_PATH/hlsl/hlslOpMap.cpp \
    $$GLSLANG_PATH/hlsl/hlslTokenStream.cpp \
    $$GLSLANG_PATH/hlsl/hlslGrammar.cpp \
    $$GLSLANG_PATH/hlsl/hlslParseables.cpp
