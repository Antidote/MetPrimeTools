#include "TextureReader.hpp"
#include "pngpp/png.hpp"

static const float sizeOutputMultiplierLut[] =
{ // number of pixels * this = number of bytes
  1.f, 1.f, 2.f, 2.f, 4.f, 4.f, 0.f, 2.f, 4.f, 4.f, 0.5f
};

static const uint32_t bppOutputMultiplierLut[] =
{ // source BPP * this = output BPP
  2, 1, 2, 1, 4, 2, 0, 1, 2, 1, 1
};

static const uint32_t pixelStrideLut[] =
{ // size of one pixel in output data in bytes
  1, 1, 2, 2, 2, 2, 0, 2, 4, 4, 8
};

static const uint32_t blockWidthLut[] =
{
    8, 8, 8, 4, 8, 8, 0, 4, 4, 4, 2
};

static const uint32_t blockHeightLut[] =
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
    : Athena::io::BinaryReader(filename),
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
    uint32_t fmt = base::readUint32();

    if (fmt > (uint32_t)GXTextureFormat::CMPR)
    {
        return nullptr;
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

        uint32_t entryCount = (m_format == GXTextureFormat::C4) ? 16 : 256;
        uint8_t* palData = base::readUBytes(entryCount * 2);

        m_paletteStream = new Athena::io::BinaryReader(palData, entryCount * 2);
    }
    else
        m_hasPalette = false;

    uint32_t imageStart = base::position();
    uint32_t imageDataLength = base::length() - imageStart;

    uint32_t dataBufferSize = imageDataLength * bppOutputMultiplierLut[(uint32_t)m_format];

    if (m_hasPalette && m_palFormat == GXPaletteFormat::RGB5A3)
        dataBufferSize *= 2;

    uint8_t* data = new uint8_t[dataBufferSize];

    Athena::io::BinaryWriter buf{data, dataBufferSize};

    decode(*this, buf);

    return createTexture(buf);
}

Texture*TextureReader::loadTexture(const std::string& texture)
{
    Texture* ret = nullptr;
    try
    {
        TextureReader reader(texture);
        ret = reader.read();
    }
    catch(...)
    {
        delete ret;
        ret = nullptr;
    }

    return ret;
}

