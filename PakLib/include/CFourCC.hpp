#ifndef CFOURCC_HPP
#define CFOURCC_HPP
// Thanks parax:

#include <Athena/IStreamReader.hpp>
#include <string>

class CFourCC
{
    char fourCC[4];
public:
    CFourCC() {*(atUint32*)fourCC = 0;}
    CFourCC(const char *src) {*this = src;}
    CFourCC(long src) {*this = src;}
    CFourCC(Athena::io::IStreamReader& src)
    {
        src.readUBytesToBuf(fourCC, 4);
    }

    inline CFourCC& operator=(const char *src)
    {
        *(atUint32*)fourCC = *(atUint32*)src;
        return *this;
    }
    inline CFourCC& operator=(long src)
    {
        fourCC[0] = (src >> 24) & 0xFF;
        fourCC[1] = (src >> 16) & 0xFF;
        fourCC[2] = (src >>  8) & 0xFF;
        fourCC[3] = (src >>  0) & 0xFF;
        return *this;
    }
    inline bool operator==(const CFourCC& other) const
    {
        return *(atUint32*)fourCC == *(atUint32*)other.fourCC;
    }
    inline bool operator==(const char *other) const {return (*this == CFourCC(other));}
    inline bool operator==(const long other) const {return (*this == CFourCC(other));}
    inline bool operator!=(const CFourCC& other) const {return !(*this == other);}
    inline bool operator!=(const char *other) const {return !(*this == other);}
    inline bool operator!=(const long other) const {return !(*this == other);}
    
    inline atUint32 toLong()
    {
        atUint32 ret = fourCC[0] << 24 | fourCC[1] << 16 | fourCC[2] << 8 | fourCC[3];
        Athena::utility::LittleUint32(ret);
        
        return ret;
    }
    
    inline std::string toString() const
    {
        return std::string(fourCC, 4);
    }

    inline bool valid()
    {
        for (atUint32 i = 0; i < 4; i++)
            if (!(isalpha(fourCC[i]) || isdigit(fourCC[i])))
                return false;

        return true;
    }
};

class CFourCCHash final
{
public:
    std::size_t operator()(CFourCC const& fourCC) const
    {
        std::string tmp = fourCC.toString();
        std::hash<std::string> hashFn;
        return hashFn(tmp);
    }
};

class CFourCC_Comparison final
{
public:
    bool operator()(CFourCC const& left, CFourCC const& right) const
    {
        return (left == right);
    }
};

#endif // CFOURCC_HPP
