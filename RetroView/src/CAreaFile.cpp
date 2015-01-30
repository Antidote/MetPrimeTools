#include "CAreaFile.hpp"
#include "GXCommon.hpp"
#include "CMaterialCache.hpp"
#include <GL/gl.h>
#include <iostream>

CAreaFile::CAreaFile()
{
}

CAreaFile::~CAreaFile()
{
}

void CAreaFile::exportToObj(const std::string& filename)
{
    std::ofstream of(filename);
    atUint32 vertOff = 1;
    atUint32 normOff = 1;
    atUint32 texOff = 1;
    for (CModelData& model : m_models)
    {
        model.exportModel(of, vertOff, normOff, texOff);
    }
}

void CAreaFile::indexIBOs()
{
    CMaterialSet& materialSet = m_materialSets[0];

    for (CModelData& model : m_models)
        model.indexIBOs(materialSet);
}

void CAreaFile::buildBBox()
{
    if ((m_boundingBox.max.x > 0 && m_boundingBox.min.x < 0) &&
            (m_boundingBox.max.y > 0 && m_boundingBox.min.y < 0) &&
            (m_boundingBox.max.z > 0 && m_boundingBox.min.z < 0))
        return;

    for (const CModelData& model : m_models)
    {
        if (model.m_boundingBox.min.x <= m_boundingBox.min.x)
            m_boundingBox.min.x = model.m_boundingBox.min.x;

        if (model.m_boundingBox.min.y <= m_boundingBox.min.y)
            m_boundingBox.min.y = model.m_boundingBox.min.y;

        if (model.m_boundingBox.min.z <= m_boundingBox.min.z)
            m_boundingBox.min.z = model.m_boundingBox.min.z;

        if (model.m_boundingBox.max.x >= m_boundingBox.max.x)
            m_boundingBox.max.x = model.m_boundingBox.max.x;

        if (model.m_boundingBox.max.y >= m_boundingBox.max.y)
            m_boundingBox.max.y = model.m_boundingBox.max.y;

        if (model.m_boundingBox.max.z >= m_boundingBox.max.z)
            m_boundingBox.max.z = model.m_boundingBox.max.z;
    }
}

void CAreaFile::draw()
{
    for (CMaterialSet& ms : m_materialSets)
        CMaterialCache::instance()->setAmbientOnMaterials(ms.materials(), QColor(0, 0, 0));

    buildBBox();
    indexIBOs();
    CMaterialSet materialSet = currentMaterial();
    for (CModelData& m : m_models)
        m.draw(materialSet);
}

void CAreaFile::drawBoundingBox()
{
    ::drawBoundingBox(m_boundingBox);
    for (CModelData& m : m_models)
        m.drawBoundingBoxes();
}

glm::mat3x4 CAreaFile::transformMatrix() const
{
    return m_transformMatrix;
}

SBoundingBox& CAreaFile::boundingBox()
{
    return m_boundingBox;
}

CMaterialSet& CAreaFile::currentMaterial()
{
    return m_materialSets[0];
}

