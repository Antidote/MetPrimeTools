#include "core/GLInclude.hpp"
#include "core/CVertexBuffer.hpp"
#include <iostream>
#include <stdexcept>
#include <algorithm>

CVertexBuffer::CVertexBuffer()
    : m_buffered(false)
{
}

CVertexBuffer::~CVertexBuffer()
{
    if (m_buffered)
    {
        release();
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_attribBuffer);
    }
    m_vertices.clear();
}

atUint32 CVertexBuffer::addVertex(const SVertex& vert)
{
    if (m_vertices.size() >= 0xFFFFFFFF)
        throw std::overflow_error("VBO contains too many vertices");

    m_vertices.push_back(vert);
    return m_vertices.size() - 1;
}

atUint32 CVertexBuffer::addVertexIfUnique(const SVertex& vert, atUint32 start)
{
    if (start < m_vertices.size())
    {
        auto it = std::find(m_vertices.begin() + start, m_vertices.end(), vert);
        if (it != m_vertices.end())
            return it - m_vertices.begin();
    }

    return addVertex(vert);
}

void CVertexBuffer::initBuffer()
{
    if (m_buffered)
        return;

    m_buffered = true;

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_attribBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_attribBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(SVertex), m_vertices.data(), GL_STATIC_DRAW);

    // assign positions and normals
    for (atUint32 i = 0; i < 2; i++)
    {
        glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex), (void*)(i * sizeof(CVector3f)));
        glEnableVertexAttribArray(i);
    }

    // colors
    for (atUint32 i = 0; i < 2; i++)
    {
        glVertexAttribPointer(i + 2, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(SVertex),
                              (void*)(i * sizeof(atUint32) + offsetof(SVertex, color)));
        glEnableVertexAttribArray(i + 2);
    }

    // uvs
    for (atUint32 i = 0; i < 8; i++)
    {
        glVertexAttribPointer(i + 4, 2, GL_FLOAT, GL_FALSE, sizeof(SVertex),
                              (void*)(i * sizeof(CVector3f) + offsetof(SVertex, texCoords)));
        glEnableVertexAttribArray(i + 4);
    }

    glBindVertexArray(0);
}

void CVertexBuffer::bind()
{
    glBindVertexArray(m_vao);
}

void CVertexBuffer::release()
{
    glBindVertexArray(0);
}

bool CVertexBuffer::isBuffered() const
{
    return m_buffered;
}

atUint32 CVertexBuffer::size() const
{
    return m_vertices.size();
}

