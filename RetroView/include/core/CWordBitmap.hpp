#ifndef CWORDBITMAP_HPP
#define CWORDBITMAP_HPP

#include <bitset>
#include <Athena/MemoryReader.hpp>

#define CWORDBITMAP_MAX_BITS 128 * 32

class CWordBitmap : std::bitset<CWORDBITMAP_MAX_BITS>
{
    atUint32 m_numBits;
public:
    void readBitmapBits(Athena::io::IStreamReader& in, atUint32 bitCount);
    void readBitmapWords(Athena::io::IStreamReader& in, atUint32 wordCount)
    {
        readBitmapBits(in, wordCount * 32);
    }
};

#endif // CWORDBITMAP_HPP
