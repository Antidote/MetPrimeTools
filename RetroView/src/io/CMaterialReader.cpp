#include "io/CMaterialReader.hpp"
#include "core/CMaterialCache.hpp"

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
                base::readUint32();
                base::readUint32();
                base::readUint32();
                // spin until we read in all commands
                while (!readMaterialCommand(ret, mat))
                    ;
                base::seek(materialStart + dataSize, Athena::SeekOrigin::Begin);
                atUint32 matID = CMaterialCache::instance()->addMaterial(mat);
                ret.m_materials.push_back(matID);
            }
        }
        else
        {
            atUint32 textureCount = base::readUint32();
            ret.m_textureIds.resize(textureCount);
            for (atUint32 i = 0; i < textureCount; i++)
                ret.m_textureIds[i] =  CAssetID(*this, CAssetID::E_32Bits);

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

// Reads the current material command, returns true when done
bool CMaterialReader::readMaterialCommand(CMaterialSet& set, CMaterial& material)
{
    EMaterialCommand cmd = (EMaterialCommand)base::readUint32();
    if (cmd == EMaterialCommand::END)
        return true;

    switch(cmd)
    {
        case EMaterialCommand::PASS:
        {
            atUint32 passSize = base::readUint32();
            atUint32 passDataStart = base::position();
            //material.m_tevStages.push_back(STEVStage());
            bool done = readMaterialCommand(set, material);
            base::seek(passDataStart + passSize, Athena::SeekOrigin::Begin);
            return done;
        }
            break;
        case EMaterialCommand::CLR:
        {
            atUint32 clrValue = base::readUint32();
            if (clrValue == (int)EMaterialCommand::CLR)
            {
                /*Uint32 RGBA = */base::readUint32();
            }
            else if (clrValue == (int)EMaterialCommand::DIFB)
            {
                /*Uint32 unknown  = */base::readUint32();
            }
            else
            {
                CAssetID textureId = CAssetID(*this, CAssetID::E_64Bits);
                /*Uint32 filler    = */base::readUint32(); // ????
                atUint32 unknownSize   = base::readUint32();
                if (textureId != CAssetID())
                {
                    set.m_textureIds.push_back(textureId);
                    material.m_textureIndices.push_back(set.m_textureIds.size() - 1);
                }
                base::seek(unknownSize);
            }
        }
            break;
        case EMaterialCommand::INT:
        {
            base::readUint32();
            base::readUint32();
        }
            break;
        case EMaterialCommand::DIFF:
        {
            atUint32 unk = base::readUint32();
            CAssetID textureId = CAssetID(*this, CAssetID::E_64Bits);
            atUint32 unk2 = base::readUint32();
            atUint32 unkSize = base::readUint32();
            if (textureId != CAssetID())
            {
                set.m_textureIds.push_back(textureId);
                material.m_textureIndices.push_back(set.m_textureIds.size() - 1);
            }
            base::seek(unkSize);
        }
            break;
        default:
        {
            atUint32 unk = base::readUint32();
            atUint64 texID = base::readUint64();
            atUint32 unk2 = base::readUint32();
            atUint32 unkSize = base::readUint32();
            base::seek(unkSize);
        }
            break;
    }

    return false;
}
