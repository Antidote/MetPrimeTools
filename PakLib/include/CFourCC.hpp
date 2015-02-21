#ifndef CFOURCC_HPP
#define CFOURCC_HPP
// Thanks parax:

#include <Athena/Stream.hpp>
#include <string>

class CFourCC
{
    char fourCC[4];
public:
    CFourCC();
    CFourCC(const char *src);
    CFourCC(long src);
    CFourCC(Athena::io::Stream& src);

    CFourCC& operator=(const char *src);
    CFourCC& operator=(long src);
    bool operator==(const CFourCC& other) const;
    bool operator==(const char *other) const;
    bool operator==(const long other) const;

    atUint32 toLong();
    std::string toString() const;
};

#endif // CFOURCC_HPP
