#ifndef CWORDBITMAP_HPP
#define CWORDBITMAP_HPP

#include <bitset>
#include <Athena/MemoryReader.hpp>
#include <Athena/Utility.hpp>

#define CWORDBITMAP_MAX_BITS 128 * 32

class CWordBitmap : public std::bitset<CWORDBITMAP_MAX_BITS>
{
    atUint32 m_numBits;
public:
    void readBitmapBits(Athena::io::IStreamReader& in, atUint32 bitCount);
    void readBitmapWords(Athena::io::IStreamReader& in, atUint32 wordCount)
    {
        readBitmapBits(in, wordCount * 32);
    }
    
    class CWordBitmapIterator : std::iterator<std::bidirectional_iterator_tag, bool>
    {
        friend CWordBitmap;
        const CWordBitmap& m_wb;
        size_t m_ipos;
    public:
        CWordBitmapIterator(const CWordBitmap& wb) : m_wb(wb), m_ipos(0) {}
        CWordBitmapIterator(const CWordBitmapIterator& other) : m_wb(other.m_wb), m_ipos(other.m_ipos) {}
        bool operator*() const {return m_wb[m_ipos];}
        CWordBitmapIterator& operator++()
        {
            if (m_ipos >= m_wb.m_numBits)
            {
                std::string exc =
                Athena::utility::sprintf("Attempted to iterate to element %u of WordBitmap size %u",
                                         m_ipos, m_wb.m_numBits);
                throw std::out_of_range(exc);
            }
            ++m_ipos;
            return *this;
        }
        CWordBitmapIterator& operator--()
        {
            if (m_ipos <= 0)
                throw std::out_of_range("Attempted to iterate to negative element of WordBitmap");
            --m_ipos;
            return *this;
        }
        bool operator!=(const CWordBitmapIterator& other) const {return !(&m_wb == &other.m_wb && m_ipos == other.m_ipos);}
    };
    
    CWordBitmapIterator begin() {CWordBitmapIterator it(*this); return it;}
    CWordBitmapIterator end() {CWordBitmapIterator it(*this); it.m_ipos = m_numBits; return it;}
};

#endif // CWORDBITMAP_HPP
