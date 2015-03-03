#ifndef CINDEXBUFFER_HPP
#define CINDEXBUFFER_HPP

#include <Athena/Global.hpp>
#include <vector>

class CIndexBuffer
{
public:
    CIndexBuffer(atUint32 materialId);
    virtual ~CIndexBuffer();

    void addIndices(atUint32 type, std::vector<atUint16> indices);
    atUint32 materialId();
private:
    friend class CModelData;
    void trianglesToStrips(atUint16* indices, atUint16 count);
    void fansToStrips(atUint16* indices, atUint16 count);
    void quadsToStrips(atUint16* indices, atUint16 count);
    atUint32 m_iboId;
    atUint32 m_materialId;
    std::vector<atUint16> m_indices;
};

#endif // CINDEXBUFFER_HPP
