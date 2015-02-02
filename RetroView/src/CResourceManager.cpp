#include "CResourceManager.hpp"
#include "CPakFileReader.hpp"
#include "GXCommon.hpp"
#include <iostream>
#include <algorithm>
#include <Athena/Utility.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <QMessageBox>

namespace
{
struct concrete_ResourceManager : public CResourceManager
{
};

}

CResourceManager::CResourceManager()
{
    std::cout << "ResourceManager created" << std::endl;
}

CResourceManager::~CResourceManager()
{
    for (std::pair<atUint64, IResource*> res : m_cachedResources)
        delete res.second;

    for (CPakFile* pak : m_pakFiles)
        delete pak;

    m_pakFiles.clear();

    std::cout << "ResourceManager destroyed" << std::endl;
}

void CResourceManager::initialize(const std::string& baseDirectory)
{
    m_baseDirectory = baseDirectory;

    std::cout << "ResourceManager initialized @ " << m_baseDirectory << std::endl;
    std::cout << "Searching for paks..." << std::endl;

    DIR* dir = opendir(m_baseDirectory.c_str());
    if (dir)
    {
        struct dirent * dp;

        while((dp = readdir(dir)) != NULL)
        {
            struct stat64 st;
            std::string filepath = m_baseDirectory + "/" + std::string(dp->d_name);
            stat64(filepath.c_str(), &st);
            if(S_ISREG(st.st_mode))
            {
                std::string filename = dp->d_name;
                std::string ext = filename.substr(filename.rfind('.') + 1, filename.size());
                Athena::utility::tolower(ext);
                if (!ext.compare("pak"))
                {
                    std::cout << "Found pak " << filename << " ...";
                    CPakFile* pak = nullptr;
                    try
                    {
                        CPakFileReader reader(filepath);
                        pak = reader.read();
                        if (pak->isWorldPak())
                        {
                            m_pakFiles.push_back(pak);
                            std::cout << " loaded" << std::endl;
                        }
                        else
                        {
                            std::cout << " not a world pak, ignored" << std::endl;
                            delete pak;
                        }
                    }
                    catch(...)
                    {
                        delete pak;
                        std::cout << " failed to load" << std::endl;
                    }
                }
            }
        }
        closedir(dir);
    }
}

IResource* CResourceManager::loadResource(const atUint64& assetID, const std::string& type)
{
    CachedResourceIterator iter = m_cachedResources.find(assetID);
    if (iter != m_cachedResources.end())
        return iter->second;

    for (CPakFile* pak : m_pakFiles)
    {
        std::vector<SPakResource> pakResources;
        if (!type.empty())
            pakResources = pak->resourcesByType(type);
        else
            pakResources = pak->resources();

        std::vector<SPakResource>::iterator iter = std::find_if(pakResources.begin(), pakResources.end(),
                                                                [&assetID](const SPakResource& r)->bool{return r.id == assetID; });
        if (iter != pakResources.end())
            return attemptLoad(*iter, pak);
    }

    return attemptLoadFromFile(assetID, type);
}

IResource* CResourceManager::loadResource(const std::string& filepath)
{
    IResource* ret = nullptr;

//    try
//    {
        std::string tag = filepath.substr(filepath.rfind(".") + 1, filepath.length());
        Athena::utility::tolower(tag);
        if (m_loaders.find(tag) == m_loaders.end())
            return nullptr;

        ret = m_loaders[tag].byFile(filepath);
        ret->m_assetID = assetIdFromPath(filepath);
        m_cachedResources[ret->assetId()] = ret;
//    }
//    catch(const Athena::error::Exception& e)
//    {
//        std::cout << e.message() << std::endl;
//        delete ret;
//        ret = nullptr;
//    }

    return ret;
}


std::shared_ptr<CResourceManager> CResourceManager::instance()
{
    static std::shared_ptr<CResourceManager> instance = std::make_shared<concrete_ResourceManager>();

    return instance;
}

IResource* CResourceManager::attemptLoad(SPakResource res, CPakFile* pak)
{
    std::string tag = std::string((const char*)res.tag, 4);
    Athena::utility::tolower(tag);
    if (m_loaders.find(tag) == m_loaders.end())
        return nullptr;

    atUint8* data = pak->loadData(res.id, std::string((const char*)res.tag, 4));
    if (data == nullptr)
        return nullptr;

    IResource* ret = nullptr;
    try
    {
        ret = m_loaders[tag].byData(data, res.size);
        ret->m_assetID = res.id;
        m_cachedResources[res.id] = ret;
    }
    catch(const Athena::error::Exception& e)
    {
        std::cout << e.message() << std::endl;
        delete ret;
        ret = nullptr;
    }

    return ret;
}

IResource* CResourceManager::attemptLoadFromFile(atUint64 assetId, const std::string& type)
{
    for (std::pair<std::string, ResourceLoaderDesc> desc : m_loaders)
    {
        if (!type.empty())
        {
            std::string tmp = type;
            Athena::utility::tolower(tmp);
            if (tmp.compare(desc.first) != 0)
                continue;
        }

        QString tag = QString::fromStdString(desc.first);
        for (atUint32 attempt = 0; attempt < 8; attempt++)
        {
            QString filepath;
            switch(attempt)
            {
                case 0:
                    filepath = QString("%1.%2").arg(assetId, 8, 16, QChar('0')).arg(tag);
                    break;
                case 1:
                    filepath = QString("%1.%2").arg(assetId, 8, 16, QChar('0')).arg(tag).toUpper();
                    break;
                case 2:
                    filepath = QString("0_%1.%2").arg(assetId, 8, 16, QChar('0')).arg(tag).toUpper();
                    break;
                case 3:
                    filepath = QString("1_%1.%2").arg(assetId, 8, 16, QChar('0')).arg(tag).toUpper();
                    break;
                case 4:
                    filepath = QString("%1.%2").arg(assetId, 16, 16, QChar('0')).arg(tag);
                    break;
                case 5:
                    filepath = QString("%1.%2").arg(assetId, 16, 16, QChar('0')).arg(tag).toUpper();
                    break;
                case 6:
                    filepath = QString("0_%1.%2").arg(assetId, 16, 16, QChar('0')).arg(tag).toUpper();
                    break;
                case 7:
                    filepath = QString("1_%1.%2").arg(assetId, 16, 16, QChar('0')).arg(tag).toUpper();
                    break;
            }

            IResource* ret = nullptr;
            try
            {
                ret = desc.second.byFile(m_baseDirectory + "/" + filepath.toStdString());

                if (ret)
                {
                    m_cachedResources[assetId] = ret;
                    return ret;
                }
            }
            catch(const Athena::error::Exception& e)
            {
                delete ret;
                ret = nullptr;
            }
        }
    }

    return nullptr;
}

void CResourceManager::registerLoader(std::string tag, ResourceFileLoaderCallback byFile, ResourceDataLoaderCallback byData)
{
    if (m_loaders.find(tag) != m_loaders.end())
    {
        std::cout << "Already had a loader for " << tag << " registered, ignored registration attempt" << std::endl;
        return;
    }

    std::cout << "Loader for resource " << tag << " registered" << std::endl;
    Athena::utility::tolower(tag);
    ResourceLoaderDesc desc;
    tag = tag;
    desc.byFile = byFile;
    desc.byData = byData;
    m_loaders[tag] = desc;
}

SResourceLoaderRegistrator::SResourceLoaderRegistrator(std::string tag, ResourceFileLoaderCallback byFile, ResourceDataLoaderCallback byData)
{
    CResourceManager::instance().get()->registerLoader(tag, byFile, byData);
}

