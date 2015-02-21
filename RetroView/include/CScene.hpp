#ifndef SCENE_HPP
#define SCENE_HPP

#include <unordered_map>
#include "CMaterial.hpp"
#include "CMesh.hpp"
#include "CModelData.hpp"

class CScene
{
public:
    CScene();
    ~CScene();

    void addModel(CModelData& model);
private:
//    std::vector<CModelData> m_models;
//    std::unordered_map<CMaterial, CMesh> m_opaqueMeshes;
//    std::unordered_map<CMaterial, CMesh> m_transparentMeshes;
};

#endif // SCENE_HPP
