Qt project files for building glslang for embedding into Qt libs and
apps.

This could become the base for creating a libshaderc-like Qt API that
could then be used both by external tools (like qsc in this repo) and
could also be built in to apps/libs that need runtime compilation for
some reason.

Assumes https://github.com/KhronosGroup/glslang is checked out under
the current directory.

Tested on Windows (MSVC2015) only for now.

Run qmake glslang.pro && (n)make to build the static libraries and the
example (which is glslangValidator as-is for now). See
glslang-validator.pro for how the static libs can be integrated into
other libs/apps.
