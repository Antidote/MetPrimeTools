#include "core/GLInclude.hpp"
#include "models/CMapArea.hpp"
#include "core/CMaterialCache.hpp"
#include "core/CMaterialSet.hpp"

#include "core/GXCommon.hpp"

CMapArea::CMapArea()
    : m_color(CColor(.5, .5, .5)),
      m_vboBuilt(false)
{
    m_vertices.clear();
}

CMapArea::~CMapArea()
{
}

void CMapArea::setTransform(const CTransform& transform)
{
    m_transform = transform;
}

void CMapArea::setColor(const CColor& color)
{
    m_color = color;
}

void CMapArea::draw()
{
    if (!m_vboBuilt)
        buildVbo();

    glDisable(GL_PRIMITIVE_RESTART);
    //glPrimitiveRestartIndex((atUint16)0xFFFF);

    CMaterial& mat = CMaterialCache::instance()->material(m_materialID);
    if (!mat.bind())
        return;


    mat.setModel(m_transform);

    mat.setKonstColor(0, m_color);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

    for (const SMapAreaDetail& detail : m_details)
    {
        for (const SMapAreaPrimitive& primitive : detail.primitives)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, primitive.elementBuffer);
            glDrawElements(primitive.type, primitive.indices.size(), GL_UNSIGNED_SHORT, (void*)0);
        }
    }

    //mat.setKonstColor(0, m_color);

    for (const SMapAreaDetail& detail : m_details)
    {
        for (const SMapBorder& border : detail.borders)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, border.elementBuffer);
            glDrawElements(GL_LINE_STRIP, border.indices.size(), GL_UNSIGNED_SHORT, (void*)0);
        }
    }

    mat.release();
    glDisableVertexAttribArray(0);
}

void CMapArea::drawBoundingBox()
{

}

SBoundingBox& CMapArea::boundingBox()
{
    return m_boundingBox;
}

void CMapArea::updateViewProjectionUniforms(const CTransform& view, const CProjection& proj)
{
    CMaterial& mat = CMaterialCache::instance()->material(m_materialID);
    mat.setView(view);
    mat.setProjection(proj);
}

void CMapArea::setCurrentMaterialSet(atUint32)
{
}

atUint32 CMapArea::currentMaterialSetIndex() const
{
    return 0;
}

CMaterialSet& CMapArea::currentMaterialSet()
{
    return (CMaterialSet&)CMaterialSet::Invalid;
}

atUint32 CMapArea::materialSetCount() const
{
    return 0;
}

void CMapArea::buildVbo()
{
    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(CVector3f), &m_vertices[0], GL_STATIC_DRAW);

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

