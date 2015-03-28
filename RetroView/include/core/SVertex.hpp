#ifndef SVERTEX
#define SVERTEX

#include <glm/glm.hpp>
#include <Athena/Global.hpp>
#include <memory.h>

struct SVertex
{
    glm::vec3 pos;
    glm::vec3 norm;
    atUint32  color[2];
    glm::vec2 texCoords[8];

    SVertex()
    {
        memset(&pos,      0, sizeof(glm::vec3));
        memset(color,     0, sizeof(color));
        memset(&norm,     0, sizeof(glm::vec3));
        memset(texCoords, 0, sizeof(texCoords));
    }

    bool operator ==(const SVertex& that) const
    {
        return !memcmp((void*)this, (void*)&that, sizeof(SVertex));
    }
};

#endif // SVERTEX

