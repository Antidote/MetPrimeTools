#include <GL/glew.h>
#include "models/CAreaFile.hpp"
#include "core/GXCommon.hpp"
#include "core/CMaterialCache.hpp"
#include "ui/CGLViewer.hpp"
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
    atUint32 modelIndex = 0;
    for (CModelData& model : m_models)
    {
        of << "o "  <<  m_assetType.toString()  << "_" << m_assetID.toString() << "_" << (modelIndex++) << std::endl;
        of << "s 1" << std::endl;
        model.exportModel(of, vertOff, normOff, texOff, currentMaterialSet());
    }
}

void CAreaFile::indexIBOs()
{
    QColor clr = (m_version != MetroidPrime3 || m_version != DKCR) ? QColor(0, 0, 0) : QColor(128, 128, 128);
    setAmbient(clr);

    CMaterialSet& materialSet = currentMaterialSet();

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

    CMaterialCache::instance()->setAmbientOnMaterials(currentMaterialSet().materials(), m_ambient);

    buildBBox();
    indexIBOs();
    CMaterialSet materialSet = currentMaterialSet();
    glm::mat4 model = glm::mat4(1);//glm::transpose(glm::inverse(glm::mat4(m_transformMatrix)));

    // our opaques go first
    for (CModelData& m : m_models)
    {
        m.preDraw(materialSet);
        m.drawIbos(false, materialSet, model);
        m.doneDraw();
    }

    // then our transparents
    for (CModelData& m : m_models)
    {
        m.preDraw(materialSet);
        m.drawIbos(true, materialSet, model);
        m.doneDraw();
    }
}

void CAreaFile::drawBoundingBox()
{
    ::drawBoundingBox(m_boundingBox);
    for (CModelData& m : m_models)
        m.drawBoundingBoxes();
}

void CAreaFile::updateViewProjectionUniforms(const glm::mat4& view, const glm::mat4& proj)
{
    for (atUint32 materialIdx : currentMaterialSet().materials())
    {
        CMaterial& mat = CMaterialCache::instance()->material(materialIdx);
        if (!mat.bind())
            continue;

        atUint32 projectionLoc = mat.program()->uniformLocation("projection");
        atUint32 viewLoc = mat.program()->uniformLocation("view");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &proj[0][0]);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        mat.release();
    }
}

glm::mat3x4 CAreaFile::transformMatrix() const
{
    return m_transformMatrix;
}

SBoundingBox& CAreaFile::boundingBox()
{
    return m_boundingBox;
}

CMaterialSet& CAreaFile::currentMaterialSet()
{
    return m_materialSets[0];
}

