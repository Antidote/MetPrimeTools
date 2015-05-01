#include "core/GLInclude.hpp"
#include "generic/CTexture.hpp"
#include "ui/CGLViewer.hpp"

#include <QDir>
#include <QGLPixelBuffer>
#include <TextureReader.hpp>

CTexture::CTexture(Texture* texture)
    : m_texture(texture),
      m_textureID(0)
{
}

CTexture::~CTexture()
{
    glDeleteTextures(1, &m_textureID);
}

IResource* CTexture::loadByData(const atUint8* data, atUint64 length)
{
    Texture* tex = nullptr;
    CTexture* ret = nullptr;
    try
    {
        TextureReader reader(data, length);
        tex = reader.read();
        ret = new CTexture(tex);
        tex->exportPNG("test.png");
    }
    catch(...)
    {
        delete tex;
        tex = nullptr;
        delete ret;
        ret = nullptr;
        throw;
    }

    return ret;
}

void CTexture::bind()
{
    if (m_textureID == 0)
    {
        glGenTextures(1, &m_textureID);
        glBindTexture(GL_TEXTURE_2D, m_textureID);

        GLenum format, type;
        bool compressed = false;

        switch(m_texture->format())
        {
            case Texture::Format::RGB565:
                format = GL_RGB;
                type = GL_UNSIGNED_SHORT_5_6_5;
                break;
            case Texture::Format::RGBA8:
                format = GL_RGBA;
                type = GL_UNSIGNED_BYTE;
                break;
            case Texture::Format::DXT1:
                format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
                compressed = true;
                break;
        }

        atUint32 mipSize = m_texture->linearSize();
        atUint32 mipOffset = 0;
        atUint16 mipW = m_texture->width(), mipH = m_texture->height();
        for (atUint32 m =0; m < m_texture->mipmaps(); m++)
        {
            if (!compressed)
                glTexImage2D(GL_TEXTURE_2D, m, format, mipW, mipH, 0, format, type, m_texture->bits() + mipOffset);
            else
                glCompressedTexImage2D(GL_TEXTURE_2D, m, format, mipW, mipH, 0, mipSize, m_texture->bits() + mipOffset);

            mipW /= 2;
            mipH /= 2;
            mipOffset += mipSize;
            mipSize /=4;
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, m_texture->mipmaps() - 1);

        // linear filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        // Aniso
        atInt32 maxAniso = 0;
        glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
    }

    glBindTexture(GL_TEXTURE_2D, m_textureID);
}

atUint32 CTexture::textureID() const
{
    return m_textureID;
}

QImage CTexture::toQImage()
{
    m_texture->exportPNG("tmp.png");
    QImage ret("tmp.png");
    QDir().remove("tmp.png");

    return ret;
}

REGISTER_RESOURCE_LOADER(CTexture, "TXTR", loadByData);
