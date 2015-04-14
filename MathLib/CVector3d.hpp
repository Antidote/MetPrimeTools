#ifndef CVECTOR3D_HPP
#define CVECTOR3D_HPP

#include <Athena/IStreamReader.hpp>

#include "CVector3f.hpp"

class CVector3d
{
public:
    CVector3d();
    CVector3d(const CVector3f& vec3);
    CVector3d(double x, double y, double z);
    CVector3d(Athena::io::IStreamReader& input);
    ~CVector3d();

    bool operator ==(const CVector3d& rhs);
    CVector3d operator+(const CVector3d& rhs);
    CVector3d operator-(const CVector3d& rhs);
    CVector3d operator-() const;
    CVector3d operator*(const CVector3d& rhs);
    CVector3d operator/(const CVector3d& rhs);
    CVector3d operator+(double val);
    CVector3d operator-(double val);
    CVector3d operator*(double val);
    CVector3d operator/(double val);
    CVector3d normalized();
    CVector3d cross(const CVector3d& rhs) const;
    double    dot(const CVector3d& rhs) const;
    double    magnitude() const;

    CVector3f asVector3f();
    union
    {
        struct
        {
            double x, y, z;
        };
        __m128 v;
    };
}
#if __SSE__
_CRT_ALIGN(16)
#endif
;

#endif // CVECTOR3D_HPP
