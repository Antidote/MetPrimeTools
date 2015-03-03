#include "core/CIndexBuffer.hpp"
#include <GL/gl.h>

CIndexBuffer::CIndexBuffer(atUint32 materialId)
    : m_materialId(materialId)
{

}

CIndexBuffer::~CIndexBuffer()
{

}

void CIndexBuffer::addIndices(atUint32 type, std::vector<atUint16> indices)
{
    if (type == GL_QUADS)
        quadsToStrips(&indices.front(), indices.size());
    else if (type == GL_TRIANGLES)
        trianglesToStrips(&indices.front(), indices.size());
    else if (type == GL_TRIANGLE_FAN)
        fansToStrips(&indices.front(), indices.size());
    else
        m_indices.insert(m_indices.end(), indices.begin(), indices.end());
}

void CIndexBuffer::trianglesToStrips(atUint16* indices, atUint16 count)
{
    m_indices.reserve(count + (count / 3));

    for (atUint16 i = 0; i < count; i+=3)
    {
        m_indices.push_back(*indices++);
        m_indices.push_back(*indices++);
        m_indices.push_back(*indices++);
        m_indices.push_back(0xFFFF);
    }
}

void CIndexBuffer::fansToStrips(atUint16* indices, atUint16 count)
{
    m_indices.push_back(count);
    atUint16 firstIndex = *indices;

    for (atUint16 i = 2; i < count; i+=3)
    {
        m_indices.push_back(indices[i - 1]);
        m_indices.push_back(indices[i]);
        m_indices.push_back(firstIndex);

        if (i + 1 < count)
            m_indices.push_back(indices[i + 1]);
        if (i + 2 < count)
            m_indices.push_back(indices[i + 2]);

        m_indices.push_back(0xFFFF);
    }
}

void CIndexBuffer::quadsToStrips(atUint16* indices, atUint16 count)
{
    m_indices.reserve(count * 1.25);

    atUint16 i = 3;
    for (; i < count; i += 4)
    {
        m_indices.push_back(indices[i - 2]);
        m_indices.push_back(indices[i - 1]);
        m_indices.push_back(indices[i - 3]);
        m_indices.push_back(indices[i]);
        m_indices.push_back(0xFFFF);
    }

    if (i == count)
    {
        m_indices.push_back(indices[i - 3]);
        m_indices.push_back(indices[i - 2]);
        m_indices.push_back(indices[i - 1]);
        m_indices.push_back(0xFFFF);
    }
}

atUint32 CIndexBuffer::materialId()
{
    return m_materialId;
}

