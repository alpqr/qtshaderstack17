TEMPLATE = app
CONFIG += console
QT =
TARGET = glslangValidator
DESTDIR = $$PWD

GLSLANG_PATH = $$PWD/glslang

INCLUDEPATH += $$GLSLANG_PATH

SOURCES += \
    $$GLSLANG_PATH/StandAlone/StandAlone.cpp \
    $$GLSLANG_PATH/StandAlone/ResourceLimits.cpp

GLSLANG_STATIC_LIBS = QtSpirvGlslangSPIRV QtSpirvGlslangOSDependent QtSpirvGlslangOGLCompiler QtSpirvGlslangHLSL QtSpirvGlslangGlslang
for(libname, GLSLANG_STATIC_LIBS) {
    staticlib = $${QMAKE_PREFIX_STATICLIB}$$qtLibraryTarget($$libname).$${QMAKE_EXTENSION_STATICLIB}
    LIBS += $$staticlib
    PRE_TARGETDEPS += $$staticlib
}
