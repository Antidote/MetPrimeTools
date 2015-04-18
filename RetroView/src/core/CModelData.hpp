#ifndef MODEL_HPP
#define MODEL_HPP

#include "CMaterialSet.hpp"
#include "CMesh.hpp"
#include "core/CIndexBuffer.hpp"
#include "core/SBoundingBox.hpp"
#include "core/SVertex.hpp"

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

class CModelData
{
public:
    CModelData();
    virtual ~CModelData();

    void exportModel(std::ofstream& of, atUint32& vertexOff, atUint32& normalOff, atUint32& texOff, CMaterialSet& ms);

    void drawBoundingBoxes();
    
    inline const SBoundingBox& getBoundingBox() const {return m_boundingBox;}
    void drawIbos(bool transparents, const CMaterialSet& materialSet, const CTransform& model);
    void drawTransparentBoxes();

protected:
    friend class CAreaFile;
    friend class CAreaReader;
    friend class CModelReader;
    friend class CMaterialViewer;
    friend void readPrimitives(CMesh&, CModelData&, const CMaterial&, Athena::io::IStreamReader&);

    atUint32 exportUVIdx(atUint32 texOff, SVertexDescriptor desc, CMaterial& mat, CMesh& mesh);
    atUint32 getIbo(atUint32 prim, atUint32 matId, atUint32 start);
    void indexVert(CMaterial& material, SVertexDescriptor& desc, CMesh& mesh, atUint32 iboId, atUint32 vertStartIndex);
    void loadIbos(CMaterialSet& ms);

    atUint32                 m_format;
    CTransform               m_transform;
    SBoundingBox             m_boundingBox;
    std::vector<CVector3f>   m_vertices;
    std::vector<CVector3f>   m_normals;
    std::vector<atUint32>    m_colors;
    std::vector<CVector3f>   m_texCoords0;
    std::vector<CVector3f>   m_texCoords1;
    std::vector<CMesh>       m_meshes;

    // GL specific code
    CVertexBuffer                              m_vertexBuffer;
    std::unordered_map<atUint32, CIndexBuffer> m_transparents;
    std::unordered_map<atUint32, CIndexBuffer> m_opaques;
};

#endif // MODEL_HPP
