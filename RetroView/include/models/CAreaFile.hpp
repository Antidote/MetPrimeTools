#ifndef MREAFILE_HPP
#define MREAFILE_HPP

#include "core/CModelData.hpp"
#include "core/IRenderableModel.hpp"
#include "core/IResource.hpp"
#include "core/GXCommon.hpp"

#include <vector>
#include <string>

struct SAABB
{
    SBoundingBox aabb;
    atInt32 unkIndex; // -1 for area extents
    atInt16 selfIndex1; // if this value != -1, this is the root AABB, 1 indexed
    atInt16 selfIndex2; // if this value != -1, this is a child AABB, 1 indexed
};

class CAreaFile : public IRenderableModel, public IResource
{
public:
    enum Version
    {
        MetroidPrimeDemo = 12,
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

    bool canExport() const { return true; }

    atUint32 materialSetCount() const;
    atUint32 currentMaterialSetIndex() const;
    void setCurrentMaterialSet(atUint32 set);
private:
    friend class CAreaReader;
    void drawIbos(bool transparents, CMaterialSet& materialSet, glm::mat4 model);
    void indexIBOs();
    void buildBBox();

    Version                         m_version;
    // Buffers
    // GL specific code
    bool                            m_ibosIndexed;
    std::unordered_map<atUint32, std::vector<SIndexBufferObject> > m_transparents;
    std::unordered_map<atUint32, std::vector<SIndexBufferObject> > m_opaques;
    std::vector<CMaterialSet>       m_materialSets;
    atUint32                        m_currentSet;
    std::vector<CModelData>         m_models;
    std::vector<SAABB>              m_aabbs;
    glm::mat3x4                     m_transformMatrix;
    SBoundingBox                    m_boundingBox;
};

#endif // MREAFILE_HPP
