#include "Math.hpp"

void Math::clamp(float& val, float min, float max)
{
    if (val < min)
        val = min;
    else if (val > max)
        val = max;
}

void Math::clamp(int& val, int min, int max)
{
    if (val < min)
        val = min;
    else if (val > max)
        val = max;
}


float Math::radToDeg(float rad)
{
    return rad * 180.f/PI;
}

float Math::degToRad(float deg)
{
    return deg * PI/180;
}
