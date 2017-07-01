TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += glslang-spirv.pro
SUBDIRS += glslang-osdependent.pro
SUBDIRS += glslang-oglcompiler.pro
SUBDIRS += glslang-hlsl.pro
SUBDIRS += glslang-glslang.pro

# temporarily, for testing
SUBDIRS += glslang-validator.pro
