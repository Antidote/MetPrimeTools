#include "core/CMesh.hpp"

CMesh::CMesh()
{
}

CMesh::~CMesh()
{
}

void CMesh::setMantissa(atUint16 mantissa)
{
    m_mantissa = mantissa;
}

atUint16 CMesh::mantissa() const
{
    return m_mantissa;
}

SBoundingBox CMesh::boundingBox() const
{
    return m_boundingBox;
}

