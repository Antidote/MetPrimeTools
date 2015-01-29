#ifndef MATERIALCACHE_HPP
#define MATERIALCACHE_HPP


#include <unordered_map>
#include <Athena/Types.hpp>

class CMaterial;
class CMaterialCache
{
public:
    typedef std::unordered_map<atUint32, CMaterial*>::iterator       MaterialIterator;
    typedef std::unordered_map<atUint32, CMaterial*>::const_iterator ConstMaterialIterator;
    CMaterialCache();
    ~CMaterialCache();


    void addMaterial(const CMaterial* mat);
private:
    std::unordered_map<atUint32, CMaterial*> m_cachedMaterials;
};

#endif // MATERIALCACHE_HPP
