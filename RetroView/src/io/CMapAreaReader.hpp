#ifndef CMAPAREAREADER_HPP
#define CMAPAREAREADER_HPP

#include <Athena/MemoryReader.hpp>
#include "core/CResourceManager.hpp"

class CMapArea;
class CMapAreaReader final : protected Athena::io::MemoryReader
{
    MEMORYREADER_BASE();
    DEFINE_RESOURCE_LOADER();
public:
    CMapAreaReader(const atUint8* data, atUint64 length);
    CMapAreaReader(const std::string& filepath);

    CMapArea* read();
    static IResource* loadByData(const atUint8* data, atUint64 length);
    virtual ~CMapAreaReader();
};

#endif // CMAPAREAREADER_HPP
