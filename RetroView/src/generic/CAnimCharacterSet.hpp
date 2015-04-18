#ifndef CANIMCHARACTERSET_HPP
#define CANIMCHARACTERSET_HPP

#include "core/IResource.hpp"
#include "generic/CAnimCharacterNode.hpp"
#include <algorithm>
#include <memory>

enum EActionNodeType
{
    eActionPrimitive,
    eActionBlend,
    eActionPhaseBlend,
    eActionRandom,
    eActionSequence
};

enum ETransitionType
{
    eTransitionAction,
    eTransitionInterpolate,
    eTransitionInterpolateAlt,
    eTransitionCut,
};

struct SActionBase
{
};

struct SActionLeaf : SActionBase
{
    CUniqueID animID;
    atUint32 actionIndex;
    std::string name;
    atUint32 unk1;
    atUint32 unk2;
};

struct SActionNode : SActionBase
{
    ~SActionNode()
    {
        std::for_each(children.begin(), children.end(), std::default_delete<SActionBase>());
    }

    EActionNodeType           nodeType;
    std::vector<SActionBase*> children;
};

struct SActionBlend : SActionBase
{
    SActionNode node1;
    SActionNode node2;
    float       blendWeight;
    atUint8     unk;
};

struct SActionEntry
{
    std::string  name;
    SActionNode  root;
};

struct SInterpolate
{
    float time;
    atUint32 unk1;
    atUint8  unk2;
    atUint8  unk3;
    atUint32 unk4;
};

struct STransitionEntry
{
    inline STransitionEntry()
        : action(nullptr),
          interpolate(nullptr)
    {
    }

    ~STransitionEntry()
    {
        delete action;
        delete interpolate;
    }

    atUint32      startMarker;
    atUint32      actionIndexA;
    atUint32      actionIndexB;
    SActionNode*  action;
    SInterpolate* interpolate;
};

struct SAnimationEventPair
{
    CUniqueID anim;
    CUniqueID evnt;
};

class CAnimCharacterSet : public IResource
{
public:
    CAnimCharacterSet();
    ~CAnimCharacterSet();

    CAnimCharacterNode* nodeByName(const std::string& name);
    CAnimCharacterNode* nodeByIndex(const atUint32 idx);
private:
    friend class CAnimCharacterSetReader;
    std::vector<CAnimCharacterNode*> m_characterNodes;
    std::vector<SActionEntry>        m_actionEntries;
};

#endif // CANIMCHARACTERSET_HPP
