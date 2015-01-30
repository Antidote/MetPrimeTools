#ifndef CTEXTURE_HPP
#define CTEXTURE_HPP

#include <Texture.hpp>
#include <QImage>
#include "CResourceManager.hpp"

class CTexture : public IResource
{
    DEFINE_RESOURCE_LOADER();
public:
    CTexture(const QImage& texture);
    ~CTexture();

    static IResource* loadByData(const atUint8* data, atUint64 length);
    static IResource* loadByFile(const std::string& file);

    void create();

    atUint32 textureID() const;
private:
    QImage   m_texture;
    atUint32 m_textureID;
};

#endif // CTEXTURE_HPP
