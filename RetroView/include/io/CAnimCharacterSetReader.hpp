#ifndef CANIMCHARACTERSETREADER_HPP
#define CANIMCHARACTERSETREADER_HPP

#include "core/CResourceManager.hpp"
#include <Athena/MemoryReader.hpp>

class CAnimCharacterSet;
class CAnimCharacterSetReader final : Athena::io::MemoryReader
{
    MEMORYREADER_BASE();
    DEFINE_RESOURCE_LOADER();
public:
    CAnimCharacterSetReader(const atUint8* data, atUint64 length);
    CAnimCharacterSetReader(const std::string& filepath);
    ~CAnimCharacterSetReader();

    CAnimCharacterSet* read();

    static IResource* loadByData(const atUint8* data, atUint64 length);
};

#endif // CANIMCHARACTERSETREADER_HPP
