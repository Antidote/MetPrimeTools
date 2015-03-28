#include "CVector3f.hpp"
#include <memory.h>
#include <cmath>
#include "Math.hpp"

CVector3f::CVector3f()
    : x(0),y(0),z(0)
{
}

CVector3f::CVector3f(float xyz)
{
    x = y = z = xyz;
}

CVector3f::CVector3f(float x, float y, float z)
    : x(x),y(y),z(z)
{
}

CVector3f::CVector3f(Athena::io::IStreamReader& input)
{
    x = input.readFloat();
    y = input.readFloat();
    z = input.readFloat();
}

CVector3f::~CVector3f()
{
}

bool CVector3f::operator ==(const CVector3f& rhs) const
{
    return (x == rhs.x && y == rhs.y && z == rhs.z);
}

CVector3f CVector3f::operator+(const CVector3f& rhs) const
{
    return CVector3f(x + rhs.x, y + rhs.y, z + rhs.z);
}

CVector3f CVector3f::operator-(const CVector3f& rhs) const
{
    return CVector3f(x - rhs.x, y - rhs.y, z - rhs.z);
}

CVector3f CVector3f::operator-() const
{
    return CVector3f(-x, -y, -z);
}

CVector3f CVector3f::operator*(const CVector3f& rhs) const
{
    return CVector3f(x * rhs.x, y * rhs.y, z * rhs.z);
}

CVector3f CVector3f::operator/(const CVector3f& rhs) const
{
    return CVector3f(x / rhs.x, y / rhs.y, z / rhs.z);
}

CVector3f CVector3f::operator+(float val) const
{
    return CVector3f(x + val, y + val, z + val);
}

CVector3f CVector3f::operator-(float val) const
{
    return CVector3f(x - val, y - val, z - val);
}


CVector3f operator+(float lhs, const CVector3f& rhs)
{
    return CVector3f(lhs + rhs.x, lhs + rhs.y, lhs + rhs.z);
}

CVector3f operator-(float lhs, const CVector3f& rhs)
{
    return CVector3f(lhs - rhs.x, lhs - rhs.y, lhs - rhs.z);
}

CVector3f operator*(float lhs, const CVector3f& rhs)
{
    return CVector3f(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
}

CVector3f operator/(float lhs, const CVector3f& rhs)
{
    return CVector3f(lhs / rhs.x, lhs / rhs.y, lhs / rhs.z);
}

CVector3f CVector3f::operator*(float val) const
{
    return CVector3f(x * val, y * val, z * val);
}

CVector3f CVector3f::operator/(float val) const
{
    return CVector3f(x / val, y / val, z / val);
}

const CVector3f& CVector3f::operator +=(const CVector3f& rhs)
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
}

const CVector3f& CVector3f::operator -=(const CVector3f& rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
}

const CVector3f& CVector3f::operator *=(const CVector3f& rhs)
{
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
    return *this;
}

const CVector3f& CVector3f::operator /=(const CVector3f& rhs)
{
    x /= rhs.x;
    y /= rhs.y;
    z /= rhs.z;
    return *this;
}

CVector3f CVector3f::normalized()
{
    CVector3f ret;
    float mag = length();
    if (!mag)
        return CVector3f();

    ret.x = x/mag;
    ret.y = y/mag;
    ret.z = z/mag;

    return ret;
}

CVector3f CVector3f::cross(const CVector3f& rhs) const
{
    return CVector3f(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
}

float CVector3f::dot(const CVector3f& rhs) const
{
    return (x * rhs.x) + (y * rhs.y) + (z * rhs.z);
}

float CVector3f::lengthSquared() const
{
    return x*x + y*y + z*z;
}

float CVector3f::length() const
{
    return sqrtf(lengthSquared());
}


float CVector3f::getAngleDiff(const CVector3f& a, const CVector3f& b)
{
    float mag1 = a.length();
    float mag2 = b.length();

    if (!mag1 || !mag2)
        return 0;

    float theta = acosf(a.dot(b) / (mag1 * mag2));
    return theta;
}

CVector3f CVector3f::lerp(const CVector3f& start, const CVector3f& end, float percent)
{
    return (start + percent * (end - start));
}

CVector3f CVector3f::slerp(const CVector3f& start, const CVector3f& end, float percent)
{
    float dot, theta, sinTheta, scale0, scale1;

    if (percent <= 0.0f)
        return start;
    else if (percent >= 1.0f)
        return end;

    dot = start.dot(end);
    if ((1.0f - dot) > 1e-6)
    {
        theta    = acos(dot);
        sinTheta = sin(theta);
        scale0   = sin((1.0f - percent) * dot) / sinTheta;
        scale1   = sin(percent * theta) / sinTheta;
    }
    else
    {
        scale0 = 1.0f - percent;
        scale1 = percent;
    }

    return (start * scale0 + end * scale1);
}
