#ifndef GXTYPES_HPP
#define GXTYPES_HPP

enum class EPrimitive : int
{
    Quads         = 0x80,
    Triangles     = 0x90,
    TriangleStrip = 0x98,
    TriangleFan   = 0xA0,
    Lines         = 0xA8,
    LineStrip     = 0xB0,
    Points        = 0xB8
};

enum class EComponentType : int
{
    Float,
    HalfFloat
};

enum class ETextureMatrix : int
{
    TextureMatrix0  = 30,
    TextureMatrix1  = 33,
    TextureMatrix2  = 36,
    TextureMatrix3  = 39,
    TextureMatrix4  = 42,
    TextureMatrix5  = 45,
    TextureMatrix6  = 48,
    TextureMatrix7  = 51,
    TextureMatrix8  = 54,
    TextureMatrix9  = 57,
    TextureIdentity = 60,
};

enum class EPostTransformMatrix : int
{
    PostTransformMatrix0  = 64,
    PostTransformMatrix1  = 67,
    PostTransformMatrix2  = 70,
    PostTransformMatrix3  = 73,
    PostTransformMatrix4  = 76,
    PostTransformMatrix5  = 79,
    PostTransformMatrix6  = 82,
    PostTransformMatrix7  = 85,
    PostTransformMatrix8  = 88,
    PostTransformMatrix9  = 91,
    PostTransformMatrix10 = 94,
    PostTransformMatrix11 = 97,
    PostTransformMatrix12 = 100,
    PostTransformMatrix13 = 103,
    PostTransformMatrix14 = 106,
    PostTransformMatrix15 = 109,
    PostTransformMatrix16 = 112,
    PostTransformMatrix17 = 115,
    PostTransformMatrix18 = 118,
    PostTransformMatrix19 = 121,
    PostTransformIdentity = 125
};

enum class EBlendMode : int
{
    Zero = 0,
    One = 1,
    SrcClr = 2,
    InvSrcClr = 3,
    SrcAlpha = 4,
    InvSrcAlpha = 5,
    DstAlpha = 6,
    InvDstAlpha = 7,
    DstClr = 8,
    InvDstClr = 9,
};

#endif // GXTYPES_HPP

