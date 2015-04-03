#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#include <QObject>
#include <string>
#include <Athena/Types.hpp>

#include <vector>
#include <unordered_map>
#include <memory>

#include <CPakFile.hpp>
#include "IResource.hpp"

class CAssetIDHash final
{
public:
    std::size_t operator()(CAssetID const& id) const
    {
        size_t hash = 0;
        atUint8* tmp = id.raw();
        for (; *tmp; ++tmp)
            hash = (hash << 2)  + *tmp;
        return hash;
    }
};

class CAssetIDComparison final
{
public:
    bool operator()(CAssetID const& left, CAssetID const& right) const
    {
        return (left == right);
    }
};

typedef IResource* (*ResourceDataLoaderCallback)(const atUint8*, atUint64);

struct ResourceLoaderDesc final
{
    std::string                tag;
    ResourceDataLoaderCallback byData;
};


struct SResourceLoaderRegistrator final
{
    SResourceLoaderRegistrator(std::string tag, ResourceDataLoaderCallback byData);
};

class CPakFile;
class CPakTreeWidget;
class CResourceManager : public QObject
{
    Q_OBJECT
public:
    virtual ~CResourceManager();
    typedef std::unordered_map<CAssetID, IResource*, CAssetIDHash, CAssetIDComparison>::iterator         CachedResourceIterator;
    typedef std::unordered_map<CAssetID, IResource*, CAssetIDHash, CAssetIDComparison>::const_iterator   ConstCachedResourceIterator;

    void initialize(const std::string& baseDirectory);
    bool addPack(const std::string& pak);
    std::vector<SPakResource*> resourcesForPack(const std::string& pak);

    IResource* loadResource(const CAssetID& assetID, const std::string& type = std::string());
    IResource* loadResourceFromPak(CPakFile* pak, const CAssetID& assetID, const std::string& type = std::string());
    void destroyResource(IResource* res);

    void registerLoader(std::string tag, ResourceDataLoaderCallback byData);
    static std::shared_ptr<CResourceManager> instance();

    std::vector<CPakTreeWidget*> pakWidgets() const;

    void loadPak(std::string filepath);

    void clear();
signals:
    void newPak(CPakTreeWidget*);
protected:
    CResourceManager();
    CResourceManager(const CResourceManager&) = delete;
    CResourceManager& operator =(const CResourceManager&)=delete;

private:
    std::unordered_map<std::string, ResourceLoaderDesc> m_loaders;
    IResource* attemptLoad(SPakResource res, CPakFile* pak);
    std::unordered_map<CAssetID, IResource*, CAssetIDHash, CAssetIDComparison> m_cachedResources;
    std::vector<CPakFile*>                   m_pakFiles;
    std::vector<CPakTreeWidget*>             m_pakTreeWidgets;
    std::vector<CAssetID>                    m_failedAssets;
    std::string                              m_baseDirectory;
};


#ifndef DEFINE_RESOURCE_LOADER
#define DEFINE_RESOURCE_LOADER() \
    private: \
    static const SResourceLoaderRegistrator ____DO_NOT_USE___p_resourceRegistrator
#endif

#ifndef REGISTER_RESOURCE_LOADER
#define REGISTER_RESOURCE_LOADER(Loader, Tag, ByData) \
    const SResourceLoaderRegistrator Loader::____DO_NOT_USE___p_resourceRegistrator = SResourceLoaderRegistrator(Tag, Loader::ByData)
#endif

#endif // RESOURCEMANAGER_HPP
