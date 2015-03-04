#ifndef CMAPUNIVERSEREADER_HPP
#define CMAPUNIVERSEREADER_HPP

#include "core/CResourceManager.hpp"

#include <Athena/MemoryReader.hpp>

class CMapUniverse;
class CMapUniverseReader final : protected Athena::io::MemoryReader
{
     MEMORYREADER_BASE();
     DEFINE_RESOURCE_LOADER();
public:
    CMapUniverseReader(const atUint8* data, atUint64 length);
    CMapUniverseReader(const std::string& filepath);

    virtual ~CMapUniverseReader();

    CMapUniverse* read();
    static IResource* loadByData(const atUint8* data, atUint64 length);
};

#endif // CMAPUNIVERSEREADER_HPP
