#ifndef MODEL_HPP
#define MODEL_HPP

#include "CMaterialSet.hpp"
#include "CMesh.hpp"
#include "core/CIndexBuffer.hpp"
#include "core/SBoundingBox.hpp"
#include "core/SVertex.hpp"

#include <dae.h>
#include <glm/glm.hpp>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <memory.h>

namespace EFormatFlags
{
enum
{
    Unknown     = (1 << 0),
    ShortNormal = (1 << 1),
    TexCoord1   = (1 << 2),
};
}

struct SIndexBufferObject
{
    atUint32              bufferID;
    glm::vec3             pos;
    atUint32              primitiveType;
    atUint32              materialId;
    std::vector<atUint32> indexBuffer;
    SBoundingBox          boundingBox;
};

class CModelData
{
public:
    CModelData();
    virtual ~CModelData();

    void exportModel(std::ofstream& of, atUint32& vertexOff, atUint32& normalOff, atUint32& texOff, CMaterialSet& ms);

    void drawBoundingBoxes();

    void indexIBOs(CMaterialSet& materialSet);

    void drawIbos(bool transparents, CMaterialSet& materialSet, glm::mat4 model);
    void drawTransparentBoxes();
protected:
    friend class CAreaFile;
    friend class CAreaReader;
    friend class CModelReader;
    friend class CMaterialViewer;

    atUint32 exportUVIdx(atUint32 texOff, SVertexDescriptor desc, CMaterial& mat, CMesh& mesh);
    atUint32 getIbo(atUint32 prim, atUint32 matId, atUint32 start);
    void indexVert(CMaterial& material, SVertexDescriptor& desc, CMesh& mesh, atUint32 iboId, atUint32 vertStartIndex);
    void loadIbos(CMaterialSet& ms);

    void sortTransparent(std::unordered_map<atUint32, std::vector<SIndexBufferObject> >& trans);

    atUint32                 m_format;
    glm::mat3x4              m_transform;
    SBoundingBox             m_boundingBox;
    std::vector<glm::vec3>   m_vertices;
    std::vector<glm::vec3>   m_normals;
    std::vector<atUint32>    m_colors;
    std::vector<glm::vec2>   m_texCoords;
    std::vector<glm::vec2>   m_lightmapCoords;
    std::vector<CMesh>       m_meshes;

    // GL specific code
    bool                           m_ibosIndexed;
    CVertexBuffer                  m_vertexBuffer;
    std::unordered_map<atUint32, CIndexBuffer> m_transparents;
    std::unordered_map<atUint32, CIndexBuffer> m_opaques;
};

#endif // MODEL_HPP
