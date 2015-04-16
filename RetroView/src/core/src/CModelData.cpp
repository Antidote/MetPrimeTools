#include "core/CResourceManager.hpp"
#include "core/CModelData.hpp"
#include "core/CMaterialCache.hpp"
#include "core/GXCommon.hpp"
#include "generic/CTexture.hpp"
#include "ui/CGLViewer.hpp"


CModelData::CModelData()
{
}

CModelData::~CModelData()
{
}

atUint32 CModelData::exportUVIdx(atUint32 texOff, SVertexDescriptor desc, CMaterial& mat, CMesh& mesh)
{
    atUint32 ret;
    if ((mat.materialFlags() & 0x2000) || (mesh.m_uvSource == 1))
        ret = desc.texCoord[1] + texOff;
    else
        ret = desc.texCoord[0] + texOff;

    return ret;
}

void CModelData::exportModel(std::ofstream& of, atUint32& vertexOff, atUint32& normalOff, atUint32& texOff, CMaterialSet& ms)
{
    CTransform rotMat = CTransformFromAxisAngle(CVector3f(-1, 0, 0), Math::degToRad(90.f));
    CTransform inverseTransform = m_transform.inverse();

    of << "# vertex positions" << std::endl;
    for (CVector3f v : m_vertices)
    {
        CVector3f rv = rotMat * inverseTransform * v;
        of << "v " << rv.x << " " << rv.y << " " << rv.z << std::endl;
    }
    of << std::endl << std::endl;

    of << "# texture coordinates" << std::endl;
    for (CVector3f uv : m_texCoords0)
    {
        uv /= 1.f;
        of << "vt " << uv.x << " " << uv.y << std::endl;
    }
    of << std::endl << std::endl;

    of << "# vertex normals" << std::endl;
    for (CVector3f n : m_normals)
    {
        of << "vn " << n.y << " " << n.x << " " << n.z << std::endl;
    }
    of << std::endl << std::endl;

    int meshIndex = 0;

    for (CMesh m : m_meshes)
    {
        CMaterial& mat = ms.material(m.m_materialID);

        of << "g mesh_" << meshIndex << std::endl;
        meshIndex++;

        for (CPrimitive s : m.m_primitives)
        {
            switch (s.primitive)
            {
                case EPrimitive::Lines:
                {
                    for (atUint32 i = 1 ; i < s.indices.size(); i += 2)
                    {
                        SVertexDescriptor& f0 = s.indices[i - 1];
                        SVertexDescriptor& f1 = s.indices[i - 0];

                        of << "l "
                           << " " << f0.position + vertexOff << "/" << exportUVIdx(texOff, f0, mat, m)
                           << " " << f1.position + vertexOff << "/" << exportUVIdx(texOff, f1, mat, m) << std::endl;
                    }
                }
                    break;
                case EPrimitive::LineStrip:
                {
                    for (atUint32 i = 1 ; i < s.indices.size(); ++i)
                    {
                        SVertexDescriptor& f0 = s.indices[i - 1];
                        SVertexDescriptor& f1 = s.indices[i - 0];

                        of << "l"
                           << " " << f0.position + vertexOff << "/" << exportUVIdx(texOff, f0, mat, m)
                           << " " << f1.position + vertexOff << "/" << exportUVIdx(texOff, f1, mat, m) << std::endl;
                    }
                }
                    break;
                case EPrimitive::Triangles:
                {
                    for (atUint32 i = 2 ; i < s.indices.size(); i += 3)
                    {
                        SVertexDescriptor& f0 = s.indices[i - 2];
                        SVertexDescriptor& f1 = s.indices[i - 1];
                        SVertexDescriptor& f2 = s.indices[i - 0];

                        of << "f"
                           << " " << f0.position + vertexOff << "/" << exportUVIdx(texOff, f0, mat, m) << "/" << f0.normal + normalOff
                           << " " << f1.position + vertexOff << "/" << exportUVIdx(texOff, f1, mat, m) << "/" << f1.normal + normalOff
                           << " " << f2.position + vertexOff << "/" << exportUVIdx(texOff, f2, mat, m) << "/" << f2.normal + normalOff << std::endl;
                    }
                }
                    break;
                case EPrimitive::TriangleStrip:
                {
                    for (atUint32 i = 2 ; i < s.indices.size(); ++i)
                    {
                        SVertexDescriptor& f0 = s.indices[i - 2];
                        SVertexDescriptor& f1 = s.indices[i - (1 - (i & 1))];
                        SVertexDescriptor& f2 = s.indices[i - ((i & 1) - 0)];

                        of << "f"
                           << " " << f0.position + vertexOff << "/" << exportUVIdx(texOff, f0, mat, m) << "/" << f0.normal + normalOff
                           << " " << f1.position + vertexOff << "/" << exportUVIdx(texOff, f1, mat, m) << "/" << f1.normal + normalOff
                           << " " << f2.position + vertexOff << "/" << exportUVIdx(texOff, f2, mat, m) << "/" << f2.normal + normalOff << std::endl;
                    }
                }
                    break;
                case EPrimitive::TriangleFan:
                {
                    SVertexDescriptor& f0 = s.indices[0];
                    for (atUint32 i = 2 ; i < s.indices.size(); ++i)
                    {
                        SVertexDescriptor& f1 = s.indices[i - 1];
                        SVertexDescriptor& f2 = s.indices[i - 0];

                        of << "f"
                           << " " << f0.position + vertexOff << "/" << exportUVIdx(texOff, f0, mat, m) << "/" << f0.normal + normalOff
                           << " " << f1.position + vertexOff << "/" << exportUVIdx(texOff, f1, mat, m) << "/" << f1.normal + normalOff
                           << " " << f2.position + vertexOff << "/" << exportUVIdx(texOff, f2, mat, m) << "/" << f2.normal + normalOff << std::endl;
                    }
                }
                    break;
                case EPrimitive::Quads:
                {
                    for (atUint32 i = 3 ; i < s.indices.size(); i += 4)
                    {
                        SVertexDescriptor& f0 = s.indices[i - 3];
                        SVertexDescriptor& f1 = s.indices[i - 2];
                        SVertexDescriptor& f2 = s.indices[i - 1];
                        SVertexDescriptor& f3 = s.indices[i - 0];

                        of << "f"
                           << " " << f0.position + vertexOff << "/" << exportUVIdx(texOff, f0, mat, m) << "/" << f0.normal + normalOff
                           << " " << f1.position + vertexOff << "/" << exportUVIdx(texOff, f1, mat, m) << "/" << f1.normal + normalOff
                           << " " << f2.position + vertexOff << "/" << exportUVIdx(texOff, f2, mat, m) << "/" << f2.normal + normalOff << std::endl;
                        of << "f"
                           << " " << f0.position + vertexOff << "/" << exportUVIdx(texOff, f0, mat, m) << "/" << f0.normal + normalOff
                           << " " << f2.position + vertexOff << "/" << exportUVIdx(texOff, f2, mat, m) << "/" << f2.normal + normalOff
                           << " " << f3.position + vertexOff << "/" << exportUVIdx(texOff, f3, mat, m) << "/" << f3.normal + normalOff << std::endl;
                    }
                }
                    break;
                default:
                {
                    for (atUint32 i = 0 ; i < s.indices.size(); ++i)
                    {
                        SVertexDescriptor& f0 = s.indices[i];
                        of << "f " << f0.position + vertexOff << " " << f0.position + vertexOff << " " << f0.position + vertexOff << std::endl;
                    }
                }
                    break;
            }
        }
    }

    vertexOff += m_vertices.size();
    normalOff += m_normals.size();
    texOff    += m_texCoords0.size();
}

