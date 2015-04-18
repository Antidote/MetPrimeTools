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

void CAreaBspTree::_drawNode(const SAreaBSPContext& context, const SOctantNodeEntry& node) const
{
    auto mreaMesh = context.area.m_models.begin();
    size_t idx = 0;
    for (bool df : *node.m_bitmap)
    {
        if (df && !context.staticBitmap[idx])
        {
            mreaMesh->drawIbos(context.transparents, context.materialSet, context.modelMatrix);
            context.staticBitmap.set(idx);
        }
        ++mreaMesh;
        ++idx;
    }
}

void CAreaBspTree::_visitNodeX(const SAreaBSPContext& context, const SOctantNodeEntry& node, const SBoundingBox& nodeBox) const
{
    /* Check if node in frustum */
    if (!context.frustum.aabbFrustumTest(nodeBox))
        return;
    
    if (!node.m_childCount)
    {
        /* Leaf node */
        _drawNode(context, node);
        return;
    }
    
    /* Subdivide X */
    if (node.m_subdivFlags & SUB_X)
    {
        SBoundingBox posX, negX;
        nodeBox.splitX(posX, negX);
        if (context.viewVec.z > 0.0f)
        {
            _visitNodeY(context, node, posX, 1, 2);
            _visitNodeY(context, node, negX, 0, 2);
        }
        else
        {
            _visitNodeY(context, node, negX, 0, 2);
            _visitNodeY(context, node, posX, 1, 2);
        }
    }
    else
    {
        _visitNodeY(context, node, nodeBox, 0, 1);
    }
    
    /* Draw remaining meshes */
    _drawNode(context, node);
}

void CAreaBspTree::_visitNodeY(const SAreaBSPContext& context, const SOctantNodeEntry& node, const SBoundingBox& nodeBox,
                               int subdivOffset, int nextOffset) const
{
    /* Subdivide Y */
    if (node.m_subdivFlags & SUB_Y)
    {
        SBoundingBox posY, negY;
        nodeBox.splitY(posY, negY);
        int cNextOffset = nextOffset * 2;
        if (context.viewVec.y > 0.0f)
        {
            _visitNodeZ(context, node, posY, subdivOffset + nextOffset, cNextOffset);
            _visitNodeZ(context, node, negY, subdivOffset, cNextOffset);
        }
        else
        {
            _visitNodeZ(context, node, negY, subdivOffset, cNextOffset);
            _visitNodeZ(context, node, posY, subdivOffset + nextOffset, cNextOffset);
        }
    }
    else
    {
        _visitNodeZ(context, node, nodeBox, subdivOffset, nextOffset);
    }
}

void CAreaBspTree::_visitNodeZ(const SAreaBSPContext& context, const SOctantNodeEntry& node, const SBoundingBox& nodeBox,
                               int subdivOffset, int nextOffset) const
{
    /* Subdivide Z */
    if (node.m_subdivFlags & SUB_Z)
    {
        SBoundingBox posZ, negZ;
        nodeBox.splitZ(posZ, negZ);
        if (context.viewVec.x > 0.0f)
        {
            _visitNodeX(context, *node.m_childNodes[subdivOffset+nextOffset], posZ);
            _visitNodeX(context, *node.m_childNodes[subdivOffset], negZ);
        }
        else
        {
            _visitNodeX(context, *node.m_childNodes[subdivOffset], negZ);
            _visitNodeX(context, *node.m_childNodes[subdivOffset+nextOffset], posZ);
        }
    }
    else
    {
        _visitNodeX(context, *node.m_childNodes[subdivOffset], nodeBox);
    }
}

void CAreaBspTree::drawArea(CAreaFile& area, bool transparents,
                            CMaterialSet& materialSet, const CTransform& modelXf) const
{
    CGLViewer* g_viewer = CGLViewer::instance();
    CFrustum frustum;
    frustum.updatePlanes(g_viewer->view(), g_viewer->projection());
    CVector3f viewPos = g_viewer->cameraPosition();
    CVector3f viewVec = g_viewer->cameraVector();
    CWordBitmap staticBitmap, actorBitmap;
    
    SAreaBSPContext ctx =
    {
        area, transparents, materialSet, viewPos, viewVec, modelXf, frustum, staticBitmap, actorBitmap
    };
    
    /* Pre-draw (objects behind octree) */
    
    /* Draw (objects within octree) */
    const SOctantNodeEntry& rootNode = m_nodes[0];
    _visitNodeX(ctx, rootNode, m_boundingBox);
    
    
    /* Post-draw (objects in front of octree) */
    
}

