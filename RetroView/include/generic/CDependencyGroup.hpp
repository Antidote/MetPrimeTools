#ifndef CDEPENDENCYGROUP_HPP
#define CDEPENDENCYGROUP_HPP

#include "CUniqueID.hpp"
#include "CFourCC.hpp"
#include <Athena/IStreamReader.hpp>

struct SDependency final
{
    CFourCC  type;
    CUniqueID id;
};

class CDependencyGroup final
{
public:
    CDependencyGroup() = default;
    CDependencyGroup(Athena::io::IStreamReader& input, CUniqueID::EIDBits bits);

    std::vector<SDependency> dependencies() const;
private:
    std::vector<SDependency> m_dependencies;
};

#endif // CDEPENDENCYGROUP_HPP

