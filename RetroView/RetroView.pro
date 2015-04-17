#-------------------------------------------------
#
# Project created by QtCreator 2014-01-28T09:00:01
#
#-------------------------------------------------

QT       += core gui xml opengl
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#DEFINES += GLEW_STATIC
#QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -Os
QMAKE_CXXFLAGS += -std=c++11

#QMAKE_CXXFLAGS += -std=c++11 -fopenmp
mac:QMAKE_LFLAGS += -stdlib=libc++
#QMAKE_LFLAGS += -fopenmp

# SIMD flags
gcc:QMAKE_CXXFLAGS += -msse -msse2 -msse3
msvc:QMAKE_CXXFLAGS += /arch:SSE2
win32:DEFINES -= WIN64

unix:{
    CONFIG += link_pkgconfig
    #LIBS += -ltinyxml
}

win32:LIBS += \
    -lopengl32 \
    -lglu32

unix:LIBS += \
    -lGL \
    -lGLU

mac:LIBS -= \
    -lGL \
    -lGLU

mac:LIBS += -lobjc -framework Foundation

DEFINES += _LARGEFILE64_SOURCE _FILE_OFFSET_BITS
include(../Athena/AthenaCore.pri)
include(../RetroCommon/RetroCommon.pri)
include(../MathLib/MathLib.pri)
include(../PakLib/PakLib.pri)
include(../TXTRLoader/TXTRLoader.pri)
include(../External/tinyxml/tinyxml.pri)
include(../External/libpng/png.pri)
!mac:include(../External/glew/glew.pri)

TARGET    = retroview
CONFIG   += console
CONFIG   += app_bundle

TEMPLATE = app

INCLUDEPATH += \
    $$PWD/src \
    $$PWD/../External/glm \
    $$PWD/../External/glew/include

SOURCES += \
    src/core/src/CCamera.cpp \
    src/core/src/CIndexBuffer.cpp \
    src/core/src/CKeyboardManager.cpp \
    src/core/src/CMaterialSet.cpp \
    src/core/src/CMesh.cpp \
    src/core/src/CModelData.cpp \
    src/core/src/CPakFileModel.cpp \
    src/core/src/CResourceManager.cpp \
    src/core/src/CResourceTreeItem.cpp \
    src/core/src/CScene.cpp \
    src/core/src/GXCommon.cpp \
    src/core/src/STEVStage.cpp \
    src/core/src/CWordBitmap.cpp \
    src/core/src/CVertexBuffer.cpp \
    src/core/src/CMaterialSection.cpp \
    src/core/src/IMaterial.cpp \
    src/core/src/CMaterial.cpp \
    src/core/src/CScriptObject.cpp \
    src/core/src/CMaterialCache.cpp \
    src/core/src/CPropertyTemplate.cpp \
    src/core/src/CProperty.cpp \
    src/core/src/CTemplateManager.cpp \
    src/core/src/IResource.cpp \
    src/core/src/CPASDatabase.cpp \
    src/ui/src/CMainWindow.cpp \
    src/ui/src/CGLViewer.cpp \
    src/ui/src/CMaterialViewer.cpp \
    src/ui/src/CPakTreeWidget.cpp \
    src/io/src/CAreaReader.cpp \
    src/io/src/CMaterialReader.cpp \
    src/io/src/CModelReader.cpp \
    src/io/src/CWorldFileReader.cpp \
    src/io/src/CMapUniverseReader.cpp \
    src/io/src/CStringTableReader.cpp \
    src/io/src/CMapAreaReader.cpp \
    src/io/src/CAnimCharacterSetReader.cpp \
    src/generic/src/CDependencyGroup.cpp \
    src/generic/src/CTexture.cpp \
    src/generic/src/CWorldFile.cpp \
    src/generic/src/CStringTable.cpp \
    src/generic/src/CAnimCharacterSet.cpp \
    src/generic/src/CAnimCharacterNode.cpp \
    src/models/src/CAreaFile.cpp \
    src/models/src/CAreaBspTree.cpp \
    src/models/src/CModelFile.cpp \
    src/models/src/CMapArea.cpp \
    src/models/src/CMapUniverse.cpp \
    src/main.cpp

HEADERS += \
    src/core/CCamera.hpp \
    src/core/CIndexBuffer.hpp \
    src/core/CKeyboardManager.hpp \
    src/core/CMaterialSet.hpp \
    src/core/CMesh.hpp \
    src/core/CModelData.hpp \
    src/core/CPakFileModel.hpp \
    src/core/CResourceManager.hpp \
    src/core/CResourceTreeItem.hpp \
    src/core/CScene.hpp \
    src/core/GXCommon.hpp \
    src/core/GXTypes.hpp \
    src/core/CColor.hpp \
    src/core/IRenderableModel.hpp \
    src/core/IResource.hpp \
    src/core/SBoundingBox.hpp \
    src/core/STEVStage.hpp \
    src/core/CWordBitmap.hpp \
    src/core/CVertexBuffer.hpp \
    src/core/CMaterialSection.hpp \
    src/core/IMaterial.hpp \
    src/core/CMaterial.hpp \
    src/core/CScriptObject.hpp \
    src/core/CMaterialCache.hpp \
    src/core/CProperty.hpp \
    src/core/CPropertyTemplate.hpp \
    src/core/EPropertyType.hpp \
    src/core/CTemplateManager.hpp \
    src/core/SAnimation.hpp \
    src/core/SVertex.hpp \
    src/core/GLInclude.hpp \
    src/core/CPASDatabase.hpp \
    src/core/CPASParameter.hpp \
    src/core/SBoundingBox.hpp \
    src/io/CAreaReader.hpp \
    src/io/CMaterialReader.hpp \
    src/io/CModelReader.hpp \
    src/io/CWorldFileReader.hpp \
    src/io/CMapUniverseReader.hpp \
    src/io/CMapAreaReader.hpp \
    src/io/CStringTableReader.hpp \
    src/io/CAnimCharacterSetReader.hpp \
    src/ui/CGLViewer.hpp \
    src/ui/CMainWindow.hpp \
    src/ui/CMaterialViewer.hpp \
    src/ui/CPakTreeWidget.hpp \
    src/generic/CDependencyGroup.hpp \
    src/generic/CTexture.hpp \
    src/generic/CWorldFile.hpp \
    src/models/CAreaFile.hpp \
    src/models/CAreaBspTree.hpp \
    src/models/CModelFile.hpp \
    src/models/CMapUniverse.hpp \
    src/models/CMapArea.hpp \
    src/generic/CStringTable.hpp \
    src/generic/CAnimCharacterSet.hpp \
    src/generic/CAnimCharacterNode.hpp

win32:HEADERS += platforms/win_dirent.h
FORMS += \
    forms/CPakTreeWidget.ui \
    forms/CMainWindow.ui \
    forms/CMaterialViewer.ui

RESOURCES += \
    resources/resources.qrc

win32:RC_FILE += \
    resources/mainicon.rc
OTHER_FILES += resources/mainicon.rc

mac:OBJECTIVE_SOURCES += platforms/osx_extra.m
QMAKE_INFO_PLIST = resources/Info.plist
RESFILES.files = resources/icons/mainicon.icns resources/icons/pakicon.icns
RESFILES.path = Contents/Resources
QMAKE_BUNDLE_DATA += RESFILES
