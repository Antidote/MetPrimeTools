#include <iostream>
#include <TextureReader.hpp>

inline uint16_t RGB565(uint8_t r, uint8_t g, uint8_t b)
{
    return (uint16_t)(r << 11) | (g <<  5) | b;
}

inline uint32_t RGB888(uint8_t r, uint8_t g, uint8_t b)
{
    return (uint32_t)(r << 16) | (g <<  8) | b;
}

inline uint32_t RGBA8888(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    return (uint32_t)(a << 24) | (r << 16) | (g << 8) | b;
}

inline uint16_t RGBAToRGB5A3(uint16_t rgba)
{
    uint8_t a = rgba >>  24;
    uint8_t r = rgba >>  16;
    uint8_t g = rgba >>   8;
    uint8_t b = rgba & 0xFF;

    uint8_t newPixel = 0;

    if (a <= 0xDA) // RGB4A3
    {
        newPixel &= ~(1 << 15);

        r = ((r * 15) / 255) & 0xf;
        g = ((g * 15) / 255) & 0xf;
        b = ((b * 15) / 255) & 0xf;
        a = ((a *  7) / 255) & 0x7;

        newPixel |= (a << 12) | (r << 8) | (g << 4) | b;
    }
    else
    {
        newPixel |= (1 << 15);
        r = ((r * 31) / 255) & 0x1f;
        g = ((g * 31) / 255) & 0x1f;
        b = ((b * 31) / 255) & 0x1f;

        newPixel |= (r << 10) | (g << 5) | b;
    }

    return newPixel;
}

int main(int argc, char* argv[])
{
    std::string progName = argv[0];
    progName = progName.substr(progName.find_last_of("/\\") + 1);
    if (argc < 2)
    {
        printf("Usage: %s <in> [out]\n", progName.c_str());
        return 1;
    }

    std::string inName = argv[1];
    std::string outName;
    if (argc >= 3)
        outName = argv[2];

    if (outName == std::string())
    {
        outName = inName.substr(0, inName.rfind('.'));
        if (outName == std::string())
            outName = inName;
    }

    Texture* tex = nullptr;

    tex = TextureReader::loadTexture(inName);

    if (tex)
    {
        std::cout << "exporting " << outName << std::endl;
        tex->exportDDS(outName + ".dds");
        //tex->exportPNG(outName + ".png");
    }
    else
    {
        std::cout << "failed to decode " << inName << std::endl;
    }

    return 0;
}

