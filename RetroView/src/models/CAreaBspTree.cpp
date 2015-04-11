#include "models/CAreaBspTree.hpp"
#include "CFourCC.hpp"
#include <Athena/InvalidDataException.hpp>

static const CFourCC skAROTFourCC("AROT");

void SOctantNodeEntry::readNodeEntry(Athena::io::IStreamReader& in)
{
    m_bitmapIdx = in.readUint16();
    m_childCount = in.readUint16();
    if (m_childCount)
    {
        m_childCount += 1;
        unsigned i;
        for (i=0 ; i<m_childCount ; ++i)
            m_childIdxs[i] = in.readUint16();
    }
}

void CAreaBspTree::readAROT(Athena::io::IStreamReader& in)
{
    
    /* Read in AROT header */
    CFourCC magic(in);
    if (magic != skAROTFourCC)
        THROW_INVALID_DATA_EXCEPTION("Not valid AROT section, expected magic 'AROT' got %.4s\n", magic);
    
    m_version = in.readUint32();
    if (m_version != 1)
        THROW_INVALID_DATA_EXCEPTION("Only version 1 AROT supported, not %u\n", m_version);

    m_bitmapCount = in.readUint32();
    m_bitmapNumBits = in.readUint32();
    m_octantNodeCount = in.readUint32();
    m_boundingBox.readBoundingBox(in);
    in.seekAlign32();
    
    /* Read in bitmaps */
    m_bitmaps.clear();
    m_bitmaps.reserve(m_bitmapCount);
    
    unsigned i;
    for (i=0 ; i<m_bitmapCount ; ++i)
    {
        CWordBitmap bmp;
        bmp.readBitmapBits(in, m_bitmapNumBits);
        m_bitmaps.push_back(bmp);
    }
    
    /* Read in temporary indirection table */
    atUint32 nodeIndTable[m_octantNodeCount];
    for (i=0 ; i<m_octantNodeCount ; ++i)
        nodeIndTable[i] = in.readUint32();
    
    /* Read in nodes */
    m_nodes.clear();
    m_nodes.reserve(m_octantNodeCount);
    atUint64 nodeBaseOff = in.position();
    for (i=0 ; i<m_octantNodeCount ; ++i)
    {
        SOctantNodeEntry entry;
        in.seek(nodeBaseOff + nodeIndTable[i], Athena::SeekOrigin::Begin);
        entry.readNodeEntry(in);
        m_nodes.push_back(entry);
    }
    
}

