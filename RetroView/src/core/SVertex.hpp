#ifndef SVERTEX
#define SVERTEX

#include <MathLib.hpp>
#include <Athena/Global.hpp>
#include <memory.h>

struct SVertex
{
    CVector3f pos;
    CVector3f norm;
    atUint32  color[2];
    CVector3f texCoords[8];

    SVertex()
    {
        memset(&pos,      0, sizeof(CVector3f));
        memset(color,     0, sizeof(color));
        memset(&norm,     0, sizeof(CVector3f));
        memset(texCoords, 0, sizeof(texCoords));
    }

    bool operator ==(const SVertex& that) const
    {
        return !memcmp((void*)this, (void*)&that, sizeof(SVertex));
    }
};

#endif // SVERTEX

