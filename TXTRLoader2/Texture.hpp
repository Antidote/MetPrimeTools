#ifndef TEXTURE_HPP
#define TEXTURE_HPP
#include <cstdint>
#include <string>

enum class GXTextureFormat : uint32_t
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

enum class GXPaletteFormat : uint32_t
{
    IA8,
    RGB565,
    RGB5A3
};

class Texture
{
public:
    enum class Format : uint32_t
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
    uint8_t* bits()       const;
    uint32_t linearSize() const;
    uint32_t dataSize()   const;

    uint16_t width()   const;
    uint16_t height()  const;
    uint32_t mipmaps() const;

    Format   format()  const;
    void exportDDS(const std::string& path);
    void exportPNG(const std::string& path);
private:
    friend class TextureReader;
    uint8_t* m_bits;
    uint32_t m_linearSize;
    uint32_t m_dataSize;
    uint16_t m_width;
    uint16_t m_height;
    uint32_t m_mipmaps;
    Format   m_format;
};

#endif // TEXTURE_HPP
