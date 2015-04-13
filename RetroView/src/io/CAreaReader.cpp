#include "io/CAreaReader.hpp"
#include "core/CMaterialSet.hpp"
#include "io/CMaterialReader.hpp"
#include "models/CAreaFile.hpp"
#include "core/GXCommon.hpp"

#include <RetroCommon.hpp>
#include <Athena/MemoryWriter.hpp>
#include <Athena/InvalidDataException.hpp>

#include <algorithm>
#include <map>
#include <glm/glm.hpp>

CAreaReader::CAreaReader(const atUint8 *data, atUint64 length)
    : base(data, length),
      m_sectionReader(new atUint8[2], 2)
{
    base::setEndian(Athena::Endian::BigEndian);
}

CAreaReader::CAreaReader(const std::string &filepath)
    : base(filepath),
      m_sectionReader(new atUint8[2], 2)
{
    base::setEndian(Athena::Endian::BigEndian);
}

CAreaReader::~CAreaReader()
{
}

CAreaFile* CAreaReader::read()
{
    {
        Athena::io::MemoryWriter out;
        if (decompressMREA(*this, out))
            setData(out.data(), out.length());
    }

    seek(0, Athena::SeekOrigin::Begin);

    atUint32 magic   = base::readUint32();
    atUint32 version = base::readUint32();

    if (magic != 0xDEADBEEF)
        THROW_INVALID_DATA_EXCEPTION("Not valid MREA file, expected magic 0xDEADBEEF got %.8X\n", magic);

    if (version != CAreaFile::MetroidPrimeDemo && version != CAreaFile::MetroidPrime1 && version != CAreaFile::MetroidPrime2
            && version != CAreaFile::MetroidPrime3 && version != CAreaFile::DKCR)
        THROW_INVALID_DATA_EXCEPTION("Unsupported version, got %i\n", version);

    CAreaFile* ret = new CAreaFile;
    ret->m_version = (CAreaFile::Version)version;

    for (int i= 0; i < 3; i++)
    {
        ret->m_transformMatrix[i].x = base::readFloat();
        ret->m_transformMatrix[i].y = base::readFloat();
        ret->m_transformMatrix[i].z = base::readFloat();
        ret->m_transformMatrix[i].w = base::readFloat();
    }

    atUint32 modelCount = base::readUint32();
    // This is also valid for MP2 and DKCR
    atUint32 sclyLayerCount = 1;
    if (version != CAreaFile::MetroidPrime1 && ret->m_version != CAreaFile::MetroidPrimeDemo)
        atUint32 sclyLayerCount = base::readUint32();

    atUint32 sectionCount = base::readUint32();

    if (version == CAreaFile::MetroidPrimeDemo || version == CAreaFile::MetroidPrime1 ||
            version == CAreaFile::MetroidPrime2)
    {
        m_materialSection  = base::readUint32();
        m_sclySection      = base::readUint32();
        if (version == CAreaFile::MetroidPrime2)
            m_scgnSection  = base::readUint32();
        m_collisionSection = base::readUint32();
        m_unknownSection   = base::readUint32();
        m_lightSection     = base::readUint32();
        m_visiSection      = base::readUint32();
        m_pathSection      = base::readUint32();
        m_arotSection      = base::readUint32();
        if (version == CAreaFile::MetroidPrime2)
        {
            m_arotSection -= 2;
            m_ptlaSection  = base::readUint32();
            m_egmcSection  = base::readUint32();
        }
    }

    atUint32 compressedBlockCount = 0;
    if (version != CAreaFile::MetroidPrime1 && ret->m_version != CAreaFile::MetroidPrimeDemo)
        compressedBlockCount = base::readUint32();
    atUint32 sectionIndexCount = 0;
    if (version == CAreaFile::MetroidPrime3 || version == CAreaFile::DKCR)
        sectionIndexCount = base::readUint32();

    base::seekAlign32();

    m_sectionSizes.resize(sectionCount);
    for (atUint32 i = 0; i < sectionCount; i++)
        m_sectionSizes[i] = base::readUint32();

    base::seekAlign32();

    if (version != CAreaFile::MetroidPrime1 && version != CAreaFile::MetroidPrimeDemo)
    {
        for (atUint32 i = 0; i < compressedBlockCount; i++)
        {
            base::readUint32(); // Block size
            base::readUint32(); // Data size
            base::readUint32(); // Compressed size
            base::readUint32(); // Section Count
        }
    }

    base::seekAlign32();

    if (version == CAreaFile::MetroidPrime3 || version == CAreaFile::DKCR)
    {
        m_sectionIndices.resize(sectionIndexCount);
        for (atUint32 i = 0; i < sectionIndexCount; i++)
        {
            atUint32 tmp = base::readUint32();
            Athena::utility::BigUint32(tmp);
            m_sectionIndices[i].tag = std::string(((char*)&tmp), 4);
            m_sectionIndices[i].index = base::readUint32();
        }
    }

    base::seekAlign32();

    m_modelMeshOffsets.resize(modelCount);

    if (version == CAreaFile::MetroidPrime3 || version == CAreaFile::DKCR)
        readSectionsMP3DKCR(ret);
    else
        readSections(ret);

    return ret;
}

