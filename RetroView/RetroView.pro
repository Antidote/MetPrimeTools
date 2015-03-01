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
    src/GXCommon.cpp \
    src/CCamera.cpp \
    src/CGLViewer.cpp \
    src/CMaterialReader.cpp \
    src/CMaterialCache.cpp \
    src/CMaterialSet.cpp \
    src/CMaterial.cpp \
    src/CMesh.cpp \
    src/CMainWindow.cpp \
    src/CModelData.cpp \
    src/CResourceManager.cpp \
    src/CScene.cpp \
    src/CModelFile.cpp \
    src/CModelReader.cpp \
    src/CAreaFile.cpp \
    src/CAreaReader.cpp \
    src/CTexture.cpp \
    src/STEVStage.cpp \
    src/CKeyboardManager.cpp \
    src/CPakFileModel.cpp \
    src/CResourceTreeItem.cpp \
    src/CPakTreeWidget.cpp \
    src/CIndexBuffer.cpp \
    src/CMaterialViewer.cpp

!contains(DEFINES, NO_MAIN): {
    SOURCES += src/main.cpp
}

HEADERS += \
    include/GXCommon.hpp \
    include/IRenderableModel.hpp \
    include/GXCommon.hpp \
    include/GXTypes.hpp \
    include/CCamera.hpp \
    include/CScene.hpp \
    include/CGLViewer.hpp \
    include/CMesh.hpp \
    include/CAreaReader.hpp \
    include/CAreaFile.hpp \
    include/CModelData.hpp \
    include/CMaterialSet.hpp \
    include/CMaterialReader.hpp \
    include/CMaterialCache.hpp \
    include/CMainWindow.hpp \
    include/CMaterial.hpp \
    include/CResourceManager.hpp \
    include/SBoundingBox.hpp \
    include/CModelFile.hpp \
    include/CModelReader.hpp \
    include/IResource.hpp \
    include/CTexture.hpp \
    include/STEVStage.hpp \
    include/CKeyboardManager.hpp \
    include/CPakFileModel.hpp \
    include/CResourceTreeItem.hpp \
    include/CPakTreeWidget.hpp \
    include/CIndexBuffer.hpp \
    include/CMaterialViewer.hpp

FORMS += \
    forms/CPakTreeWidget.ui \
    forms/CMainWindow.ui \
    forms/CMaterialViewer.ui

RESOURCES += \
    resources/resources.qrc

win32:RC_FILE += \
    resources/mainicon.rc
