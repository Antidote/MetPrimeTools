#ifndef CMAPUNIVERSE_HPP
#define CMAPUNIVERSE_HPP

#include "core/IResource.hpp"
#include "core/IRenderableModel.hpp"
#include <QColor>

struct SMapWorld final
{
    std::string name;
    CUniqueID    mlvlID;
    CTransform   transform;
    std::vector<CTransform> hexagonTransforms;
    CColor      color;
};

class CMapUniverse final : public IResource, public IRenderableModel
{
public:
    CMapUniverse();
    virtual ~CMapUniverse();

    void draw();
    void drawBoundingBox();

    void updateViewProjectionUniforms(const CTransform& view, const CProjection& proj);

    SBoundingBox& boundingBox();

    void setCurrentMaterialSet(atUint32 set);
    CMaterialSet& currentMaterialSet();
    atUint32 currentMaterialSetIndex() const;
    atUint32 materialSetCount() const;
private:
    friend class CMapUniverseReader;

    CTransform m_view;
    CProjection m_projection;
    atUint32 m_materialID;

    CUniqueID m_mapAreaID; // Hexagon

    std::vector<SMapWorld> m_worlds;

    SBoundingBox m_boundingBox;
};



#endif // CMAPUNIVERSE_HPP