IResource* CAreaReader::loadByData(const atUint8* data, atUint64 length)
{
    return CAreaReader(data, length).read();
}

void CAreaReader::readSections(CAreaFile* ret)
{
    m_sectionReader.setEndian(endian());
    for (atUint32 i = 0; i < m_sectionSizes.size(); i++)
    {
        atUint64 sectionStart = base::position();
        bool needsAdvance = true;

        if (i == 0)
        {
            CMaterial::Version matVer = (ret->m_version == CAreaFile::MetroidPrime1 || ret->m_version == CAreaFile::MetroidPrimeDemo ?
                                             CMaterial::MetroidPrime1 : CMaterial::MetroidPrime2);

            atUint8* data = base::readUBytes(m_sectionSizes[i]);
            CMaterialReader matReader(data, m_sectionSizes[i]);
            ret->m_materialSets.push_back(matReader.read(matVer));
        }
        else if ((i - 1) < m_modelMeshOffsets.size())
        {
            for (atUint32 j = 0; j < m_modelMeshOffsets.size(); j++)
                readModelHeader(ret, sectionStart, i);
            --i; /* Reverses the final increment (which this loop performs anyway) */
            needsAdvance = false;
        }
        else if (i == m_arotSection)
        {
            atUint8* data = base::readUBytes(m_sectionSizes[i]);
            m_sectionReader.setData(data, m_sectionSizes[i]);
            ret->m_bspTree.readAROT(m_sectionReader);
        }
        else if (i == m_sclySection)
        {
            atUint8* data = base::readUBytes(m_sectionSizes[i]);
            m_sectionReader.setData(data, m_sectionSizes[i]);
            readSCLY(ret, m_sectionReader);
        }
        
        if (needsAdvance)
            base::seek(sectionStart + m_sectionSizes[i], Athena::SeekOrigin::Begin);
    }
}

void CAreaReader::readSectionsMP3DKCR(CAreaFile* ret)
{
    m_sectionReader.setEndian(endian());

    for (atUint32 i = 0; i < m_sectionSizes.size(); i++)
    {
        atUint64 sectionStart = base::position();
        bool needsAdvance = true;
        if (i == 0)
        {
            atUint8* data = base::readUBytes(m_sectionSizes[i]);
            CMaterialReader matReader(data, m_sectionSizes[i]);
            ret->m_materialSets.push_back(matReader.read(CMaterial::MetroidPrime3));
        }
        else if (i == 1)
        {
            for (atUint32 m = 0; m < m_modelMeshOffsets.size(); m++)
                readModelHeader(ret, sectionStart, i);
            --i; /* Reverses the final increment (which this loop performs anyway) */
            needsAdvance = false;
        }
        else
        {
            std::vector<SAreaSectionIndex>::iterator iter = std::find_if(m_sectionIndices.begin(), m_sectionIndices.end(),
                                                                         [&i](SAreaSectionIndex si)->bool{return si.index == i; });

            if (iter != m_sectionIndices.end())
            {
                SAreaSectionIndex idx = *iter;
                if (!idx.tag.compare("ROCT"))
                {
                    atUint8* data = base::readUBytes(m_sectionSizes[i]);
                    m_sectionReader.setData(data, m_sectionSizes[i]);
                    ret->m_bspTree.readAROT(m_sectionReader);
                }
                else if (!idx.tag.compare("AABB"))
                {
                    atUint8* data = base::readUBytes(m_sectionSizes[i]);
                    m_sectionReader.setData(data, m_sectionSizes[i]);
                    readAABB(ret, m_sectionReader);
                }
                else if (!idx.tag.compare("GPUD"))
                {
                    for (atUint32 m = 0; m < ret->m_models.size(); m++)
                    {
                        CModelData& model = ret->m_models[m];
                        readModelData(ret, model, sectionStart, i);
                        readMeshes(ret, model, sectionStart, i, m);
                    }
                }
            }
        }

        if (needsAdvance)
            base::seek(sectionStart + m_sectionSizes[i], Athena::SeekOrigin::Begin);
    }
}

