#ifndef CVECTOR3F_HPP
#define CVECTOR3F_HPP

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

    static CVector3f lerp(const CVector3f& start, const CVector3f& end, float percent);
    static CVector3f slerp(const CVector3f& start, const CVector3f& end, float percent);

    float x, y, z;

    inline float operator[](int idx) const
    {
        return (&x)[idx];
    }

};

CVector3f operator+(float lhs, const CVector3f& rhs);
CVector3f operator-(float lhs, const CVector3f& rhs);
CVector3f operator*(float lhs, const CVector3f& rhs);
#endif // CVECTOR3F_HPP
