#ifndef CTEXTURE_HPP
#define CTEXTURE_HPP

#include <Texture.hpp>
#include <QImage>
#include "core/CResourceManager.hpp"

class CTexture final : public IResource
{
    DEFINE_RESOURCE_LOADER();
public:
    CTexture(Texture* texture);
    ~CTexture();

    static IResource* loadByData(const atUint8* data, atUint64 length);

    void bind();

    atUint32 textureID() const;

    QImage toQImage();
private:
    Texture* m_texture;
    atUint32 m_textureID;
};

#endif // CTEXTURE_HPP
