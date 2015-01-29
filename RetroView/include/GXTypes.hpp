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

enum class EBlendMode : int
{
    Zero,
    One,
    SrcClr,
    InvSrcClr,
    SrcAlpha,
    InvSrcAlpha,
    DstAlpha,
    InvDstAlpha,
    DstClr      = EBlendMode::SrcClr,
    InvDstClr   = EBlendMode::InvSrcClr,
};

#endif // GXTYPES_HPP

