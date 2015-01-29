#ifndef DDS_H
#define DDS_H

#include <stdint.h>

struct DDS_PIXELFORMAT
{
    uint32_t dwSize;
    uint32_t dwFlags;
    uint32_t dwFourCC;
    uint32_t dwRGBBitCount;
    uint32_t dwRBitMask;
    uint32_t dwGBitMask;
    uint32_t dwBBitMask;
    uint32_t dwABitMask;
};

struct DDS_HEADER
{
    uint32_t dwSize;
    uint32_t dwFlags;
    uint32_t dwHeight;
    uint32_t dwWidth;
    uint32_t dwPitchOrLinearSize;
    uint32_t dwDepth;
    uint32_t dwMipMapCount;
    uint32_t dwReserved1[11];
    DDS_PIXELFORMAT ddspf;
    uint32_t dwCaps1;
    uint32_t dwCaps2;
    uint32_t dwReserved2[3];
};

const uint32_t DDSF_CAPS           = 0x00000001l;
const uint32_t DDSF_HEIGHT         = 0x00000002l;
const uint32_t DDSF_WIDTH          = 0x00000004l;
const uint32_t DDSF_PIXELFORMAT    = 0x00001000l;
const uint32_t DDSF_MIPMAPCOUNT    = 0x00020000l;
const uint32_t DDSF_LINEARSIZE     = 0x00080000l;

// pixel format flags
const uint32_t DDSF_ALPHAPIXELS     = 0x00000001l;
const uint32_t DDSF_FOURCC          = 0x00000004l;
const uint32_t DDSF_RGB             = 0x00000040l;
const uint32_t DDSF_RGBA            = 0x00000041l;
const uint32_t DDSF_LUMINANCE       = 0x00020000l;
const uint32_t DDSF_LUMINANCE_ALPHA = DDSF_LUMINANCE | DDSF_ALPHAPIXELS;

// dwCaps1 flags
const uint32_t DDSF_TEXTURE         = 0x00001000l;
const uint32_t DDSF_MIPMAP          = 0x00400000l;

#endif // DDS_H

