#include "core/CScene.hpp"
#include <algorithm>
#include <float.h>

CScene::CScene()
{
}

CScene::~CScene()
{
}

bool CScene::isSkyEnabled()
{
    CScriptObject* areaAttributes = objectByTypeName("AreaAttributes");

    if (areaAttributes)
        return areaAttributes->skyEnabled();

    return false;
}

CScriptObject* CScene::objectByTypeName(const std::string& name)
{
    auto iter = std::find_if(m_objects.begin(), m_objects.end(), [&name](CScriptObject& o)->bool{return o.typeName() == name; });
    if (iter != m_objects.end())
    {
        CScriptObject& obj = *iter;
        return &obj;
    }

    return nullptr;
}

void CScene::nearestSpawnPoint(const glm::vec3& pos, glm::vec3& targetPos, glm::vec3& rot)
{
    float minDist = FLT_MAX;

    glm::vec3 nearestPos = pos;
    for (CScriptObject obj : m_objects)
    {
        if (obj.typeName() != "SpawnPoint")
            continue;

        glm::vec3 tmp = obj.position() - pos;
        float sqrDist = glm::dot(tmp, tmp);
        if (sqrDist < minDist)
        {
            minDist = sqrDist;
            nearestPos = obj.position();
            rot = obj.rotation();
        }
    }
    nearestPos.z += 3.f;
    targetPos = nearestPos;
}

