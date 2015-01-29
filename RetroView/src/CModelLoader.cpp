#include "CModelLoader.hpp"
#include "IRenderableModel.hpp"
#include <Athena/Exception.hpp>
#include <iostream>
#include <algorithm>
#include <QMessageBox>

SModelLoader::SModelLoader()
{
    std::cout << "Model Loader initialized, waiting on loader registration..." << std::endl;
}

SModelLoader::~SModelLoader()
{
    std::cout << "Model Loader shutdown" << std::endl;
}

IRenderableModel* SModelLoader::loadFile(const std::string& filename)
{
    std::string extension = filename.substr(filename.find_last_of('.') + 1, filename.size() - (filename.find_last_of('.') + 1));
    Athena::utility::tolower(extension);
    if (m_loaders.find(extension) != m_loaders.end())
    {
//        try
//        {
            return m_loaders[extension].LoaderFunc(filename);
//        }
//        catch(...)
//        {
//            throw;
//        }
    }

    for (std::pair<std::string, SModelLoaderDescriptor> cb : m_loaders)
    {
//        try
//        {
            return cb.second.LoaderFunc(filename);
//        }
//        catch(const Athena::error::Exception& e)
//        {
//            std::cout << e.message() << std::endl;
//        }
    }
    THROW_EXCEPTION("Unable to find an appropriate loader, file \n%s\n is invalid or corrupt", filename.c_str());
}

void SModelLoader::registerLoader(std::string extension, std::string modelFilter, LoaderCallback loader)
{
    if (m_loaders.find(extension) != m_loaders.end())
    {
        std::cout << "Already had a loader for " << extension << " registered, ignored registration attempt" << std::endl;
        return;
    }

    std::cout << "Loader for format " << extension << " registered" << std::endl;
    Athena::utility::tolower(extension);
    SModelLoaderDescriptor desc;
    desc.ModelFilter = modelFilter;
    desc.LoaderFunc = loader;
    m_loaders[extension] = desc;
}

std::shared_ptr<SModelLoader> SModelLoader::instance()
{
    static std::shared_ptr<SModelLoader> instance(new SModelLoader);
    return instance;
}

std::vector<std::string> SModelLoader::filters() const
{
    std::vector<std::string> filters;
    for (std::pair<std::string, SModelLoaderDescriptor> pair : m_loaders)
        filters.push_back(pair.second.ModelFilter);

    return filters;
}

std::vector<std::string> SModelLoader::extensions() const
{
    std::vector<std::string> extensions;
    for (std::pair<std::string, SModelLoaderDescriptor> pair : m_loaders)
        extensions.push_back(pair.first);

    return extensions;
}

SModelLoaderRegistrator::SModelLoaderRegistrator(std::string extension, std::string modelFilter, LoaderCallback loader)
{
    SModelLoader::instance().get()->registerLoader(extension, modelFilter, loader);
}
