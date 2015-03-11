#include "core/STEVStage.hpp"
#include "core/GXCommon.hpp"
#include <QStringList>

static const char *tevKSelTableC[] =
{
    "1, 1 ,1",  // 1   = 0x00
    "0.9, 0.9, 0.9",  // 7_8 = 0x01
    "0.7, 0.7, 0.7",  // 3_4 = 0x02
    "0.6, 0.6, 0.6",  // 5_8 = 0x03
    "0.5, 0.5, 0.5",  // 1_2 = 0x04
    "0.4, 0.4, 0.4",     // 3_8 = 0x05
    "0.25, 0.25, 0.25",     // 1_4 = 0x06
    "0.13, 0.13, 0.13",     // 1_8 = 0x07
    "0, 0, 0",        // INVALID = 0x08
    "0, 0, 0",        // INVALID = 0x09
    "0, 0, 0",        // INVALID = 0x0a
    "0, 0, 0",        // INVALID = 0x0b
    KCOLORS"[0].rgb", // K0 = 0x0C
    KCOLORS"[1].rgb", // K1 = 0x0D
    KCOLORS"[2].rgb", // K2 = 0x0E
    KCOLORS"[3].rgb", // K3 = 0x0F
    KCOLORS"[0].rrr", // K0_R = 0x10
    KCOLORS"[1].rrr", // K1_R = 0x11
    KCOLORS"[2].rrr", // K2_R = 0x12
    KCOLORS"[3].rrr", // K3_R = 0x13
    KCOLORS"[0].ggg", // K0_G = 0x14
    KCOLORS"[1].ggg", // K1_G = 0x15
    KCOLORS"[2].ggg", // K2_G = 0x16
    KCOLORS"[3].ggg", // K3_G = 0x17
    KCOLORS"[0].bbb", // K0_B = 0x18
    KCOLORS"[1].bbb", // K1_B = 0x19
    KCOLORS"[2].bbb", // K2_B = 0x1A
    KCOLORS"[3].bbb", // K3_B = 0x1B
    KCOLORS"[0].aaa", // K0_A = 0x1C
    KCOLORS"[1].aaa", // K1_A = 0x1D
    KCOLORS"[2].aaa", // K2_A = 0x1E
    KCOLORS"[3].aaa", // K3_A = 0x1F
};

static const char *tevKSelTableA[] =
{
    "1",  // 1   = 0x00
    "0.9",  // 7_8 = 0x01
    "0.7",  // 3_4 = 0x02
    "0.6",  // 5_8 = 0x03
    "0.5",  // 1_2 = 0x04
    "0.4",   // 3_8 = 0x05
    "0.25",   // 1_4 = 0x06
    "0.13",   // 1_8 = 0x07
    "0",    // INVALID = 0x08
    "0",    // INVALID = 0x09
    "0",    // INVALID = 0x0a
    "0",    // INVALID = 0x0b
    "0",    // INVALID = 0x0c
    "0",    // INVALID = 0x0d
    "0",    // INVALID = 0x0e
    "0",    // INVALID = 0x0f
    KCOLORS"[0].r", // K0_R = 0x10
    KCOLORS"[1].r", // K1_R = 0x11
    KCOLORS"[2].r", // K2_R = 0x12
    KCOLORS"[3].r", // K3_R = 0x13
    KCOLORS"[0].g", // K0_G = 0x14
    KCOLORS"[1].g", // K1_G = 0x15
    KCOLORS"[2].g", // K2_G = 0x16
    KCOLORS"[3].g", // K3_G = 0x17
    KCOLORS"[0].b", // K0_B = 0x18
    KCOLORS"[1].b", // K1_B = 0x19
    KCOLORS"[2].b", // K2_B = 0x1A
    KCOLORS"[3].b", // K3_B = 0x1B
    KCOLORS"[0].a", // K0_A = 0x1C
    KCOLORS"[1].a", // K1_A = 0x1D
    KCOLORS"[2].a", // K2_A = 0x1E
    KCOLORS"[3].a", // K3_A = 0x1F
};

static const char *cRegNames[] =
{
    "prev.rgb",
    "prev.aaa",
    "c0.rgb",
    "c0.aaa",
    "c1.rgb",
    "c1.aaa",
    "c2.rgb",
    "c2.aaa",
    "tex.rgb",
    "tex.aaa",
    "rast.rgb",
    "rast.aaa",
    "vec3(1.0,1.0,1.0)",
    "vec3(0.5,0.5,0.5)",
    "konstc.rgb",
    "vec3(0.0,0.0,0.0)",
};

static const char *aRegNames[] =
{
    "prev.a",
    "c0.a",
    "c1.a",
    "c2.a",
    "tex.a",
    "tex.a",
    "konstc.a",
    "0",
};

static const char *rasTable[] =
{
    "color0",
    "color1",
    "vec4(0, 0, 0, color0.a)", //2
    "vec4(0, 0, 0, color1.a)", //3
    "color0", //4
    "color1", // bump alpha (0..248)
    "vec4(0, 0, 0, 0)"
};

static const char *tevRegNames[] =
{
    "prev",
    "c0",
    "c1",
    "c2",
    "tex",
};

