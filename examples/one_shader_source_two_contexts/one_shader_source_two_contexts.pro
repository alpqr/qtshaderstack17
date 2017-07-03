TEMPLATE = app

QT += shadertools

SOURCES = \
    main.cpp \
    renderwindow.cpp

HEADERS = \
    renderwindow.h

RESOURCES = tri.qrc

# just get those godawful qmake rules to shut up
target.path = $$[QT_INSTALL_EXAMPLES]/one_shader_source_two_contexts
INSTALLS += target
