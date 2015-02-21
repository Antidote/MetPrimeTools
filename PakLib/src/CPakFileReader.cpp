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
            atUint64 id = base::readUint32();
            atUint32 nameLength = base::readUint32();
            if (nameLength > 1024) // While the engine doesn't cap it to 1024, it's a reasonable to assume nobody will be using names this long
            {
                ret->m_version = EPakVersion::MetroidPrime3Beta;
                base::seek(-8);
                id = base::readUint64();
                nameLength = base::readUint32();
            }

            res.id   = id;
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
            name.id = base::readUint64();
            ret->m_namedResources.push_back(name);
        }
    }
}

CPakFile* CPakFileReader::load(const std::string& filename)
{
    CPakFileReader reader(filename);

    return reader.read();
}

void CPakFileReader::loadResourceTable(CPakFile* ret)
{
    atUint32 resourceCount = base::readUint32();
    ret->m_resources.resize(resourceCount);
    ret->m_tableStart = base::position();
    if (ret->m_version == EPakVersion::MetroidPrime1_2)
    {

        // We're going to cheat here:
        // Since all the entries are contiguous, we're able to just read in the entire entry buffer
        // and then assign it to a temporary vector via memcpy so we can pass it on to PakFile::m_resources,
        // after that we just swap the necessary values, and voila complete entry table, this is a bit more involved
        // than MP3 and MP3Beta.
        std::vector<SMP1_2_Resource> tmpResources;
        tmpResources.resize(resourceCount);
        atUint8* data = base::readUBytes(sizeof(SMP1_2_Resource) * resourceCount);
        memcpy((char*)tmpResources.data(), data, sizeof(SMP1_2_Resource) * resourceCount);

        delete[] data;
        for (atUint32 i = 0; i < resourceCount; i++)
        {
            ret->m_resources[i].compressed = Athena::utility::BigUint32(tmpResources[i].compressed);
            ret->m_resources[i].tag        = tmpResources[i].tag;
            ret->m_resources[i].id         = Athena::utility::BigUint32(tmpResources[i].id);
            ret->m_resources[i].size       = Athena::utility::BigUint32(tmpResources[i].size);
            ret->m_resources[i].offset     = Athena::utility::BigUint32(tmpResources[i].offset);
        }
    }
    else
    {
        // We're going to cheat here:
        // Since all the entries are contiguous, we're able to just read in the entire entry buffer
        // and then assign it to PakFile::m_resources via memcpy, after that we just swap the
        // necessary values, and voila complete entry table
        ret->m_resources.resize(resourceCount);
        atUint8* data = base::readUBytes(sizeof(SPakResource) * resourceCount);
        memcpy((char*)ret->m_resources.data(), data, sizeof(SPakResource) * resourceCount);
        delete[] data;
        for (SPakResource& res : ret->m_resources)
        {
            Athena::utility::BigUint32(res.compressed);
            Athena::utility::BigUint64(res.id);
            Athena::utility::BigUint32(res.size);
            Athena::utility::BigUint32(res.offset);
        }
    }
}

std::vector<SSectionEntry> CPakFileReader::loadSectionTable()
{
    atUint32 sectionCount = base::readUint32();

    std::vector<SSectionEntry> sections;
    sections.resize(sectionCount);

    atUint8* data = base::readUBytes(sizeof(SSectionEntry) * sectionCount);

    memcpy(sections.data(), data, sizeof(SSectionEntry) * sectionCount);
    delete[] data;

    return sections;
}
