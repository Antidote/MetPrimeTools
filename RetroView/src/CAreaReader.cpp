#include "CAreaReader.hpp"
#include "CAreaFile.hpp"
#include "CMaterialReader.hpp"
#include "CMaterialSet.hpp"
#include "GXCommon.hpp"
#include "RetroCommon.hpp"

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
        Athena::io::BinaryWriter out;
        if (decompressMREA(*this, out))
            setData(out.data(), out.length());
    }

    seek(0, Athena::SeekOrigin::Begin);

    atUint32 magic   = base::readUint32();
    atUint32 version = base::readUint32();

    if (magic != 0xDEADBEEF)
        THROW_INVALID_DATA_EXCEPTION("Not valid MREA file, expected magic 0xDEADBEEF got %.8X\n", magic);

    if (version != CAreaFile::MetroidPrime1 && version != CAreaFile::MetroidPrime2
            && version != CAreaFile::MetroidPrime3)
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
    if (version != CAreaFile::MetroidPrime1)
        atUint32 sclyLayerCount = base::readUint32();

    atUint32 sectionCount = base::readUint32();

    if (version == CAreaFile::MetroidPrime1 || version == CAreaFile::MetroidPrime2)
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
            m_ptlaSection  = base::readUint32();
            m_egmcSection  = base::readUint32();
        }
    }

    atUint32 compressedBlockCount = 0;
    if (version != CAreaFile::MetroidPrime1)
        compressedBlockCount = base::readUint32();
    atUint32 sectionIndexCount = 0;
    if (version == CAreaFile::MetroidPrime3 || version == CAreaFile::DKCR)
        sectionIndexCount = base::readUint32();

    base::seek((base::position() + 31) & ~31, Athena::SeekOrigin::Begin);

    m_sectionSizes.resize(sectionCount);
    for (atUint32 i = 0; i < sectionCount; i++)
        m_sectionSizes[i] = base::readUint32();

    base::seek((base::position() + 31) & ~31, Athena::SeekOrigin::Begin);

    if (version != CAreaFile::MetroidPrime1)
    {
        for (atUint32 i = 0; i < compressedBlockCount; i++)
        {
            base::readUint32(); // Block size
            base::readUint32(); // Data size
            base::readUint32(); // Compressed size
            base::readUint32(); // Section Count
        }
    }

    base::seek((base::position() + 31) & ~31, Athena::SeekOrigin::Begin);

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

    base::seek((base::position() + 31) & ~31, Athena::SeekOrigin::Begin);

    m_modelMeshOffsets.resize(modelCount);

    if (version == CAreaFile::MetroidPrime3 || version == CAreaFile::DKCR)
        readSectionsMP3DKCR(ret);
    else
        readSections(ret);

    return ret;
}

IResource* CAreaReader::loadByFile(const std::string& filepath)
{
    CAreaReader reader(filepath);
    return reader.read();
}

IResource* CAreaReader::loadByData(const atUint8* data, atUint64 length)
{
    CAreaReader reader(data, length);
    return reader.read();
}

void CAreaReader::readSections(CAreaFile* ret)
{
    m_sectionReader.setEndian(endian());
    for (atUint32 i = 0; i < m_sectionSizes.size(); i++)
    {
        atUint64 sectionStart = base::position();

        if (i == 0)
        {
            CMaterial::Version matVer = (ret->m_version == CAreaFile::MetroidPrime1 ?
                                             CMaterial::MetroidPrime1 : CMaterial::MetroidPrime2);

            atUint8* data = base::readUBytes(m_sectionSizes[i]);
            CMaterialReader matReader(data, m_sectionSizes[i]);
            ret->m_materialSets.push_back(matReader.read(matVer));
        }
        else if ((i - 1) < m_modelMeshOffsets.size())
        {
            for (atUint32 j = 0; j < m_modelMeshOffsets.size(); j++)
                readModelHeader(ret, sectionStart, i);
        }

        base::seek(sectionStart + m_sectionSizes[i], Athena::SeekOrigin::Begin);
    }
}

