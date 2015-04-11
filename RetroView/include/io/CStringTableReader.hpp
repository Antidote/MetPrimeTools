#ifndef CSTRINGTABLEREADER_HPP
#define CSTRINGTABLEREADER_HPP

#include <Athena/MemoryReader.hpp>
#include "core/CResourceManager.hpp"

struct SLanguageInfo final
{
    atUint32 tableOffset;
    atUint32 tableLength;
    std::vector<atUint32> stringOffsets;
};

class CStringTable;
class CStringTableReader final : protected Athena::io::MemoryReader
{
    MEMORYREADER_BASE();
    DEFINE_RESOURCE_LOADER();
public:
    CStringTableReader(const atUint8* data, atUint64 length);
    CStringTableReader(const std::string& filepath);
    ~CStringTableReader();

    CStringTable* read();

    static IResource* loadByData(const atUint8* data, atUint64 length);
private:
    void readStringsMP1(CStringTable* ret);
    void readStringsMP2(CStringTable* ret);
    void readStringsMP3DKCR(CStringTable* ret);
    void readNameTable(CStringTable* ret);
    atUint32                   m_languageCount;
    atUint32                   m_stringCount;
    std::vector<SLanguageInfo> m_languageInfo;
};

#endif // CSTRINGTABLEREADER_HPP
