#ifndef TEXTUREDECODER_HPP
#define TEXTUREDECODER_HPP
#include <cstdint>
#include <Athena/MemoryReader.hpp>
#include <Athena/MemoryWriter.hpp>
#include "Texture.hpp"


class TextureReader final : public Athena::io::MemoryReader
{
    MEMORYREADER_BASE();
public:
    TextureReader(const atUint8* data, atUint64 length);
    TextureReader(const std::string& filename);
    virtual ~TextureReader();

    Texture* read();

private:
    void decode(Athena::io::MemoryReader& in, Athena::io::MemoryWriter& dst);
    Athena::io::MemoryReader* m_paletteStream;
    atUint16             m_width;
    atUint16             m_height;
    atUint32             m_mipmaps;
    GXTextureFormat      m_format;
    GXPaletteFormat      m_palFormat;
    bool                 m_hasPalette;

    void readPixelI4     (Athena::io::MemoryReader& in, Athena::io::MemoryWriter& out);
    void readPixelI8     (Athena::io::MemoryReader& in, Athena::io::MemoryWriter& out);
    void readPixelIA4    (Athena::io::MemoryReader& in, Athena::io::MemoryWriter& out);
    void readPixelIA8    (Athena::io::MemoryReader& in, Athena::io::MemoryWriter& out);
    void readPixelC4     (Athena::io::MemoryReader& in, Athena::io::MemoryWriter& out);
    void readPixelC8     (Athena::io::MemoryReader& in, Athena::io::MemoryWriter& out);
    void readPixelRGB565 (Athena::io::MemoryReader& in, Athena::io::MemoryWriter& out);
    void readPixelRGB5A3 (Athena::io::MemoryReader& in, Athena::io::MemoryWriter& out);
    void readPixelRGBA8  (Athena::io::MemoryReader& in, Athena::io::MemoryWriter& out);
    void readCMPRSubBlock(Athena::io::MemoryReader& in, Athena::io::MemoryWriter& out);

    atUint8 extend3To8(atUint8 in);
    atUint8 extend4To8(atUint8 in);
    atUint8 extend5To8(atUint8 in);
    atUint8 extend6To8(atUint8 in);
};

#endif // TEXTUREDECODER_HPP
