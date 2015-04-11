#ifndef MATH_HPP
#define MATH_HPP

#include <math.h>

namespace Math
{
    const float PI = 3.14159265358979323846f;
    const float PI_2 = PI / 2;
    const float PI_4 = PI / 4;
    void clamp(float& val, float min, float max);
    void clamp(int& val, int min, int max);
    float radToDeg(float rad);
    float degToRad(float deg);
}

#endif // MATH_HPP
