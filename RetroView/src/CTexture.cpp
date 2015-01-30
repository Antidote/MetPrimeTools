#include "CTexture.hpp"
#include "CGLViewer.hpp"
#include <QDir>
#include <QGLPixelBuffer>
#include <TextureReader.hpp>
#include <GL/gl.h>

CTexture::CTexture(const QImage& texture)
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
        tex->exportDDS("tmp.dds");

        ret = new CTexture(QImage("tmp.dds"));

        QDir().remove("tmp.dds");
    }
    catch(...)
    {
        delete tex;
        tex = nullptr;
        delete ret;
        ret = nullptr;
    }

    return ret;
}

IResource* CTexture::loadByFile(const std::string& file)
{
    Texture* tex = nullptr;
    CTexture* ret = nullptr;
    try
    {
        TextureReader reader(file);
        tex = reader.read();
        tex->exportDDS("tmp.dds");

        ret = new CTexture(QImage("tmp.dds"));

        QDir().remove("tmp.dds");
    }
    catch(...)
    {
        delete tex;
        tex = nullptr;
        delete ret;
        ret = nullptr;
    }

    return ret;
}

void CTexture::create()
{
    if (m_textureID == 0)
    {
        // Ensure the image data is RGBA formatted
        if (m_texture.format() != QImage::Format_ARGB32)
            m_texture = m_texture.convertToFormat(QImage::Format_ARGB32);

        m_texture = CGLViewer::convertToGLFormat(m_texture);

        m_texture = m_texture.transformed(QTransform::fromScale(1, -1));
        glGenTextures(1, &m_textureID);
        glBindTexture(GL_TEXTURE_2D, m_textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_texture.width(), m_texture.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_texture.bits());
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // Linear Filtering
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // Linear Filtering
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT); // Linear Filtering
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT); // Linear Filtering
    }
}

atUint32 CTexture::textureID() const
{
    return m_textureID;
}

REGISTER_RESOURCE_LOADER(CTexture, "txtr", loadByFile, loadByData);
