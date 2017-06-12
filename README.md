Experiments for more modern graphics shader management in future Qt
===================================================================

Built on top of https://github.com/KhronosGroup/SPIRV-Cross

Note: expects SPIRV-Cross to be checked out under src/3rdparty

See shaderstack.txt for vision and motivation.

All APIs are WIP. To manually run qsc, glslc needs to be in the PATH.
Alternatively, set QT_GLSLC, for example: export QT_GLSLC=~/android-ndk-r13b/shader-tools/linux-x86_64/glslc

For instance, going to test/playground and running "qsc color_phong.frag" results in:

 * color_phong.frag.spv -> SPIR-V binary
 * color_phong.frag.refl.json -> reflection data (text, for debugging)
 * color_phong.frag.refl -> binary JSON version (binary, to be deployed/loaded at runtime)
 * color_phong.frag.glsl100 -> SPIRV-Cross' translation to GLSL (ES) 100
 * color_phong.frag.glsl150 -> SPIRV-Cross' translation to GLSL 150

See qsc --help for configuration options.

It's not hard to see where this is going. Once qsc generated the stuff at build
time, we can include it in the resource system and do things like:

```
QShader vs(QLatin1String(":/color.vert")); // just the common prefix
QShader fs(QLatin1String(":/color.frag"));

qDebug() << vs.availableGlslVersions();
qDebug() << fs.availableGlslVersions();

QSurfaceFormat fmt;
...

QByteArray vertexShaderSource = vs.glsl(fmt); // picks 100 or 150 variant depending on profile by default

QByteArray shaderBlob = vs.spirv(); // can be passed as-is to Vulkan

QShaderDescription desc = vs.description(); // reflection: in/out vars, uniform blocks, ...

```
