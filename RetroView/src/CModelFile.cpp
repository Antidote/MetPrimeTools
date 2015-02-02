#include "CModelFile.hpp"
#include "CMaterialCache.hpp"
#include "CGLViewer.hpp"

CModelFile::CModelFile()
{
}

CModelFile::~CModelFile()
{

}

void CModelFile::draw()
{
    QColor amb = QColor(128, 128, 128);

    if (m_format & 0x2000)
        amb = QColor(0, 0, 0);

    for (CMaterialSet& ms : m_materialSets)
        CMaterialCache::instance()->setAmbientOnMaterials(ms.materials(), amb);

    glm::mat4 model = CGLViewer::instance()->modelMatrix();
    CModelData::preDraw(currentMaterialSet());
    CModelData::drawIbos(false, currentMaterialSet(), model);
    CModelData::drawIbos(true,  currentMaterialSet(), model);
    CModelData::doneDraw();
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

