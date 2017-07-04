
Experiments for more modern graphics shader management in future Qt
===================================================================

1. Check out https://github.com/KhronosGroup/SPIRV-Cross and https://github.com/KhronosGroup/glslang under src/3rdparty

2. qmake && (n)make

3. QT += shadertools

4. Use QSpirvCompiler to compile (Vulkan-flavored) GLSL to SPIR-V.

5. Use QSpirvShader parse a SPIR-V binary to get reflection data and to translate to GLSL suitable for various OpenGL (ES) versions.

6. The reflection data (QShaderDescription) can also be serialized to binary and human-readable JSON, and deserialized from binary JSON.
