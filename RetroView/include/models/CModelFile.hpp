#ifndef CMDLFILE_HPP
#define CMDLFILE_HPP

#include "core/CModelData.hpp"
#include "core/IRenderableModel.hpp"
#include "core/IResource.hpp"
#include "core/CVertexBuffer.hpp"
#include <glm/gtc/quaternion.hpp>

class CModelFile : public CModelData, public IRenderableModel, public IResource
{
public:
    enum Version
    {
        MetroidPrime1 = 2,
        MetroidPrime2 = 4,
        MetroidPrime3 = 5,
        DKCR          = 6
    };

    CModelFile();
    virtual ~CModelFile();

    SBoundingBox& boundingBox();

    void draw();
    void drawBoundingBox();
    void updateViewProjectionUniforms(const glm::mat4& view, const glm::mat4& proj);
    void updateTexturesEnabled(const bool& enabled);

    CMaterialSet& currentMaterialSet();

    bool canExport() const;
    void exportToObj(const std::string& filepath);
    atUint32 materialSetCount() const;
    void setCurrentMaterialSet(atUint32 set);
    atUint32 currentMaterialSetIndex() const;

    void setPosition(const glm::vec3& pos);
    void setRotation(const glm::vec3& rotation);
    void setScale(const glm::vec3& scale);
    void restoreDefaults();
private:
    friend class CModelReader;

    Version                   m_version;
    atUint32                  m_flags;
    std::vector<CMaterialSet> m_materialSets;
    atUint32                  m_currentSet;
    glm::vec3                 m_position;
    glm::quat                 m_rotation;
    glm::vec3                 m_scale;
};

#endif // CMDLFILE_HPP
