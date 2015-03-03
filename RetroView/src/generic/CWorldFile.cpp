#include "generic/CWorldFile.hpp"
#include "core/CResourceManager.hpp"
#include "core/IRenderableModel.hpp"

CWorldFile::CWorldFile()
{

}

CWorldFile::~CWorldFile()
{

}

IRenderableModel* CWorldFile::skyboxModel()
{
    return dynamic_cast<IRenderableModel*>(CResourceManager::instance()->loadResourceFromPak(m_source, m_skyboxID, "cmdl"));
}

