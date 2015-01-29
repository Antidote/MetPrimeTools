#include "CResourceManager.hpp"
#include <iostream>

namespace
{
struct concrete_ResourceManager : public ResourceManager
{
};

}

ResourceManager::~ResourceManager()
{

}

void ResourceManager::initialize(const std::string& baseDirectory)
{
    m_baseDirectory = baseDirectory;

    std::cout << "ResourceManager initialized @ " << m_baseDirectory << std::endl;
}

IResource* ResourceManager::loadResource(const atUint64& assetID)
{
}

std::shared_ptr<ResourceManager> ResourceManager::instance()
{
    static std::shared_ptr<ResourceManager> instance = std::make_shared<concrete_ResourceManager>();

    return instance;
}
