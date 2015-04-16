#include "io/CModelReader.hpp"
#include "io/CMaterialReader.hpp"
#include "models/CModelFile.hpp"
#include "core/GXCommon.hpp"
#include "core/CVertexBuffer.hpp"

#include <RetroCommon.hpp>
#include <Athena/InvalidDataException.hpp>
#include <Athena/MemoryWriter.hpp>

enum class SectionType : atUint32
{
    Vertices,
    Normals,
    Colors,
    TexCoord0,
    TexCoord1orMeshOffsets,
    MeshInfo
};

CModelReader::CModelReader(const atUint8 *data, atUint64 length)
    : base(data, length),
      m_result(nullptr)
{
    base::setEndian(Athena::Endian::BigEndian);
}

CModelReader::CModelReader(const std::string& filepath)
    : base(filepath)
{
    base::setEndian(Athena::Endian::BigEndian);
}

CModelReader::~CModelReader()
{
}

CModelFile* CModelReader::read()
{
    atUint32 magic = base::readUint32();

    if (magic != 0xDEADBABE)
    {
        Athena::io::MemoryWriter tmp;
        decompressFile(tmp, base::data(), base::length());

        if (tmp.length() > 0x10)
            base::setData(tmp.data(), tmp.length());

        magic = base::readUint32();
    }

    if (magic != 0xDEADBABE /*&& magic != 0x9381000A*/)
        THROW_INVALID_DATA_EXCEPTION("Not a valid CMDL magic, expected 0xDEADBABE got 0x%.8X\n", magic);
    atUint32 version;
    if (magic != 0x9381000A)
        version = base::readUint32();
    else
        version = CModelFile::DKCR;

    if (!(version >= CModelFile::MetroidPrime1 && version <= CModelFile::DKCR))
        THROW_INVALID_DATA_EXCEPTION("Only Metroid Prime 1 to 3 models are supported got v%i\n", version);

    try
    {
        m_result = new CModelFile;
        m_result->m_version = (CModelFile::Version)version;
        m_result->m_flags = base::readUint32();
        m_result->m_boundingBox.readBoundingBox(*this);

        atUint32 sectionCount = base::readUint32();
        atInt32 materialCount = base::readInt32();

        m_sectionSizes.resize(sectionCount);

        if (m_result->m_flags & 0x10)
        {
            base::readUint32();
            atUint32 visGroupCount = base::readUint32();
            while((visGroupCount--) > 0)
            {
                atUint32 len = base::readUint32();
                base::readString(len);
            }

            base::readUint32();
            base::readUint32();
            base::readUint32();
            base::readUint32();
            base::readUint32();
        }

        m_result->m_materialSets.resize(materialCount);
        for (atUint32 i = 0; i < sectionCount; i++)
            m_sectionSizes[i] = base::readUint32();

        base::seekAlign32();

        const atUint32 sectionBias = ((m_result->m_version == CModelFile::DKCR || m_result->m_version == CModelFile::MetroidPrime3)
                                      ? 1 : materialCount);

        Athena::io::MemoryReader sectionReader(new atUint8[2], 2);
        sectionReader.setEndian(endian());
        for (atUint32 i = 0; i < sectionCount; i++)
        {
            if (m_sectionSizes[i] == 0)
                continue;

            if (materialCount > 0)
            {
                if (m_result->m_version != CModelFile::DKCR && m_result->m_version != CModelFile::MetroidPrime3)
                {
                    atUint8* data = base::readUBytes(m_sectionSizes[i]);
                    CMaterialReader reader(data, m_sectionSizes[i]);
                    CMaterialSet materialSet;
                    switch(m_result->m_version)
                    {
                        case CModelFile::MetroidPrime1:
                            materialSet = reader.read(CMaterial::MetroidPrime1);
                            break;
                        case CModelFile::MetroidPrime2:
                            materialSet = reader.read(CMaterial::MetroidPrime2);
                            break;
                        default:
                            break;
                    }

                    m_result->m_materialSets[i] = materialSet;
                    materialCount--;
                    continue;
                }
                else
                {
                    atUint8* data = base::readUBytes(m_sectionSizes[i]);
                    CMaterialReader reader(data, m_sectionSizes[i]);
                    atUint32 setIdx = 0;
                    while ((materialCount--) > 0)
                    {
                        CMaterialSet& materialSet = m_result->m_materialSets[setIdx];
                        switch(m_result->m_version)
                        {
                            case CModelFile::MetroidPrime3:
                                materialSet = reader.read(CMaterial::MetroidPrime3);
                                break;
                            case CModelFile::DKCR:
                                materialSet = reader.read(CMaterial::DKCR);
                            default:
                                break;
                        }
                        setIdx++;
                    }
                    continue;
                }
            }
            else
            {
                SectionType section = (SectionType)(i - sectionBias);
                atUint8* data = base::readUBytes(m_sectionSizes[i]);
                sectionReader.setData(data, m_sectionSizes[i]);
                switch(section)
                {
                    case SectionType::Vertices:
                    {
                        if (m_result->m_flags & 0x20)
                            readVertices(sectionReader, true);
                        else
                            readVertices(sectionReader);
                    }
                        break;
                    case SectionType::Normals:
                        readNormals(sectionReader);
                        break;
                    case SectionType::Colors:
                        readColors(sectionReader);
                        break;
                    case SectionType::TexCoord0:
                        readTexCoords(0, sectionReader);
                        break;
                    case SectionType::TexCoord1orMeshOffsets:
                    {
                        if (m_result->m_flags & EFormatFlags::TexCoord1 || m_result->m_version == CModelFile::DKCR)
                            readTexCoords(1, sectionReader);
                        else
                            readMeshOffsets(sectionReader);
                        break;
                    }
                    case SectionType::MeshInfo:
                    {
                        if (m_meshOffsets.size() == 0)
                        {
                            readMeshOffsets(sectionReader);
                            break;
                        }
                    }
                    default:
                        if ((i - sectionBias) >= 5)
                            readMesh(sectionReader);
                        break;
                }
            }
        }
    }
    catch(...)
    {
        delete m_result;
        m_result = nullptr;
        throw;
    }

    return m_result;
}

