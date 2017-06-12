TARGET     = QtShaderStack
QT         = core gui

load(qt_module)

DEFINES += QT_BUILD_SHADERSTACK_LIB

include(qspirvprocess.pri)