STEVStage::STEVStage()
    : ColorInFlags(0),
      AlphaInFlags(0),
      ColorOpFlags(0),
      AlphaOpFlags(0),
      Padding(0),
      KonstAlphaIn(0),
      KonstColorIn(0),
      RasterizedIn(0xFF)
{
}

const char* tevBiasOp[] =
{
    "",
    " + 0.5",
    " - 0.5",
    ""
};
const char* tevScaleOp[] =
{
    "",
    " * 2",
    " / 4",
    " / 2"
};

QStringList STEVStage::fragmentSource(atUint32 texTEVIn, atUint32 textureCount, atUint32 idx)
{
    QStringList fragmentSource;

    atUint32 texUVid = (texTEVIn & 0x0000FF);
    if (texUVid < textureCount)
        fragmentSource << QString("    tevCoord = texCoord%1.xy;").arg(texUVid);
    else
        fragmentSource << QString("    tevCoord = texCoord%1.xy;").arg(idx);

    atUint32 clrRegASrc   =  ColorInFlags        & 15;
    atUint32 alphaRegASrc =  AlphaInFlags        &  7;
    atUint32 clrRegBSrc   = (ColorInFlags >>  5) & 15;
    atUint32 alphaRegBSrc = (AlphaInFlags >>  5) &  7;
    atUint32 clrRegCSrc   = (ColorInFlags >> 10) & 15;
    atUint32 alphaRegCSrc = (AlphaInFlags >> 10) &  7;
    atUint32 clrRegDSrc   = (ColorInFlags >> 15) & 15;
    atUint32 alphaRegDSrc = (AlphaInFlags >> 15) &  7;
    atUint32 texSampId    = ((texTEVIn & 0x000FF00) >> 8);

    if(texSampId < 8)
        fragmentSource << QString("    tex = texture(tex%1, tevCoord);").arg(texSampId);
    else
        fragmentSource << "vec4(1.0, 1.0, 1.0, 1.0);\n";

    fragmentSource << QString("    konstc = vec4(%1, %2);").arg(tevKSelTableC[KonstColorIn]).arg(tevKSelTableA[KonstAlphaIn]);

    if (RasterizedIn != 0xFF && RasterizedIn < 7)
        fragmentSource << QString("    rast = %1;").arg(rasTable[RasterizedIn]);

    fragmentSource << QString("    tevin_a = vec4(%1, %2);").arg(cRegNames[clrRegASrc]).arg(aRegNames[alphaRegASrc]);
    fragmentSource << QString("    tevin_b = vec4(%1, %2);").arg(cRegNames[clrRegBSrc]).arg(aRegNames[alphaRegBSrc]);
    fragmentSource << QString("    tevin_c = vec4(%1, %2);").arg(cRegNames[clrRegCSrc]).arg(aRegNames[alphaRegCSrc]);
    fragmentSource << QString("    tevin_d = vec4(%1, %2);").arg(cRegNames[clrRegDSrc]).arg(aRegNames[alphaRegDSrc]);
    fragmentSource << "    // ColorCombine";
    QString clrCombine = QString("    %1.rgb =").arg(tevRegNames[(ColorOpFlags & 0x600) >> 9]);
    if ((ColorOpFlags >> 8) & 1)
        clrCombine += "clamp(";

    atUint8 combinerOp = (ColorOpFlags & 0xF);
    atUint8 biasOp = (ColorOpFlags >> 3) & 3;
    atUint8 scaleOp = (ColorOpFlags >> 6) & 3;
    clrCombine += QString("((tevin_d.rgb %1 ((1.0 - tevin_c.rgb) * tevin_a.rgb + tevin_c.rgb * tevin_b.rgb) %2) %3)").arg((combinerOp == 1 ? "-" : "+")).arg(tevBiasOp[biasOp]).arg(tevScaleOp[scaleOp]);
    if ((ColorOpFlags >> 8) & 1)
        clrCombine += ", vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0))";
    fragmentSource << clrCombine + ";";
    fragmentSource << "    // AlphaCombine";
    QString alphaCombine = QString("    %1.a = ").arg(tevRegNames[(AlphaOpFlags & 0x600) >> 9]);

    if ((AlphaOpFlags >> 8) & 1)
        alphaCombine += "clamp(";

    combinerOp = (AlphaOpFlags & 0xF);
    biasOp = (AlphaOpFlags >> 3) & 3;
    scaleOp = (AlphaOpFlags >> 6) & 3;

    alphaCombine +=  QString("((tevin_d.a %1 ((1.0 - tevin_c.a) * tevin_a.a + tevin_c.a * tevin_b.a) %2) %3)").arg((combinerOp == 1 ? "-" : "+")).arg(tevBiasOp[biasOp]).arg(tevScaleOp[scaleOp]);
    if ((AlphaOpFlags >> 8) & 1)
        alphaCombine +=  ", 0.0, 1.0)";
    fragmentSource << alphaCombine + ";"
                   << QString("    // End TEV Stage %1").arg(idx);

    return fragmentSource;
}

bool operator==(const STEVStage& left, const STEVStage& right)
{
    return !memcmp(&left, &right, sizeof(STEVStage));
}
