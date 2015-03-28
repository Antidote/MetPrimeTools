#ifndef CINDEXBUFFER_HPP
#define CINDEXBUFFER_HPP

#include <Athena/Global.hpp>
#include "core/GXTypes.hpp"
#include <vector>

class CIndexBuffer
{
public:
    CIndexBuffer();
    virtual ~CIndexBuffer();

    void addIndices(EPrimitive type, std::vector<atUint32> indices);
    void initBuffer();
    void bindBuffer();
    void drawBuffer();
private:
    void trianglesToStrips(atUint32* indices, atUint32 count);
    void fansToStrips(atUint32* indices, atUint32 count);
    void quadsToStrips(atUint32* indices, atUint32 count);
    bool     m_buffered;
    atUint32 m_iboId;
    std::vector<atUint32> m_indices;
};

#endif // CINDEXBUFFER_HPP
