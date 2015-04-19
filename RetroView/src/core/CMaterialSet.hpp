#ifndef MATERIALCONTAINER_HPP
#define MATERIALCONTAINER_HPP
#include "CUniqueID.hpp"
#include "CMaterial.hpp"
#include <vector>

class CMaterialSet final
{
public:
    static const CMaterialSet Invalid;
    CMaterialSet();
    virtual ~CMaterialSet();

    CMaterial& material(const atUint32& index) const;
    std::vector<atUint32> materials();

    void setAmbient(const CColor& amb);

    void dumpSources(const CUniqueID& id);

private:
    friend class CMaterialReader;
    friend struct SPASSCommand;

    std::vector<atUint32> m_materials;
};

#endif // MATERIALCONTAINER_HPP
