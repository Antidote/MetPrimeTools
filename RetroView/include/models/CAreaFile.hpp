#ifndef MREAFILE_HPP
#define MREAFILE_HPP

#include "core/CModelData.hpp"
#include "core/IRenderableModel.hpp"
#include "core/IResource.hpp"

#include <vector>
#include <string>

class CAreaFile : public IRenderableModel, public IResource
{
public:
    enum Version
    {
        MetroidPrime1 = 15,
        MetroidPrime2 = 25,
        MetroidPrime3 = 30,
        DKCR          = 32
    };

    CAreaFile();
    virtual ~CAreaFile();

    void exportToObj(const std::string& filename);


    void draw();
    void drawBoundingBox();
    void updateViewProjectionUniforms(const glm::mat4& view, const glm::mat4& proj);
    void updateTexturesEnabled(const bool& enabled);

    glm::mat3x4 transformMatrix() const;

    SBoundingBox& boundingBox();

    CMaterialSet& currentMaterialSet();
    CMaterialSet& currentMaterialSet() const;

    bool canExport() const { return true; }
private:
    friend class CAreaReader;
    void indexIBOs();
    void buildBBox();

    Version                   m_version;
    std::vector<CMaterialSet> m_materialSets;
    std::vector<CModelData>   m_models;

    glm::mat3x4               m_transformMatrix;
    SBoundingBox              m_boundingBox;
};

#endif // MREAFILE_HPP
