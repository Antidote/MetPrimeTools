#ifndef CMATRIX3F_HPP
#define CMATRIX3F_HPP

#include "CVector3f.hpp"

class CMatrix3f
{
public:
    CMatrix3f();
    CMatrix3f(bool zero);
    CMatrix3f(float m00, float m01, float m02,
              float m10, float m11, float m12,
              float m20, float m21, float m22);
    CMatrix3f(const CVector3f& u, const CVector3f& v, const CVector3f& w);
    CMatrix3f(const CVector3f& axis, float angle);

    ~CMatrix3f();

protected:
    float v[3][3];
};

#endif // CMATRIX3F_HPP