IResource* CModelReader::loadByData(const atUint8* data, atUint64 length)
{
    CModelReader reader(data, length);
    return reader.read();
}

void CModelReader::readVertices(Athena::io::MemoryReader& in, bool isShort)
{
    if (!isShort)
    {
        atUint32 vertexCount = in.length() / 12;
        while ((vertexCount--) > 0)
        {
            CVector3f vert(in);
            m_result->m_vertices.push_back(vert);
        }
    }
    else
    {
        atUint32 vertexCount = in.length() / (sizeof(atInt16) * 3);
        while ((vertexCount--) > 0)
        {
            CVector3f vert(in.readInt16() / 32768.f, in.readInt16() / 32768.f, in.readInt16() / 32768.f);
            m_result->m_vertices.push_back(vert);
        }
    }
}

void CModelReader::readNormals(Athena::io::MemoryReader& in)
{
    if (!(m_result->m_flags & EFormatFlags::ShortNormal) && m_result->m_version != CModelFile::DKCR)
    {
        // floats
        atUint32 normalCount = in.length() / 12;
        while ((normalCount--) > 0)
        {
            CVector3f nrm(in);
            m_result->m_normals.push_back(nrm);
        }
    }
    else
    {
        // shorts
        atUint32 normalCount = in.length() / (sizeof(atUint16) * 3);
        while ((normalCount--) > 0)
        {
            CVector3f nrm(in.readInt16() / 32768.f, in.readInt16() / 32768.f, in.readInt16() / 32768.f);
            m_result->m_normals.push_back(nrm);
        }
    }
}

void CModelReader::readColors(Athena::io::MemoryReader& in)
{
    atUint32 colorCount = in.length() / sizeof(atUint32);

    while((colorCount--) > 0)
        m_result->m_colors.push_back(in.readUint32());
}

void CModelReader::readTexCoords(atUint32 slot, Athena::io::MemoryReader& in)
{
    if (slot == 0)
    {
        atUint32 texCoordCount = in.length() / 8;
        while ((texCoordCount--) > 0)
        {
            CVector3f tex(in.readFloat(), in.readFloat(), 0.0f);
            m_result->m_texCoords0.push_back(tex);
        }
    }
    else if (slot == 1)
    {
        atUint32 lightmapCoordCount = in.length() / (sizeof(atUint16) * 2);
        while ((lightmapCoordCount--) > 0)
        {
            CVector3f tex;
            if (m_result->m_version != CModelFile::DKCR)
            {
                tex.x = in.readInt16() / 32768.f;
                tex.y = in.readInt16() / 32768.f;
            }
            else
            {
                tex.x = (float)(in.readInt16() / 32768.f);
                tex.y = (float)(in.readInt16() / 32768.f);
                //tex = glm::normalize(tex);
            }
            m_result->m_texCoords1.push_back(tex);
        }
    }
}

void CModelReader::readMeshOffsets(Athena::io::MemoryReader& in)
{
    atInt32 meshCount = in.readUint32();
    while((meshCount--) > 0)
        m_meshOffsets.push_back(in.readUint32());
}

void CModelReader::readMesh(Athena::io::MemoryReader& in)
{
    CMesh mesh;
    for (atUint32 i = 0; i < 3; i++)
        mesh.m_pivot[i] = in.readFloat();
    if (m_result->m_version != CModelFile::DKCR)
    {
        mesh.m_materialID = in.readUint32();
        mesh.m_mantissa = in.readUint16();
        in.readUint16();
        in.seek(2 * sizeof(atUint32));
        atUint32 extraDataSize = in.readUint32();

        for (atUint32 i = 0; i < 3; i++)
            mesh.m_reflectionDirection[i] = in.readFloat();

        in.seek(extraDataSize);
    }
    else
    {
        mesh.m_mantissa = in.readUint16();
        in.readUint16();
        in.readUint32();
        in.readUint32();
        in.readUint16();
        mesh.m_materialID = in.readUint16();
        in.readByte(); // 0xff?
        in.readByte(); // visgroupidx
        mesh.m_uvSource = in.readUByte();
        in.readByte();
    }

    in.seekAlign32();

    CMaterial& material = m_result->m_materialSets[0].material(mesh.m_materialID);
    readPrimitives(mesh, *m_result, material, in);
    m_result->m_meshes.push_back(mesh);
}

REGISTER_RESOURCE_LOADER(CModelReader, "CMDL", loadByData);
