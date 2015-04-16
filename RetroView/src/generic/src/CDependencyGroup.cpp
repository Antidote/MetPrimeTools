#include "generic/CDependencyGroup.hpp"

#include <Athena/InvalidDataException.hpp>

CDependencyGroup::CDependencyGroup(Athena::io::IStreamReader& input, CUniqueID::EIDBits bits)
{
    if (bits == CUniqueID::E_Invalid)
        THROW_INVALID_DATA_EXCEPTION("AssetID bit length is invalid, expected 32, 64, or 128 bits");

    input.setEndian(Athena::Endian::BigEndian);
    atUint32 dependCount = input.readUint32();
    while ((dependCount)-- > 0)
    {
        SDependency dep;
        dep.type = CFourCC(input);
        dep.id   = CUniqueID(input, bits);
        m_dependencies.push_back(dep);
    }
}

std::vector<SDependency> CDependencyGroup::dependencies() const
{
    return m_dependencies;
}
