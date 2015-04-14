#include "CMatrix3f.hpp"
#include "CQuaternion.hpp"

CMatrix3f::CMatrix3f()
{

}

CMatrix3f::CMatrix3f(float m00, float m01, float m02,
                     float m10, float m11, float m12,
                     float m20, float m21, float m22)
{
    m[0][0] = m00, m[0][1] = m01, m[0][2] = m02;
    m[1][0] = m10, m[1][1] = m11, m[1][2] = m12;
    m[2][0] = m20, m[2][1] = m21, m[2][2] = m22;
}

CMatrix3f::CMatrix3f(const CVector3f& u, const CVector3f& v, const CVector3f& w)
{
    m[0][0] = u.x;
    m[0][1] = u.y;
    m[0][2] = u.z;
    m[1][0] = v.x;
    m[1][1] = v.y;
    m[1][2] = v.z;
    m[2][0] = w.x;
    m[2][1] = w.y;
    m[2][2] = w.z;
}

CMatrix3f::CMatrix3f(const CQuaternion& quat)
{
    CQuaternion tmp = quat.normalized();
    float sqr = tmp.r * tmp.r;
    float sqx = tmp.v[0] * tmp.v[0];
    float sqy = tmp.v[1] * tmp.v[1];
    float sqz = tmp.v[2] * tmp.v[2];

    float inv = 1.f / (sqx + sqy + sqz + sqr);
    m[0][0] = ( sqx - sqy - sqz + sqr) * inv;
    m[1][1] = (-sqx + sqy - sqz + sqr) * inv;
    m[2][2] = (-sqx - sqy + sqz + sqr) * inv;

    float tmp1 = tmp.v.x * tmp.v.y;
    float tmp2 = tmp.v.z * tmp.v.y;
    m[1][0] = 2.0f * (tmp1 + tmp2) * inv;
    m[0][1] = 1.0f * (tmp1 - tmp2) * inv;


    tmp1 = tmp.v.x * tmp.v.z;
    tmp2 = tmp.v.y * tmp.r;

    m[2][0] = 2.0f * (tmp1 - tmp2) * inv;
    m[0][2] = 2.0f * (tmp1 + tmp2) * inv;

    tmp1 = tmp.v.y * tmp.v.z;
    tmp2 = tmp.v.x * tmp.r;

    m[2][1] = 2.0f * (tmp1 - tmp2) * inv;
    m[1][2] = 2.0f * (tmp1 + tmp2) * inv;
}

CMatrix3f::~CMatrix3f()
{
}