void CModelData::drawIbos(bool transparents, const CMaterialSet& materialSet, const CTransform& model)
{
    std::unordered_map<atUint32, CIndexBuffer> ibos;
    if (transparents)
    {
        ibos = m_transparents;
        //sortTransparent(ibos);
    }
    else
        ibos = m_opaques;

    m_vertexBuffer.initBuffer();
    m_vertexBuffer.bind();
    for (std::pair<atUint32, CIndexBuffer> iboPair : ibos)
    {
        CMaterial& mat = materialSet.material(iboPair.first);

        if (mat.materialFlags() & Occluder)
            continue;

        mat.setModel(model);

        if (!mat.bind())
            continue;

        iboPair.second.initBuffer();
        iboPair.second.bindBuffer();
        iboPair.second.drawBuffer();

        mat.release();
    }
}

void CModelData::drawTransparentBoxes()
{
//    for (std::pair<atUint32, std::vector<SIndexBufferObject> > iboPair : m_transparents)
//    {
//        for (SIndexBufferObject ibo : iboPair.second)
//            drawBoundingBox(ibo.boundingBox);
    //    }
}

void CModelData::drawBoundingBoxes()
{
    drawBoundingBox(m_boundingBox);
    for (const CMesh& mesh : m_meshes)
        drawBoundingBox(mesh.boundingBox());
}
