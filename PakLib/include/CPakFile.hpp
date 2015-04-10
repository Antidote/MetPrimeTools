#ifndef PAKFILE_HPP
#define PAKFILE_HPP

#include <string>
#include <Athena/Types.hpp>
#include <unordered_map>
#include <vector>
#include <memory.h>
#include <cstring>

#include "CUniqueID.hpp"
#include "CFourCC.hpp"

struct SPakResource
{
    atUint32 compressed;
    CFourCC  tag;
    CUniqueID id;
    atUint32 size;
    atUint32 offset;
};

struct SPakNamedResource
{
    CFourCC     tag;
    CUniqueID    id;
    std::string name;
};

namespace EPakVersion
{
enum
{
    MetroidPrime1_2   = 0x00030005, //!< Used in 1, 2 and the MP3 beta
    MetroidPrime3     = 0x00000002,
    MetroidPrime3Beta = 0x00030002 //!< Not the real value, this is only to keep the two separate
};
}

class CPakFile final
{
public:
    typedef std::vector<SPakResource     >::iterator       ResourceIterator;
    typedef std::vector<SPakResource     >::const_iterator ConstResourceIterator;
    typedef std::vector<SPakNamedResource>::iterator       NamedResourceIterator;
    typedef std::vector<SPakNamedResource>::const_iterator ConstNamedResourceIterator;

    CPakFile(const std::string& filename, atUint32 version);
    ~CPakFile();


    std::string filename() const;

    void loadTable();

    atUint8* loadData(CUniqueID assetID, const std::string& type = std::string());

    std::vector<SPakResource> resourcesByType(const std::string& type);
    std::vector<SPakResource> resources() const;

    std::string resourceName(const atUint64& assetID);
    void dumpPak(const std::string& path, bool decompress=true);

    bool isWorldPak();

    bool resourceExists(const CUniqueID& assetID);
    bool resourceExists(const SPakResource& resource);

    int version() const;

    void removeDuplicates();
private:
    friend class CPakFileReader;

    bool        m_isWorldPak;
    std::string m_filename;
    atUint32    m_version;
    atUint32    m_dataStart;
    atUint32    m_tableStart;

    std::vector<SPakNamedResource> m_namedResources;
    std::vector<SPakResource>      m_resources;
};

bool operator ==(const SPakNamedResource& left, const SPakNamedResource& right);
bool operator ==(const SPakResource& left, const SPakResource& right);
bool operator <(const SPakResource& left, const SPakResource& right);

#endif // PAKFILE_HPP