void CAreaReader::readModelHeader(CAreaFile* ret, atUint64& sectionStart, atUint32& i)
{
    ret->m_models.push_back(CModelData());
    CModelData& model = ret->m_models.back();

    base::readUint32(); // not really useful

    for (atUint32 j = 0; j < 3; j++)
    {
        model.m_transform[j].x = base::readFloat();
        model.m_transform[j].y = base::readFloat();
        model.m_transform[j].z = base::readFloat();
        model.m_transform[j].w = base::readFloat();
    }

    model.m_boundingBox.min.x = base::readFloat();
    model.m_boundingBox.min.y = base::readFloat();
    model.m_boundingBox.min.z = base::readFloat();
    model.m_boundingBox.max.x = base::readFloat();
    model.m_boundingBox.max.y = base::readFloat();
    model.m_boundingBox.max.z = base::readFloat();

    base::seekAlign32();
    sectionStart = base::position();
    i++;

    if (ret->m_version == CAreaFile::MetroidPrimeDemo || ret->m_version == CAreaFile::MetroidPrime1 ||
            ret->m_version == CAreaFile::MetroidPrime2)
        readModelData(ret, model, sectionStart, i);

    atUint32 meshCount = base::readUint32();
    while((meshCount--) > 0)
        m_modelMeshOffsets[ret->m_models.size() - 1].push_back(base::readUint32());

    base::seekAlign32();
    sectionStart = base::position();
    i++;

    if (ret->m_version == CAreaFile::MetroidPrimeDemo ||  ret->m_version == CAreaFile::MetroidPrime1 ||
            ret->m_version == CAreaFile::MetroidPrime2)
        readMeshes(ret, model, sectionStart, i, ret->m_models.size() - 1);

    if (ret->m_version != CAreaFile::MetroidPrimeDemo && ret->m_version != CAreaFile::MetroidPrime1)
    {
        base::seek(sectionStart + m_sectionSizes[i], Athena::SeekOrigin::Begin);
        sectionStart = base::position();
        i++;

        base::seek(sectionStart + m_sectionSizes[i], Athena::SeekOrigin::Begin);
        sectionStart = base::position();
        i++;
    }
}

void CAreaReader::readModelData(CAreaFile* ret, CModelData& model, atUint64& sectionStart, atUint32& i)
{
    atUint8* data = base::readUBytes(m_sectionSizes[i]);
    m_sectionReader.setData(data, m_sectionSizes[i]);
    readVertices(model, m_sectionReader);
    sectionStart = base::position();
    i++;

    data = base::readUBytes(m_sectionSizes[i]);
    m_sectionReader.setData(data, m_sectionSizes[i]);
    readNormals(model, m_sectionReader);
    sectionStart = base::position();
    i++;

    data = base::readUBytes(m_sectionSizes[i]);
    m_sectionReader.setData(data, m_sectionSizes[i]);
    readColors(model, m_sectionReader);
    sectionStart = base::position();
    i++;

    data = base::readUBytes(m_sectionSizes[i]);
    m_sectionReader.setData(data, m_sectionSizes[i]);
    readTexCoords(model, m_sectionReader, false);
    sectionStart = base::position();
    i++;

    // Something else goes here in DKCR, is it a short UV table?
    if (ret->m_version == CAreaFile::MetroidPrimeDemo || ret->m_version == CAreaFile::MetroidPrime1 ||
            ret->m_version == CAreaFile::MetroidPrime2 || ret->m_version == CAreaFile::DKCR)
    {
        data = base::readUBytes(m_sectionSizes[i]);
        m_sectionReader.setData(data, m_sectionSizes[i]);
        readTexCoords(model, m_sectionReader, true);
        sectionStart = base::position();
        i++;
    }
}

