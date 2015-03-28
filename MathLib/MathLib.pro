TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    CVector3f.cpp \
    CVector3d.cpp \
    Math.cpp \
    CQuaternion.cpp \
    CMatrix3f.cpp

include (../Athena/AthenaCore.pri)
HEADERS += \
    CVector3f.hpp \
    CVector3d.hpp \
    Math.hpp \
    CQuaternion.hpp \
    CMatrix3f.hpp

