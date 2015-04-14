#ifndef CMATRIX3F_HPP
#define CMATRIX3F_HPP

#include "Global.hpp"
#include "CVector3f.hpp"

class CQuaternion;
class CMatrix3f
{
public:
    explicit CMatrix3f(bool zero = false);
    CMatrix3f(float m00, float m01, float m02,
              float m10, float m11, float m12,
              float m20, float m21, float m22);
    CMatrix3f(const CVector3f& u, const CVector3f& m, const CVector3f& w);
    CMatrix3f(const CVector3f& axis, float angle);
    CMatrix3f(const CQuaternion& quat);

    ~CMatrix3f();

    static const CMatrix3f skIdentityMatrix3f;

protected:
    union
    {
        float m[3][3];
#ifdef __SSE__
        __m128 _m[3];
#endif
    };
};

#endif // CMATRIX3F_HPP
