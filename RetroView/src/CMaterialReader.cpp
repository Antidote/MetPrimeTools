#include "CMaterialReader.hpp"
#include "CMaterialCache.hpp"
#include <memory.h>

CMaterialReader::CMaterialReader(const atUint8 *data, atUint64 length)
    : base(data, length)
{
    base::setEndian(Athena::Endian::BigEndian);
}

CMaterialReader::~CMaterialReader()
{
}

CMaterialSet CMaterialReader::read(CMaterial::Version version)
{
    CMaterialSet ret;

    try
    {
        if (version == CMaterial::MetroidPrime3 || version == CMaterial::DKCR)
        {
            atUint32 materialCount = base::readUint32();

            for (atUint32 i = 0; i < materialCount; i++)
            {
                CMaterial mat;
                mat.m_version = version;
                atUint32 dataSize = base::readUint32();
                atUint64 materialStart = base::position();

                base::readUint32();
                base::readUint32();
                base::readUint32();
                mat.m_version = version;
                mat.m_vertexAttributes = base::readUint32();
                base::seek(materialStart + dataSize, Athena::SeekOrigin::Begin);
                ret.m_materials.push_back(CMaterialCache::instance()->addMaterial(mat));
            }
        }
        else
        {
            atUint32 textureCount = base::readUint32();
            ret.m_textureIds.resize(textureCount);
            for (atUint32 i = 0; i < textureCount; i++)
                ret.m_textureIds[i] = base::readUint32();

            atUint32 materialCount = base::readUint32();

            std::vector<atUint32> materialOffsets;
            materialOffsets.resize(materialCount);

            for (atUint32 i = 0; i < materialCount; i++)
                materialOffsets[i] = base::readUint32();

            atUint64 materialStart = base::position();

            for (atUint32 m = 0; m < materialCount; m++)
            {
                CMaterial mat;
                mat.m_version = version;

                mat.m_materialFlags = base::readUint32();
                atUint32 textureIdxCount = base::readInt32();
                mat.m_textureIndices.resize(textureIdxCount);

                for (atUint32 i = 0; i < textureIdxCount; i++)
                    mat.m_textureIndices[i] = base::readUint32();

                mat.m_vertexAttributes = base::readUint32();
                if (version == CMaterial::MetroidPrime2)
                {
                    mat.m_unknown1_mp2 = base::readUint32();
                    mat.m_unknown2_mp2 = base::readUint32();
                }

                mat.m_unknown1 = base::readUint32();

                if (mat.m_materialFlags & 0x8)
                {
                    mat.m_konstCount = base::readUint32() % 4;
                    for (atUint32 i = 0; i < mat.m_konstCount; i++)
                    {
                        mat.m_konstColor[i] = base::readUint32();
                        Athena::utility::BigUint32(mat.m_konstColor[i]);
                    }
                }

                mat.m_blendDstFactor = (EBlendMode)base::readUint16();
                mat.m_blendSrcFactor = (EBlendMode)base::readUint16();

                if (mat.m_blendDstFactor == EBlendMode::SrcClr)
                    mat.m_blendDstFactor = EBlendMode::DstClr;
                else if (mat.m_blendDstFactor == EBlendMode::InvSrcClr)
                    mat.m_blendDstFactor = EBlendMode::InvDstClr;

                if (mat.m_materialFlags & 0x400)
                    mat.m_unknown2 = base::readUint32();

                atUint32 unkFlagCount = base::readUint32();
                while ((unkFlagCount--) > 0)
                    mat.m_unkFlags.push_back(base::readUint32());

                atUint32 tevStageCount = base::readUint32();
                mat.m_tevStages.resize(tevStageCount);

                for (atUint32 i = 0; i < tevStageCount; i++)
                {
                    STEVStage& stage = mat.m_tevStages[i];

                    stage.ColorInFlags = base::readUint32();
                    stage.AlphaInFlags = base::readUint32();
                    stage.ColorOpFlags = base::readUint32();
                    stage.AlphaOpFlags = base::readUint32();
                    stage.Padding      = base::readUByte ();
                    stage.KonstAlphaIn = base::readUByte ();
                    stage.KonstColorIn = base::readUByte ();
                    stage.RasterizedIn = base::readUByte ();
                }

                mat.m_texTEVIn.resize(tevStageCount);
                for (atUint32 i = 0; i < tevStageCount; i++)
                    mat.m_texTEVIn[i] = base::readUint32();

                atUint32 texGenCount = base::readUint32();
                mat.m_texGenFlags.resize(texGenCount);
                for (atUint32 i = 0; i < texGenCount; i++)
                    mat.m_texGenFlags[i] = base::readUint32();

                atUint32 animationSize = base::readUint32();
                if (animationSize > 0)
                {
                    atUint32 animationCount = base::readUint32();
                    mat.m_animations.resize(animationCount);

                    for (atUint32 i = 0; i < animationCount; i++)
                    {
                        SAnimation& anim = mat.m_animations[i];

                        anim.mode = base::readUint32();
                        atUint32 parmCount = 0;
                        switch(anim.mode)
                        {
                            case 5:
                            case 4:
                            case 2:
                                parmCount = 4;
                                break;
                            case 7:
                            case 3:
                                parmCount = 2;
                                break;
                        }
                        for (atUint32 p = 0; p < parmCount; p++)
                            anim.parms[p] = base::readFloat();
                    }
                }

                if (m < materialCount)
                    base::seek(materialStart + materialOffsets[m], Athena::SeekOrigin::Begin);

                ret.m_materials.push_back(CMaterialCache::instance()->addMaterial(mat));
            }
        }
    }
    catch(...)
    {
    }

    return ret;
}
