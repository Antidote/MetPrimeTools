#ifndef DDS_H
#define DDS_H

#include <Athena/Types.hpp>

struct DDS_PIXELFORMAT
{
    atUint32 dwSize;
    atUint32 dwFlags;
    atUint32 dwFourCC;
    atUint32 dwRGBBitCount;
    atUint32 dwRBitMask;
    atUint32 dwGBitMask;
    atUint32 dwBBitMask;
    atUint32 dwABitMask;
};

struct DDS_HEADER
{
    atUint32 dwSize;
    atUint32 dwFlags;
    atUint32 dwHeight;
    atUint32 dwWidth;
    atUint32 dwPitchOrLinearSize;
    atUint32 dwDepth;
    atUint32 dwMipMapCount;
    atUint32 dwReserved1[11];
    DDS_PIXELFORMAT ddspf;
    atUint32 dwCaps1;
    atUint32 dwCaps2;
    atUint32 dwReserved2[3];
};

const atUint32 DDSF_CAPS           = 0x00000001l;
const atUint32 DDSF_HEIGHT         = 0x00000002l;
const atUint32 DDSF_WIDTH          = 0x00000004l;
const atUint32 DDSF_PIXELFORMAT    = 0x00001000l;
const atUint32 DDSF_MIPMAPCOUNT    = 0x00020000l;
const atUint32 DDSF_LINEARSIZE     = 0x00080000l;

// pixel format flags
const atUint32 DDSF_ALPHAPIXELS     = 0x00000001l;
const atUint32 DDSF_FOURCC          = 0x00000004l;
const atUint32 DDSF_RGB             = 0x00000040l;
const atUint32 DDSF_RGBA            = 0x00000041l;
const atUint32 DDSF_LUMINANCE       = 0x00020000l;
const atUint32 DDSF_LUMINANCE_ALPHA = DDSF_LUMINANCE | DDSF_ALPHAPIXELS;

// dwCaps1 flags
const atUint32 DDSF_TEXTURE         = 0x00001000l;
const atUint32 DDSF_MIPMAP          = 0x00400000l;

#endif // DDS_H

