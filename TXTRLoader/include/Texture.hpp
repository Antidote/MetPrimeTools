#ifndef TEXTURE_HPP
#define TEXTURE_HPP
#include <Athena/Types.hpp>
#include <string>

enum class GXTextureFormat : atUint32
{
    I4,
    I8,
    IA4,
    IA8,
    C4,
    C8,
    RGB565=7,
    RGB5A3,
    RGBA8,
    CMPR
};

enum class GXPaletteFormat : atUint32
{
    IA8,
    RGB565,
    RGB5A3
};

class Texture
{
public:
    enum class Format : atUint32
    {
        Luminance,
        LuminanceAlpha,
        RGB565,
        RGBA8,
        DXT1
    };

    Texture();
    ~Texture();

    bool     isNull()     const;
    atUint8* bits()       const;
    atUint32 linearSize() const;
    atUint32 dataSize()   const;

    atUint16 width()   const;
    atUint16 height()  const;
    atUint32 mipmaps() const;

    Format   format()  const;
    void exportDDS(const std::string& path);
    void exportPNG(const std::string& path);
private:
    friend class TextureReader;
    atUint8* m_bits;
    atUint32 m_linearSize;
    atUint32 m_dataSize;
    atUint16 m_width;
    atUint16 m_height;
    atUint32 m_mipmaps;
    Format   m_format;
};

#endif // TEXTURE_HPP
