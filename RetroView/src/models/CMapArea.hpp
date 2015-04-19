#ifndef CMAPAREA_HPP
#define CMAPAREA_HPP

#include "core/IResource.hpp"
#include "core/IRenderableModel.hpp"
#include "core/GXTypes.hpp"
#include "core/CMaterial.hpp"
#include "core/CColor.hpp"

struct SPointOfInterest
{
    atUint32  unknown1;
    atUint32  type;
    atUint32  unknown2;
    atUint16  id;
    atUint16  unknown3;
    CColor unknown4;
    CTransform transform;
};

struct SMapAreaPrimitive
{
    atUint32              type; // GL primitive NOT GX
    std::vector<atUint16> indices;
    atUint32              elementBuffer;
};

struct SMapBorder
{
    std::vector<atUint16> indices;
    atUint32              elementBuffer;
};

struct SMapAreaDetail
{
    SBoundingBox                   boundingBox;
    std::vector<SMapAreaPrimitive> primitives;
    std::vector<SMapBorder>        borders;
    atUint32                       borderBuffer;
};

class CMapArea : public IResource, public IRenderableModel
{
public:
    enum class Version
    {
        MetroidPrime1 = 2,
        MetroidPrime2 = 3,
        MetroidPrime3 = 5
    };

    CMapArea();
    ~CMapArea();

    void setTransform(const CTransform& transform);
    void setColor(const CColor& color);
    void draw();
    void drawBoundingBox();

    SBoundingBox& boundingBox();
    void updateViewProjectionUniforms(const CTransform& view, const CProjection& proj);

    void setCurrentMaterialSet(atUint32 set);
    atUint32 currentMaterialSetIndex() const;
    CMaterialSet& currentMaterialSet();
    atUint32 materialSetCount() const;

private:
    friend class CMapAreaReader;
    void buildVbo();
    atUint32                       m_unknown1;
    atUint32                       m_unknown2;
    SBoundingBox                   m_boundingBox;
    std::vector<SPointOfInterest>  m_pointsOfInterest;
    std::vector<CVector3f>         m_vertices;
    std::vector<SMapAreaDetail>    m_details;
    atUint32                       m_vertexBuffer;

    // Internal variables
    atUint32                       m_materialID;
    CColor                         m_color;
    bool                           m_vboBuilt;
    CTransform                     m_transform;
};

#endif // CMAPAREA_HPP
