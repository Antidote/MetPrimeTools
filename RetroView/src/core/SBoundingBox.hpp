#ifndef BOUNDINGBOX_HPP
#define BOUNDINGBOX_HPP
#include <MathLib.hpp>
#include <Athena/IStreamReader.hpp>

struct SBoundingBox final
{
    CVector3f m_min;
    CVector3f m_max;

    SBoundingBox()
        : m_min( 1000000.f,  1000000.f, 1000000.f),
          m_max(-1000000.f, -1000000.f,-1000000.f)
    {
    }

    SBoundingBox(const CVector3f& min, const CVector3f& max)
        : m_min(min),
          m_max(max)
    {
    }

    SBoundingBox(const float& minX,const float& minY,const float& minZ,
                const float& maxX,const float& maxY,const float& maxZ)
        : m_min(minX, minY, minZ),
          m_max(maxX, maxY, maxZ)
    {
    }
    
    inline void readBoundingBox(Athena::io::IStreamReader& in)
    {
        m_min[0] = in.readFloat();
        m_min[1] = in.readFloat();
        m_min[2] = in.readFloat();
        m_max[0] = in.readFloat();
        m_max[1] = in.readFloat();
        m_max[2] = in.readFloat();
    }
    SBoundingBox(Athena::io::IStreamReader& in) {readBoundingBox(in);}
    
    inline bool intersects(SBoundingBox& other)
    {
        if (m_max[0] < other.m_min[0]) return false;
        if (m_min[0] > other.m_max[0]) return false;
        if (m_max[1] < other.m_min[1]) return false;
        if (m_min[1] > other.m_max[1]) return false;
        if (m_max[2] < other.m_min[2]) return false;
        if (m_min[2] > other.m_max[2]) return false;
        return true;
    }
};

inline bool operator ==(const SBoundingBox& left, const SBoundingBox& right)
{
    return (left.m_min == right.m_min && left.m_max == right.m_max);
}
inline bool operator !=(const SBoundingBox& left, const SBoundingBox& right)
{
    return (left.m_min != right.m_min || left.m_max != right.m_max);

}

#endif // BOUNDINGBOX_HPP
