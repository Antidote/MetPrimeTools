#include "CMaterial.hpp"

CMaterial::CMaterial()
    : m_isValid(false),
      m_materialFlags(0)
{

}

CMaterial::~CMaterial()
{

}

bool CMaterial::isValid() const
{
    return m_isValid;
}

atUint32 CMaterial::materialFlags() const
{
    return m_materialFlags;
}

atUint32 CMaterial::vertexAttributes() const
{
    return m_vertexAttributes;
}

std::vector<atUint32> CMaterial::textureIndices() const
{
    return m_textureIndices;
}

bool CMaterial::hasAttribute(atUint32 index)
{
    atUint32 tmp = ((m_vertexAttributes >> (index << 1)) & 3);
    return (tmp == 2 || tmp == 3);
}

bool CMaterial::hasPosition()
{
    return hasAttribute(0);
}

bool CMaterial::hasNormal()
{
    return hasAttribute(1);
}

bool CMaterial::hasColor(atUint8 slot)
{
    if (slot >= 2)
        return false;

    return hasAttribute(2 + slot);
}

bool CMaterial::hasUV(atUint8 slot)
{
    if (slot >= 7)
        return false;

    return hasAttribute(4 + slot);
}

