#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#include <string>
#include <Athena/Types.hpp>

#include <vector>
#include <unordered_map>
#include <memory>

class IResource
{
public:
    virtual bool load()=0;
    atUint64 assetId() const;

protected:
    atUint64 m_assetID;
};

class PakFile;
class ResourceManager
{
public:
    virtual ~ResourceManager();
    typedef std::unordered_map<atUint32, IResource*>::iterator         ResourceIterator;
    typedef std::unordered_map<atUint32, IResource*>::const_iterator   ConstResourceIterator;

    void initialize(const std::string& baseDirectory);
    bool addPack(const std::string& pak);

    IResource* loadResource(const atUint64& assetID);

    static std::shared_ptr<ResourceManager> instance();
protected:
    ResourceManager() {}
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator =(const ResourceManager&)=delete;

private:
    std::unordered_map<atUint32, IResource*> m_cachedResources;
    std::vector<PakFile*>                    m_pakFiles;
    std::string                              m_baseDirectory;
};

#endif // RESOURCEMANAGER_HPP
