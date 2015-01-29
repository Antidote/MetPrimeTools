#include "CResourceManager.hpp"
#include <iostream>

namespace
{
struct concrete_ResourceManager : public CResourceManager
{
};

}

CResourceManager::~CResourceManager()
{

}

void CResourceManager::initialize(const std::string& baseDirectory)
{
    m_baseDirectory = baseDirectory;

    std::cout << "ResourceManager initialized @ " << m_baseDirectory << std::endl;
}

IResource* CResourceManager::loadResource(const atUint64& assetID)
{
}

std::shared_ptr<CResourceManager> CResourceManager::instance()
{
    static std::shared_ptr<CResourceManager> instance = std::make_shared<concrete_ResourceManager>();

    return instance;
}
