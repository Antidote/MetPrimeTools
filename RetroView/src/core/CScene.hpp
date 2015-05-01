#ifndef SCENE_HPP
#define SCENE_HPP

#include <map>
#include "core/CMaterial.hpp"
#include "core/CMesh.hpp"
#include "core/CModelData.hpp"
#include "core/CScriptObject.hpp"

class CScene
{
public:
    CScene();
    ~CScene();

    bool isSkyEnabled();

    CScriptObject* objectByTypeName(const std::string& name);
    CScriptObject* objectByID(const CUniqueID& id);

    void nearestSpawnPoint(const CVector3f& pos, CVector3f& targetPos, CVector3f& rot) const;

private:
    friend class CAreaReader;
    friend class CAreaFile;
    std::vector<CScriptObject> m_objects;
};

#endif // SCENE_HPP
