#ifndef CANIMCHARACTERNODE_HPP
#define CANIMCHARACTERNODE_HPP

#include <CUniqueID.hpp>
#include <CFourCC.hpp>
#include <string>
#include <vector>
#include "core/CPASDatabase.hpp"
#include "core/SBoundingBox.hpp"

struct SAction
{
    atUint32    id;
    atUint8     unk;
    std::string name;
};

struct SActionAABB
{
    std::string name;
    SBoundingBox aabb;
};

struct SActionExtentsMP2
{
    atUint32 id;
    SBoundingBox extents;
};

struct SSubEffectAttachement
{
    std::string name;
    CFourCC     type;
    CUniqueID   id;
    std::string name2;
    atUint32    unkIntMP2;
    float       unkFloat;
    atUint32    unkInt1;
    atUint32    unkInt2;
};

struct SEffectAttachement
{
    std::string name;
    std::vector<SSubEffectAttachement> subAttachments;
};

class CAnimCharacterNode
{
public:
    CAnimCharacterNode();
    ~CAnimCharacterNode();

    inline std::string name()  const { return m_name; }
    inline CUniqueID modelID() const { return m_modelId; }
    inline CUniqueID skinID()  const { return m_skinId; }

private:
    friend class CAnimCharacterSetReader;
    atUint32     m_id;
    atUint16     m_enumVal;
    std::string  m_name;
    CUniqueID    m_modelId;
    CUniqueID    m_skinId;
    CUniqueID    m_rigId;
    std::vector<SAction> m_actions;
    CPASDatabase* m_pasDatabase;
    std::vector<CUniqueID> m_particleIds;
    std::vector<CUniqueID> m_swooshIds;
    std::vector<CUniqueID> m_unkIds1;
    std::vector<CUniqueID> m_electricIds;
    std::vector<CUniqueID> m_unkIds2;
    std::vector<CUniqueID> m_unkIds3;
    atUint32               m_attachementMeta;
    std::vector<SActionAABB> m_actionAABBs;
    std::vector<SEffectAttachement> m_effectAttachments;
    CUniqueID m_modelOverride;
    CUniqueID m_skinOverride;
    std::vector<atUint32> m_actionIds;
    atUint32 m_unknown1;
    atUint8  m_unknown2;
    std::vector<SActionExtentsMP2> m_actionExtents;
};

#endif // CANIMCHARACTERNODE_HPP
