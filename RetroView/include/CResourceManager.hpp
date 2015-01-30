#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#include <string>
#include <Athena/Types.hpp>

#include <vector>
#include <unordered_map>
#include <memory>

#include <CPakFile.hpp>
#include "IResource.hpp"


typedef IResource* (*ResourceFileLoaderCallback)(const std::string&);
typedef IResource* (*ResourceDataLoaderCallback)(const atUint8*, atUint64);

struct ResourceLoaderDesc
{
    std::string                tag;
    ResourceFileLoaderCallback byFile;
    ResourceDataLoaderCallback byData;
};


struct SResourceLoaderRegistrator
{
    SResourceLoaderRegistrator(std::string tag, ResourceFileLoaderCallback byFile, ResourceDataLoaderCallback byData);
};

class CPakFile;
class CResourceManager
{
public:
    virtual ~CResourceManager();
    typedef std::unordered_map<atUint64, IResource*>::iterator         CachedResourceIterator;
    typedef std::unordered_map<atUint64, IResource*>::const_iterator   ConstCachedResourceIterator;

    void initialize(const std::string& baseDirectory);
    bool addPack(const std::string& pak);
    std::vector<SPakResource*> resourcesForPack(const std::string& pak);

    IResource* loadResource(const atUint64& assetID, const std::string& type = std::string());

    IResource* loadResource(const std::string& filepath);

    void registerLoader(std::string tag, ResourceFileLoaderCallback byFile, ResourceDataLoaderCallback byData);
    static std::shared_ptr<CResourceManager> instance();

protected:
    CResourceManager();
    CResourceManager(const CResourceManager&) = delete;
    CResourceManager& operator =(const CResourceManager&)=delete;

private:
    std::unordered_map<std::string, ResourceLoaderDesc> m_loaders;
    IResource* attemptLoad(SPakResource res, CPakFile* pak);
    IResource* attemptLoadFromFile(atUint64 assetID, const std::string& type);
    std::unordered_map<atUint64, IResource*> m_cachedResources;
    std::vector<CPakFile*>                   m_pakFiles;
    std::string                              m_baseDirectory;
};

#ifndef DEFINE_RESOURCE_LOADER
#define DEFINE_RESOURCE_LOADER() \
    private: \
    static const SResourceLoaderRegistrator ____DO_NOT_USE___p_resourceRegistrator
#endif

#ifndef REGISTER_RESOURCE_LOADER
#define REGISTER_RESOURCE_LOADER(Loader, Tag, ByFile, ByData) \
    const SResourceLoaderRegistrator Loader::____DO_NOT_USE___p_resourceRegistrator = SResourceLoaderRegistrator(Tag, Loader::ByFile, Loader::ByData)
#endif

#endif // RESOURCEMANAGER_HPP
