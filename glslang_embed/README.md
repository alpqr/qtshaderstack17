Qt project files for building glslang for embedding into Qt libs and
apps.

This could become the base for creating a libshaderc-like Qt API that
could then be used both by external tools (like qsc in this repo) and
could also be built in to apps/libs that need runtime compilation for
some reason.

Unlike external glslang wrappers (shaderc and others), this would
allow handling Qt-specifics properly (e.g. could support #include for
files in qrc), and would only involve a minimal,
easy-to-integrate-with-qmake-or-qbs-or-whatever set of dependencies.

Assumes https://github.com/KhronosGroup/glslang is checked out under
the current directory.

Tested on Windows (MSVC2015) only for now.

Run qmake glslang.pro && (n)make to build the static libraries and the
example (which is glslangValidator as-is for now). See
glslang-validator.pro for how the static libs can be integrated into
other libs/apps.
