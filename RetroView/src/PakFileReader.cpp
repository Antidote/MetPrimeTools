#include "PakFileReader.hpp"
#include "PakFile.hpp"
#include <Athena/InvalidDataException.hpp>
#include <memory.h>
#include <cstring>

struct MP1_2_Resource
{
    atUint32 compressed;
    char tag[4];
    atUint32 id;
    atUint32 size;
    atUint32 offset;
};

struct SectionEntry
{
    char tag[4];
    atUint32 dataLen;
};

PakFileReader::PakFileReader(const std::string &filename)
    : base(filename)
{
    base::setEndian(Athena::Endian::BigEndian);
}

PakFileReader::~PakFileReader()
{
}

PakFile* PakFileReader::read()
{
    atUint32 version = base::readUint32();

    if (version != PakVersion::MetroidPrime1_2 && version != PakVersion::MetroidPrime3)
        THROW_INVALID_DATA_EXCEPTION("Invalid or unsupported version: 0x%.8X", version);

    PakFile* ret = nullptr;
    try
    {
        ret = new PakFile(m_filename, version);
        switch(version)
        {
            case PakVersion::MetroidPrime1_2:
            {
                base::readUint32(); // unknown;
                loadNameTable(ret);
                loadResourceTable(ret);
            }
                break;
            case PakVersion::MetroidPrime3:
            {
                atUint32 headerLen = base::readUint32();
                base::seek(headerLen, Athena::SeekOrigin::Begin);

                std::vector<SectionEntry> sections = loadSectionTable();

                for (SectionEntry sectionEntry : sections)
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
    }
    catch(...)
    {
        delete ret;
        throw;
    }

    return ret;
}

void PakFileReader::loadNameTable(PakFile* ret)
{
    atUint32 namedResourceCount = base::readUint32();
    if (ret->m_version == PakVersion::MetroidPrime1_2)
    {
        while ((namedResourceCount--) > 0)
        {
            PakNamedResource res;
            atUint8* tag = base::readUBytes(4);
            memcpy(res.tag, tag, 4);
            delete[] tag;
            atUint64 id = base::readUint32();
            atUint32 nameLength = base::readUint32();
            if (nameLength > 1024) // While the engine doesn't cap it to 1024, it's a reasonable to assume nobody will be using names this long
            {
                ret->m_version = PakVersion::MetroidPrime3Beta;
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
            PakNamedResource name;
            name.name = base::readString();
            atUint8* tag = base::readUBytes(4);
            memcpy(name.tag, tag, 4);
            name.id = base::readUint64();

            delete[] tag;

            ret->m_namedResources.push_back(name);
        }
    }
}

PakFile* PakFileReader::load(const std::string& filename)
{
    PakFileReader reader(filename);

    return reader.read();
}

void PakFileReader::loadResourceTable(PakFile* ret)
{
    atUint32 resourceCount = base::readUint32();
    ret->m_resources.resize(resourceCount);
    if (ret->m_version == PakVersion::MetroidPrime1_2)
    {

        // We're going to cheat here:
        // Since all the entries are contiguous, we're able to just read in the entire entry buffer
        // and then assign it to a temporary vector via memcpy so we can pass it on to PakFile::m_resources,
        // after that we just swap the necessary values, and voila complete entry table, this is a bit more involved
        // than MP3 and MP3Beta.
        std::vector<MP1_2_Resource> tmpResources;
        tmpResources.resize(resourceCount);
        atUint8* data = base::readUBytes(sizeof(MP1_2_Resource) * resourceCount);
        memcpy((char*)tmpResources.data(), data, sizeof(MP1_2_Resource) * resourceCount);

        delete[] data;
        for (atUint32 i = 0; i < resourceCount; i++)
        {
            tmpResources[i].compressed = Athena::utility::BigUint32(tmpResources[i].compressed);
            memcpy(tmpResources[i].tag, tmpResources[i].tag, 4);
            tmpResources[i].id         = Athena::utility::BigUint32(tmpResources[i].id);
            tmpResources[i].size       = Athena::utility::BigUint32(tmpResources[i].size);
            tmpResources[i].offset     = Athena::utility::BigUint32(tmpResources[i].offset);
        }
    }
    else
    {
        // We're going to cheat here:
        // Since all the entries are contiguous, we're able to just read in the entire entry buffer
        // and then assign it to PakFile::m_resources via memcpy, after that we just swap the
        // necessary values, and voila complete entry table
        ret->m_resources.resize(resourceCount);
        atUint8* data = base::readUBytes(sizeof(PakResource) * resourceCount);
        memcpy((char*)ret->m_resources.data(), data, sizeof(PakResource) * resourceCount);
        delete[] data;
        for (PakResource& res : ret->m_resources)
        {
            Athena::utility::BigUint32(res.compressed);
            Athena::utility::BigUint64(res.id);
            Athena::utility::BigUint32(res.size);
            Athena::utility::BigUint32(res.offset);
        }
    }
}

std::vector<SectionEntry> PakFileReader::loadSectionTable()
{
    atUint32 sectionCount = base::readUint32();

    std::vector<SectionEntry> sections;
    sections.resize(sectionCount);

    atUint8* data = base::readUBytes(sizeof(SectionEntry) * sectionCount);

    memcpy(sections.data(), data, sizeof(SectionEntry) * sectionCount);
    delete[] data;

    return sections;
}
