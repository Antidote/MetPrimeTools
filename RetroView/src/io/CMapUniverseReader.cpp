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
            THROW_INVALID_DATA_EXCEPTION("Invalid Map Universe file magic, expected 0xABCDEF01 got %.8X", magic);

        atUint32 version = base::readUint32();

        if (version != 1)
            THROW_INVALID_DATA_EXCEPTION("Invalid Map Universe file version, expected version 1 got %i", version);

        ret = new CMapUniverse;

        ret->m_mapAreaID = CUniqueID(*this, CUniqueID::E_32Bits);

        atUint32 worldCount = base::readUint32();

        while ((worldCount--) > 0)
        {
            SMapWorld world;
            world.name = base::readString();
            world.mlvlID = CUniqueID(*this, CUniqueID::E_32Bits);
            glm::mat3x4 transform;
            for (atUint32 i = 0; i < 3; i++)
            {
                transform[i].x = base::readFloat();
                transform[i].y = base::readFloat();
                transform[i].z = base::readFloat();
                transform[i].w = base::readFloat();
            }

            world.transform = glm::transpose(glm::mat4(transform));

            atUint32 hexagonCount = base::readUint32();
            while((hexagonCount--) > 0)
            {
                for (atUint32 i = 0; i < 3; i++)
                {
                    transform[i].x = base::readFloat();
                    transform[i].y = base::readFloat();
                    transform[i].z = base::readFloat();
                    transform[i].w = base::readFloat();
                }

                world.hexagonTransforms.push_back(glm::transpose(glm::mat4(transform)));
            }

            world.color.setRedF(base::readFloat());
            world.color.setGreenF(base::readFloat());
            world.color.setBlueF(base::readFloat());
            world.color.setAlphaF(base::readFloat());

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
