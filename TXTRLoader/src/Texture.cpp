#include "Texture.hpp"
#include <Athena/MemoryWriter.hpp>
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

atUint8* Texture::bits() const
{
    return m_bits;
}

atUint32 Texture::linearSize() const
{
    return m_linearSize;
}

atUint32 Texture::dataSize() const
{
    return m_dataSize;
}

atUint16 Texture::width() const
{
    return m_width;
}

atUint16 Texture::height() const
{
    return m_height;
}

atUint32 Texture::mipmaps() const
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
    memset(header.dwReserved1, 0, sizeof(atUint32) * 11);
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
        header.ddspf.dwRBitMask = 0x000000FF;
        header.ddspf.dwGBitMask = 0x0000FF00;
        header.ddspf.dwBBitMask = 0x00FF0000;
        header.ddspf.dwABitMask = 0xFF000000;
    }
    else if (m_format == Format::DXT1)
    {
        header.dwFlags |= DDSF_LINEARSIZE;
        header.dwPitchOrLinearSize = m_linearSize;
        header.ddspf.dwFlags = DDSF_FOURCC;
        atUint32 fourCC = *(atUint32*)("DXT1");
        Athena::utility::LittleUint32(fourCC);
        header.ddspf.dwFourCC = fourCC;
        header.ddspf.dwRGBBitCount = 32;
    }

    header.dwCaps1 = DDSF_TEXTURE | DDSF_MIPMAP;

    Athena::io::MemoryWriter writer(path);
    writer.writeUint32(*(atUint32*)("DDS\x20"));
    writer.writeUBytes((atUint8*)&header, sizeof(DDS_HEADER));
    writer.writeUBytes(m_bits, m_dataSize);
    writer.save();
}

void Texture::exportPNG(const std::string& path)
{
    // Temporary
    png::image<png::rgba_pixel> output(m_width, m_height);
    atUint8* pixels = nullptr;


    if (m_format == Format::RGBA8 || m_format == Format::RGB565)
        pixels = m_bits;
    else if (m_format == Format::DXT1)
    {
        pixels = new atUint8[m_width * m_height * 4];
        squish::DecompressImage(pixels, m_width, m_height, m_bits, squish::kDxt1);
    }


    atUint32 i = 0;
    for (atUint32 y = 0; y < m_height; ++y)
    {
        for (atUint32 x = 0; x < m_width; ++x)
        {
            // Luminance, and Luminance Alpha are pretty simple
            // for Luminance we just assign 'l' to our RGB values, and set alpha to 255
            // for Luminance Alpha the alpha value is directly after the luminance value,
            // and we use that value instead
            if (m_format == Format::Luminance || m_format == Format::LuminanceAlpha)
            {
                atUint8 l, a;
                l = *(atUint8*)(m_bits + i);
                i++;
                if (m_format == Format::LuminanceAlpha)
                {
                    a = *(atUint8*)(m_bits + i);
                    i++;
                }
                else
                    a = 0xFF;

                output.set_pixel(x, y, png::rgba_pixel(l, l, l, a));
            }
            // RGB565 is an interesting format, it's in a 565 bit packing (hence RGB565 = Red 5-bits Green 6-bits Blue 5-bits)
            // what this also means is that green gets truncated less in comparison to red and blue,
            // Other than havingt to unpack the bits, it's in reverse order (red being last)
            else if (m_format == Format::RGB565)
            {
                atUint16 rgb = *(atUint16*)(pixels + i);
                atUint8 r, g, b;
                r = ((rgb & 0xF800) >> 11) << 3;
                g = ((rgb & 0x07E0) >>  5) << 2;
                b = ((rgb & 0x001F) >>  0) << 3;
                output.set_pixel(x, y, png::rgba_pixel(r, g, b, 0xFF));
                i += 2;
            }
            // DXT1 and RGBA8 are the same, it's simply a straight copy.
            else if (m_format == Format::DXT1 || m_format == Format::RGBA8)
            {

                output.set_pixel(x, y, png::rgba_pixel(*(char*)(pixels + i + 0),
                                                       *(char*)(pixels + i + 1),
                                                       *(char*)(pixels + i + 2),
                                                       *(char*)(pixels + i + 3)));
                i += 4;
            }
        }
    }

    // We only delete the pixelbuf if the format is DXT1, otherwise you roast the original data
    if (m_format == Format::DXT1)
        delete[] pixels;

    output.write(path);
}

