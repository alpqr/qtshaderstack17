TEMPLATE = app

QT += shadertools

SOURCES = \
    main.cpp \
    renderwindow.cpp \
    trianglerenderer.cpp

HEADERS = \
    renderwindow.h \
    trianglerenderer.h

RESOURCES = tri.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/one_shader_three_contexts
INSTALLS += target
