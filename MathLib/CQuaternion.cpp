#include "CQuaternion.hpp"
#include <math.h>

CQuaternion::CQuaternion()
    : r(1), v(0)
{
}

CQuaternion::CQuaternion(float r, float x, float y, float z)
    : r(r), v(x, y, z)
{
}

CQuaternion::CQuaternion(float x, float y, float z)
{
    fromVector3f(CVector3f(x, y, z));
}

CQuaternion::CQuaternion(const CVector3f& vec)
{
    fromVector3f(vec);
}

CQuaternion::CQuaternion(float r, const CVector3f& vec)
    : r(r), v(vec)
{
}

CQuaternion::CQuaternion(Athena::io::IStreamReader& input)
{
    r = input.readFloat();
    v = CVector3f(input);
}

CQuaternion::~CQuaternion()
{
}

void CQuaternion::fromVector3f(const CVector3f& vec)
{
    float cosX = cosf(0.5 * vec.x);
    float cosY = cosf(0.5 * vec.y);
    float cosZ = cosf(0.5 * vec.z);

    float sinX = sinf(0.5 * vec.x);
    float sinY = sinf(0.5 * vec.y);
    float sinZ = sinf(0.5 * vec.z);

    r   = cosZ * cosY * cosX + sinZ * sinY * sinX;
    v.x = cosZ * cosY * sinX - sinZ * sinY * cosX;
    v.y = cosZ * sinY * cosX + sinZ * cosY * sinX;
    v.z = sinZ * cosY * cosX - cosZ * sinY * sinX;
}

CQuaternion& CQuaternion::operator=(const CQuaternion& q)
{
    r = q.r;
    v = q.v;
    return *this;
}

CQuaternion CQuaternion::operator+(const CQuaternion& q) const
{
    return CQuaternion(r + q.r, v+q.v);
}

CQuaternion CQuaternion::operator-(const CQuaternion& q) const
{
    return CQuaternion(r - q.r, v-q.v);
}

CQuaternion CQuaternion::operator*(const CQuaternion& q) const
{
    return CQuaternion(r*q.r - v.dot(q.v),
                       v.y * q.v.z - v.z * q.v.y + r * q.v.x + v.x*q.r,
                       v.z * q.v.x - v.x * q.v.z + r * q.v.y + v.y*q.r,
                       v.x * q.v.y - v.y * q.v.x + r * q.v.z + v.z*q.r);
}

CQuaternion CQuaternion::operator/(const CQuaternion& q) const
{
    CQuaternion p(q);
    p.invert();
    return *this * p;
}

CQuaternion CQuaternion::operator*(float scale) const
{
    return CQuaternion(r*scale, v*scale);
}

CQuaternion CQuaternion::operator/(float scale) const
{
    return CQuaternion(r/scale, v/scale);
}

CQuaternion CQuaternion::operator-() const
{
    return CQuaternion(-r, -v);
}

const CQuaternion& CQuaternion::operator+=(const CQuaternion& q)
{
    r += q.r;
    v += q.v;
    return *this;
}

const CQuaternion& CQuaternion::operator-=(const CQuaternion& q)
{
    r -= q.r;
    v -= q.v;
    return *this;
}

const CQuaternion& CQuaternion::operator *=(const CQuaternion& q)
{
   r = r*q.r - v.dot(q.v);
   v.x = v.y * q.v.z - v.z * q.v.y + r * q.v.x + v.x*q.r;
   v.y = v.z * q.v.x - v.x * q.v.z + r * q.v.y + v.y*q.r;
   v.z = v.x * q.v.y - v.y * q.v.x + r * q.v.z + v.z*q.r;

   return *this;
}

const CQuaternion& CQuaternion::operator *=(float scale)
{
    r *= scale;
    v *= scale;
    return *this;
}

const CQuaternion& CQuaternion::operator/=(float scale)
{
    r /= scale;
    v /= scale;
    return *this;
}

float CQuaternion::length() const
{
    return sqrt(lengthSquared());
}

float CQuaternion::lengthSquared() const
{
    return (r*r + (v.dot(v)));
}

void CQuaternion::normalize()
{
    *this /= length();
}

CQuaternion CQuaternion::normalized() const
{
    return *this/length();
}

void CQuaternion::invert()
{
    v = -v;
}

CQuaternion CQuaternion::inverse() const
{
    return CQuaternion(r, -v);
}

void CQuaternion::toAxisAngle(CVector3f& axis, float& angle) const
{
    angle = acosf(r);

    float thetaInv = 1.0f/sinf(angle);

    axis.x = v.x * thetaInv;
    axis.y = v.y * thetaInv;
    axis.z = v.z * thetaInv;

    angle *= 2;
}

CQuaternion CQuaternion::log() const
{
    float a = acosf(r);
    float sina = sinf(a);
    CQuaternion ret;

    ret.r = 0;

    if (sina > 0)
    {
        ret.v.x = a * v.x / sina;
        ret.v.y = a * v.y / sina;
        ret.v.z = a * v.z / sina;
    }
    else
        ret.v = CVector3f(0);

    return ret;
}

CQuaternion CQuaternion::exp() const
{
    float a = (v.length());
    float sina = sinf(a);
    float cosa = cos(a);
    CQuaternion ret;

    ret.r = cosa;
    if (a > 0)
    {
        ret.v.x = sina * v.x / a;
        ret.v.y = sina * v.y / a;
        ret.v.z = sina * v.z / a;
    }
    else
        ret.v = CVector3f(0);

    return ret;
}