void TextureReader::decode(Athena::io::BinaryReader& in, Athena::io::BinaryWriter& buf)
{
    uint32_t mipWidth = m_width, mipHeight = m_height;
    uint32_t mipOffset = 0;

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

    uint32_t blockWidth = blockWidthLut[(uint32_t)m_format];
    uint32_t blockHeight = blockHeightLut[(uint32_t)m_format];

    uint32_t pxStride = pixelStrideLut[(uint32_t)m_format];
    if (m_hasPalette && (m_palFormat == GXPaletteFormat::RGB5A3))
        pxStride = 4;

    for (uint32_t m = 0; m < m_mipmaps; m++)
    {
        for (uint32_t blockY = 0; blockY < mipHeight; blockY += blockHeight)
        {
            for (uint32_t blockX = 0; blockX < mipWidth; blockX += blockWidth)
            {
                for (uint32_t imgY = blockY; imgY < blockY + blockHeight; imgY++)
                {
                    for (uint32_t imgX = blockX; imgX < blockX + blockWidth; imgX++)
                    {
                        uint32_t pos = ((imgY * mipWidth) + imgX) * pxStride;
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

        uint32_t mipSize = mipWidth * mipHeight * sizeOutputMultiplierLut[(uint32_t)m_format];
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

Texture* TextureReader::createTexture(Athena::io::BinaryWriter& buf)
{
    Texture* ret  = new Texture;
    switch (m_format)
    {
    case GXTextureFormat::I4:
    case GXTextureFormat::I8:
        ret->m_format = Texture::Format::Luminance;
        break;
    case GXTextureFormat::IA4:
    case GXTextureFormat::IA8:
        ret->m_format = Texture::Format::LuminanceAlpha;
        break;
    case GXTextureFormat::C4:
    case GXTextureFormat::C8:
        if (m_palFormat == GXPaletteFormat::IA8)
            ret->m_format = Texture::Format::LuminanceAlpha;
        if (m_palFormat == GXPaletteFormat::RGB565)
            ret->m_format = Texture::Format::RGB565;
        if (m_palFormat == GXPaletteFormat::RGB5A3)
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
    ret->m_linearSize = m_width * m_height * sizeOutputMultiplierLut[(uint32_t)m_format];

    return ret;
}

void TextureReader::readPixelI4(Athena::io::BinaryReader& in, Athena::io::BinaryWriter& out)
{
    uint8_t px = in.readByte();
    out.writeByte(extend4To8(px >> 4));
    out.writeByte(extend4To8(px));
}

void TextureReader::readPixelI8(Athena::io::BinaryReader& in, Athena::io::BinaryWriter& out)
{
    out.writeByte(in.readByte());
}

void TextureReader::readPixelIA4(Athena::io::BinaryReader& in, Athena::io::BinaryWriter& out)
{
    uint8_t b = in.readByte();
    uint8_t a = extend4To8(b >> 4);
    uint8_t l = extend4To8(b);
    out.writeUint16((l << 8) | a);
}

void TextureReader::readPixelIA8(Athena::io::BinaryReader& in, Athena::io::BinaryWriter& out)
{
    out.writeUint16(in.readUint16());
}

void TextureReader::readPixelC4(Athena::io::BinaryReader& in, Athena::io::BinaryWriter& out)
{
    uint8_t b = in.readByte();
    uint8_t indices[2];
    indices[0] = (b >> 4) & 0xF;
    indices[1] = b & 0xF;

    for (uint32_t i = 0; i < 2; i++)
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

void TextureReader::readPixelC8(Athena::io::BinaryReader& in, Athena::io::BinaryWriter& out)
{
    uint8_t index = in.readByte();
    m_paletteStream->seek(index * 2, Athena::SeekOrigin::Begin);

    if (m_palFormat == GXPaletteFormat::IA8)
        readPixelIA8(*m_paletteStream, out);
    else if (m_palFormat == GXPaletteFormat::RGB565)
        readPixelRGB565(*m_paletteStream, out);
    else if (m_palFormat == GXPaletteFormat::RGB5A3)
        readPixelRGB5A3(*m_paletteStream, out);
}

void TextureReader::readPixelRGB565(Athena::io::BinaryReader& in, Athena::io::BinaryWriter& out)
{
    out.writeUint16(in.readUint16());
}

void TextureReader::readPixelRGB5A3(Athena::io::BinaryReader& in, Athena::io::BinaryWriter& out)
{
    uint16_t px = in.readUint16();
    uint8_t r, g, b, a;
    uint32_t rgba;
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

    rgba = (uint32_t)((a << 24) | (r << 16) | (g << 8) | b);
    out.writeUint32(rgba);
}

void TextureReader::readPixelRGBA8(Athena::io::BinaryReader& in, Athena::io::BinaryWriter& out)
{
    uint16_t ar = in.readUint16();
    in.seek(0x1E);
    uint16_t gb = in.readUint16();
    in.seek(-0x20);

    uint32_t px = (ar << 16) | gb;
    out.writeUint32(px);
}

void TextureReader::readCMPRSubBlock(Athena::io::BinaryReader& in, Athena::io::BinaryWriter& out)
{
    out.writeUint16(in.readUint16());
    out.writeUint16(in.readUint16());

    for (uint32_t i = 0; i < 4; i++)
    {
        uint8_t b = in.readByte();
        b = ((b & 0x3) << 6) | ((b & 0xC) << 2) | ((b & 0x30) >> 2) | ((b & 0xC0) >> 6);
        out.writeUByte(b);
    }
}

uint8_t TextureReader::extend3To8(uint8_t in)
{
    in &= 0x7;
    return (in << 5) | (in << 2) | (in >> 1);
}

uint8_t TextureReader::extend4To8(uint8_t in)
{
    in &= 0xF;
    return (in << 4) | in;
}

uint8_t TextureReader::extend5To8(uint8_t in)
{
    in &= 0x1F;
    return (in << 3) | (in >> 2);
}

uint8_t TextureReader::extend6To8(uint8_t in)
{
    in &= 0x3F;
    return (in << 2) | (in >> 4);
}

