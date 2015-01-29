CONFIG = -std=c++11

SOURCES += \
    Texture.cpp \
    TextureReader.cpp

include(../Athena/Athena.pri)

LIBS += -lsquish -lpng -lz

HEADERS += \
    Texture.hpp \
    TextureReader.hpp \
    dds.h

