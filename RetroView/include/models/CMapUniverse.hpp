#ifndef CMAPUNIVERSE_HPP
#define CMAPUNIVERSE_HPP

#include "core/IResource.hpp"
#include "core/IRenderableModel.hpp"
#include <QColor>
#include <glm/glm.hpp>

struct SMapWorld final
{
    std::string name;
    CAssetID    mlvlID;
    glm::mat4   transform;
    std::vector<glm::mat4> hexagonTransforms;
    QColor      color;
};

class CMapUniverse final : public IResource, public IRenderableModel
{
public:
    CMapUniverse();
    virtual ~CMapUniverse();

    void draw();
    void drawBoundingBox();

    void updateViewProjectionUniforms(const glm::mat4& view, const glm::mat4& proj);

    SBoundingBox& boundingBox();

    void setCurrentMaterialSet(atUint32 set);
    CMaterialSet& currentMaterialSet();
    atUint32 currentMaterialSetIndex() const;
    atUint32 materialSetCount() const;
private:
    friend class CMapUniverseReader;

    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;
    atUint32 m_materialID;

    CAssetID m_mapAreaID; // Hexagon

    std::vector<SMapWorld> m_worlds;

    SBoundingBox m_boundingBox;
};



#endif // CMAPUNIVERSE_HPP
