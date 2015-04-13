#include "CPakFileReader.hpp"
#include "CPakFile.hpp"
#include <Athena/InvalidDataException.hpp>
#include <memory.h>
#include <cstring>

struct SMP1_2_Resource
{
    atUint32 compressed;
    char tag[4];
    atUint32 id;
    atUint32 size;
    atUint32 offset;
};

struct SSectionEntry
{
    char tag[4];
    atUint32 dataLen;
};

CPakFileReader::CPakFileReader(const std::string &filename)
    : base(filename)
{
    base::setEndian(Athena::Endian::BigEndian);
}

CPakFileReader::~CPakFileReader()
{
}

CPakFile* CPakFileReader::read()
{
    atUint32 version = base::readUint32();

    if (version != EPakVersion::MetroidPrime1_2 && version != EPakVersion::MetroidPrime3)
        THROW_INVALID_DATA_EXCEPTION("Invalid or unsupported version: 0x%.8X", version);

    CPakFile* ret = nullptr;
    try
    {
        ret = new CPakFile(m_filename, version);
        switch(version)
        {
            case EPakVersion::MetroidPrime1_2:
            {
                base::readUint32(); // unknown;
                loadNameTable(ret);
                loadResourceTable(ret);
            }
                break;
            case EPakVersion::MetroidPrime3:
            {
                atUint32 headerLen = base::readUint32();
                base::seek(headerLen, Athena::SeekOrigin::Begin);

                std::vector<SSectionEntry> sections = loadSectionTable();

                for (SSectionEntry sectionEntry : sections)
                {
                    base::seek((base::position() + 0x3F) & ~0x3F, Athena::SeekOrigin::Begin);
                    if (!strncmp(sectionEntry.tag, "STRG", 4))
                        loadNameTable(ret);
                    else if (!strncmp(sectionEntry.tag, "RSHD", 4))
                        loadResourceTable(ret);
                }

                base::seek((base::position() + 0x3F) & ~0x3F, Athena::SeekOrigin::Begin);
                ret->m_dataStart = base::position();
            }
                break;
        }
        ret->m_isWorldPak = ret->resourcesByType("MLVL").size() > 0 && ret->resourcesByType("MREA").size() > 0;
    }
    catch(...)
    {
        delete ret;
        throw;
    }

    return ret;
}

void CPakFileReader::loadNameTable(CPakFile* ret)
{
    atUint32 namedResourceCount = base::readUint32();
    if (ret->m_version == EPakVersion::MetroidPrime1_2)
    {
        while ((namedResourceCount--) > 0)
        {
            SPakNamedResource res;
            res.tag = CFourCC(*this);
            base::readUint32();
            atUint32 nameLength = base::readUint32();
            base::seek(-8);
            if (nameLength > 1024) // While the engine doesn't cap it to 1024, it's a reasonable to assume nobody will be using names this long
            {
                ret->m_version = EPakVersion::MetroidPrime3Beta;
                res.id = CUniqueID(*this, CUniqueID::E_64Bits);
            }
            else
            {
                res.id = CUniqueID(*this, CUniqueID::E_32Bits);
            }

            nameLength = base::readUint32();
            res.name = base::readString(nameLength);
            res.name = std::string(res.name);

            ret->m_namedResources.push_back(res);
        }
    }
    else
    {
        while ((namedResourceCount--) > 0)
        {
            SPakNamedResource name;
            name.name = base::readString();
            name.tag = CFourCC(*this);
            name.id = CUniqueID(*this, CUniqueID::E_64Bits);
            ret->m_namedResources.push_back(name);
        }
    }
}

CPakFile* CPakFileReader::load(const std::string& filename)
{
    return CPakFileReader(filename).read();
}

bool CPakFileReader::canLoad(const std::string& filename)
{
    try
    {
        Athena::io::FileReader reader(filename);
        reader.setEndian(Athena::Endian::BigEndian);

        atUint32 magic = reader.readUint32();

        return (magic == EPakVersion::MetroidPrime1_2 || magic == EPakVersion::MetroidPrime3);
    }
    catch(...)
    {
        return false;
    }
}

void CPakFileReader::loadResourceTable(CPakFile* ret)
{
    atUint32 resourceCount = base::readUint32();
    ret->m_resources.resize(resourceCount);
    ret->m_tableStart = base::position();
    if (ret->m_version == EPakVersion::MetroidPrime1_2)
    {
        for (atUint32 i = 0; i < resourceCount; i++)
        {
            ret->m_resources[i].compressed = base::readUint32();
            ret->m_resources[i].tag        = CFourCC(*this);
            ret->m_resources[i].id         = CUniqueID(*this, CUniqueID::E_32Bits);
            ret->m_resources[i].size       = base::readUint32();
            ret->m_resources[i].offset     = base::readUint32();
        }
    }
    else
    {
        for (atUint32 i = 0; i < resourceCount; i++)
        {
            ret->m_resources[i].compressed = base::readUint32();
            ret->m_resources[i].tag        = CFourCC(*this);
            ret->m_resources[i].id         = CUniqueID(*this, CUniqueID::E_64Bits);
            ret->m_resources[i].size       = base::readUint32();
            ret->m_resources[i].offset     = base::readUint32();
        }
    }
}

std::vector<SSectionEntry> CPakFileReader::loadSectionTable()
{
    atUint32 sectionCount = base::readUint32();

    std::vector<SSectionEntry> sections;
    sections.resize(sectionCount);

    base::readUBytesToBuf(sections.data(), sizeof(SSectionEntry) * sectionCount);
    
    return sections;
}
