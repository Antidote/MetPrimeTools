TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    Texture.cpp \
    TextureReader.cpp

include(../Athena/Athena.pri)

LIBS += -lsquish -lpng -lz

HEADERS += \
    Texture.hpp \
    TextureReader.hpp \
    dds.h

