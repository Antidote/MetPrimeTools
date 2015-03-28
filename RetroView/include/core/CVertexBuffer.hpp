#ifndef CVERTEXBUFFER_HPP
#define CVERTEXBUFFER_HPP

#include "core/SVertex.hpp"
#include <vector>

class CVertexBuffer
{
public:
    CVertexBuffer();
    ~CVertexBuffer();

    atUint32 addVertex(const SVertex& vert);
    atUint32 addVertexIfUnique(const SVertex& vert, atUint32 start);

    void initBuffer();

    void bind();
    void release();

    void reserve();
    void clear();
    bool isBuffered() const;
    atUint32 size() const;
private:
    atUint32 m_attribBuffer;
    atUint32 m_vao;
    bool m_buffered;
    std::vector<SVertex> m_vertices;
};

#endif // CVERTEXBUFFER_HPP
