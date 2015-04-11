#ifndef CQUATERNION_HPP
#define CQUATERNION_HPP

#include "CVector3f.hpp"
#include <math.h>
#include <Athena/IStreamReader.hpp>

class CQuaternion
{
public:
    CQuaternion();
    CQuaternion(float r, float x, float y, float z);
    CQuaternion(float x, float y, float z);
    CQuaternion(const CVector3f& vec);
    CQuaternion(float r, const CVector3f& vec);
    CQuaternion(Athena::io::IStreamReader& input);
    ~CQuaternion();

    void fromVector3f(const CVector3f& vec);

    CQuaternion& operator=(const CQuaternion& q);
    CQuaternion operator+(const CQuaternion& q) const;
    CQuaternion operator-(const CQuaternion& q) const;
    CQuaternion operator*(const CQuaternion& q) const;
    CQuaternion operator/(const CQuaternion& q) const;
    CQuaternion operator*(float scale) const;
    CQuaternion operator/(float scale) const;
    CQuaternion operator-() const;
    const CQuaternion& operator+=(const CQuaternion& q);
    const CQuaternion& operator-=(const CQuaternion& q);
    const CQuaternion& operator*=(const CQuaternion& q);
    const CQuaternion& operator*=(float scale);
    const CQuaternion& operator/=(float scale);
    float length() const;
    float lengthSquared() const;
    void normalize();
    CQuaternion normalized() const;
    void invert();
    CQuaternion inverse() const;

    /**
     * @brief Set the rotation using axis angle notation
     * @param axis The axis to rotate around
     * @param angle The magnitude of the rotation in radians
     * @return
     */
    static inline CQuaternion fromAxisAngle(const CVector3f& axis, float angle)
    {
        return CQuaternion(cosf(angle/2), axis*sinf(angle/2));
    }

    void toAxisAngle(CVector3f& axis, float& angle) const;

    float r;
    CVector3f v;

    static inline CVector3f rotate(const CQuaternion& rotation, const CVector3f& v)
    {
        CQuaternion q = rotation * v;
        q *= rotation.inverse();

        return q.v;
    }

    CQuaternion log() const;

    CQuaternion exp() const;
};

#endif // CQUATERNION_HPP
