!contains(CONFIG, -std=c++11):CONFIG += -std=c++11

LIBS += -lsquish -lpng -lz

INCLUDEPATH += $$PWD/include $$PWD/Externals/include

SOURCES += \
    $$PWD/src/Texture.cpp \
    $$PWD/src/TextureReader.cpp

HEADERS += \
    $$PWD/include/Texture.hpp \
    $$PWD/include/TextureReader.hpp \
    $$PWD/include/dds.h

