#ifndef MATERIALCONTAINER_HPP
#define MATERIALCONTAINER_HPP
#include "CAssetID.hpp"
#include "CMaterial.hpp"
#include <vector>

class CMaterialSet
{
public:
    CMaterialSet();
    ~CMaterialSet();

    CAssetID textureID(const atUint32& index) const;

    CMaterial& material(const atUint32& index);
    CMaterial& material(const atUint32& index) const;
    std::vector<atUint32> materials() const;
private:
    friend class CMaterialReader;

    std::vector<CAssetID> m_textureIds;
    std::vector<atUint32> m_materials;
};

#endif // MATERIALCONTAINER_HPP
