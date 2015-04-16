#include "io/CWorldFileReader.hpp"
#include "generic/CWorldFile.hpp"

#include <Athena/InvalidDataException.hpp>

CWorldFileReader::CWorldFileReader(const atUint8* data, atUint64 length)
    : base(data, length)
{
    base::setEndian(Athena::Endian::BigEndian);
}

CWorldFileReader::CWorldFileReader(const std::string& filename)
    : base(filename)
{
    base::setEndian(Athena::Endian::BigEndian);
}


CWorldFileReader::~CWorldFileReader()
{
}

CWorldFile* CWorldFileReader::read()
{
    CWorldFile* ret = nullptr;
    try
    {
        atUint32 magic = base::readUint32();
        if (magic != 0xDEAFBABE) // DCLN also uses this ID so we have to make absolutely sure this is correct
            THROW_INVALID_DATA_EXCEPTION("Invalid World File magic expected 0xDEAFBABE got 0x%8.X", magic);

        CWorldFile::Version version = (CWorldFile::Version)base::readUint32();

        if (version != CWorldFile::Version::MetroidPrime1 && version != CWorldFile::Version::MetroidPrime2 &&
                version != CWorldFile::Version::MetroidPrime3 && version != CWorldFile::Version::DonkeyKongCountryReturns)
            THROW_INVALID_DATA_EXCEPTION("Invalid World File version expected 0x0D, 0x11, 0x14, 0x19, 0x1B; got 0x%8.X", (atUint32)version);

        ret = new CWorldFile();
        const CUniqueID::EIDBits bits = ((version == CWorldFile::Version::MetroidPrime3 || version == CWorldFile::Version::DonkeyKongCountryReturns)
                                        ? CUniqueID::E_64Bits :  CUniqueID::E_32Bits);

        ret->m_worldName   = CUniqueID(*this, bits);
        if (version == CWorldFile::Version::MetroidPrime2)
            ret->m_darkWorldName = CUniqueID(*this, bits);

        if (version != CWorldFile::Version::MetroidPrime1 && version != CWorldFile::Version::DonkeyKongCountryReturns)
            ret->m_unknown1 = base::readUint32();

        if (version == CWorldFile::Version::DonkeyKongCountryReturns)
        {
            bool exists = base::readBool();
            if (exists)
            {
                ret->m_levelNumber = base::readString();
                ret->m_unknownFloat1 = base::readFloat();
                ret->m_unknownFloat2 = base::readFloat();
                ret->m_unknownFloat3 = base::readFloat();
                ret->m_unknownFloat4 = base::readFloat();
            }
        }

        ret->m_saveWorldID = CUniqueID(*this, bits);
        ret->m_skyboxID    = CUniqueID(*this, bits);

        if (version == CWorldFile::Version::MetroidPrime1)
        {
            atUint32 relayCount = base::readUint32();

            while((relayCount--) > 0)
            {
                SMemoryRelay relay;
                relay.instance       = base::readUint32();
                relay.targetInstance = base::readUint32();
                relay.message        = base::readUint16();
                relay.unknown        = base::readByte();
                ret->m_memoryRelays.push_back(relay);
            }
        }

        atUint32 areaCount = base::readUint32();
        if (version == CWorldFile::Version::MetroidPrime1)
            ret->m_unknown2 = base::readUint32();

        while((areaCount--) > 0)
        {
            SWorldArea area;
            area.nameID = CUniqueID(*this, bits);

            CMatrix3f transformBasis;
            CVector3f transformOrigin;
            for (atUint32 i = 0; i < 3; i++)
            {
                transformBasis[i].x = base::readFloat();
                transformBasis[i].y = base::readFloat();
                transformBasis[i].z = base::readFloat();
                transformOrigin[i] = base::readFloat();
            }
            area.transform = CTransform(transformBasis.transposed(), transformOrigin);

            area.boundingBox.readBoundingBox(*this);

            area.mreaID = CUniqueID(*this, bits);
            area.areaID = CUniqueID(*this, bits);

            if (version != CWorldFile::Version::DonkeyKongCountryReturns)
            {
                atUint32 attachedAreaCount = base::readUint32();
                while((attachedAreaCount--) > 0)
                    area.attachedAreas.push_back(base::readUint16());

                if (version != CWorldFile::Version::MetroidPrime3)
                    area.unknown1 = base::readUint32();
            }

            if (version != CWorldFile::Version::MetroidPrime3 && version != CWorldFile::Version::DonkeyKongCountryReturns)
            {
                area.dependencies = CDependencyGroup(*this, bits);
                atUint32 layerCount = base::readUint32();
                while((layerCount--) > 0)
                    area.layerDependencyIndices.push_back(base::readUint32());
            }

            if (version != CWorldFile::Version::DonkeyKongCountryReturns)
            {
                atUint32 dockCount = base::readUint32();
                while((dockCount--) > 0)
                {
                    SDock dock;
                    atUint32 dockConnectionCount = base::readUint32();
                    while((dockConnectionCount--) > 0)
                    {
                        SDockConnection dc;
                        dc.areaIndex = base::readUint32();
                        dc.dockIndex = base::readUint32();
                        dock.connections.push_back(dc);
                    }

                    atUint32 coordCount = base::readUint32();
                    while ((coordCount--) > 0)
                    {
                        CVector3f dockCoord(*this);
                        dock.dockCoords.push_back(dockCoord);
                    }

                    area.docks.push_back(dock);
                }
            }

            if (version == CWorldFile::Version::MetroidPrime2)
            {
                atUint32 relCount = base::readUint32();
                while ((relCount--) > 0)
                    area.relInfo.relFilename.push_back(base::readString());
                atUint32 layerCount = base::readUint32();
                while ((layerCount--) > 0)
                    area.relInfo.layerRelOffsets.push_back(base::readUint32());
            }

            area.hasName = false;
            if (version != CWorldFile::Version::MetroidPrime1)
            {
                if (version == CWorldFile::Version::DonkeyKongCountryReturns)
                    area.unknown2 = base::readUint32();
                area.hasName = true;
                area.internalName = base::readString();
            }

            ret->m_areas.push_back(area);
        }

        if (version != CWorldFile::Version::DonkeyKongCountryReturns)
        {
            ret->m_worldMap.id = CUniqueID(*this, bits);
            ret->m_worldMap.unknown1 = base::readUint32();
            ret->m_worldMap.unknown2 = base::readBool();
        }

        if (version == CWorldFile::Version::MetroidPrime1)
        {
            atUint32 agscCount = base::readUint32();
            while ((agscCount--) > 0)
            {
                SAudioGroup audioGroup;
                audioGroup.unknown = base::readUint32();
                audioGroup.id = CUniqueID(*this, bits);
            }

            base::readBool();
        }

        atUint32 layerCount = base::readUint32();
        while((layerCount--) > 0)
        {
            SLayerFlags lf;
            lf.layerCount = base::readUint32();
            lf.layerFlags = base::readUint64();
            ret->m_layerFlags.push_back(lf);
        }

        layerCount = base::readUint32();
        while ((layerCount--) > 0)
        {
            ret->m_layerNames.push_back(base::readString());
        }

        if (version == CWorldFile::Version::MetroidPrime3 || version == CWorldFile::Version::DonkeyKongCountryReturns)
        {
            layerCount = base::readUint32();
            while ((layerCount--) > 0)
            {
                CUniqueID id = CUniqueID(*this, CUniqueID::E_128Bits);
                ret->m_layerIds.push_back(id);
            }
        }

        areaCount = base::readUint32();
        while ((areaCount--) > 0)
            ret->m_layerNameIndices.push_back(base::readUint32());
    }
    catch(...)
    {
        delete ret;
        ret = nullptr;
        throw;
    }

    return ret;
}

IResource* CWorldFileReader::loadByData(const atUint8 *data, atUint64 length)
{
    CWorldFileReader reader(data, length);
    return reader.read();
}

REGISTER_RESOURCE_LOADER(CWorldFileReader, "MLVL", loadByData);
