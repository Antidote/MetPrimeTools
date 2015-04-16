#include "core/CMaterialSet.hpp"
#include "core/CMaterialCache.hpp"
#include <fstream>

const CMaterialSet CMaterialSet::Invalid;

CMaterialSet::CMaterialSet()
{
}

CMaterialSet::~CMaterialSet()
{
}

CMaterial& CMaterialSet::material(const atUint32& index) const
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

void CMaterialSet::dumpSources(const CUniqueID& id)
{
    atUint32 i = 0;
    for (atUint32 matId : m_materials)
    {
        std::ofstream fragment(Athena::utility::sprintf("fs_%s_%i.txt", id.toString().c_str(), i));
        CMaterial& mat = CMaterialCache::instance()->material(matId);
        fragment << mat.fragmentSource().toStdString();
        fragment.flush();
        std::ofstream vertex(Athena::utility::sprintf("vs_%s_%i.txt", id.toString().c_str(), i));
        vertex << mat.vertexSource().toStdString();
        vertex.flush();
        i++;
    }
}
