#include "CModelFile.hpp"

CModelFile::CModelFile()
    : IRenderableModel(0)
{
}

CModelFile::~CModelFile()
{

}

void CModelFile::draw()
{
    CModelData::draw(currentMaterialSet());
}

void CModelFile::drawBoundingBox()
{
    CModelData::drawBoundingBoxes();
}

CMaterialSet& CModelFile::currentMaterialSet()
{
    return m_materialSets[0];
}

CMaterialSet& CModelFile::currentMaterialSet() const
{
    return currentMaterialSet();
}

bool CModelFile::canExport() const
{
    return true;
}

void CModelFile::exportToObj(const std::string& filepath)
{
    atUint32 vertexOff = 1;
    atUint32 normalOff = 1;
    atUint32 texCoordOff = 1;
    std::ofstream out(filepath);
    CModelData::exportModel(out, vertexOff, normalOff, texCoordOff);
    out.flush();
    out.close();
}

SBoundingBox& CModelFile::boundingBox()
{
    static SBoundingBox bbox{m_boundingBox};

    if (bbox != SBoundingBox{m_boundingBox})
        bbox =  SBoundingBox{m_boundingBox};

    return bbox;
}

