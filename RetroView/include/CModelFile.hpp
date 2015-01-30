#ifndef CMDLFILE_HPP
#define CMDLFILE_HPP

#include "CModelData.hpp"
#include "IRenderableModel.hpp"
#include "IResource.hpp"

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
