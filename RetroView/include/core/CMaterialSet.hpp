#ifndef MATERIALCONTAINER_HPP
#define MATERIALCONTAINER_HPP
#include "CAssetID.hpp"
#include "CMaterial.hpp"
#include <vector>

class CMaterialSet final
{
public:
    CMaterialSet();
    virtual ~CMaterialSet();

    CMaterial& material(const atUint32& index);
    std::vector<atUint32> materials();

    void setAmbient(const QColor& amb);
private:
    friend class CMaterialReader;
    friend struct SPASSCommand;

    std::vector<atUint32> m_materials;
};

#endif // MATERIALCONTAINER_HPP
