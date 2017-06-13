option(host_build)

SOURCES += qsc.cpp

DEFINES += QT_SHADERSTACK_BUILTIN
include(../../src/qspirv/qspirvhost.pri)

load(qt_tool)
