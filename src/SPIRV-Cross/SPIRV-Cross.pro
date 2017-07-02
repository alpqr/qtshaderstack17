TARGET = qtspirv-cross

CONFIG += \
    static \
    hide_symbols \
    exceptions_on rtti_off warn_off

load(qt_helper_lib)

SPIRVCROSS_PATH=$$PWD/../3rdparty/SPIRV-Cross

SOURCES += \
    $$SPIRVCROSS_PATH/spirv_cross.cpp \
    $$SPIRVCROSS_PATH/spirv_cfg.cpp \
    $$SPIRVCROSS_PATH/spirv_glsl.cpp \
    $$SPIRVCROSS_PATH/spirv_cpp.cpp \
    $$SPIRVCROSS_PATH/spirv_msl.cpp \
    $$SPIRVCROSS_PATH/spirv_hlsl.cpp
