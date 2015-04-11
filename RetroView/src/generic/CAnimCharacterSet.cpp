#include "generic/CAnimCharacterSet.hpp"

CAnimCharacterSet::CAnimCharacterSet()
{

}

CAnimCharacterSet::~CAnimCharacterSet()
{

}

CAnimCharacterNode* CAnimCharacterSet::nodeByName(const std::string& name)
{
    auto iter = std::find_if(m_characterNodes.begin(), m_characterNodes.end(), [&name](CAnimCharacterNode* node)->bool
        {return node->name() == name;});

    if (iter != m_characterNodes.end())
        return *iter;

    return nullptr;
}

CAnimCharacterNode*CAnimCharacterSet::nodeByIndex(const atUint32 idx)
{
    if (idx >= m_characterNodes.size())
        return nullptr;

    return m_characterNodes[idx];
}

