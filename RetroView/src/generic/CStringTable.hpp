#ifndef CSTRINGTABLE
#define CSTRINGTABLE

#include "core/IResource.hpp"

struct SLanguageEntry
{
    CFourCC ident;
    std::vector<std::string> strings;
};

struct SStringName
{
    std::string name;
    atUint32    index;
};

class CStringTable final : public IResource
{
public:
    CStringTable();
    virtual ~CStringTable();

    std::string string(const CFourCC& language = "ENGL", atUint32 index = 0);
private:
    friend class CStringTableReader;
    atUint32                    m_version;
    std::vector<SStringName>    m_stringNames;
    std::vector<SLanguageEntry> m_languages;
};

#endif // CSTRINGTABLE

