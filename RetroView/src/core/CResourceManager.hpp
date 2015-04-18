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

typedef IResource* (*ResourceDataLoaderCallback)(const atUint8*, atUint64);

struct ResourceLoaderDesc final
{
    CFourCC                    tag;
    ResourceDataLoaderCallback byData;
};


struct SResourceLoaderRegistrator final
{
    SResourceLoaderRegistrator(const CFourCC&, ResourceDataLoaderCallback byData);
};

class CPakFile;
class CPakTreeWidget;
class CResourceManager : public QObject
{
    Q_OBJECT
public:
    virtual ~CResourceManager();
    typedef std::unordered_map<CUniqueID, IResource*, CUniqueIDHash, CUniqueIDComparison>::iterator         CachedResourceIterator;
    typedef std::unordered_map<CUniqueID, IResource*, CUniqueIDHash, CUniqueIDComparison>::const_iterator   ConstCachedResourceIterator;

    void loadBasepath(const std::string& baseDirectory);
    bool addPack(const std::string& pak);
    std::vector<SPakResource*> resourcesForPack(const std::string& pak);

    IResource* loadResource(const CUniqueID& assetID, const std::string& type = std::string());
    IResource* loadResourceFromPak(CPakFile* pak, const CUniqueID& assetID, const std::string& type = std::string());
    void destroyResource(IResource* res);

    void registerLoader(const CFourCC& tag, ResourceDataLoaderCallback byData);
    static std::shared_ptr<CResourceManager> instance();

    void loadPak(std::string filepath);

    void clear();

    bool hasPaks() const;

    void setCurrentBasepath(const std::string& basepath);
    std::string currentBasepath() const;

    static bool canLoad(const std::string& filename);
    void removePak(CPakFile* pak, const std::string& basepath);
signals:
    void newPak(CPakTreeWidget*);
protected:
    CResourceManager();
    CResourceManager(const CResourceManager&) = delete;
    CResourceManager& operator =(const CResourceManager&)=delete;

private:
    std::unordered_map<CFourCC, ResourceLoaderDesc, CFourCCHash, CFourCC_Comparison> m_loaders;
    IResource* attemptLoad(SPakResource res, CPakFile* pak);
    std::unordered_map<CUniqueID, IResource*, CUniqueIDHash, CUniqueIDComparison> m_cachedResources;
    std::unordered_map<std::string, std::vector<CPakFile*>> m_pakFiles;
    std::vector<CUniqueID>                     m_failedAssets;
    std::string                                m_currentBasepath;
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
