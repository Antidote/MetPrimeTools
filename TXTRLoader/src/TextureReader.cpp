#include "TextureReader.hpp"
#include <Athena/InvalidDataException.hpp>
#include <RetroCommon.hpp>
#include "pngpp/png.hpp"

static const float sizeOutputMultiplierLut[] =
{ // number of pixels * this = number of bytes
  4.f, 4.f, 4.f, 4.f, 4.f, 4.f, 0.f, 2.f, 4.f, 4.f, 0.5f
};

static const atUint32 bppOutputMultiplierLut[] =
{ // source BPP * this = output BPP
  2, 1, 2, 1, 4, 2, 0, 1, 2, 1, 1
};

static const atUint32 pixelStrideLut[] =
{ // size of one pixel in output data in bytes
  4, 4, 4, 4, 2, 2, 0, 2, 4, 4, 8
};

static const atUint32 blockWidthLut[] =
{
    8, 8, 8, 4, 8, 8, 0, 4, 4, 4, 2
};

static const atUint32 blockHeightLut[] =
{
    8, 4, 4, 4, 8, 4, 0, 4, 4, 4, 2
};

TextureReader::TextureReader(const atUint8* data, atUint64 length)
    : base(data, length),
      m_paletteStream(nullptr)
{
    base::setEndian(Athena::Endian::BigEndian);
}

TextureReader::TextureReader(const std::string &filename)
    : base(filename),
      m_paletteStream(nullptr)
{
    base::setEndian(Athena::Endian::BigEndian);
}

TextureReader::~TextureReader()
{
    delete m_paletteStream;
}

Texture* TextureReader::read()
{
    Texture* ret = nullptr;

    try
    {
        atUint32 fmt = base::readUint32();

        if (fmt > (atUint32)GXTextureFormat::CMPR)
        {
            Athena::io::MemoryWriter writer;

            decompressFile(writer, data(), length());

            if (writer.length() > 0)
            {
                setData(writer.data(), writer.length());
                fmt = base::readUint32();
            }
            else
                THROW_INVALID_DATA_EXCEPTION("Invalid texture data, unable to decompress");
        }

        m_format  = (GXTextureFormat)fmt;
        m_width   = base::readUint16();
        m_height  = base::readUint16();
        m_mipmaps = base::readUint32();

        if (m_format == GXTextureFormat::C4 || m_format == GXTextureFormat::C8)
        {
            m_hasPalette = true;

            m_palFormat = (GXPaletteFormat)base::readUint32();

            base::seek(4);

            atUint32 entryCount = (m_format == GXTextureFormat::C4) ? 16 : 256;
            atUint8* palData = base::readUBytes(entryCount * 2);

            m_paletteStream = new Athena::io::MemoryReader(palData, entryCount * 2);
        }
        else
            m_hasPalette = false;

        atUint32 imageStart = base::position();
        atUint32 imageDataLength = base::length() - imageStart;

        atUint32 dataBufferSize = imageDataLength * bppOutputMultiplierLut[(atUint32)m_format];

        if (m_hasPalette && m_palFormat == GXPaletteFormat::RGB5A3)
            dataBufferSize *= 2;

        atUint8* data = new atUint8[dataBufferSize];

        Athena::io::MemoryWriter buf{data, dataBufferSize};

        decode(*this, buf);

        ret = new Texture;
        switch (m_format)
        {
            case GXTextureFormat::I4:
            case GXTextureFormat::IA4:
            case GXTextureFormat::IA8:
            case GXTextureFormat::I8:
                ret->m_format = Texture::Format::RGBA8;
                break;
            case GXTextureFormat::C4:
            case GXTextureFormat::C8:
                if (m_palFormat == GXPaletteFormat::RGB565)
                    ret->m_format = Texture::Format::RGB565;
                if (m_palFormat == GXPaletteFormat::RGB5A3 || m_palFormat == GXPaletteFormat::IA8)
                    ret->m_format = Texture::Format::RGBA8;
                break;
            case GXTextureFormat::RGB565:
                ret->m_format = Texture::Format::RGB565;
                break;
            case GXTextureFormat::RGB5A3:
            case GXTextureFormat::RGBA8:
                ret->m_format = Texture::Format::RGBA8;
                break;
            case GXTextureFormat::CMPR:
                ret->m_format = Texture::Format::DXT1;
                break;
        }

        ret->m_width      = m_width;
        ret->m_height     = m_height;
        ret->m_mipmaps    = m_mipmaps;
        ret->m_bits       = buf.data();
        ret->m_dataSize   = buf.length();
        ret->m_linearSize = m_width * m_height * sizeOutputMultiplierLut[(atUint32)m_format];
    }
    catch(...)
    {
        delete ret;
        ret = nullptr;
        throw;
    }

    return ret;
}

