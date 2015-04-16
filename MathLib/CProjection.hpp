#ifndef CPROJECTION_HPP
#define CPROJECTION_HPP

#include "Global.hpp"
#include <stdexcept>
#include <math.h>

typedef union
{
    float m[4][4];
#if __SSE__
    __m128 mVec128[4];
#endif
} TMatrix4f;
static inline void copyMatrix4f(TMatrix4f& dest, const TMatrix4f& src)
{
#if __SSE__
    dest.mVec128[0] = src.mVec128[0];
    dest.mVec128[1] = src.mVec128[1];
    dest.mVec128[2] = src.mVec128[2];
    dest.mVec128[3] = src.mVec128[3];
#else
    dest.m[0][0] = src.m[0][0];
    dest.m[0][1] = src.m[0][1];
    dest.m[0][2] = src.m[0][2];
    dest.m[0][3] = src.m[0][3];
    dest.m[1][0] = src.m[1][0];
    dest.m[1][1] = src.m[1][1];
    dest.m[1][2] = src.m[1][2];
    dest.m[1][3] = src.m[1][3];
    dest.m[2][0] = src.m[2][0];
    dest.m[2][1] = src.m[2][1];
    dest.m[2][2] = src.m[2][2];
    dest.m[2][3] = src.m[2][3];
    dest.m[3][0] = src.m[3][0];
    dest.m[3][1] = src.m[3][1];
    dest.m[3][2] = src.m[3][2];
    dest.m[3][3] = src.m[3][3];
#endif
}
extern const TMatrix4f kIdentityMtx4;

enum EProjType
{
    PROJ_NONE = 0,
    PROJ_ORTHO = 1,
    PROJ_PERSP = 2
};
struct SProjOrtho
{
    float top, bottom, left, right, near, far;
    SProjOrtho(float top=1.0f, float bottom=-1.0f, float left=-1.0f, float right=1.0f,
               float near=1.0f, float far=-1.0f) :
    top(top), bottom(bottom), left(left), right(right), near(near), far(far) {}
};
struct SProjPersp
{
    float fov, aspect, near, far;
    SProjPersp(float fov=55.0f * M_PI / 180.0f, float aspect=1.0f, float near=0.1f, float far=4096.f) :
    fov(fov), aspect(aspect), near(near), far(far) {}
};
extern const SProjOrtho kOrthoIdentity;

class ZE_ALIGN(16) CProjection
{
    void _updateCachedMatrix();
public:
    ZE_DECLARE_ALIGNED_ALLOCATOR();

    CProjection()
    {
        m_projType = PROJ_ORTHO;
        m_ortho = SProjOrtho();
        copyMatrix4f(m_mtx, kIdentityMtx4);
    }
    CProjection(const CProjection& other) {*this = other;}
    CProjection(const SProjOrtho& ortho) {setOrtho(ortho);}
    CProjection(const SProjPersp& persp) {setPersp(persp);}
    
    inline CProjection& operator=(const CProjection& other)
    {
        if (this != &other)
        {
            m_projType = other.m_projType;
            m_ortho = other.m_ortho;
            copyMatrix4f(m_mtx, other.m_mtx);
        }
        return *this;
    }
    
    inline void setOrtho(const SProjOrtho& ortho)
    {m_projType = PROJ_ORTHO; m_ortho = ortho; _updateCachedMatrix();}
    inline void setPersp(const SProjPersp& persp)
    {m_projType = PROJ_PERSP; m_persp = persp; _updateCachedMatrix();}
    
    inline EProjType getType() const {return m_projType;}
    inline const SProjOrtho& getOrtho() const
    {
        if (m_projType != PROJ_ORTHO)
            throw std::runtime_error("attempted to access orthographic structure of non-ortho projection");
        return m_ortho;
    }
    inline const SProjPersp& getPersp() const
    {
        if (m_projType != PROJ_PERSP)
            throw std::runtime_error("attempted to access perspective structure of non-persp projection");
        return m_persp;
    }
    
    inline const TMatrix4f& getCachedMatrix() {return m_mtx;}

protected:
    
    /* Projection type */
    enum EProjType m_projType;
    
    /* Projection intermediate */
    union
    {
        SProjOrtho m_ortho;
        SProjPersp m_persp;
    };
    
    /* Cached projection matrix */
    TMatrix4f m_mtx;
    
};

#endif // CMATRIX3F_HPP
