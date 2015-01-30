#ifndef TEXTUREDECODER_HPP
#define TEXTUREDECODER_HPP
#include <cstdint>
#include <Athena/BinaryReader.hpp>
#include <Athena/BinaryWriter.hpp>
#include "Texture.hpp"


class TextureReader final : public Athena::io::BinaryReader
{
    BINARYREADER_BASE();
public:
    TextureReader(const atUint8* data, atUint64 length);
    TextureReader(const std::string& filename);
    virtual ~TextureReader();

    Texture* read();

    static Texture* loadTexture(const std::string& texture);
private:
    void decode(Athena::io::BinaryReader& in, Athena::io::BinaryWriter& dst);
    Texture* createTexture(Athena::io::BinaryWriter& buf);
    Athena::io::BinaryReader* m_paletteStream;
    atUint16             m_width;
    atUint16             m_height;
    atUint32             m_mipmaps;
    GXTextureFormat      m_format;
    GXPaletteFormat      m_palFormat;
    bool                 m_hasPalette;

    void readPixelI4     (Athena::io::BinaryReader& in, Athena::io::BinaryWriter& out);
    void readPixelI8     (Athena::io::BinaryReader& in, Athena::io::BinaryWriter& out);
    void readPixelIA4    (Athena::io::BinaryReader& in, Athena::io::BinaryWriter& out);
    void readPixelIA8    (Athena::io::BinaryReader& in, Athena::io::BinaryWriter& out);
    void readPixelC4     (Athena::io::BinaryReader& in, Athena::io::BinaryWriter& out);
    void readPixelC8     (Athena::io::BinaryReader& in, Athena::io::BinaryWriter& out);
    void readPixelRGB565 (Athena::io::BinaryReader& in, Athena::io::BinaryWriter& out);
    void readPixelRGB5A3 (Athena::io::BinaryReader& in, Athena::io::BinaryWriter& out);
    void readPixelRGBA8  (Athena::io::BinaryReader& in, Athena::io::BinaryWriter& out);
    void readCMPRSubBlock(Athena::io::BinaryReader& in, Athena::io::BinaryWriter& out);

    atUint8 extend3To8(atUint8 in);
    atUint8 extend4To8(atUint8 in);
    atUint8 extend5To8(atUint8 in);
    atUint8 extend6To8(atUint8 in);
};

#endif // TEXTUREDECODER_HPP
