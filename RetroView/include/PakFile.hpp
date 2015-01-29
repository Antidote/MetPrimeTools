#ifndef PAKFILE_HPP
#define PAKFILE_HPP

#include <string>
#include <Athena/Types.hpp>
#include <unordered_map>
#include <vector>
#include <memory.h>
#include <cstring>

struct PakResource
{
    atUint32 compressed;
    char     tag[4];
    atUint64 id;
    atUint32 size;
    atUint32 offset;
};

struct PakNamedResource
{
    char        tag[4];
    atUint64    id;
    std::string name;
};

namespace PakVersion
{
enum
{
    MetroidPrime1_2   = 0x00030005, //!< Used in 1, 2 and the MP3 beta
    MetroidPrime3     = 0x00000002,
    MetroidPrime3Beta = 0x00030002 //!< Not the real value, this is only to keep the two separate
};
}

class PakFile final
{
public:
    typedef std::vector<PakResource     >::iterator       ResourceIterator;
    typedef std::vector<PakResource     >::const_iterator ConstResourceIterator;
    typedef std::vector<PakNamedResource>::iterator       NamedResourceIterator;
    typedef std::vector<PakNamedResource>::const_iterator ConstNamedResourceIterator;

    PakFile(const std::string& filename, atUint32 version);
    ~PakFile();


    std::string filename();

    void loadTable();

    atUint8* loadData(atUint64 assetID, const std::string& type = std::string());

    std::vector<PakResource> resourcesByType(const std::string& type);

    std::string resourceName(const atUint64& assetID);
    void dumpPak(const std::string& path, bool decompress=true);

    bool isWorldPak();

    bool resourceExists(const atUint64& assetID);
    bool resourceExists(const PakResource& resource);

private:
    friend class PakFileReader;

    std::string m_filename;
    atUint32    m_version;
    atUint32    m_dataStart;

    std::vector<PakNamedResource> m_namedResources;
    std::vector<PakResource>      m_resources;
};

bool operator ==(const PakNamedResource& left, const PakNamedResource& right);
bool operator ==(const PakResource& left, const PakResource& right);

#endif // PAKFILE_HPP
