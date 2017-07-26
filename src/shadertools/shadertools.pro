TARGET = QtShaderTools

load(qt_module)

DEFINES += QT_BUILD_SHADERTOOLS_LIB

HEADERS += \
    $$PWD/qtshadertoolsglobal.h \
    $$PWD/qspirvshader.h \
    $$PWD/qspirvcompiler.h \
    $$PWD/qshaderdescription.h \
    $$PWD/qshaderdescription_p.h \
    $$PWD/qshaderbatchablerewriter_p.h

SOURCES += \
    $$PWD/qspirvshader.cpp \
    $$PWD/qspirvcompiler.cpp \
    $$PWD/qshaderdescription.cpp \
    $$PWD/qshaderbatchablerewriter.cpp

INCLUDEPATH += $$PWD $$PWD/../3rdparty/SPIRV-Cross $$PWD/../3rdparty/glslang

STATICLIBS = qtspirv-cross qtglslang-glslang qtglslang-spirv qtglslang-osdependent qtglslang-oglcompiler # qtglslang-hlsl
for(libname, STATICLIBS) {
    staticlib = $$[QT_HOST_LIBS]/$${QMAKE_PREFIX_STATICLIB}$$qtLibraryTarget($$libname).$${QMAKE_EXTENSION_STATICLIB}
    LIBS_PRIVATE += $$staticlib
    PRE_TARGETDEPS += $$staticlib
}
