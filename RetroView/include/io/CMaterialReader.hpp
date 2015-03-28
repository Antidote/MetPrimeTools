#ifndef MATERIALREADER_HPP
#define MATERIALREADER_HPP

#include "core/CMaterialSet.hpp"
#include "core/CMaterialSection.hpp"

#include <Athena/MemoryReader.hpp>


class CMaterialReader final : protected Athena::io::MemoryReader
{
    MEMORYREADER_BASE();
public:
    CMaterialReader(const atUint8* data, atUint64 length);
    CMaterialReader(const std::string&) = delete;
    virtual ~CMaterialReader();

    CMaterialSet read(CMaterial::Version version);
private:
    bool readMaterialCommand(CMaterialSet& set, CMaterial& material);
    std::vector<CAssetID> m_textures;
};

#endif // MATERIALREADER_HPP
