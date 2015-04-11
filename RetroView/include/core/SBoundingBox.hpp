#ifndef BOUNDINGBOX_HPP
#define BOUNDINGBOX_HPP
#include <glm/glm.hpp>
#include <Athena/IStreamReader.hpp>

struct SBoundingBox final
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
    
    inline void readBoundingBox(Athena::io::IStreamReader& in)
    {
        min[0] = in.readFloat();
        min[1] = in.readFloat();
        min[2] = in.readFloat();
        max[0] = in.readFloat();
        max[1] = in.readFloat();
        max[2] = in.readFloat();
    }
    SBoundingBox(Athena::io::IStreamReader& in) {readBoundingBox(in);}
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
