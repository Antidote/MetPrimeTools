#include "core/CWordBitmap.hpp"

void CWordBitmap::readBitmapBits(Athena::io::IStreamReader& in, atUint32 bitCount)
{
    if (bitCount > CWORDBITMAP_MAX_BITS)
    {
        std::string exc_str = Athena::utility::sprintf("%u bits exceeds CWordBitmap's maximum %u bits",
                                                       bitCount, CWORDBITMAP_MAX_BITS);
        throw std::length_error(exc_str);
    }
    
    m_numBits = bitCount;
    reset();
    
    atUint32 wordCount = bitCount / 32;
    if (bitCount % 32)
        wordCount += 1;
    
    atUint32 i,j;
    for (i=0 ; i<wordCount ; ++i)
    {
        atUint32 word = in.readUint32();
        for (j=0 ; j<32 ; ++j)
            set(i*32+j, word & (1 << j));
    }
    
}
