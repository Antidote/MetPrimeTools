#ifndef CWORLDFILEREADER_HPP
#define CWORLDFILEREADER_HPP

#include "core/CResourceManager.hpp"

#include <Athena/MemoryReader.hpp>

class CWorldFile;
class CWorldFileReader final : protected Athena::io::MemoryReader
{
    MEMORYREADER_BASE();
    DEFINE_RESOURCE_LOADER();
public:
    CWorldFileReader(const atUint8* data, atUint64 length);
    CWorldFileReader(const std::string& filename);
    virtual ~CWorldFileReader();

    CWorldFile* read();

    static IResource* loadByData(const atUint8* data, atUint64 length);
};

#endif // CWORLDFILEREADER_HPP
