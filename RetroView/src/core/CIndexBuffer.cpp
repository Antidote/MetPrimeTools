#include <GL/glew.h>
#include "core/GLInclude.h"
#include "core/CIndexBuffer.hpp"
#include <stdexcept>

CIndexBuffer::CIndexBuffer()
    : m_buffered(false)
{
}

CIndexBuffer::~CIndexBuffer()
{
    if (m_buffered)
    {
        glDeleteBuffers(1, &m_iboId);
    }
    m_indices.clear();
}

void CIndexBuffer::addIndices(EPrimitive type, std::vector<atUint32> indices)
{
    if (m_indices.size() >= 0xFFFFFFFF)
        throw std::overflow_error("UBO contains too many indices");

    if (type == EPrimitive::Quads)
        quadsToStrips(&indices.front(), indices.size());
    else if (type == EPrimitive::Triangles)
        trianglesToStrips(&indices.front(), indices.size());
    else if (type == EPrimitive::TriangleFan)
        fansToStrips(&indices.front(), indices.size());
    else
    {
        m_indices.insert(m_indices.end(), indices.begin(), indices.end());
        m_indices.push_back(0xFFFFFFFF);
    }
}

void CIndexBuffer::initBuffer()
{
    if (m_buffered)
        return;

    m_buffered = true;
    glGenBuffers(1, &m_iboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(atUint32), &m_indices[0], GL_STATIC_DRAW);
}

void CIndexBuffer::bindBuffer()
{
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(0xFFFFFFFF);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboId);
}

void CIndexBuffer::drawBuffer()
{
    glDrawElements(GL_TRIANGLE_STRIP, m_indices.size(), GL_UNSIGNED_INT, (void*)0);
}

void CIndexBuffer::trianglesToStrips(atUint32* indices, atUint32 count)
{
    m_indices.reserve(count + (count / 3));

    for (atUint32 i = 0; i < count; i +=3)
    {
        m_indices.push_back(*indices++);
        m_indices.push_back(*indices++);
        m_indices.push_back(*indices++);
        m_indices.push_back(0xFFFFFFFF);
    }
}

void CIndexBuffer::fansToStrips(atUint32* indices, atUint32 count)
{
    m_indices.reserve(count);
    atUint32 firstIndex = *indices;

    for (atUint32 i = 2; i < count; i+=3)
    {
        m_indices.push_back(indices[i - 1]);
        m_indices.push_back(indices[i]);
        m_indices.push_back(firstIndex);

        if (i + 1 < count)
            m_indices.push_back(indices[i + 1]);
        if (i + 2 < count)
            m_indices.push_back(indices[i + 2]);

        m_indices.push_back(0xFFFFFFFF);
    }
}

void CIndexBuffer::quadsToStrips(atUint32* indices, atUint32 count)
{
    m_indices.reserve(count * 1.25);

    atUint32 i = 3;
    for (; i < count; i += 4)
    {
        m_indices.push_back(indices[i - 2]);
        m_indices.push_back(indices[i - 1]);
        m_indices.push_back(indices[i - 3]);
        m_indices.push_back(indices[i]);
        m_indices.push_back(0xFFFFFFFF);
    }

    if (i == count)
    {
        m_indices.push_back(indices[i - 3]);
        m_indices.push_back(indices[i - 2]);
        m_indices.push_back(indices[i - 1]);
        m_indices.push_back(0xFFFFFFFF);
    }
}
