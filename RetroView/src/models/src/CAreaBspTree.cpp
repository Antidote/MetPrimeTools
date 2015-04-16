#include "models/CAreaBspTree.hpp"
#include "CFourCC.hpp"
#include "models/CAreaFile.hpp"
#include "ui/CGLViewer.hpp"
#include <Athena/InvalidDataException.hpp>
#if __SSE__
#include <immintrin.h>
#endif

static const CFourCC skAROTFourCC("AROT");

void SOctantNodeEntry::readNodeEntry(Athena::io::IStreamReader& in)
{
    m_bitmapIdx = in.readUint16();
    m_subdivFlags = (EAROTSubdivFlags)in.readUint16();
    if (m_subdivFlags)
    {
        m_childCount = 1;
        if (m_subdivFlags & SUB_Z)
            m_childCount *= 2;
        if (m_subdivFlags & SUB_Y)
            m_childCount *= 2;
        if (m_subdivFlags & SUB_X)
            m_childCount *= 2;
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
    atUint32* nodeIndTable = new atUint32[m_octantNodeCount];
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
        assert(entry.m_bitmapIdx < m_bitmapCount);
        entry.m_bitmap = &m_bitmaps[entry.m_bitmapIdx];
        m_nodes.push_back(entry);
    }
    
    delete[] nodeIndTable;
    
    /* Resolve octant links */
    for (SOctantNodeEntry& node : m_nodes)
        for (i=0 ; i<node.m_childCount ; ++i)
            node.m_childNodes[i] = &m_nodes[node.m_childIdxs[i]];
    
}

static const CVector3f skZeroVec(0.0);
static const CVector3f skTwoVec(2.0);

static inline unsigned vec3_cmpgt(const CVector3f& lval, const CVector3f& rval)
{
    unsigned res = 0;
    if (lval[0] > rval[0])
        res |= 1 << 0;
    if (lval[1] > rval[1])
        res |= 1 << 1;
    if (lval[2] > rval[2])
        res |= 1 << 2;
    return res;
}
#define X_CMP(cmp) ((cmp) & 0x1)
#define Y_CMP(cmp) ((cmp) & 0x2)
#define Z_CMP(cmp) ((cmp) & 0x4)

void CAreaBspTree::_visitNodeX(const SAreaBSPContext& context, const SOctantNodeEntry& node, const SBoundingBox& nodeBox) const
{
    
}
void CAreaBspTree::_visitNodeY(const SAreaBSPContext& context, const SOctantNodeEntry& node, const SBoundingBox& nodeBox) const
{
    
}
void CAreaBspTree::_visitNodeZ(const SAreaBSPContext& context, const SOctantNodeEntry& node, const SBoundingBox& nodeBox) const
{
    
}

void CAreaBspTree::drawArea(CAreaFile& area, bool transparents,
                            CMaterialSet& materialSet, const CTransform& modelXf) const
{
    CGLViewer* g_viewer = CGLViewer::instance();
    CVector3f viewPos = g_viewer->cameraPosition();
    CVector3f viewVec = g_viewer->cameraVector();
    
    SAreaBSPContext ctx =
    {
        area, transparents, materialSet, viewPos, viewVec, modelXf
    };
    
    /* Pre-draw (objects behind octree) */
    
    /* Draw (objects within octree) */
    _visitNodeX(ctx, m_nodes[0], m_boundingBox);
    
    
    /* Post-draw (objects in front of octree) */
    
}

