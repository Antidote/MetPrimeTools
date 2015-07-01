#include "io/CMapUniverseReader.hpp"
#include "models/CMapUniverse.hpp"
#include "core/CMaterialCache.hpp"
#include "core/CMaterial.hpp"
#include "core/GXTypes.hpp"

#include <Athena/InvalidDataException.hpp>

CMapUniverseReader::CMapUniverseReader(const atUint8 *data, atUint64 length)
    : base(data, length)
{
    base::setEndian(Athena::Endian::BigEndian);
}

CMapUniverseReader::CMapUniverseReader(const std::string &filepath)
    : base(filepath)
{
    base::setEndian(Athena::Endian::BigEndian);
}

CMapUniverseReader::~CMapUniverseReader()
{
}

CMapUniverse* CMapUniverseReader::read()
{
    CMapUniverse* ret = nullptr;
    try
    {
        atUint32 magic = base::readUint32();
        if (magic != 0xABCDEF01)
            THROW_INVALID_DATA_EXCEPTION_RETURN(nullptr, "Invalid Map Universe file magic, expected 0xABCDEF01 got %.8X", magic);

        atUint32 version = base::readUint32();

        if (version != 1)
            THROW_INVALID_DATA_EXCEPTION_RETURN(nullptr, "Invalid Map Universe file version, expected version 1 got %i", version);

        ret = new CMapUniverse;

        ret->m_mapAreaID = CUniqueID(*this, CUniqueID::E_32Bits);

        atUint32 worldCount = base::readUint32();

        while ((worldCount--) > 0)
        {
            SMapWorld world;
            world.name = base::readString();
            world.mlvlID = CUniqueID(*this, CUniqueID::E_32Bits);
            CMatrix3f transformBasis;
            CVector3f transformOffset;
            for (atUint32 i = 0; i < 3; i++)
            {
                transformBasis[i].x = base::readFloat();
                transformBasis[i].y = base::readFloat();
                transformBasis[i].z = base::readFloat();
                transformOffset[i] = base::readFloat();
            }

            world.transform = CTransform(transformBasis.transposed(), transformOffset);

            atUint32 hexagonCount = base::readUint32();
            while((hexagonCount--) > 0)
            {
                for (atUint32 i = 0; i < 3; i++)
                {
                    transformBasis[i].x = base::readFloat();
                    transformBasis[i].y = base::readFloat();
                    transformBasis[i].z = base::readFloat();
                    transformOffset[i] = base::readFloat();
                }

                world.hexagonTransforms.push_back(CTransform(transformBasis.transposed(), transformOffset));
            }

            world.color.readBGRA(*this);

            ret->m_worlds.push_back(world);
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

IResource* CMapUniverseReader::loadByData(const atUint8* data, atUint64 length)
{
    CMapUniverseReader reader(data, length);
    return reader.read();
}

REGISTER_RESOURCE_LOADER(CMapUniverseReader, "MAPU", loadByData);
