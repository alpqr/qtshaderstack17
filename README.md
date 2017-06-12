Experiments for more modern graphics shader management in future Qt
===================================================================

Built on top of https://github.com/KhronosGroup/SPIRV-Cross

Note: expects SPIRV-Cross to be checked out under src/3rdparty

See shaderstack.txt for vision and motivation.

All APIs are WIP or missing. For now, use qsc to test. This needs
glslc to be in the PATH. Alternatively, set QT_GLSLC, for example:
export QT_GLSLC=~/android-ndk-r13b/shader-tools/linux-x86_64/glslc

For instance, going to test/playground and running qsc color_phong.frag results in:

 * color_phong.frag.spv -> SPIR-V binary
 * color_phong.frag.refl.json -> reflection data (text, for debugging)
 * color_phong.frag.refl -> binary JSON version (binary, to be deployed/loaded at runtime)
 * color_phong.frag.glsl100 -> SPIRV-Cross' translation to GLSL (ES) 100
 * color_phong.frag.glsl150 -> SPIRV-Cross' translation to GLSL 150

It's not hard to see where this is going. Looks promising for now.

See qsc --help for configuration options.
