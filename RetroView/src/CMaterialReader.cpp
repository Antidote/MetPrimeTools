#include "CMaterialReader.hpp"
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
            ret.m_materials.resize(materialCount);

            for (atUint32 i = 0; i < materialCount; i++)
            {
                CMaterial& mat = ret.m_materials[i];
                atUint32 dataSize = base::readUint32();
                atUint64 materialStart = base::position();

                base::readUint32();
                base::readUint32();
                base::readUint32();
                mat.m_version = version;
                mat.m_vertexAttributes = base::readUint32();
                mat.m_isValid = true;
                base::seek(materialStart + dataSize, Athena::SeekOrigin::Begin);
            }
        }
        else
        {
            atUint32 textureCount = base::readUint32();
            ret.m_textureIds.resize(textureCount);
            for (atUint32 i = 0; i < textureCount; i++)
                ret.m_textureIds[i] = base::readUint32();

            atUint32 materialCount = base::readUint32();

            ret.m_materials.resize(materialCount);
            std::vector<atUint32> materialOffsets;
            materialOffsets.resize(materialCount);

            for (atUint32 i = 0; i < materialCount; i++)
                materialOffsets[i] = base::readUint32();

            atUint64 materialStart = base::position();

            for (atUint32 m = 0; m < materialCount; m++)
            {
                CMaterial& mat = ret.m_materials[m];
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
                        mat.m_konstColor[i] = base::readUint32();
                }

                mat.m_blendDstFactor = (EBlendMode)base::readUint16();
                mat.m_blendSrcFactor = (EBlendMode)base::readUint16();

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
//                    mat.m_animationData.resize(animationSize);

//                    atUint8* data = base::readUBytes(animationSize);
//                    memcpy(&mat.m_animationData[0], data, animationSize);
//                    delete[] data;
                }
                mat.m_isValid = true;
                if (m < materialCount)
                    base::seek(materialStart + materialOffsets[m], Athena::SeekOrigin::Begin);
            }
        }
    }
    catch(...)
    {
    }

    return ret;
}
