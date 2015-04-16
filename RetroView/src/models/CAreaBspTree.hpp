#ifndef CAREABSPTREE_HPP
#define CAREABSPTREE_HPP

#include <Athena/Types.hpp>
#include "core/SBoundingBox.hpp"
#include "core/CWordBitmap.hpp"
#include "core/CMaterialSet.hpp"

class CAreaFile;

#define CAREABSPTREE_NODE_MAX_CHILDREN 8

enum EAROTSubdivFlags : atUint16
{
    SUB_NONE = 0x0,
    SUB_Z = 0x1,
    SUB_Y = 0x2,
    SUB_X = 0x4
};

struct SOctantNodeEntry
{
    atUint16 m_bitmapIdx = 0;
    EAROTSubdivFlags m_subdivFlags = EAROTSubdivFlags::SUB_NONE;
    atUint16 m_childCount = 0;
    atUint16 m_childIdxs[CAREABSPTREE_NODE_MAX_CHILDREN];
    void readNodeEntry(Athena::io::IStreamReader& in);
    
    CWordBitmap* m_bitmap = NULL;
    SOctantNodeEntry* m_childNodes[CAREABSPTREE_NODE_MAX_CHILDREN] = {NULL};
    
};

class CAreaBspTree
{
public:
    void readAROT(Athena::io::IStreamReader& in);
    void drawArea(CAreaFile& area, bool transparents,
                  CMaterialSet& materialSet, const CTransform& model) const;

private:
    friend class CAreaReader;
    atUint32 m_version = 0;
    atUint32 m_bitmapCount = 0;
    atUint32 m_bitmapNumBits = 0;
    atUint32 m_octantNodeCount = 0;
    SBoundingBox m_boundingBox;
    
    std::vector<CWordBitmap> m_bitmaps;
    std::vector<SOctantNodeEntry> m_nodes;
    
    struct SAreaBSPContext
    {
        CAreaFile& area;
        bool transparents;
        const CMaterialSet& materialSet;
        const CVector3f& viewPos;
        const CVector3f& viewVec;
        const CTransform& modelMatrix;
    };
    
    void _visitNodeX(const SAreaBSPContext& context, const SOctantNodeEntry& node, const SBoundingBox& nodeBox) const;
    void _visitNodeY(const SAreaBSPContext& context, const SOctantNodeEntry& node, const SBoundingBox& nodeBox) const;
    void _visitNodeZ(const SAreaBSPContext& context, const SOctantNodeEntry& node, const SBoundingBox& nodeBox) const;

};

#endif // CAREABSPTREE_HPP
