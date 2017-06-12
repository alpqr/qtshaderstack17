HEADERS += \
    $$PWD/qtshaderstackglobal.h \
    $$PWD/qspirv.h

SOURCES += \
    $$PWD/qspirv.cpp

INCLUDEPATH += $$PWD/../3rdparty/SPIRV-Cross
SOURCES += \
    $$PWD/../3rdparty/SPIRV-Cross/spirv_cross.cpp \
    $$PWD/../3rdparty/SPIRV-Cross/spirv_cfg.cpp \
    $$PWD/../3rdparty/SPIRV-Cross/spirv_glsl.cpp \
    $$PWD/../3rdparty/SPIRV-Cross/spirv_cpp.cpp \
    $$PWD/../3rdparty/SPIRV-Cross/spirv_msl.cpp \
    $$PWD/../3rdparty/SPIRV-Cross/spirv_hlsl.cpp

CONFIG += exceptions
