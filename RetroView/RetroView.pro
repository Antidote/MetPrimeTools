#-------------------------------------------------
#
# Project created by QtCreator 2014-01-28T09:00:01
#
#-------------------------------------------------

QT       += core gui xml opengl
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#DEFINES += GLEW_STATIC
#QMAKE_CXXFLAGS_RELEASE -= -O2
#QMAKE_CXXFLAGS_RELEASE += -Os
QMAKE_CXXFLAGS += -std=c++11
#QMAKE_CXXFLAGS += -std=c++11 -fopenmp
mac:QMAKE_LFLAGS += -stdlib=libc++
#QMAKE_LFLAGS += -fopenmp

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
include(../PakLib/PakLib.pri)
include(../TXTRLoader/TXTRLoader.pri)
include(../External/tinyxml/tinyxml.pri)
include(../External/libpng/png.pri)
include(../External/zlib/zlib.pri)
include(../External/glew/glew.pri)

TARGET    = retroview
CONFIG   += console
CONFIG   += app_bundle

TEMPLATE = app

INCLUDEPATH += \
    $$PWD/include

win32:INCLUDEPATH += \
    $$PWD/../External/glm \
    $$PWD/../External/glew/include

SOURCES += \
    src/core/CCamera.cpp \
    src/core/CIndexBuffer.cpp \
    src/core/CKeyboardManager.cpp \
    src/core/CMaterialSet.cpp \
    src/core/CMesh.cpp \
    src/core/CModelData.cpp \
    src/core/CPakFileModel.cpp \
    src/core/CResourceManager.cpp \
    src/core/CResourceTreeItem.cpp \
    src/core/CScene.cpp \
    src/core/GXCommon.cpp \
    src/core/STEVStage.cpp \
    src/core/CWordBitmap.cpp \
    src/core/CVertexBuffer.cpp \
    src/core/CMaterialSection.cpp \
    src/core/IMaterial.cpp \
    src/core/CMaterial.cpp \
    src/core/CScriptObject.cpp \
    src/core/CMaterialCache.cpp \
    src/core/CPropertyTemplate.cpp \
    src/core/CProperty.cpp \
    src/core/CTemplateManager.cpp \
    src/core/IResource.cpp \
    src/core/CPASDatabase.cpp \
    src/ui/CMainWindow.cpp \
    src/ui/CGLViewer.cpp \
    src/ui/CMaterialViewer.cpp \
    src/ui/CPakTreeWidget.cpp \
    src/io/CAreaReader.cpp \
    src/io/CMaterialReader.cpp \
    src/io/CModelReader.cpp \
    src/io/CWorldFileReader.cpp \
    src/io/CMapUniverseReader.cpp \
    src/io/CStringTableReader.cpp \
    src/io/CMapAreaReader.cpp \
    src/io/CAnimCharacterSetReader.cpp \
    src/generic/CDependencyGroup.cpp \
    src/generic/CTexture.cpp \
    src/generic/CWorldFile.cpp \
    src/generic/CStringTable.cpp \
    src/generic/CAnimCharacterSet.cpp \
    src/generic/CAnimCharacterNode.cpp \
    src/models/CAreaFile.cpp \
    src/models/CAreaBspTree.cpp \
    src/models/CModelFile.cpp \
    src/models/CMapArea.cpp \
    src/models/CMapUniverse.cpp \
    src/main.cpp

HEADERS += \
    include/core/CCamera.hpp \
    include/core/CIndexBuffer.hpp \
    include/core/CKeyboardManager.hpp \
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
    include/core/CWordBitmap.hpp \
    include/core/CVertexBuffer.hpp \
    include/core/CMaterialSection.hpp \
    include/core/IMaterial.hpp \
    include/core/CMaterial.hpp \
    include/core/CScriptObject.hpp \
    include/core/CMaterialCache.hpp \
    include/core/CProperty.hpp \
    include/core/CPropertyTemplate.hpp \
    include/core/EPropertyType.hpp \
    include/core/CTemplateManager.hpp \
    include/core/SAnimation.hpp \
    include/core/SVertex.hpp \
    include/core/GLInclude.hpp \
    include/core/CPASDatabase.hpp \
    include/core/CPASParameter.hpp \
    include/core/SBoundingBox.hpp \
    include/io/CAreaReader.hpp \
    include/io/CMaterialReader.hpp \
    include/io/CModelReader.hpp \
    include/io/CWorldFileReader.hpp \
    include/io/CMapUniverseReader.hpp \
    include/io/CMapAreaReader.hpp \
    include/io/CStringTableReader.hpp \
    include/io/CAnimCharacterSetReader.hpp \
    include/ui/CGLViewer.hpp \
    include/ui/CMainWindow.hpp \
    include/ui/CMaterialViewer.hpp \
    include/ui/CPakTreeWidget.hpp \
    include/generic/CDependencyGroup.hpp \
    include/generic/CTexture.hpp \
    include/generic/CWorldFile.hpp \
    include/models/CAreaFile.hpp \
    include/models/CAreaBspTree.hpp \
    include/models/CModelFile.hpp \
    include/models/CMapUniverse.hpp \
    include/models/CMapArea.hpp \
    include/generic/CStringTable.hpp \
    include/generic/CAnimCharacterSet.hpp \
    include/generic/CAnimCharacterNode.hpp

FORMS += \
    forms/CPakTreeWidget.ui \
    forms/CMainWindow.ui \
    forms/CMaterialViewer.ui

RESOURCES += \
    resources/resources.qrc

win32:RC_FILE += \
    resources/mainicon.rc

mac:OBJECTIVE_SOURCES += platforms/osx_extra.m
QMAKE_INFO_PLIST = resources/Info.plist
RESFILES.files = resources/icons/mainicon.icns resources/icons/pakicon.icns
RESFILES.path = Contents/Resources
QMAKE_BUNDLE_DATA += RESFILES
