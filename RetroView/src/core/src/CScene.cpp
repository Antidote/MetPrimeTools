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

void CScene::nearestSpawnPoint(const CVector3f& pos, CVector3f& targetPos, CVector3f& rot) const
{
    float minDist = FLT_MAX;

    CVector3f nearestPos = pos;
    for (CScriptObject obj : m_objects)
    {
        if (obj.typeName() != "SpawnPoint")
            continue;

        CVector3f tmp = obj.position() - pos;
        float sqrDist = tmp.lengthSquared();
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

