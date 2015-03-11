#ifndef CMDLFILE_HPP
#define CMDLFILE_HPP

#include "core/CModelData.hpp"
#include "core/IRenderableModel.hpp"
#include "core/IResource.hpp"

class CModelFile : public CModelData, public IRenderableModel, public IResource
{
public:
    enum Version
    {
        MetroidPrime1 = 2,
        MetroidPrime2 = 4,
        MetroidPrime3 = 5
    };

    CModelFile();
    virtual ~CModelFile();

    SBoundingBox& boundingBox();

    void draw();
    void drawBoundingBox();
    void updateViewProjectionUniforms(const glm::mat4& view, const glm::mat4& proj);
    void updateTexturesEnabled(const bool& enabled);

    CMaterialSet& currentMaterialSet();
    CMaterialSet& currentMaterialSet() const;

    bool canExport() const;
    void exportToObj(const std::string& filepath);
private:
    friend class CModelReader;

    Version                  m_version;
    atUint32                 m_format;
    std::vector<CMaterialSet> m_materialSets;
};

#endif // CMDLFILE_HPP
