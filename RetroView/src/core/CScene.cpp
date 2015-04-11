#include "core/CScene.hpp"
#include <algorithm>

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

