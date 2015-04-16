#include "models/CMapUniverse.hpp"
#include "models/CMapArea.hpp"
#include "core/GXCommon.hpp"
#include "core/CMaterialSet.hpp"
#include "core/CMaterial.hpp"
#include "core/CMaterialCache.hpp"
#include "core/CResourceManager.hpp"

CMapUniverse::CMapUniverse()
{

}

CMapUniverse::~CMapUniverse()
{

}

void CMapUniverse::draw()
{
    CMapArea* hex = dynamic_cast<CMapArea*>(CResourceManager::instance()->loadResourceFromPak(m_source, m_mapAreaID, "MAPA"));

    if (!hex)
        return;

    CTransform scaleMatrix = CTransformFromScaleVector(CVector3f(.025));
    hex->updateViewProjectionUniforms(m_view, m_projection);
    for (SMapWorld world : m_worlds)
    {
        for (CTransform& transform : world.hexagonTransforms)
        {
            CTransform model = scaleMatrix * world.transform * transform;
            hex->setTransform(model);
            hex->setColor(world.color);
            hex->draw();
        }
    }
    hex->setTransform(CTransform());
}

void CMapUniverse::drawBoundingBox()
{

}

void CMapUniverse::updateViewProjectionUniforms(const CTransform& view, const CProjection& proj)
{
    m_view = view;
    m_projection = proj;
}

SBoundingBox& CMapUniverse::boundingBox()
{
    return m_boundingBox;
}

void CMapUniverse::setCurrentMaterialSet(atUint32)
{
}

CMaterialSet& CMapUniverse::currentMaterialSet()
{
    return (CMaterialSet&)CMaterialSet::Invalid;
}

atUint32 CMapUniverse::currentMaterialSetIndex() const
{
    return 0;
}

atUint32 CMapUniverse::materialSetCount() const
{
    return 0;
}
