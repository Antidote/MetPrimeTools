#include "PakFile.hpp"
#include "RetroCommon.hpp"
#include <Athena/FileReader.hpp>
#include <Athena/BinaryWriter.hpp>
#include <algorithm>
#include <iomanip>
#include <cinttypes>
#include <string.h>

PakFile::PakFile(const std::string& filename, atUint32 version)
    : m_filename(filename),
      m_version(version),
      m_dataStart(0) // MP1 and 2 have absolute addresses
{
}

PakFile::~PakFile()
{
}

atUint8* PakFile::loadData(atUint64 assetID, const std::string& type)
{
    ConstResourceIterator iter = std::find_if(m_resources.begin(), m_resources.end(),
                                              [&assetID](PakResource res)->bool{return res.id == assetID; });

    if (iter == m_resources.end())
        return nullptr;

    PakResource resource = m_resources[assetID];

    if (!type.empty() && strcmp(resource.tag, type.c_str()) != 0)
        return nullptr;

    atUint8* data = nullptr;
    try
    {
        Athena::io::FileReader reader(m_filename);
        reader.seek(m_dataStart + resource.offset);
        data = reader.readUBytes(resource.size);
    }
    catch(const Athena::error::Exception& e)
    {
        std::cout << e.file() << ": " << e.function() << "(" << e.line() << ")" << e.message() << std::endl;
        delete[] data;
        data = nullptr;
    }

    return data;
}

std::vector<PakResource> PakFile::resourcesByType(const std::string& type)
{
    std::vector<PakResource> ret;
    for (PakResource& res : m_resources)
    {
        if (!strcmp(res.tag, type.c_str()))
            ret.push_back(res);
    }

    return ret;
}

std::string PakFile::resourceName(const atUint64& assetID)
{
    ConstNamedResourceIterator iter = std::find_if(m_namedResources.begin(), m_namedResources.end(),
                                                   [&assetID](PakNamedResource res)->bool{return res.id == assetID; });
    if (iter != m_namedResources.end())
        return iter->name;

    return std::string();
}

void PakFile::dumpPak(const std::string& path, bool decompress)
{
    Athena::io::FileReader reader(m_filename);

    for (atUint32 i = 0; i < m_resources.size(); i++)
    {
        const PakResource& resource = m_resources[i];

        //std::cout << outName << std::endl;
        std::cout << "\rFile " << std::dec << std::fixed << (i+1) << " of " << m_resources.size() << " " << std::setprecision(2)
                  << ((float)((i+1)*100)/m_resources.size()) << "%" << std::flush;

        reader.seek(m_dataStart + resource.offset, Athena::SeekOrigin::Begin);
        atUint8* data = reader.readUBytes(resource.size);

        atUint32 len = resource.size;
        std::string outName;
        if (decompress)
        {
            if(m_version == PakVersion::MetroidPrime1_2)
                outName = Athena::utility::sprintf("%.8" PRIX64 ".%s", resource.id, std::string(resource.tag, 4).c_str());
            else
                outName = Athena::utility::sprintf("%.16" PRIX64 ".%s", resource.id, std::string(resource.tag, 4).c_str());
        }
        else
        {
            if(m_version == PakVersion::MetroidPrime1_2)
                outName = Athena::utility::sprintf("%i_%.8" PRIX64 ".%s", resource.compressed, resource.id, std::string(resource.tag, 4).c_str());
            else
                outName = Athena::utility::sprintf("%i_%.16" PRIX64 ".%s", resource.compressed, resource.id, std::string(resource.tag, 4).c_str());
        }

        if (decompress && resource.compressed)
        {
            Athena::io::BinaryWriter tmp;
            decompressFile(tmp, data, len);
            if (tmp.length() > 0)
            {
                delete[] data;
                data = tmp.data();
                len = tmp.length();
            }
        }

        std::string outPath = path + "/" + outName;
        Athena::io::BinaryWriter writer(outPath);
        writer.writeUBytes(data, len);
        writer.save();

        if (!strncmp(resource.tag, "MREA", 4))
        {
            Athena::io::BinaryReader  in(data, len);
            Athena::io::BinaryWriter out(outPath);

            if(decompressMREA(in, out))
                out.save();

        }
        else
            delete[] data;
    }
}

bool PakFile::isWorldPak()
{
    if (m_filename.empty() || m_resources.size() == 0)
        return false;

    static const bool isWorldPak = resourcesByType("MLVL").size() > 0 && resourcesByType("MREA").size() > 0;

    return isWorldPak;
}

bool PakFile::resourceExists(const atUint64& assetID)
{
    std::vector<PakResource>::iterator iter = std::find_if(m_resources.begin(), m_resources.end(),
                                                            [&assetID](const PakResource& res)->bool{return res.id == assetID;});

    return iter != m_resources.end();
}

bool PakFile::resourceExists(const PakResource& resource)
{
    return std::find(m_resources.begin(), m_resources.end(), resource) != m_resources.end();
}

bool operator ==(const PakNamedResource& left, const PakNamedResource& right)
{
    return (!strncmp(left.tag, right.tag, 4) && right.id == left.id && !left.name.compare(right.name));

}

bool operator ==(const PakResource& left, const PakResource& right)
{
    // We only check up to the data size, since resources can be stored multiple times
    // Eventually we should add a CRC and check that as well....
    return (left.compressed == right.compressed && !strncmp(left.tag, right.tag, 4) && right.id == left.id && left.size == right.size);
}
