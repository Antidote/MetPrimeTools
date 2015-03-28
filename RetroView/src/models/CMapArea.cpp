#include <GL/glew.h>
#include "models/CMapArea.hpp"
#include "core/CMaterialCache.hpp"

#include "core/GXCommon.hpp"

CMapArea::CMapArea()
    : m_color(QColor(128, 128, 128)),
      m_vboBuilt(false)
{
}

CMapArea::~CMapArea()
{
}

void CMapArea::setTransformMatrix(const glm::mat4& transformMatrix)
{
    m_transform = transformMatrix;
}

void CMapArea::setColor(const QColor& color)
{
    m_color = color;
}

void CMapArea::draw()
{
    if (!m_vboBuilt)
        buildVbo();

    glPrimitiveRestartIndex(~0);

    CMaterial& mat = CMaterialCache::instance()->material(m_materialID);
    if (!mat.bind())
        return;


    mat.setModelMatrix(m_transform);

    mat.setKonstColor(0, m_color);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    for (const SMapAreaDetail& detail : m_details)
    {
        for (const SMapAreaPrimitive& primitive : detail.primitives)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, primitive.elementBuffer);
            glDrawElements(primitive.type, primitive.indices.size(), GL_UNSIGNED_SHORT, (void*)0);
        }
    }

    glLineWidth(2.f);
    mat.setKonstColor(0, m_color.lighter());

    for (const SMapAreaDetail& detail : m_details)
    {
        for (const SMapBorder& border : detail.borders)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, border.elementBuffer);
            glDrawElements(GL_LINE_STRIP, border.indices.size(), GL_UNSIGNED_SHORT, (void*)0);
        }
    }

    mat.release();
    glLineWidth(1.f);
    glDisableVertexAttribArray(0);
}

void CMapArea::drawBoundingBox()
{

}

SBoundingBox& CMapArea::boundingBox()
{
    return m_boundingBox;
}

void CMapArea::updateViewProjectionUniforms(const glm::mat4& view, const glm::mat4& proj)
{
    CMaterial& mat = CMaterialCache::instance()->material(m_materialID);
    mat.setViewMatrix(view);
    mat.setProjectionMatrix(proj);
}

void CMapArea::buildVbo()
{
    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec3), &m_vertices[0], GL_STATIC_DRAW);

    for (SMapAreaDetail& detail : m_details)
    {
        for (SMapAreaPrimitive& primitive : detail.primitives)
        {
            glGenBuffers(1, &primitive.elementBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, primitive.elementBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, primitive.indices.size() * sizeof(atUint16), &primitive.indices[0], GL_STATIC_DRAW);
        }

        for (SMapBorder& border : detail.borders)
        {
            glGenBuffers(1, &border.elementBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, border.elementBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, border.indices.size() * sizeof(atUint16), &border.indices[0], GL_STATIC_DRAW);
        }
    }

    m_vboBuilt = true;
}

