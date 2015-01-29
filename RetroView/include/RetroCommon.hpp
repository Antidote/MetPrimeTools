#ifndef RETROCOMMON_HPP
#define RETROCOMMON_HPP

#include <Athena/BinaryWriter.hpp>
#include <Athena/BinaryReader.hpp>

namespace aIO = Athena::io;


enum class EPAKSection
{
    STRG = 0x53545247,
    RSHD = 0x52534844,
    DATA = 0x44415441
};

void decompressData(aIO::BinaryWriter& outbuf, atUint8* srcData, atUint32 srcLength, atInt32 uncompressedLength);
void decompressFile(aIO::BinaryWriter& outbuf, atUint8* srcData, atUint32 srcLength);
bool decompressMREA(aIO::BinaryReader& in, aIO::BinaryWriter& out);

#endif // RETROCOMMON_HPP
