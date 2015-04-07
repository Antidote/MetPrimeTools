#ifndef CMDLFILE_HPP
#define CMDLFILE_HPP

#include "core/CModelData.hpp"
#include "core/IRenderableModel.hpp"
#include "core/IResource.hpp"
#include "core/CVertexBuffer.hpp"

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
private:
    friend class CModelReader;

    Version                   m_version;
    atUint32                  m_flags;
    std::vector<CMaterialSet> m_materialSets;
    atUint32                  m_currentSet;
};

#endif // CMDLFILE_HPP
