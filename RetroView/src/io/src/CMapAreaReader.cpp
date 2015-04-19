#include "core/GLInclude.hpp"
#include "io/CMapAreaReader.hpp"
#include "models/CMapArea.hpp"
#include "core/CMaterialCache.hpp"

#include <Athena/InvalidDataException.hpp>

CMapAreaReader::CMapAreaReader(const atUint8* data, atUint64 length)
    : base(data, length)
{
    base::setEndian(Athena::Endian::BigEndian);
}

CMapAreaReader::CMapAreaReader(const std::string& filepath)
    : base(filepath)
{
    base::setEndian(Athena::Endian::BigEndian);
}

CMapAreaReader::~CMapAreaReader()
{
}

CMapArea* CMapAreaReader::read()
{
    CMapArea* ret = nullptr;
    try
    {
        atUint32 magic = base::readUint32();
        if (magic != 0xDEADD00D)
            THROW_INVALID_DATA_EXCEPTION("Not a valid MAPA file got 0x%.8X", magic);
        CMapArea::Version version = (CMapArea::Version)base::readUint32();

        if (version != CMapArea::Version::MetroidPrime1 && version != CMapArea::Version::MetroidPrime2 && version != CMapArea::Version::MetroidPrime3)
            THROW_INVALID_DATA_EXCEPTION("Only Metroid Prime 1-3 Minimaps are supported got version %i", (atUint32)version);

        ret = new CMapArea;

        CMaterial material;
        atUint32 materialFlags = 0;
        materialFlags |= 0x8;
        materialFlags |= 0x80;
        material.setMaterialFlags(materialFlags);
        atUint32 vertexAttributes = 0;
        vertexAttributes |= 3;
        material.setVertexAttributes(vertexAttributes);
        material.setBlendMode(EBlendMode::Zero, EBlendMode::One);

        STEVStage tevStage;
        tevStage.ColorInFlags = 0xE | (0xF << 5) | (0xF << 10) | (0xF << 15);
        tevStage.AlphaInFlags = 0x6 | (0x7 << 5) | (0x7 << 10) | (0x7 << 15);

        tevStage.ColorOpFlags = 0;
        tevStage.AlphaOpFlags = 0;
        tevStage.KonstColorIn  = 0x0C;
        tevStage.KonstAlphaIn  = 0x1C;

        material.addTevStage(tevStage, 0);

        ret->m_materialID = CMaterialCache::instance()->addMaterial(material);

        ret->m_unknown1 = base::readUint32();
        ret->m_unknown2 = base::readUint32();
        
        ret->m_boundingBox.readBoundingBox(*this);

        if (version != CMapArea::Version::MetroidPrime1)
        {
            base::readUint32();
            base::readUint32();
            base::readUint32();
            if (version == CMapArea::Version::MetroidPrime3)
                base::readUint32();
        }

        atUint32 poiCount    = base::readUint32();
        atUint32 vertexCount = base::readUint32();
        atUint32 detailCount = base::readUint32();
        atUint64 headerSize = base::position();

        if (version == CMapArea::Version::MetroidPrime3)
        {
            atUint32 stringLength = base::readUint32();
            base::readUint32();
            headerSize = base::position();
            if (stringLength > 0)
                base::readString(stringLength);
        }

        for (atUint32 i = 0; i < poiCount; i++)
        {
            SPointOfInterest pointOfInterest;
            if (version == CMapArea::Version::MetroidPrime3)
                pointOfInterest.unknown1 = readUint32();

            pointOfInterest.type = base::readUint32();
            pointOfInterest.unknown2 = base::readUint32();
            if (version != CMapArea::Version::MetroidPrime3)
            {
                pointOfInterest.unknown3 = base::readUint16();
                pointOfInterest.id = base::readUint16();
            }
            else
            {
                pointOfInterest.unknown4.readRGBA(*this);
            }
            base::seek(4); // padding;

            CMatrix3f transformBasis;
            CVector3f transformOrigin;
            for (atUint32 i = 0; i < 3; i++)
            {
                transformBasis[i].x = base::readFloat();
                transformBasis[i].y = base::readFloat();
                transformBasis[i].z = base::readFloat();
                transformOrigin[i] = base::readFloat();
            }
            pointOfInterest.transform = CTransform(transformBasis.transposed(), transformOrigin);

            base::seek(4*4); // padding;

            ret->m_pointsOfInterest.push_back(pointOfInterest);
        }

        for (atUint32 i = 0; i < vertexCount; ++i)
        {
            CVector3f vec(*this);
            ret->m_vertices.push_back(vec);
        }

        for (atUint32 i = 0; i < detailCount; i++)
        {
            SMapAreaDetail detail;
            detail.boundingBox.readBoundingBox(*this);

            atUint32 primDataStart = base::readUint32();
            atUint32 primDataEnd   = base::readUint32();
            atUint64 oldPos = base::position();

            base::seek(primDataStart + headerSize, Athena::SeekOrigin::Begin);

            atUint32 primitiveCount = base::readUint32();

            for (atUint32 j = 0; j < primitiveCount; j++)
            {
                SMapAreaPrimitive primitive;
                atUint32 type = base::readUint32();

                switch((EPrimitive)type)
                {
                    case EPrimitive::Quads:
                        primitive.type = GL_QUADS;
                        break;
                    case EPrimitive::Triangles:
                        primitive.type = GL_TRIANGLES;
                        break;
                    case EPrimitive::TriangleStrip:
                        primitive.type = GL_TRIANGLE_STRIP;
                        break;
                    case EPrimitive::TriangleFan:
                        primitive.type = GL_TRIANGLE_FAN;
                        break;
                    case EPrimitive::Lines:
                        primitive.type = GL_LINES;
                        break;
                    case EPrimitive::LineStrip:
                        primitive.type = GL_LINE_STRIP;
                        break;
                    case EPrimitive::Points:
                        primitive.type = GL_POINTS;
                        break;
                }

                atUint32 indexCount = base::readUint32();

                for (atUint32 k = 0; k < indexCount; ++k)
                {
                    atUint16 idx = (atUint16)base::readUByte();
                    primitive.indices.push_back(idx);
                }

                if (type != 0)
                {
                    primitive.indices.push_back(0xFFFF);
                    detail.primitives.push_back(primitive);
                }
                base::seek((base::position() + 3) & ~3, Athena::SeekOrigin::Begin);
            }
            base::seek(primDataEnd + headerSize, Athena::SeekOrigin::Begin);

            atUint32 borderCount = base::readUint32();

            for (atUint32 j = 0; j < borderCount; j++)
            {
                SMapBorder border;
                atUint32 borderIndexCount = base::readUint32();
                for (atUint32 k = 0; k < borderIndexCount; ++k)
                {
                    atInt16 idx =(atInt16)base::readUByte();
                    border.indices.push_back(idx);
                }

                border.indices.push_back(0xFFFF);
                detail.borders.push_back(border);
                base::seek((base::position() + 3) & ~3, Athena::SeekOrigin::Begin);
            }

            ret->m_details.push_back(detail);
            base::seek(oldPos, Athena::SeekOrigin::Begin);
        }
    }
    catch(...)
    {
        delete ret;
        ret = nullptr;
        throw;
    }

    return ret;
}

IResource* CMapAreaReader::loadByData(const atUint8* data, atUint64 length)
{
    return CMapAreaReader(data, length).read();
}

REGISTER_RESOURCE_LOADER(CMapAreaReader, "MAPA", loadByData);