void CAreaReader::readAABB(CAreaFile* file, Athena::io::MemoryReader& in)
{
    atUint32 aabbCount = in.readUint32();
    std::cout << aabbCount << std::endl;
    SAABB rootAABB;
    while ((aabbCount--) > 0)
    {
        SAABB aabb;
        aabb.aabb.min.x = in.readFloat();
        aabb.aabb.min.y = in.readFloat();
        aabb.aabb.min.z = in.readFloat();
        aabb.aabb.max.x = in.readFloat();
        aabb.aabb.max.y = in.readFloat();
        aabb.aabb.max.z = in.readFloat();
        aabb.unkIndex = in.readInt32();
        aabb.selfIndex1 = in.readInt16();
        aabb.selfIndex2 = in.readInt16();
        /*
        if (aabb.selfIndex1 != -1)
        {
            std::cout << "rootaabb" << std::endl;
            if (rootAABB.aabb != SBoundingBox())
                file->m_aabbs.push_back(rootAABB);

            rootAABB = aabb;
        }
        else
        {
            rootAABB.aabb.min += aabb.aabb.min;
            rootAABB.aabb.max += aabb.aabb.max;
        }*/
        file->m_aabbs.push_back(aabb);
    }
    //file->m_aabbs.push_back(rootAABB);
}

void CAreaReader::readVertices(CModelData& model, Athena::io::MemoryReader& in)
{
    atInt32 vertexCount = in.length() / sizeof(glm::vec3);

    while ((vertexCount--) > 0)
    {
        glm::vec3 vert;
        vert.x = in.readFloat();
        vert.y = in.readFloat();
        vert.z = in.readFloat();
        model.m_vertices.push_back(vert);
    }
}

void CAreaReader::readNormals(CModelData& model, Athena::io::MemoryReader& in)
{
    atInt32 normalCount = in.length() / (sizeof(atUint16) * 3);
    while ((normalCount--) > 0)
    {
        glm::vec3 nrm;
        nrm.x = in.readInt16() / 32768.f; // really shouldn't do this here
        nrm.y = in.readInt16() / 32768.f; // but it's constant enough to be reliable
        nrm.z = in.readInt16() / 32768.f;
        model.m_normals.push_back(nrm);
    }
}

void CAreaReader::readColors(CModelData& model, Athena::io::MemoryReader& in)
{
    atInt32 colorCount = in.length() / sizeof(atUint32);

    while ((colorCount--) > 0)
        model.m_colors.push_back(in.readUint32());
}

void CAreaReader::readTexCoords(CModelData& model, Athena::io::MemoryReader& in, bool isLightmap)
{
    if (isLightmap)
    {
        atInt32 texCoordCount = in.length() / (sizeof(atUint16) * 2);
        while ((texCoordCount--) > 0)
        {
            glm::vec2 texCoord;
            texCoord.s = (float)(in.readInt16() / 32768.f); // really shouldn't do this here
            texCoord.t = (float)(in.readInt16() / 32768.f); // but it's constant enough to be reliable
            model.m_texCoords1.push_back(texCoord);
        }
    }
    else
    {
        atUint32 texCoordCount = in.length() / sizeof(glm::vec2);

        while ((texCoordCount--) > 0)
        {
            glm::vec2 texCoord;
            texCoord.s = in.readFloat();
            texCoord.t = in.readFloat();
            model.m_texCoords0.push_back(texCoord);
        }
    }
}

