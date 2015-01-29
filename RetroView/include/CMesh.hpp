#ifndef MESH_HPP
#define MESH_HPP

#include <Athena/Types.hpp>
#include <vector>
#include <glm/glm.hpp>
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
class BinaryReader;
}
}

class CMaterial;
class CMesh
{
public:
    CMesh();
    ~CMesh();

    void setMantissa(atUint16 mantissa);
    atUint16 mantissa() const;

    std::vector<CPrimitive>& primitives();
    std::vector<CPrimitive>& primitives() const;
private:
    friend void readPrimitives(CMesh& mesh, const CMaterial& material, atUint16 dataSize, Athena::io::BinaryReader& reader);
    friend class CModelData;
    friend class CAreaFile;
    friend class CAreaReader;
    friend class CModelReader;

    glm::vec3              m_pivot;
    atUint32               m_materialID;
    atUint16               m_mantissa;
    std::vector<CPrimitive> m_primitives;
};

#endif // MESH_HPP
