#ifndef BOUNDINGBOX_HPP
#define BOUNDINGBOX_HPP
#include <glm/glm.hpp>

struct SBoundingBox
{
    glm::vec3 min;
    glm::vec3 max;

    SBoundingBox()
        : min( 1000000.f,  1000000.f, 1000000.f),
          max(-1000000.f, -1000000.f,-1000000.f)
    {
    }

    SBoundingBox(const glm::mat2x3& bbox)
        : min(bbox[0]),
          max(bbox[1])
    {
    }

    SBoundingBox(const float& minX,const float& minY,const float& minZ,
                const float& maxX,const float& maxY,const float& maxZ)
        : min(minX, minY, minZ),
          max(maxX, maxY, maxZ)
    {
    }
};

inline bool operator ==(const SBoundingBox& left, const SBoundingBox& right)
{
    return (left.min == right.min && left.max == right.max);
}
inline bool operator !=(const SBoundingBox& left, const SBoundingBox& right)
{
    return (left.min != right.min || left.max != right.max);

}

#endif // BOUNDINGBOX_HPP