void CAreaReader::readMesh(CModelData& model, CAreaFile* ret, Athena::io::MemoryReader& in)
{
    model.m_meshes.push_back(CMesh());
    CMesh& mesh = model.m_meshes.back();

    mesh.m_pivot.x    = in.readFloat();
    mesh.m_pivot.y    = in.readFloat();
    mesh.m_pivot.z    = in.readFloat();
    if (ret->m_version != CAreaFile::DKCR)
    {
        mesh.m_materialID = in.readUint32();
        mesh.m_mantissa   = in.readUint16();
        in.readUint16();
        atUint32 extraDataSize = 0;
        if (ret->m_version == CAreaFile::MetroidPrime3)
        {
            in.readUint32();
            in.readUint32();
            extraDataSize = in.readUint32();
            in.readUint32();
            in.readUint32();
            in.readUint32();
            in.readUint16();
            in.readUint16();
        }
        else
        {
            atUint32 assertTest;
            assertTest = in.readUint32();
            assert(assertTest == 0);
            assertTest = in.readUint32();
            assert(assertTest == 0);
            extraDataSize = in.readUint32();
            for (atUint32 i = 0; i < 3; i++)
                mesh.m_reflectionDirection[i] = in.readFloat();
        }

        if (ret->m_version == CAreaFile::MetroidPrime2)
        {
            in.readUint16();
            in.readUint16();
        }

        if (extraDataSize > 0)
        {
            mesh.m_boundingBox.min.x = in.readFloat();
            mesh.m_boundingBox.min.y = in.readFloat();
            mesh.m_boundingBox.min.z = in.readFloat();
            mesh.m_boundingBox.max.x = in.readFloat();
            mesh.m_boundingBox.max.y = in.readFloat();
            mesh.m_boundingBox.max.z = in.readFloat();
        }
    }
    else
    {
        mesh.m_mantissa = in.readUint16();
        in.readUint16();
        in.readUint32();
        in.readUint32();
        in.readUint16();
        mesh.m_materialID = in.readUint16();
        in.readUint16();
        mesh.m_uvSource = in.readUByte();
        in.readUByte();
        mesh.m_boundingBox.min.x = in.readFloat();
        mesh.m_boundingBox.min.y = in.readFloat();
        mesh.m_boundingBox.min.z = in.readFloat();
        mesh.m_boundingBox.max.x = in.readFloat();
        mesh.m_boundingBox.max.y = in.readFloat();
        mesh.m_boundingBox.max.z = in.readFloat();
    }

    in.seekAlign32();

    const CMaterial& material = ret->m_materialSets[0].material(mesh.m_materialID);

    readPrimitives(mesh, model, material, in);
}

void CAreaReader::readMeshes(CAreaFile* ret, CModelData& model, atUint64& sectionStart, atUint32& i, atUint32 m)
{
    atUint32 meshCount = m_modelMeshOffsets[m].size();
    atUint64 meshStart = base::position();

    for (atUint32 s = 0; s < meshCount; s++)
    {
        atUint8* data = base::readUBytes(m_sectionSizes[i]);

        m_sectionReader.setData(data, m_sectionSizes[i]);
        readMesh(model, ret, m_sectionReader);
        base::seek(meshStart + m_modelMeshOffsets[m][s], Athena::SeekOrigin::Begin);
        sectionStart = base::position();
        i++;
    }
}

static const CFourCC skSCLYFourCC("SCLY");

void CAreaReader::readSCLY(CAreaFile* ret, Athena::io::MemoryReader& in)
{
    if (ret->m_version == CAreaFile::MetroidPrime1)
    {
        CFourCC magic(in);
        if (magic != skSCLYFourCC)
            return;

        atUint32 version = in.readUint32();
        EScriptVersion scriptVersion;
        if (version != 1)
            return;

        scriptVersion = eSCLY_MetroidPrime1;

        atUint32 layerCount = in.readUint32();

        std::vector<atUint32> layerSizes;
        layerSizes.reserve(layerCount);
        while ((layerCount--) > 0)
            layerSizes.push_back(in.readUint32());

        ret->m_scriptLayers.resize(layerSizes.size());
        for (atUint32 i = 0; i < layerSizes.size(); i++)
        {
            atUint8* layerData = in.readUBytes(layerSizes[i]);
            Athena::io::MemoryReader layerReader(layerData, layerSizes[i]);
            layerReader.setEndian(Athena::Endian::BigEndian);

            ret->m_scriptLayers[i] = readObjectLayer(layerReader, scriptVersion);
        }
    }
}

CScene* CAreaReader::readObjectLayer(Athena::io::MemoryReader& in, EScriptVersion version)
{
    in.readByte(); // unknown;
    atUint32 objectCount = in.readUint32();
    CScene* ret = new CScene;
    ret->m_objects.resize(objectCount);
    for (atUint32 i =0; i < objectCount; i++)
        ret->m_objects[i] = CScriptObject(in, version);

    return ret;
}

REGISTER_RESOURCE_LOADER(CAreaReader, "MREA", loadByData);
