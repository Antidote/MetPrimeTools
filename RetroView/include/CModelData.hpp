#ifndef MODEL_HPP
#define MODEL_HPP

#include "CMaterialSet.hpp"
#include "CMesh.hpp"
#include "CIndexBuffer.hpp"
#include "SBoundingBox.hpp"

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

struct SVertex
{
    glm::vec3 pos;
    glm::vec3 norm;
    atUint32  color[2];
    glm::vec2 texCoords[7];

    SVertex()
    {
        memset(color, 0, sizeof(color));
    }

    bool operator ==(const SVertex& that) const
    {
        return !memcmp((void*)this, (void*)&that, sizeof(SVertex));
    }
};

class CModelData
{
public:
    CModelData();
    ~CModelData();

    void exportModel(std::ofstream& of, atUint32& vertexOff, atUint32& normalOff, atUint32& texOff, CMaterialSet& ms);

    void preDraw(CMaterialSet& materialSet);
    void drawBoundingBoxes();

    void indexIBOs(CMaterialSet& materialSet);

    void drawIbos(bool transparents, CMaterialSet& materialSet, glm::mat4 model);
    void drawTransparentBoxes();
    void doneDraw();
protected:
    friend class CAreaFile;
    friend class CAreaReader;
    friend class CModelReader;
    friend class CMaterialViewer;

    atUint32 exportUVIdx(atUint32 texOff, VertexDescriptor desc, CMaterial& mat);
    atUint32 getIbo(atUint32 prim, atUint32 matId, atUint32 start);
    void indexVert(CMaterial& material, VertexDescriptor& desc, CMesh& mesh, atUint32 iboId, atUint32 vertStartIndex);
    void loadIbos(CMaterialSet& ms);

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
    bool                           m_ibosLoaded;
    atUint32                       m_vertexObj;
    atUint32                       m_normalObj;
    atUint32                       m_clrObjs[2];
    atUint32                       m_texCoordObjs[7];

    // Buffers
    std::vector<glm::vec3> m_posBuf;
    std::vector<glm::vec3> m_normBuf;
    std::vector<atUint32>  m_clrBufs[2];
    std::vector<glm::vec2> m_texCoordBuf[7];
    std::unordered_map<atUint32, std::vector<SIndexBufferObject> > m_transparents;
    std::unordered_map<atUint32, std::vector<SIndexBufferObject> > m_opaques;
    std::vector<SIndexBufferObject> m_ibos;
    std::vector<SVertex>            m_vertexBuffer;
};

#endif // MODEL_HPP
