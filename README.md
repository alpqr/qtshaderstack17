
Experiments for more modern graphics shader management in future Qt
===================================================================

1. Check out https://github.com/KhronosGroup/SPIRV-Cross and https://github.com/KhronosGroup/glslang under src/3rdparty

2. qmake && (n)make

3. QT += shadertools

4. Use QSpirvCompiler to compile (Vulkan-flavored) GLSL to SPIR-V. This can also optionally rewrite the input source to make it suitable for the batched pass of Qt Quick's default OpenGL renderer.

5. Use QSpirvShader parse a SPIR-V binary to get reflection data, to strip the binary, and to translate to GLSL suitable for various OpenGL (ES) versions. (or to HLSL/MSL)

6. The reflection data (QShaderDescription) can also be serialized to binary and human-readable JSON, and deserialized from binary JSON.
