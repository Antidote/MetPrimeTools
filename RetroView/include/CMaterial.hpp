#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "CTEVStage.hpp"
#include "GXTypes.hpp"
#include <vector>

class CMaterial
{
public:
    enum Version
    {
        MetroidPrime1,
        MetroidPrime2,
        MetroidPrime3,
        DKCR
    };
    CMaterial();
    ~CMaterial();


    bool isValid() const;
    atUint32 materialFlags() const;
    atUint32 vertexAttributes() const;

    std::vector<atUint32> textureIndices() const;

    bool hasPosition();
    bool hasNormal();
    bool hasColor(atUint8 slot);
    bool hasUV(atUint8 slot);

private:
    friend class CMaterialReader;

    bool hasAttribute(atUint32 index);

    Version               m_version;
    bool                  m_isValid;
    atUint32              m_materialFlags;
    std::vector<atUint32> m_textureIndices;
    atUint32              m_vertexAttributes;
    atUint32              m_unknown1_mp2;
    atUint32              m_unknown2_mp2;
    atUint32              m_unknown1;
    atUint32              m_konstCount;
    atUint32              m_konstColor[4];
    EBlendMode            m_blendDstFactor;
    EBlendMode            m_blendSrcFactor;
    atUint32              m_unknown2;
    std::vector<atUint32> m_unkFlags;
    std::vector<STEVStage> m_tevStages;
    std::vector<atUint32> m_texTEVIn;
    std::vector<atUint32> m_texGenFlags;
    std::vector<atUint8>  m_animationData;
};

#endif // MATERIAL_HPP
