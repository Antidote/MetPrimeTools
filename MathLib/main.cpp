#include <iostream>
#include "CVector3d.hpp"
#include "CQuaternion.hpp"
#include "Math.hpp"
#include <math.h>
#include <iomanip>

int main()
{
    CVector3f slerp1 = CVector3f(1.0, 0.0, 0.0);
    CVector3f slerp2 = CVector3f(0.5, 256.0, 4096.0);
    CQuaternion quat = CQuaternion::fromAxisAngle(CVector3f(1.0, .5, 0.0), Math::degToRad(45.f));
    CVector3f axis;
    float angle;
    quat.invert();
    quat.toAxisAngle(axis, angle);
    CVector3f test = CQuaternion::rotate(quat, CVector3f(1.0, 1.0f, 1.0f));
    CQuaternion quat2 = CQuaternion(test);
    std::cout << axis.x << " " << axis.y << " " << axis.z << " " << Math::radToDeg(angle) << "d" << std::endl;
    quat2.toAxisAngle(axis, angle);
    std::cout << axis.x << " " << axis.y << " " << axis.z << " " << Math::radToDeg(angle) << "d" << std::endl;
    std::cout << test.x << " " << test.y << " " << test.z << std::endl;

    return 0;
}

