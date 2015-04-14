#ifndef CVECTOR3F_HPP
#define CVECTOR3F_HPP

#include "Global.hpp"
#include <Athena/IStreamReader.hpp>

class CVector3f
{
public:
    CVector3f();
    CVector3f(float xyz);
    CVector3f(float x, float y, float z);
    CVector3f(Athena::io::IStreamReader& input);
    ~CVector3f();

    bool operator ==(const CVector3f& rhs) const;
    CVector3f operator+(const CVector3f& rhs) const;
    CVector3f operator-(const CVector3f& rhs) const;
    CVector3f operator-() const;
    CVector3f operator*(const CVector3f& rhs) const;
    CVector3f operator/(const CVector3f& rhs) const;
    CVector3f operator+(float val) const;
    CVector3f operator-(float val) const;
    CVector3f operator*(float val) const;
    CVector3f operator/(float val) const;
    const CVector3f& operator +=(const CVector3f& rhs);
    const CVector3f& operator -=(const CVector3f& rhs);
    const CVector3f& operator *=(const CVector3f& rhs);
    const CVector3f& operator /=(const CVector3f& rhs);
    CVector3f normalized();
    CVector3f cross(const CVector3f& rhs) const;
    float     dot(const CVector3f& rhs) const;
    float     lengthSquared() const;
    float     length() const;
    static float getAngleDiff(const CVector3f& a, const CVector3f& b);

    static CVector3f lerp(const CVector3f& a, const CVector3f& b, float t);
    static CVector3f nlerp(const CVector3f& a, const CVector3f& b, float t);
    static CVector3f slerp(const CVector3f& a, const CVector3f& b, float t);


    inline float operator[](int idx) const
    {
        return (&x)[idx];
    }


    union
    {
        struct
        {
            float x, y, z;
        };
#if __SSE__
        __m128 v;
#else
        float v[4];
#endif
    };

    static const CVector3f skOne;
    static const CVector3f skZero;
}
#if __SSE__
_CRT_ALIGN(16)
#endif
;

CVector3f operator+(float lhs, const CVector3f& rhs);
CVector3f operator-(float lhs, const CVector3f& rhs);
CVector3f operator*(float lhs, const CVector3f& rhs);
#endif // CVECTOR3F_HPP
