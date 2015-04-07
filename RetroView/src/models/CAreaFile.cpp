#include "core/GLInclude.hpp"
#include "models/CAreaFile.hpp"
#include "core/CMaterialCache.hpp"
#include "ui/CGLViewer.hpp"
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

CAreaFile::CAreaFile()
    : m_currentSet(0)
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
    currentMaterialSet().setAmbient(m_ambient);

    CMaterialSet materialSet = currentMaterialSet();
    glm::mat4 model = glm::mat4(1);

    drawIbos(false, materialSet, model);
    drawIbos(true, materialSet, model);
}

void CAreaFile::drawBoundingBox()
{
    /*for (const SAABB& aabb : m_aabbs)
    {
        ::drawBoundingBox(aabb.aabb);
    }*/

    ::drawBoundingBox(m_boundingBox);
    for (CModelData& m : m_models)
    {
        for (CMesh& mesh : m.m_meshes)
        {
            CMaterial& mat = currentMaterialSet().material(mesh.m_materialID);
            if (!mat.isTransparent())
                continue;
            ::drawBoundingBox(mesh.m_boundingBox);
        }
    }
}

void CAreaFile::updateViewProjectionUniforms(const glm::mat4& view, const glm::mat4& proj)
{
    for (atUint32 matId : currentMaterialSet().materials())
    {
        CMaterial& mat = CMaterialCache::instance()->material(matId);
        mat.setViewMatrix(view);
        mat.setProjectionMatrix(proj);
    }
}

void CAreaFile::updateTexturesEnabled(const bool& enabled)
{
    for (atUint32 matId : currentMaterialSet().materials())
    {
        CMaterial& mat = CMaterialCache::instance()->material(matId);
        mat.setTexturesEnabled(enabled);
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
    return m_materialSets[m_currentSet];
}

atUint32 CAreaFile::materialSetCount() const
{
    return m_materialSets.size();
}

atUint32 CAreaFile::currentMaterialSetIndex() const
{
    return m_currentSet;
}

void CAreaFile::setCurrentMaterialSet(atUint32 set)
{
    m_currentSet = set % m_materialSets.size();;
}

void CAreaFile::drawIbos(bool transparents, CMaterialSet& materialSet, glm::mat4 modelMat)
{
    std::for_each(m_models.begin(), m_models.end(), [&transparents, &materialSet, &modelMat](CModelData& model)
    {
        model.drawIbos(transparents, materialSet, modelMat);
    });
}
