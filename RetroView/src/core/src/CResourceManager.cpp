#include "core/CResourceManager.hpp"
#include "ui/CPakTreeWidget.hpp"
#include "core/GXCommon.hpp"

#include <CPakFileReader.hpp>
#include <iostream>
#include <algorithm>
#include <Athena/Utility.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#if _WIN32
#include <platforms/win_dirent.h>
#define stat64 _stat64
#else
#include <dirent.h>
#endif
#include <QMessageBox>
#include <QLabel>

namespace
{
struct concrete_ResourceManager : public CResourceManager
{
};

QThread* resourceManagerThread = nullptr;
}

CResourceManager::CResourceManager()
{
    std::cout << "ResourceManager created" << std::endl;
    m_cachedResources.clear();
}

CResourceManager::~CResourceManager()
{
    for (std::pair<CUniqueID, IResource*> res : m_cachedResources)
        delete res.second;

    for (CPakFile* pak : m_pakFiles)
        delete pak;

    m_pakFiles.clear();

    std::cout << "ResourceManager destroyed" << std::endl;
}

void CResourceManager::loadPak(std::string filepath)
{
    size_t off = filepath.rfind("/");
    if (off == std::string::npos)
        off = filepath.rfind("\\");
    off += 1;
    std::string filename = filepath.substr(off, filepath.size() - off);
    std::cout << "Found pak " << filename << " ...";
    std::cout.flush();

    if (!canLoad(filepath))
    {
        std::cout << " invalid" << std::endl;
        return;
    }

    std::vector<CPakFile*>::iterator iter = std::find_if(m_pakFiles.begin(), m_pakFiles.end(),
                                                            [&filepath](const CPakFile* r)->bool{return r->filename() == filepath; });

    if (iter != m_pakFiles.end())
    {
        std::cout << " already loaded" << std::endl;
        return;
    }

    CPakFile* pak = nullptr;
    try
    {
        CPakFileReader reader(filepath);
        pak = reader.read();
        pak->removeDuplicates();
        m_pakFiles.push_back(pak);
        CPakTreeWidget* widget = new CPakTreeWidget(pak);
        emit newPak(widget);

        std::cout << " loaded" << std::endl;
    }
    catch(...)
    {
        delete pak;
        std::cout << " failed to load" << std::endl;
    }
}

void CResourceManager::clear()
{
    for (std::pair<CUniqueID, IResource*> res : m_cachedResources)
        delete res.second;
    m_cachedResources.clear();
    m_failedAssets.clear();
}

bool CResourceManager::hasPaks() const
{
    return m_pakFiles.size() > 0;
}

bool CResourceManager::canLoad( const std::string& filename)
{
    return CPakFileReader::canLoad(filename);
}

void CResourceManager::removePak(CPakFile* pak)
{
    auto it = std::find(m_pakFiles.begin(), m_pakFiles.end(), pak);
    if (it == m_pakFiles.end())
        return;

    bool clearCache = false;
    for (std::pair<CUniqueID, IResource*> pair : m_cachedResources)
    {
        if (pair.second && pair.second->source() == *it)
        {
            clearCache = true;
            break;
        }
    }

    CPakFile* tmp = *it;
    m_pakFiles.erase(it);
    delete tmp;

    if (clearCache)
        clear();
}

void CResourceManager::loadBasepath(const std::string& baseDirectory)
{
    std::cout << "Searching for paks @ " << baseDirectory << "..." << std::endl;

    DIR* dir = opendir(baseDirectory.c_str());
    if (dir)
    {
        struct dirent * dp;

        while((dp = readdir(dir)) != NULL)
        {
            struct stat64 st;
            std::string filepath = baseDirectory + "/" + std::string(dp->d_name);
            stat64(filepath.c_str(), &st);
            if(S_ISREG(st.st_mode))
            {
                std::string filename = dp->d_name;
                std::string ext = filename.substr(filename.rfind('.') + 1, filename.size());
                Athena::utility::tolower(ext);
                if (!ext.compare("pak"))
                {
                    loadPak(filepath);
                }
            }
        }
        closedir(dir);
    }
}

IResource* CResourceManager::loadResource(const CUniqueID& assetID, const std::string& type)
{
    if (assetID == CUniqueID::InvalidAsset)
        return nullptr;

    std::vector<CUniqueID>::iterator failedIter = std::find(m_failedAssets.begin(), m_failedAssets.end(), assetID);
    if (failedIter != m_failedAssets.end())
        return nullptr;

    CachedResourceIterator iter = m_cachedResources.find(assetID);
    if (iter != m_cachedResources.end())
        return iter->second;

    for (CPakFile* pak : m_pakFiles)
    {
        IResource* ret = loadResourceFromPak(pak, assetID, type);
        if (ret)
            return ret;
    }

    return nullptr;
}

IResource* CResourceManager::loadResourceFromPak(CPakFile* pak, const CUniqueID& assetID, const std::string& type)
{
    if (assetID == CUniqueID::InvalidAsset)
        return nullptr;

    std::vector<CUniqueID>::iterator failedIter = std::find(m_failedAssets.begin(), m_failedAssets.end(), assetID);
    if (failedIter != m_failedAssets.end())
        return nullptr;


    IResource* res = m_cachedResources[assetID];
    if (res != nullptr)
        return res;

    std::vector<SPakResource> pakResources;
    if (!type.empty())
        pakResources = pak->resourcesByType(type);
    else
        pakResources = pak->resources();

    std::vector<SPakResource>::iterator iter = std::find_if(pakResources.begin(), pakResources.end(),
                                                            [&assetID](const SPakResource& r)->bool{return r.id == assetID; });
    if (iter != pakResources.end())
        return attemptLoad(*iter, pak);

    return nullptr;
}

void CResourceManager::destroyResource(IResource* res)
{
    m_cachedResources.erase(res->assetId());
    delete res;
}

std::shared_ptr<CResourceManager> CResourceManager::instance()
{
    static std::shared_ptr<CResourceManager> instance = std::make_shared<concrete_ResourceManager>();

    return instance;
}

IResource* CResourceManager::attemptLoad(SPakResource res, CPakFile* pak)
{
    if (m_loaders.find(res.tag) == m_loaders.end())
        return nullptr;

    atUint8* data = pak->loadData(res.id, res.tag.toString());
    if (data == nullptr)
        return nullptr;

    IResource* ret = nullptr;
    try
    {
        ret = m_loaders[res.tag].byData(data, res.size);
        if (ret)
        {
            ret->m_assetType = res.tag;
            ret->m_assetID = res.id;
            ret->m_source = pak;
            m_cachedResources[res.id] = ret;
        }
    }
    catch(const Athena::error::Exception& e)
    {
        std::cout << e.file() << " " << e.message() << std::endl;
        delete ret;
        ret = nullptr;
        m_failedAssets.push_back(res.id);
    }

    return ret;
}

void CResourceManager::registerLoader(const CFourCC& tag, ResourceDataLoaderCallback byData)
{
    if (m_loaders.find(tag) != m_loaders.end())
    {
        std::cout << "Already had a loader for " << tag.toString() << " registered, ignored registration attempt" << std::endl;
        return;
    }

    std::cout << "Loader for resource " << tag.toString() << " registered" << std::endl;
    ResourceLoaderDesc desc;
    desc.tag = tag;
    desc.byData = byData;
    m_loaders[tag] = desc;
}

SResourceLoaderRegistrator::SResourceLoaderRegistrator(const CFourCC& tag, ResourceDataLoaderCallback byData)
{
    CResourceManager::instance().get()->registerLoader(tag, byData);
}


