#ifndef CAREABSPTREE_HPP
#define CAREABSPTREE_HPP

#include <Athena/Types.hpp>
#include "core/SBoundingBox.hpp"
#include "core/CWordBitmap.hpp"

class CAreaFile;

#define CAREABSPTREE_NODE_MAX_CHILDREN 8

struct SOctantNodeEntry
{
    atUint16 m_bitmapIdx = 0;
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
    void drawArea(const CAreaFile& area, bool transparents) const;
    
private:
    friend class CAreaReader;
    atUint32 m_version = 0;
    atUint32 m_bitmapCount = 0;
    atUint32 m_bitmapNumBits = 0;
    atUint32 m_octantNodeCount = 0;
    SBoundingBox m_boundingBox;
    
    std::vector<CWordBitmap> m_bitmaps;
    std::vector<SOctantNodeEntry> m_nodes;
    
};

#endif // CAREABSPTREE_HPP
