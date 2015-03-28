#include "generic/CStringTable.hpp"

CStringTable::CStringTable()
{

}

CStringTable::~CStringTable()
{

}

std::string CStringTable::string(const CFourCC& language, atUint32 index)
{
    std::vector<SLanguageEntry>::iterator iter = std::find_if(m_languages.begin(), m_languages.end(),
                                                            [&language](const SLanguageEntry l)->bool{return l.ident == language;});
    try
    {
        if (iter != m_languages.end())
            return ((SLanguageEntry)(*iter)).strings.at(index);
    }
    catch(...)
    {
    }

    return std::string();
}

