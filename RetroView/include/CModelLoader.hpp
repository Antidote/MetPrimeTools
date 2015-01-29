#ifndef MODELLOADER_HPP
#define MODELLOADER_HPP

class IRenderableModel;
#include <vector>
#include <unordered_map>
#include <memory>

typedef IRenderableModel* (*LoaderCallback)(const std::string&);

struct SModelLoaderDescriptor final
{
    std::string ModelFilter;
    LoaderCallback LoaderFunc;
};

struct SModelLoader final
{
    SModelLoader();
    ~SModelLoader();
    IRenderableModel* loadFile(const std::string& filename);

    void registerLoader(std::string extension, std::string modelFilter, LoaderCallback loader);

    static std::shared_ptr<SModelLoader> instance();
    std::vector<std::string> filters() const;
    std::vector<std::string> extensions() const;
private:
    std::unordered_map<std::string, SModelLoaderDescriptor> m_loaders;
};

struct SModelLoaderRegistrator
{
    SModelLoaderRegistrator(std::string extension, std::string modelFilter, LoaderCallback loader);
};

#ifndef DEFINE_MODEL_LOADER
#define DEFINE_MODEL_LOADER() \
    private: \
    static const SModelLoaderRegistrator ____DO_NOT_USE___p_registrator
#endif

#ifndef REGISTER_MODEL_LOADER
#define REGISTER_MODEL_LOADER(Loader, Ext, Filter, Function) \
    const SModelLoaderRegistrator Loader::____DO_NOT_USE___p_registrator = SModelLoaderRegistrator(Ext, Filter, Loader::Function)
#endif

#endif // MODELLOADER_HPP