void TextureReader::decode(Athena::io::MemoryReader& in, Athena::io::MemoryWriter& buf)
{
    atUint32 mipWidth = m_width, mipHeight = m_height;
    atUint32 mipOffset = 0;

    // I stole this little trick from Parax, so I'll let him explain what's going on here:
    // With CMPR, we're using a little trick.
    // CMPR stores pixels in 8x8 blocks, with four 4x4 subblocks.
    // An easy way to convert it is to pretend each block is 2x2 and each subblock is one pixel.
    // So to do that we need to calculate the "new" dimensions of the image, 1/4 the size of the original.
    if (m_format == GXTextureFormat::CMPR)
    {
        mipWidth /= 4;
        mipHeight /= 4;
    }

    atUint32 blockWidth = blockWidthLut[(atUint32)m_format];
    atUint32 blockHeight = blockHeightLut[(atUint32)m_format];

    atUint32 pxStride = pixelStrideLut[(atUint32)m_format];
    if (m_hasPalette && (m_palFormat == GXPaletteFormat::RGB5A3))
        pxStride = 4;

    for (atUint32 m = 0; m < m_mipmaps; m++)
    {
        for (atUint32 blockY = 0; blockY < mipHeight; blockY += blockHeight)
        {
            for (atUint32 blockX = 0; blockX < mipWidth; blockX += blockWidth)
            {
                for (atUint32 imgY = blockY; imgY < blockY + blockHeight; imgY++)
                {
                    for (atUint32 imgX = blockX; imgX < blockX + blockWidth; imgX++)
                    {
                        atUint32 pos = ((imgY * mipWidth) + imgX) * pxStride;
                        buf.seek(mipOffset + pos, Athena::SeekOrigin::Begin);

                        switch(m_format)
                        {
                            case GXTextureFormat::I4:     readPixelI4     (in, buf); break;
                            case GXTextureFormat::I8:     readPixelI8     (in, buf); break;
                            case GXTextureFormat::IA4:    readPixelIA4    (in, buf); break;
                            case GXTextureFormat::IA8:    readPixelIA8    (in, buf); break;
                            case GXTextureFormat::C4:     readPixelC4     (in, buf); break;
                            case GXTextureFormat::C8:     readPixelC8     (in, buf); break;
                            case GXTextureFormat::RGB565: readPixelRGB565 (in, buf); break;
                            case GXTextureFormat::RGB5A3: readPixelRGB5A3 (in, buf); break;
                            case GXTextureFormat::RGBA8:  readPixelRGBA8  (in, buf); break;
                            case GXTextureFormat::CMPR:   readCMPRSubBlock(in, buf); break;
                        }

                        if (m_format == GXTextureFormat::I4 || m_format == GXTextureFormat::C4)
                            imgX++;
                    }
                }
                if (m_format ==  GXTextureFormat::RGBA8)
                    in.seek(0x20, Athena::SeekOrigin::Current);
            }
        }

        atUint32 mipSize = mipWidth * mipHeight * sizeOutputMultiplierLut[(atUint32)m_format];
        if (m_format == GXTextureFormat::CMPR)
            mipSize *= 16;

        mipOffset += mipSize;
        mipWidth /= 2;
        mipHeight /= 2;
        if (mipWidth < blockWidth)
            mipWidth = blockWidth;
        if (mipHeight < blockHeight)
            mipHeight = blockHeight;
    }
}

void TextureReader::readPixelI4(Athena::io::MemoryReader& in, Athena::io::MemoryWriter& out)
{
    atUint8 px = in.readByte();
    atUint8 l1 = extend4To8(px >> 4);
    atUint8 l2 = extend4To8(px);
    out.writeByte(l1);
    out.writeByte(l1);
    out.writeByte(l1);
    out.writeByte(l1);
    out.writeByte(l2);
    out.writeByte(l2);
    out.writeByte(l2);
    out.writeByte(l2);
}

void TextureReader::readPixelI8(Athena::io::MemoryReader& in, Athena::io::MemoryWriter& out)
{
    atUint8 l = in.readByte();
    out.writeByte(l);
    out.writeByte(l);
    out.writeByte(l);
    out.writeByte(l);
}

void TextureReader::readPixelIA4(Athena::io::MemoryReader& in, Athena::io::MemoryWriter& out)
{
    atUint8 b = in.readByte();
    atUint8 a = extend4To8(b >> 4);
    atUint8 l = extend4To8(b);
    out.writeByte(l);
    out.writeByte(l);
    out.writeByte(l);
    out.writeByte(a);
}

