#ifndef CCOLOR_HPP
#define CCOLOR_HPP

#include <MathLib.hpp>

class CColor
{
public:
    ZE_DECLARE_ALIGNED_ALLOCATOR();
    
    CColor() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {};
    CColor(Athena::io::IStreamReader& reader) {readBGRA(reader);}
    
    inline void readRGBA(Athena::io::IStreamReader& reader)
    {
        r = reader.readFloat();
        g = reader.readFloat();
        b = reader.readFloat();
        a = reader.readFloat();
    }
    
    inline void readBGRA(Athena::io::IStreamReader& reader)
    {
        b = reader.readFloat();
        g = reader.readFloat();
        r = reader.readFloat();
        a = reader.readFloat();
    }
    
private:
    union
    {
        struct
        {
            float r, g, b, a;
        };
#if __SSE__
        __m128 mVec128;
#endif
    };
    
};

#endif // CCOLOR_HPP