void CAreaReader::readSectionsMP3DKCR(CAreaFile* ret)
{
    m_sectionReader.setEndian(endian());

    for (atUint32 i = 0; i < m_sectionSizes.size(); i++)
    {
        atUint64 sectionStart = base::position();
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
        }
        else
        {
            std::vector<SAreaSectionIndex>::iterator iter = std::find_if(m_sectionIndices.begin(), m_sectionIndices.end(),
                                                                         [&i](SAreaSectionIndex si)->bool{return si.index == i; });

            if (iter != m_sectionIndices.end())
            {
                SAreaSectionIndex idx = *iter;
                if (!idx.tag.compare("GPUD"))
                {
                    for (atUint32 m = 0; m < ret->m_models.size(); m++)
                    {
                        CModelData& model = ret->m_models[m];
                        readModelData(ret, model, sectionStart, i);
                        readMeshes(ret, model, sectionStart, i, m);
                        model.indexIBOs(ret->m_materialSets[0]);
                    }
                }
            }
        }

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

    base::seek((base::position() + 31) & ~31, Athena::SeekOrigin::Begin);
    sectionStart = base::position();
    i++;

    if (ret->m_version == CAreaFile::MetroidPrime1 || ret->m_version == CAreaFile::MetroidPrime2)
        readModelData(ret, model, sectionStart, i);

    atUint32 meshCount = base::readUint32();
    while((meshCount--) > 0)
        m_modelMeshOffsets[ret->m_models.size() - 1].push_back(base::readUint32());

    base::seek((base::position() + 31) & ~31, Athena::SeekOrigin::Begin);
    sectionStart = base::position();
    i++;

    if (ret->m_version == CAreaFile::MetroidPrime1 || ret->m_version == CAreaFile::MetroidPrime2)
        readMeshes(ret, model, sectionStart, i, ret->m_models.size() - 1);

    if (ret->m_version != CAreaFile::MetroidPrime1)
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
    if (ret->m_version == CAreaFile::MetroidPrime1 || ret->m_version == CAreaFile::MetroidPrime2)
    {
        data = base::readUBytes(m_sectionSizes[i]);
        m_sectionReader.setData(data, m_sectionSizes[i]);
        readTexCoords(model, m_sectionReader, true);
        sectionStart = base::position();
        i++;
    }
}

void CAreaReader::readVertices(CModelData& model, Athena::io::BinaryReader& in)
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

void CAreaReader::readNormals(CModelData& model, Athena::io::BinaryReader& in)
{
    atInt32 normalCount = in.length() / (sizeof(atUint16) * 3);
    while ((normalCount--) > 0)
    {
        glm::vec3 nrm;
        nrm.x = in.readUint16() / 32768.f; // really shouldn't do this here
        nrm.y = in.readUint16() / 32768.f; // but it's constant enough to be reliable
        nrm.z = in.readUint16() / 32768.f;
        model.m_normals.push_back(nrm);
    }
}

void CAreaReader::readColors(CModelData& model, Athena::io::BinaryReader& in)
{
    atInt32 colorCount = in.length() / sizeof(atUint32);

    while ((colorCount--) > 0)
        model.m_colors.push_back(in.readUint32());
}

void CAreaReader::readTexCoords(CModelData& model, Athena::io::BinaryReader& in, bool isLightmap)
{
    if (isLightmap)
    {
        atInt32 texCoordCount = in.length() / (sizeof(atUint16) * 2);
        while ((texCoordCount--) > 0)
        {
            glm::vec2 texCoord;
            texCoord.s = (float)(in.readUint16() / 32768.f); // really shouldn't do this here
            texCoord.t = (float)(in.readUint16() / 32768.f); // but it's constant enough to be reliable
            model.m_lightmapCoords.push_back(texCoord);
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
            model.m_texCoords.push_back(texCoord);
        }
    }
}

void CAreaReader::readMesh(CModelData& model, CAreaFile* ret, Athena::io::BinaryReader& in)
{
    model.m_meshes.push_back(CMesh());
    CMesh& mesh = model.m_meshes.back();

    mesh.m_pivot.x    = in.readFloat();
    mesh.m_pivot.y    = in.readFloat();
    mesh.m_pivot.z    = in.readFloat();
    atUint16 dataSize = 0;
    if (ret->m_version != CAreaFile::DKCR)
    {
        mesh.m_materialID = in.readUint32();
        mesh.m_mantissa   = in.readUint16();
        dataSize = in.readUint16();
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
            assert(in.readUint32() == 0);
            assert(in.readUint32() == 0);
            extraDataSize = in.readUint32();
            for (int i = 0; i < 3; i++)
                in.readFloat(); // Normalized vector
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

    in.seek((in.position() + 31) & ~31, Athena::SeekOrigin::Begin);

    const CMaterial& material = ret->m_materialSets[0].material(mesh.m_materialID);

    readPrimitives(mesh, material, dataSize, in);
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

REGISTER_RESOURCE_LOADER(CAreaReader, "mrea", loadByData);
