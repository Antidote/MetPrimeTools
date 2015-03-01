TEMPLATE = app
TARGET = mptexconv

!contains(CONFIG, -std=c++11):CONFIG += -std=c++11

include(../Athena/AthenaCore.pri)
include(../RetroCommon/RetroCommon.pri)

win32:LIBS += -L$$PWD/Externals/squish/lib

LIBS += -lsquish -lpng -lz

INCLUDEPATH += $$PWD/include $$PWD/Externals/pngpp/include
win32:INCLUDEPATH += $$PWD/Externals/squish/include

SOURCES += \
    $$PWD/src/Texture.cpp \
    $$PWD/src/TextureReader.cpp \
    $$PWD/src/main.cpp

HEADERS += \
    $$PWD/include/Texture.hpp \
    $$PWD/include/TextureReader.hpp \
    $$PWD/include/dds.h

