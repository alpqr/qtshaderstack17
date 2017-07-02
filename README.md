
1. Check out https://github.com/KhronosGroup/SPIRV-Cross and https://github.com/KhronosGroup/glslang under src/3rdparty

2. qmake && (n)make

3. QT += shadertools

4. Use QSpirvCompiler to compile (Vulkan-flavored) GLSL to SPIR-V.

5. Use QSpirvShader parse a SPIR-V binary to get reflection data and to translate to GLSL suitable for various OpenGL (ES) versions.
