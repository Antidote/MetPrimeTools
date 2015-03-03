#include "RetroCommon.hpp"
#include <Athena/Compression.hpp>
#include <Athena/InvalidDataException.hpp>
#include <memory.h>

struct CMPDBlock
{
    atUint32 compressedLen;
    atUint32 uncompressedLen;
};

void decompressData(aIO::IStreamWriter& outbuf, atUint8* srcData, atUint32 srcLength, atUint32 uncompressedLength)
{
    atUint16 compressionMethod = *(atUint16*)(srcData);
    Athena::utility::BigUint16(compressionMethod);
    if (compressionMethod == 0x78DA || compressionMethod == 0x7801 || compressionMethod == 0x789C)
    {
        atUint8*  decompData = new atUint8[uncompressedLength];
        if (aIO::Compression::decompressZlib(srcData, srcLength,  decompData, uncompressedLength) == uncompressedLength)
            outbuf.writeUBytes(decompData, uncompressedLength);
        delete[] decompData;
    }
    else
    {
        bool result = true;
        atUint8* newData = new atUint8[uncompressedLength];
        atInt32 remainingSize = uncompressedLength;
        while (remainingSize)
        {
            atInt16 segmentSize = *(atInt16*)(srcData);
            if (!segmentSize)
            {
                result = false;
                break;
            }

            srcData += 2;
            Athena::utility::BigInt16(segmentSize);
            atInt32 lzoStatus = Athena::io::Compression::decompressLZO((atUint8*)srcData, segmentSize, &newData[uncompressedLength - remainingSize], remainingSize);

            if ((lzoStatus & 8) != 0)
            {
                result = false;
                break;
            }
            srcData += segmentSize;
        }

        if (result)
            outbuf.writeUBytes(newData, uncompressedLength);

        delete[] newData;
    }
}

void decompressFile(aIO::IStreamWriter& outbuf, atUint8* data, atUint32 srcLength)
{
    atUint32 magic = *(atUint32*)(data);
    Athena::utility::BigUint32(magic);
    if (magic == 0x434D5044)
    {
        atUint32 currentOffset = 4;
        atUint32 blockCount = *(atUint32*)(data + currentOffset);
        currentOffset += 4;
        Athena::utility::BigUint32(blockCount);
        CMPDBlock* blocks = new CMPDBlock[blockCount];
        memcpy(blocks, data + currentOffset, sizeof(CMPDBlock)*blockCount);
        currentOffset += (sizeof(CMPDBlock)*blockCount);

        for (atUint32 i = 0; i < blockCount; i++)
        {
            Athena::utility::BigUint32(blocks[i].compressedLen);
            Athena::utility::BigUint32(blocks[i].uncompressedLen);

            blocks[i].compressedLen &= 0x00FFFFFF;

            if (blocks[i].compressedLen == blocks[i].uncompressedLen)
                outbuf.writeUBytes(data + currentOffset, blocks[i].uncompressedLen);
            else
            {
                decompressData(outbuf, data + currentOffset, blocks[i].compressedLen, blocks[i].uncompressedLen);
            }

            currentOffset += blocks[i].compressedLen;
        }
    }
    else
    {
        atUint32 uncompressedLength = *(atUint32*)(data);
        Athena::utility::BigUint32(uncompressedLength);
        decompressData(outbuf, data + 4, srcLength - 4, uncompressedLength);
    }
}
