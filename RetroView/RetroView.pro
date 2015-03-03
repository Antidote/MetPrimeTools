#-------------------------------------------------
#
# Project created by QtCreator 2014-01-28T09:00:01
#
#-------------------------------------------------

QT       += core gui xml opengl
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#DEFINES += GLEW_STATIC
QMAKE_CXXFLAGS += -std=c++11 -fopenmp
QMAKE_LFLAGS += -fopenmp

unix:{
    CONFIG += link_pkgconfig
    PKGCONFIG += collada-dom-141
    LIBS += -lboost_system -lboost_filesystem
}

win32:LIBS += \
    -lopengl32 \
    -lglu32 \
    -L../External/glew/lib/win32 \
    -L../External/lzo/lib \
    -lglew32 \
    -lz

unix:LIBS += \
    -lGL \
    -lGLU \
    -lGLEW \
    -lpng \
    -lz


DEFINES += _LARGEFILE64_SOURCE _FILE_OFFSET_BITS
include(../Athena/AthenaCore.pri)
include(../RetroCommon/RetroCommon.pri)
include(../PakLib/PakLib.pri)
include(../TXTRLoader/TXTRLoader.pri)

TARGET    = RetroView
CONFIG   += console
CONFIG   += app_bundle

TEMPLATE = app

INCLUDEPATH += include
win32:INCLUDEPATH += ../External/glm ../External/glew/include ../External/lzo/include

SOURCES += \
    src/core/CCamera.cpp \
    src/core/CIndexBuffer.cpp \
    src/core/CKeyboardManager.cpp \
    src/core/CMainWindow.cpp \
    src/core/CMaterial.cpp \
    src/core/CMaterialCache.cpp \
    src/core/CMaterialSet.cpp \
    src/core/CMesh.cpp \
    src/core/CModelData.cpp \
    src/core/CPakFileModel.cpp \
    src/core/CResourceManager.cpp \
    src/core/CResourceTreeItem.cpp \
    src/core/CScene.cpp \
    src/core/GXCommon.cpp \
    src/generic/CDependencyGroup.cpp \
    src/generic/CTexture.cpp \
    src/generic/CWorldFile.cpp \
    src/io/CAreaReader.cpp \
    src/io/CMaterialReader.cpp \
    src/io/CModelReader.cpp \
    src/io/CWorldFileReader.cpp \
    src/material/STEVStage.cpp \
    src/models/CAreaFile.cpp \
    src/models/CModelFile.cpp \
    src/ui/CGLViewer.cpp \
    src/ui/CMaterialViewer.cpp \
    src/ui/CPakTreeWidget.cpp

!contains(DEFINES, NO_MAIN): {
    SOURCES += src/main.cpp
}

HEADERS += \
    include/core/CCamera.hpp \
    include/core/CIndexBuffer.hpp \
    include/core/CKeyboardManager.hpp \
    include/core/CMaterial.hpp \
    include/core/CMaterialCache.hpp \
    include/core/CMaterialSet.hpp \
    include/core/CMesh.hpp \
    include/core/CModelData.hpp \
    include/core/CPakFileModel.hpp \
    include/core/CResourceManager.hpp \
    include/core/CResourceTreeItem.hpp \
    include/core/CScene.hpp \
    include/core/GXCommon.hpp \
    include/core/GXTypes.hpp \
    include/core/IRenderableModel.hpp \
    include/core/IResource.hpp \
    include/core/SBoundingBox.hpp \
    include/core/STEVStage.hpp \
    include/generic/CDependencyGroup.hpp \
    include/generic/CTexture.hpp \
    include/generic/CWorldFile.hpp \
    include/io/CAreaReader.hpp \
    include/io/CMaterialReader.hpp \
    include/io/CModelReader.hpp \
    include/io/CWorldFileReader.hpp \
    include/models/CAreaFile.hpp \
    include/models/CModelFile.hpp \
    include/ui/CGLViewer.hpp \
    include/ui/CMainWindow.hpp \
    include/ui/CMaterialViewer.hpp \
    include/ui/CPakTreeWidget.hpp

FORMS += \
    forms/CPakTreeWidget.ui \
    forms/CMainWindow.ui \
    forms/CMaterialViewer.ui

RESOURCES += \
    resources/resources.qrc

win32:RC_FILE += \
    resources/mainicon.rc
