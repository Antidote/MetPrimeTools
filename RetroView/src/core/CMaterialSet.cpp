#include "core/CMaterialSet.hpp"
#include "core/CMaterialCache.hpp"

CMaterialSet::CMaterialSet()
{
}

CMaterialSet::~CMaterialSet()
{
}

CMaterial& CMaterialSet::material(const atUint32& index)
{
    static CMaterial invalid;
    if (index >= m_materials.size())
        return invalid;

    return CMaterialCache::instance()->material(m_materials[index]);
}

std::vector<atUint32> CMaterialSet::materials()
{
    return m_materials;
}

void CMaterialSet::setAmbient(const QColor& amb)
{
    for (atUint32 matId : m_materials)
    {
        CMaterial& mat = CMaterialCache::instance()->material(matId);
        mat.setAmbient(amb);
    }
}

