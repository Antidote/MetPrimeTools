#ifndef CDEPENDENCYGROUP_HPP
#define CDEPENDENCYGROUP_HPP

#include "CAssetID.hpp"
#include "CFourCC.hpp"
#include <Athena/IStreamReader.hpp>

struct SDependency final
{
    CFourCC  type;
    CAssetID id;
};

class CDependencyGroup final
{
public:
    CDependencyGroup() = default;
    CDependencyGroup(Athena::io::IStreamReader& input, CAssetID::EIDBits bits);

    std::vector<SDependency> dependencies() const;
private:
    std::vector<SDependency> m_dependencies;
};

#endif // CDEPENDENCYGROUP_HPP

