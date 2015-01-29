#ifndef MREAFILE_HPP
#define MREAFILE_HPP

#include "CModelData.hpp"
#include <vector>
#include <string>
#include "IRenderableModel.hpp"

class CAreaFile : public IRenderableModel
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
    ~CAreaFile();

    void exportToObj(const std::string& filename);


    void draw();

    glm::mat3x4 transformMatrix() const;

    SBoundingBox& boundingBox();

    CMaterialSet& currentMaterial();
    CMaterialSet& currentMaterial() const;

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
