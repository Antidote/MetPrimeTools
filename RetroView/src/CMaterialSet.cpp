#include "CMaterialSet.hpp"
#include "CMaterialCache.hpp"

CMaterialSet::CMaterialSet()
{
}

CMaterialSet::~CMaterialSet()
{
}

CAssetID CMaterialSet::textureID(const atUint32& index) const
{
    if (index >= m_textureIds.size())
        return CAssetID::InvalidAsset;

    return m_textureIds[index];
}

CMaterial& CMaterialSet::material(const atUint32& index)
{
    static CMaterial invalid;
    if (index >= m_materials.size())
        return invalid;

    return CMaterialCache::instance()->material(m_materials[index]);
}

CMaterial& CMaterialSet::material(const atUint32& index) const
{
    return material(index);
}

std::vector<atUint32> CMaterialSet::materials() const
{
    return m_materials;
}

