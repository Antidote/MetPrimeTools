#ifndef CWORLDFILE_HPP
#define CWORLDFILE_HPP

#include "core/IResource.hpp"
#include "core/SBoundingBox.hpp"
#include "generic/CDependencyGroup.hpp"
#include <glm/glm.hpp>

struct SMemoryRelay
{
    atUint32 instance;
    atUint32 targetInstance;
    atUint16 message;
    atUint8  unknown;
};

struct SDockConnection
{
    atUint32 areaIndex;
    atUint32 dockIndex;
};

struct SDock
{
    std::vector<SDockConnection> connections;
    std::vector<glm::vec3>       dockCoords;
};

struct SRelocatableObjectInfo
{
    std::vector<std::string> relFilename;
    std::vector<atUint32>    layerRelOffsets;
};

struct SWorldArea
{
    CAssetID nameID;
    glm::mat4 transformMatrix;
    SBoundingBox boundingBox;
    CAssetID mreaID;
    CAssetID areaID;
    std::vector<atUint16> attachedAreas;
    atUint32 unknown1; // not in MP3
    CDependencyGroup dependencies; // Not in MP3/DKCR (stored in MREA)
    std::vector<atUint32> layerDependencyIndices;
    std::vector<SDock> docks;
    SRelocatableObjectInfo relInfo; // MP2 only
    atUint32 unknown2; // DKCR only
    bool hasName; // internal, does not exist in file
    std::string internalName; // not in MP1
};

struct SWorldMap
{
    CAssetID id;
    atUint32 unknown1;
    bool unknown2;
};

struct SAudioGroup
{
    atUint32 unknown;
    CAssetID id;
};

struct SLayerFlags
{
    atUint32 layerCount;
    atUint64 layerFlags;
};

class IRenderableModel;
class CWorldFile final : public IResource
{
public:
    enum class Version : atUint32
    {
        MetroidPrimeDemo          = 0x0D,
        MetroidPrime1             = 0x11,
        MetroidPrime2             = 0x17,
        MetroidPrime3             = 0x19,
        DonkeyKongCountryReturns  = 0x1B
    };

    CWorldFile();
    virtual ~CWorldFile();

    std::string areaName(const CAssetID& assetId, CPakFile* pak = nullptr);
    IRenderableModel* skyboxModel();
private:
    friend class CWorldFileReader;
    Version                   m_version;
    CAssetID                  m_worldName;
    CAssetID                  m_darkWorldName;  // MP2 only
    atUint32                  m_unknown1;       // MP2 and 3 only
    bool                      m_hasLevelNumber; // Begin Donkey Kong Country Returns only
    std::string               m_levelNumber;
    float                     m_unknownFloat1;
    float                     m_unknownFloat2;
    float                     m_unknownFloat3;
    float                     m_unknownFloat4; // End Donkey Kong Country Returns only
    CAssetID                  m_saveWorldID;
    CAssetID                  m_skyboxID;
    std::vector<SMemoryRelay> m_memoryRelays;  // only in MP1
    atUint32                  m_unknown2;      // only in MP1
    std::vector<SWorldArea>   m_areas;
    SWorldMap                 m_worldMap;
    std::vector<SAudioGroup>  m_audioGroups;
    std::vector<SLayerFlags>  m_layerFlags;
    std::vector<std::string>  m_layerNames;
    std::vector<CAssetID>     m_layerIds;         // MP3 and DKCR only
    std::vector<atUint32>     m_layerNameIndices; // First index for each room
};

#endif // CWORLDFILE_HPP
