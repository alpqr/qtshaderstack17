TARGET     = QtShaderStack
QT         = core gui

load(qt_module)

DEFINES += QT_BUILD_SHADERSTACK_LIB

HEADERS += \
    qtshaderstackglobal.h \
    qspirv.h

SOURCES += \
    qspirv.cpp
