#ifndef CAREABSPTREE_HPP
#define CAREABSPTREE_HPP

#include <Athena/Types.hpp>
#include "core/SboundingBox.hpp"
#include "core/CWordBitmap.hpp"

#define CAREABSPTREE_NODE_MAX_CHILDREN 8

struct SOctantNodeEntry
{
    atUint16 m_bitmapIdx;
    atUint16 m_childCount;
    atUint16 m_childIdxs[CAREABSPTREE_NODE_MAX_CHILDREN];
    
    void readNodeEntry(Athena::io::IStreamReader& in);
};

class CAreaBspTree
{
public:
    void readAROT(Athena::io::IStreamReader& in);
    
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