void TextureReader::readPixelIA8(Athena::io::MemoryReader& in, Athena::io::MemoryWriter& out)
{
    atUint8 a = in.readByte();
    atUint8 l = in.readByte();
    out.writeByte(l);
    out.writeByte(l);
    out.writeByte(l);
    out.writeByte(a);
}

void TextureReader::readPixelC4(Athena::io::MemoryReader& in, Athena::io::MemoryWriter& out)
{
    atUint8 b = in.readByte();
    atUint8 indices[2];
    indices[0] = (b >> 4) & 0xF;
    indices[1] = b & 0xF;

    for (atUint32 i = 0; i < 2; i++)
    {
        m_paletteStream->seek(indices[i] * 2, Athena::SeekOrigin::Begin);

        if (m_palFormat == GXPaletteFormat::IA8)
            readPixelIA8(*m_paletteStream, out);
        else if (m_palFormat == GXPaletteFormat::RGB565)
            readPixelRGB565(*m_paletteStream, out);
        else if (m_palFormat == GXPaletteFormat::RGB5A3)
            readPixelRGB5A3(*m_paletteStream, out);
    }
}

void TextureReader::readPixelC8(Athena::io::MemoryReader& in, Athena::io::MemoryWriter& out)
{
    atUint8 index = in.readByte();
    m_paletteStream->seek(index * 2, Athena::SeekOrigin::Begin);

    if (m_palFormat == GXPaletteFormat::IA8)
        readPixelIA8(*m_paletteStream, out);
    else if (m_palFormat == GXPaletteFormat::RGB565)
        readPixelRGB565(*m_paletteStream, out);
    else if (m_palFormat == GXPaletteFormat::RGB5A3)
        readPixelRGB5A3(*m_paletteStream, out);
}

void TextureReader::readPixelRGB565(Athena::io::MemoryReader& in, Athena::io::MemoryWriter& out)
{
    out.writeUint16(in.readUint16());
}

void TextureReader::readPixelRGB5A3(Athena::io::MemoryReader& in, Athena::io::MemoryWriter& out)
{
    atUint16 px = in.readUint16();
    atUint8 r, g, b, a;
    atUint32 rgba;
    if (px & 0x8000) // RGB5
    {
        b = extend5To8(px >> 10);
        g = extend5To8(px >>  5);
        r = extend5To8(px >>  0);
        a = 0xFF;
    }
    else // RGB4A3
    {
        a = extend3To8(px >> 12);
        b = extend4To8(px >>  8);
        g = extend4To8(px >>  4);
        r = extend4To8(px >>  0);
    }

    rgba = (atUint32)((a << 24) | (r << 16) | (g << 8) | b);
    out.writeUint32(rgba);
}

void TextureReader::readPixelRGBA8(Athena::io::MemoryReader& in, Athena::io::MemoryWriter& out)
{
    atUint16 ar = in.readUint16();
    in.seek(0x1E);
    atUint16 gb = in.readUint16();
    in.seek(-0x20);

    atUint32 px = (ar << 16) | gb;
    out.writeUint32(px);
}

void TextureReader::readCMPRSubBlock(Athena::io::MemoryReader& in, Athena::io::MemoryWriter& out)
{
    out.writeUint16(in.readUint16());
    out.writeUint16(in.readUint16());

    atUint32 b = in.readUint32();
    Athena::utility::BigUint32(b);
    b = ((b & 0x03)       << 6) | ((b & 0x0C)       << 2) | ((b & 0x30)       >> 2) | ((b & 0xC0)       >> 6) |
            ((b & 0x0300)     << 6) | ((b & 0x0C00)     << 2) | ((b & 0x3000)     >> 2) | ((b & 0xC000)     >> 6) |
            ((b & 0x030000)   << 6) | ((b & 0x0C0000)   << 2) | ((b & 0x300000)   >> 2) | ((b & 0xC00000)   >> 6) |
            ((b & 0x03000000) << 6) | ((b & 0x0C000000) << 2) | ((b & 0x30000000) >> 2) | ((b & 0xC0000000) >> 6);
    out.writeUint32(b);
}

atUint8 TextureReader::extend3To8(atUint8 in)
{
    in &= 0x7;
    return (in << 5) | (in << 2) | (in >> 1);
}

atUint8 TextureReader::extend4To8(atUint8 in)
{
    in &= 0xF;
    return (in << 4) | in;
}

atUint8 TextureReader::extend5To8(atUint8 in)
{
    in &= 0x1F;
    return (in << 3) | (in >> 2);
}

atUint8 TextureReader::extend6To8(atUint8 in)
{
    in &= 0x3F;
    return (in << 2) | (in >> 4);
}

