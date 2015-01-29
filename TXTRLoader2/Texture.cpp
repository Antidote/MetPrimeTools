#include "Texture.hpp"
#include <Athena/BinaryWriter.hpp>
#include <sys/cdefs.h>
#include <memory.h>
#include <dds.h>
#include <squish.h>
#include <pngpp/png.hpp>

Texture::Texture()
{

}

Texture::~Texture()
{
    delete m_bits;
}

bool Texture::isNull() const
{
    return !m_bits;
}

uint8_t* Texture::bits() const
{
    return m_bits;
}

uint32_t Texture::linearSize() const
{
    return m_linearSize;
}

uint32_t Texture::dataSize() const
{
    return m_dataSize;
}

uint16_t Texture::width() const
{
    return m_width;
}

uint32_t Texture::mipmaps() const
{
    return m_mipmaps;
}

Texture::Format Texture::format() const
{
    return m_format;
}

void Texture::exportDDS(const std::string& path)
{
    DDS_HEADER header;
    memset(&header, 0, sizeof(DDS_HEADER));
    header.dwSize = sizeof(DDS_HEADER);
    header.dwFlags = DDSF_CAPS | DDSF_WIDTH | DDSF_HEIGHT | DDSF_MIPMAPCOUNT | DDSF_PIXELFORMAT;
    header.dwHeight = m_height;
    header.dwWidth = m_width;
    header.dwMipMapCount = m_mipmaps;
    memset(header.dwReserved1, 0, sizeof(uint32_t) * 11);
    header.ddspf.dwSize = sizeof(DDS_PIXELFORMAT);

    if (m_format == Format::Luminance || m_format == Format::LuminanceAlpha)
    {
        header.ddspf.dwFlags = (m_format == Format::Luminance ? DDSF_LUMINANCE : DDSF_LUMINANCE_ALPHA);
        header.ddspf.dwRGBBitCount = (m_format == Format::Luminance ? 8 : 16);
        header.ddspf.dwRBitMask = 0x000000FF;
        header.ddspf.dwABitMask = 0x0000FF00;
    }
    else if (m_format == Format::RGB565)
    {
        header.ddspf.dwFlags    = DDSF_RGB;
        header.ddspf.dwFourCC   = 0;
        header.ddspf.dwRGBBitCount = 16;
        header.ddspf.dwRBitMask = 0x0000F800;
        header.ddspf.dwGBitMask = 0x000007E0;
        header.ddspf.dwBBitMask = 0x0000001F;
    }
    else if (m_format == Format::RGBA8)
    {
        header.ddspf.dwFlags    = DDSF_RGBA;
        header.ddspf.dwFourCC   = 0;
        header.ddspf.dwRGBBitCount = 32;
        header.ddspf.dwRBitMask = 0x00FF0000;
        header.ddspf.dwGBitMask = 0x0000FF00;
        header.ddspf.dwBBitMask = 0x000000FF;
        header.ddspf.dwABitMask = 0xFF000000;
    }
    else if (m_format == Format::DXT1)
    {
        header.dwFlags |= DDSF_LINEARSIZE;
        header.dwPitchOrLinearSize = m_linearSize;
        header.ddspf.dwFlags = DDSF_FOURCC;
        header.ddspf.dwFourCC = *(uint32_t*)("DXT1");
        header.ddspf.dwRGBBitCount = 32;
    }

    header.dwCaps1 = DDSF_TEXTURE | DDSF_MIPMAP;

    Athena::io::BinaryWriter writer(path);
    writer.writeUint32(*(uint32_t*)("DDS\x20"));
    writer.writeUBytes((uint8_t*)&header, sizeof(DDS_HEADER));
    writer.writeUBytes(m_bits, m_dataSize);
    writer.save();
}

void Texture::exportPNG(const std::string& path)
{
    // Temporary
    png::image<png::rgba_pixel> output(m_width, m_height);
    uint8_t* rgba = nullptr;


    if (m_format == Format::RGBA8 || m_format == Format::RGB565)
        rgba = m_bits;
    else if (m_format == Format::DXT1)
    {
        rgba = new uint8_t[m_width * m_height * 4];
        squish::DecompressImage(rgba, m_width, m_height, m_bits, squish::kDxt1);
    }


    uint32_t i = 0;
    for (uint32_t y = 0; y < m_height; ++y)
    {
        for (uint32_t x = 0; x < m_width; ++x)
        {
            if (m_format == Format::Luminance || m_format == Format::LuminanceAlpha)
            {
                uint8_t l, a;
                if (m_format == Format::LuminanceAlpha)
                {
                    a = *(uint8_t*)(m_bits + i);
                    i++;
                }
                else
                    a = 0xFF;
                l = *(uint8_t*)(m_bits + i);
                i++;

                output.set_pixel(x, y, png::rgba_pixel(l, l, l, a));
            }
            else if (m_format == Format::RGB565)
            {
                uint16_t rgb = *(uint16_t*)(rgba + i);
                uint8_t r, g, b;
                r = ((rgb & 0xF800) >> 11) << 3;
                g = ((rgb & 0x07E0) >>  5) << 2;
                b = ((rgb & 0x001F) >>  0) << 3;
                output.set_pixel(x, y, png::rgba_pixel(r, g, b, 0xFF));
                i += 2;
            }
            else if (m_format == Format::RGBA8 || m_format == Format::DXT1)
            {

                output.set_pixel(x, y, png::rgba_pixel(*(char*)(rgba + i + 0),
                                                       *(char*)(rgba + i + 1),
                                                       *(char*)(rgba + i + 2),
                                                       *(char*)(rgba + i + 3)));
                i += 4;
            }
        }
    }

    output.write(path);
}

