#include <iostream>
#include <Athena/MemoryReader.hpp>

struct JointHeader
{
    atUint16 startCode;
    atUint16 jointId;
    atUint16 keyCount;
    atUint16 alphaRot;
    atUint8 alphaRotSize;
    atUint16 betaRot;
    atUint8 betaRotSize;
    atUint16 gammaRot;
    atUint8 gammaRotSize;
    atUint16 nextCode;
    atUint16 xLoc;
    atUint8 xLocSz;
    atUint16 yLoc;
    atUint8 yLocSz;
    atUint16 zLoc;
    atUint8 zLocSz;
    atInt32 euler_accum[3];
    atInt32 euler_signs[3];
    atInt32 trans_accum[3];
    bool prevFlipBit;
};

atUint32 getKeyframeCount(std::vector<atUint32> bitmasks, atUint32 potkfCount)
{
    atUint32 curKfBit = 0;
    atUint32 curKfCnt = 0;

    while (curKfBit < potkfCount)
    {
        atUint32 curKfBool = bitmasks[curKfBit / 32];
        curKfBool = curKfBool >> (curKfBit % 32);
        curKfBool &= 0x1;

        if (curKfBool)
            ++curKfCnt;

        ++curKfBit;
    }

    return curKfCnt;
}

atUint32 getKfFrame(atUint32 kfIdx, std::vector<atUint32> bitmasks, atUint32 potkfCount)
{
    atUint32 curKfBit = 0;
    atInt32 curKfIdx = -1;

    while(curKfBit < potkfCount)
    {
        atUint32 curKfBool = bitmasks[curKfBit / 32];
        curKfBool = curKfBool >> (curKfBit % 32);
        curKfBool &= 0x1;

        if(curKfBool)
        {
            ++curKfIdx;
            if(curKfIdx == kfIdx)
                return curKfBit;
        }

        ++curKfBit;

    }
    return 0;
}

int main()
{
    std::vector<JointHeader> joints;
    try
    {
        Athena::io::MemoryReader reader("/media/Storage/MetroidPrimeAssets/MP1Metroid2/ANIMs/88B6E0EB.ANIM");
        reader.setEndian(Athena::Endian::BigEndian);
        atUint32 format = reader.readUint32();
        if (format == 2)
        {
            atUint32 keyCount = reader.readUint32();
            atUint32 eventRef = reader.readUint32();
            atUint32 unknown0 = reader.readUint32();
            float duration = reader.readFloat();
            float interval = reader.readFloat();
            atUint32 unknown1 = reader.readUint32();
            atUint32 unknown2 = reader.readUint32();
            atUint32 rotationDivisor = reader.readUint32();
            float translationMult = reader.readFloat();
            atUint32 boneCount = reader.readUint32();
            atUint32 unknown3 = reader.readUint32();
            atUint32 keyFrameBitfieldLen = reader.readUint32(); // in bits
            atUint32 bitfieldByteSize = keyFrameBitfieldLen / 32;
            if (keyFrameBitfieldLen % 32)
                ++bitfieldByteSize;

            bitfieldByteSize *= 4;

            std::vector<atUint32> keyframeBitfields;
            for (atUint32 i = 0; i < (keyFrameBitfieldLen/32) + 1; ++i)
                keyframeBitfields.push_back(reader.readUint32());

            reader.seek(8); // two redundant joint counts

            for (atUint32 i = 0; i < boneCount; ++i)
            {
                JointHeader jnt;
                jnt.startCode = reader.readUint16();
                jnt.jointId = reader.readUint16();
                jnt.keyCount = reader.readUint16();
                jnt.alphaRot = reader.readUint16();
                jnt.alphaRotSize = reader.readUByte();
                jnt.betaRot = reader.readUint16();
                jnt.betaRotSize = reader.readUByte();
                jnt.gammaRot = reader.readUint16();
                jnt.gammaRotSize = reader.readUByte();
                jnt.nextCode = reader.readUint16();

                if (jnt.nextCode)
                {
                    jnt.xLoc = reader.readUint16();
                    jnt.xLocSz = reader.readByte();
                    jnt.yLoc = reader.readUint16();
                    jnt.yLocSz = reader.readByte();
                    jnt.zLoc = reader.readUint16();
                    jnt.zLocSz = reader.readByte();
                }

                jnt.euler_accum[0] = jnt.alphaRot;
                jnt.euler_accum[1] = jnt.betaRot;
                jnt.euler_accum[2] = jnt.gammaRot;

                jnt.euler_signs[0] = 0.0;
                jnt.euler_signs[1] = 0.0;
                jnt.euler_signs[2] = 0.0;

                jnt.trans_accum[0] = jnt.xLoc;
                jnt.trans_accum[1] = jnt.yLoc;
                jnt.trans_accum[2] = jnt.zLoc;

                jnt.prevFlipBit = false;

                joints.push_back(jnt);
            }

            atUint32 keyframeCount = getKeyframeCount(keyframeBitfields, keyFrameBitfieldLen);
        }
    }
    catch(...)
    {
    }

    return 0;
}

