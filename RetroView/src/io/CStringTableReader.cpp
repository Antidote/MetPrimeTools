#include "io/CStringTableReader.hpp"
#include "generic/CStringTable.hpp"

#include <RetroCommon.hpp>
#include <Athena/MemoryWriter.hpp>
#include <Athena/InvalidDataException.hpp>

CStringTableReader::CStringTableReader(const atUint8* data, atUint64 length)
    : base(data, length)
{
    base::setEndian(Athena::Endian::BigEndian);
}

CStringTableReader::CStringTableReader(const std::string& filepath)
    : base(filepath)
{
    base::setEndian(Athena::Endian::BigEndian);
}

CStringTableReader::~CStringTableReader()
{
}

CStringTable* CStringTableReader::read()
{
    CStringTable* ret = nullptr;
    try
    {
        atUint32 magic = base::readUint32();
        if (magic != 0x87654321)
        {
            // not an STRG file? Try to decompress
            Athena::io::MemoryWriter tmp;
            decompressFile(tmp, base::data(), base::length());

            if (tmp.length() > 0)
                base::setData(tmp.data(), tmp.length());

            magic = base::readUint32();
        }

        // check again
        if (magic != 0x87654321) // yes, really
            THROW_INVALID_DATA_EXCEPTION("Invalid StringTable magic, expected 0x87654321 got 0x%.8X", magic);

        atUint32 version = base::readUint32();

        if (version != 0 && version != 1 && version != 3)
            THROW_INVALID_DATA_EXCEPTION("Unsupported StringTable version, only Metroid Prime 1 - DKCR are supported");

        ret = new CStringTable;
        ret->m_version = version;

        m_languageCount = base::readUint32();
        m_stringCount   = base::readUint32();

        ret->m_languages.resize(m_languageCount);
        m_languageInfo.resize(m_languageCount);

        switch(version)
        {
            case 0:
                readStringsMP1(ret);
                break;
            case 1:
                readStringsMP2(ret);
                break;
            case 3:
                readStringsMP3DKCR(ret);
                break;
        }
    }
    catch(...)
    {
        delete ret;
        ret = nullptr;
        throw;
    }

    return ret;
}

IResource* CStringTableReader::loadByData(const atUint8* data, atUint64 length)
{
    return CStringTableReader(data, length).read();
}

void CStringTableReader::readStringsMP1(CStringTable* ret)
{
    for (atUint32 i = 0; i < m_languageCount; i++)
    {
        SLanguageEntry& lang = ret->m_languages.at(i);
        SLanguageInfo& info = m_languageInfo.at(i);
        lang.ident = CFourCC(*this);
        info.tableOffset = base::readUint32();
    }

    atUint64 poolStart = base::position();
    for (atUint32 i = 0; i < m_languageCount; i++)
    {
        SLanguageEntry& lang = ret->m_languages.at(i);
        SLanguageInfo& info = m_languageInfo.at(i);

        base::seek(poolStart + info.tableOffset, Athena::SeekOrigin::Begin);
        info.tableLength = base::readUint32();
        atUint64 tableOffset = base::position();
        for (atUint32 s = 0; s < m_stringCount; s++)
            info.stringOffsets.push_back(base::readUint32());

        for (atUint32 s = 0; s < m_stringCount; s++)
        {
            base::seek(tableOffset + info.stringOffsets.at(s), Athena::SeekOrigin::Begin);
            lang.strings.push_back(base::readUnicode());
        }
    }
}

void CStringTableReader::readStringsMP2(CStringTable* ret)
{
    for (atUint32 i = 0; i < m_languageCount; i++)
    {
        SLanguageEntry& lang = ret->m_languages.at(i);
        SLanguageInfo& info = m_languageInfo.at(i);
        lang.ident = CFourCC(*this);
        info.tableOffset = base::readUint32();
        info.tableLength = base::readUint32();
    }

    readNameTable(ret);

    atUint64 poolStart = base::position();
    for (atUint32 i = 0; i < m_languageCount; i++)
    {
        SLanguageEntry& lang = ret->m_languages.at(i);
        SLanguageInfo& info = m_languageInfo.at(i);

        base::seek(poolStart + info.tableOffset, Athena::SeekOrigin::Begin);

        atUint64 tableOffset = base::position();
        for (atUint32 s = 0; s < m_stringCount; s++)
            info.stringOffsets.push_back(base::readUint32());

        for (atUint32 s = 0; s < m_stringCount; s++)
        {
            base::seek(tableOffset + info.stringOffsets.at(s), Athena::SeekOrigin::Begin);
            lang.strings.push_back(base::readUnicode());
        }
    }
}

void CStringTableReader::readStringsMP3DKCR(CStringTable* ret)
{
    readNameTable(ret);
    for (atUint32 i = 0; i < m_languageCount; i++)
    {
        SLanguageEntry& lang = ret->m_languages.at(i);
        lang.ident = CFourCC(*this);

    }

    for (atUint32 i = 0; i < m_languageCount; i++)
    {
        SLanguageInfo& info = m_languageInfo.at(i);
        info.tableLength = base::readUint32();
        for (atUint32 s = 0; s < m_stringCount; s++)
            info.stringOffsets.push_back(base::readUint32());
    }

    atUint32 tableOffset = base::position();
    for (atUint32 i = 0; i < m_languageCount; i++)
    {
        SLanguageEntry& lang = ret->m_languages.at(i);
        SLanguageInfo& info  = m_languageInfo.at(i);

        for (atUint32 s = 0; s < m_stringCount; s++)
        {
            base::seek(tableOffset + info.stringOffsets.at(s), Athena::SeekOrigin::Begin);
            atUint32 strLen = base::readUint32();
            lang.strings.push_back(base::readString(strLen));
        }
    }
}

void CStringTableReader::readNameTable(CStringTable* ret)
{
    atUint32 stringNameCount = base::readUint32();
    atUint32 tableLen = base::readUint32();

    if (stringNameCount == 0 || tableLen == 0)
        return;

    atUint8* data = base::readUBytes(tableLen);
    Athena::io::MemoryReader reader(data, tableLen);
    reader.setEndian(Athena::Endian::BigEndian);

    while ((stringNameCount--) > 0)
    {
        SStringName name;
        atUint32 nameOffset = reader.readUint32();
        name.index = reader.readUint32();
        atUint64 oldPos = reader.position();
        reader.seek(nameOffset, Athena::SeekOrigin::Begin);
        name.name = reader.readString();
        reader.seek(oldPos, Athena::SeekOrigin::Begin);
        ret->m_stringNames.push_back(name);
    }
}

REGISTER_RESOURCE_LOADER(CStringTableReader, "strg", loadByData);
