#ifndef MESH_HPP
#define MESH_HPP

#include <Athena/Types.hpp>
#include <vector>
#include <glm/glm.hpp>
#include "SBoundingBox.hpp"
#include "GXTypes.hpp"

struct VertexDescriptor
{
    atUint16 position;
    atUint16 normal;
    atUint16 clr[2];
    atUint16 texCoord[8];
    atUint16 unkIndex[4];
};

struct CPrimitive final
{
    EPrimitive primitive;
    atUint32    vertexFormatIdx;
    std::vector<VertexDescriptor> indices;
};

namespace Athena
{
namespace io
{
class IStreamReader;
}
}

class CMaterial;
class CMesh final
{
public:
    CMesh();
    virtual ~CMesh();

    void setMantissa(atUint16 mantissa);
    atUint16 mantissa() const;

    SBoundingBox boundingBox() const;

    std::vector<CPrimitive>& primitives();
    std::vector<CPrimitive>& primitives() const;
private:
    friend void readPrimitives(CMesh& mesh, const CMaterial& material, atUint16 dataSize, Athena::io::IStreamReader& reader);
    friend class CModelData;
    friend class CAreaFile;
    friend class CAreaReader;
    friend class CModelReader;

    SBoundingBox           m_boundingBox;
    glm::vec3              m_pivot;
    atUint32               m_materialID;
    atUint16               m_mantissa;
    atUint16               m_uvSource;
    std::vector<CPrimitive> m_primitives;
};

#endif // MESH_HPP
