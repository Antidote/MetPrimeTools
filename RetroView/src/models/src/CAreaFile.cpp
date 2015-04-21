#include "core/GLInclude.hpp"
#include "models/CAreaFile.hpp"
#include "core/CMaterialCache.hpp"
#include "ui/CGLViewer.hpp"
#include <iostream>

CAreaFile::CAreaFile()
    : m_currentSet(0)
{
    m_ambient = CColor(0, 0, 0);
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
    if ((m_boundingBox.m_max.x > 0 && m_boundingBox.m_min.x < 0) &&
            (m_boundingBox.m_max.y > 0 && m_boundingBox.m_min.y < 0) &&
            (m_boundingBox.m_max.z > 0 && m_boundingBox.m_min.z < 0))
        return;

    for (const CModelData& model : m_models)
    {
        if (model.m_boundingBox.m_min.x <= m_boundingBox.m_min.x)
            m_boundingBox.m_min.x = model.m_boundingBox.m_min.x;

        if (model.m_boundingBox.m_min.y <= m_boundingBox.m_min.y)
            m_boundingBox.m_min.y = model.m_boundingBox.m_min.y;

        if (model.m_boundingBox.m_min.z <= m_boundingBox.m_min.z)
            m_boundingBox.m_min.z = model.m_boundingBox.m_min.z;

        if (model.m_boundingBox.m_max.x >= m_boundingBox.m_max.x)
            m_boundingBox.m_max.x = model.m_boundingBox.m_max.x;

        if (model.m_boundingBox.m_max.y >= m_boundingBox.m_max.y)
            m_boundingBox.m_max.y = model.m_boundingBox.m_max.y;

        if (model.m_boundingBox.m_max.z >= m_boundingBox.m_max.z)
            m_boundingBox.m_max.z = model.m_boundingBox.m_max.z;
    }
}

void CAreaFile::draw()
{
    currentMaterialSet().setAmbient(m_ambient);
    CMaterialSet materialSet = currentMaterialSet();
    CTransform model;
    if (m_version != MetroidPrime1)
        drawIbos(false, materialSet, model);
    else
        m_bspTree.drawArea(*this, false, materialSet, model);


    for (CScene* scene : m_scriptLayers)
    {
        for (CScriptObject obj : scene->m_objects)
        {
            if (obj.isAreaAttributes())
                continue;

            obj.draw();
        }
    }

    if (m_version != MetroidPrime1)
        drawIbos(true, materialSet, model);
    else
        m_bspTree.drawArea(*this, true, materialSet, model);
}

void CAreaFile::drawBoundingBox()
{
#if 0
    for (const SAABB& aabb : m_aabbs)
    {
        ::drawBoundingBox(aabb.aabb, CColor(0.0, 0.0, 1.0));
    }
#endif

    ::drawBoundingBox(m_boundingBox);
    for (CModelData& m : m_models)
    {
        for (CMesh& mesh : m.m_meshes)
        {
            CMaterial& mat = currentMaterialSet().material(mesh.m_materialID);
            //if (!mat.isTransparent())
            //    continue;
            ::drawBoundingBox(mesh.m_boundingBox);
        }
    }
}

void CAreaFile::updateViewProjectionUniforms(const CTransform& view, const CProjection& proj)
{
    for (atUint32 matId : currentMaterialSet().materials())
    {
        CMaterial& mat = CMaterialCache::instance()->material(matId);
        mat.setView(view);
        mat.setProjection(proj);
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

CTransform CAreaFile::transform() const
{
    return m_transform;
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
    m_currentSet = set % m_materialSets.size();
}

void CAreaFile::nearestSpawnPoint(const CVector3f& pos, CVector3f& targetPos, CVector3f& rot)
{
    if (m_scriptLayers.size() == 0)
    {
        targetPos = pos;
        return;
    }

    return m_scriptLayers[0]->nearestSpawnPoint(pos, targetPos, rot);
}

bool CAreaFile::skyEnabled()
{
    if (m_scriptLayers.size() > 0)
    {
        return m_scriptLayers[0]->isSkyEnabled();
    }

    return false;
}

void CAreaFile::drawIbos(bool transparents, CMaterialSet& materialSet, const CTransform& modelXf)
{
    std::for_each(m_models.begin(), m_models.end(), [&transparents, &materialSet, &modelXf](CModelData& model)
    {
        model.drawIbos(transparents, materialSet, modelXf);
    });
}
