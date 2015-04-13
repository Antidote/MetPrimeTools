SOURCES += \
    $$PWD/tinystr.cpp \
    $$PWD/tinyxml.cpp \
    $$PWD/tinyxmlerror.cpp \
    $$PWD/tinyxmlparser.cpp
    
HEADERS += \
    $$PWD/tinystr.h \
    $$PWD/tinyxml.h
    
INCLUDEPATH += $$PWD
DEFINES += TIXML_USE_STL=1
