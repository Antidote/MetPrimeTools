#include "CMaterial.hpp"
#include "CMaterialCache.hpp"
#include <QStringList>
#include <QFile>
#include <QDebug>
#include <QColor>
#include "GXCommon.hpp"

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

CMaterial::CMaterial()
    : m_program(nullptr),
      m_materialFlags(0),
      m_unknown1_mp2(0),
      m_unknown2_mp2(0),
      m_konstCount(0)
{
    memset(m_konstColor, 0, sizeof(m_konstColor));
}

CMaterial::~CMaterial()
{

}

atUint32 CMaterial::materialFlags() const
{
    return m_materialFlags;
}

atUint32 CMaterial::vertexAttributes() const
{
    return m_vertexAttributes;
}

std::vector<atUint32> CMaterial::textureIndices() const
{
    return m_textureIndices;
}

bool CMaterial::hasAttribute(atUint32 index)
{
    atUint32 tmp = ((m_vertexAttributes >> (index << 1)) & 3);
    return (tmp == 2 || tmp == 3);
}

QString CMaterial::getSource(QString Filename){
    QFile file(Filename);

    if(!file.open(QFile::ReadOnly | QFile::Text)){
        qDebug() << "could not open file for read";
        return QString();
    }

    QTextStream in(&file);
    QString source = in.readAll();

    file.close();
    return source;
}

static const char* texcoordTransform[] =
{
    "in_Position",
    "vec3((0.5 * transformedNormal.x + 0.5),(0.5 * transformedNormal.y + 0.5), 1)",
    "in_Color0.rgb",
    "in_Color1.rgb",
    "vec3(in_TexCoord0, 1)",
    "vec3(in_TexCoord1, 1)",
    "vec3(in_TexCoord2, 1)",
    "vec3(in_TexCoord3, 1)",
    "vec3(in_TexCoord4, 1)",
    "vec3(in_TexCoord5, 1)",
    "vec3(in_TexCoord6, 1)",
    "vec3(in_TexCoord7, 1)"
};

QOpenGLShader* CMaterial::buildVertex()
{
    QStringList vertSource;

    QString source = getSource(":/shaders/default_vertex.glsl");

    if  (m_version != MetroidPrime3 && m_version != DKCR)
    {
        vertSource << "    // Texgen";
        for (atUint32 i = 0; i < m_texGenFlags.size(); i++)
        {
            atUint32 transformSource = ((m_texGenFlags[i] & 0xF0) >> 4);
            vertSource << QString("    texCoord%1 = %2;").arg(i).arg(QString::fromLatin1(texcoordTransform[transformSource]));
        }
        source = source.replace("//{GXSHADERINFO}", QString("// %1 TEV Stages %2 TexGens").arg(m_tevStages.size()).arg(m_texGenFlags.size()));
        source = source.replace("//{TEXGEN}", vertSource.join("\n"));
    }

    return CMaterialCache::instance()->shaderFromSource(source, QOpenGLShader::Vertex);
}

static const char *tevRegNames[] =
{
    "prev",
    "c0",
    "c1",
    "c2",
    "tex",
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

QOpenGLShader* CMaterial::buildFragment()
{
    QStringList fragmentSource;
    QString source = getSource(":/shaders/default_fragment.glsl");
    if  (m_version != MetroidPrime3 && m_version != DKCR)
    {
        source = source.replace("//{GXSHADERINFO}", QString("// %1 TEV Stages %2 TexGens\n").arg(m_tevStages.size()).arg(m_texGenFlags.size()));

        for (atUint32 i = 0; i < m_tevStages.size(); i++)
        {
            atUint32 texUVid = (m_texTEVIn[i] & 0x0000FF);
            if (texUVid < m_textureIndices.size())
                fragmentSource << QString("    tevCoord = (texCoord%1.z == 0.0) ? texCoord%1.xy : texCoord%1.xy / texCoord%1.z;").arg(texUVid);
            else
                fragmentSource << QString("    tevCoord = (texCoord%1.z == 0.0) ? texCoord%1.xy : texCoord%1.xy / texCoord%1.z;").arg(i);

            atUint32 clrRegASrc   =  m_tevStages[i].ColorInFlags        & 15;
            atUint32 alphaRegASrc =  m_tevStages[i].AlphaInFlags        &  7;
            atUint32 clrRegBSrc   = (m_tevStages[i].ColorInFlags >>  5) & 15;
            atUint32 alphaRegBSrc = (m_tevStages[i].AlphaInFlags >>  5) &  7;
            atUint32 clrRegCSrc   = (m_tevStages[i].ColorInFlags >> 10) & 15;
            atUint32 alphaRegCSrc = (m_tevStages[i].AlphaInFlags >> 10) &  7;
            atUint32 clrRegDSrc   = (m_tevStages[i].ColorInFlags >> 15) & 15;
            atUint32 alphaRegDSrc = (m_tevStages[i].AlphaInFlags >> 15) &  7;
            atUint32 texSampId    = ((m_texTEVIn[i] & 0x000FF00) >> 8);

            if(texSampId < 8)
                fragmentSource << QString("    tex = texture(tex%1, tevCoord);").arg(texSampId);
            else
                fragmentSource << "vec4(1.0, 1.0, 1.0, 1.0);\n";

            fragmentSource << QString("    konstc = vec4(%1, %2);").arg(tevKSelTableC[m_tevStages[i].KonstColorIn]).arg(tevKSelTableA[m_tevStages[i].KonstAlphaIn]);

            if (m_tevStages[i].RasterizedIn != 0xFF)
                fragmentSource << QString("    rast = %1;").arg(rasTable[m_tevStages[i].RasterizedIn]);

            fragmentSource << QString("    tevin_a = vec4(%1, %2);").arg(cRegNames[clrRegASrc]).arg(aRegNames[alphaRegASrc]);
            fragmentSource << QString("    tevin_b = vec4(%1, %2);").arg(cRegNames[clrRegBSrc]).arg(aRegNames[alphaRegBSrc]);
            fragmentSource << QString("    tevin_c = vec4(%1, %2);").arg(cRegNames[clrRegCSrc]).arg(aRegNames[alphaRegCSrc]);
            fragmentSource << QString("    tevin_d = vec4(%1, %2);").arg(cRegNames[clrRegDSrc]).arg(aRegNames[alphaRegDSrc]);
            fragmentSource << "    // ColorCombine";
            QString clrCombine = QString("    %1.rgb =").arg(tevRegNames[(m_tevStages[i].ColorOpFlags & 0x600) >> 9]);
            if ((m_tevStages[i].ColorOpFlags & 0x100) >> 8)
                clrCombine += "clamp(";

            clrCombine += "(tevin_d.rgb + ((1.0 - tevin_c.rgb) * tevin_a.rgb + tevin_c.rgb * tevin_b.rgb))";
            if ((m_tevStages[i].ColorOpFlags & 0x100) >> 8)
                clrCombine += ", vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0))";
            fragmentSource << clrCombine + ";";
            fragmentSource << "    // AlphaCombine";
            QString alphaCombine = QString("    %1.a = ").arg(tevRegNames[(m_tevStages[i].AlphaOpFlags & 0x600) >> 9]);

            if ((m_tevStages[i].AlphaOpFlags & 0x100) >> 8)
                alphaCombine += "clamp(";

            alphaCombine +=  "(tevin_d.a + ((1.0 - tevin_c.a) * tevin_a.a + tevin_c.a * tevin_b.a))";
            if ((m_tevStages[i].AlphaOpFlags & 0x100) >> 8)
                alphaCombine +=  ", 0.0, 1.0)";
            fragmentSource << alphaCombine + ";"
                           << QString("    // End TEV Stage %1").arg(i);
        }

        source = source.replace("//{TEVSTAGES}", fragmentSource.join("\n"));
    }
    else
        source = source.replace("//{TEVSTAGES}", "prev = clamp(normalize(vec4(norm, 1.0)), 0.0, 1.0) - color0;");

    return CMaterialCache::instance()->shaderFromSource(source, QOpenGLShader::Fragment);
}

bool CMaterial::hasPosition()
{
    return hasAttribute(0);
}

bool CMaterial::hasNormal()
{
    return hasAttribute(1);
}

bool CMaterial::hasColor(atUint8 slot)
{
    if (slot >= 2)
        return false;

    return hasAttribute(2 + slot);
}

bool CMaterial::hasUV(atUint8 slot)
{
    if (slot >= 7)
        return false;

    return hasAttribute(4 + slot);
}

bool CMaterial::isTransparent()
{
    return (m_materialFlags & 0x10);
}

void CMaterial::setAmbient(const QColor& ambient)
{
    m_ambient = ambient;
}

bool CMaterial::bind()
{
    if (!m_program)
    {
        QOpenGLShader* vertexShader = buildVertex();
        QOpenGLShader* fragmentShader = buildFragment();
        if (!vertexShader || !fragmentShader)
            return false;

        m_program = new QOpenGLShaderProgram;
        m_program->addShader(vertexShader);
        m_program->addShader(fragmentShader);
        m_program->link();
        m_program->bind();

        if (m_version != MetroidPrime3 && m_version != DKCR)
            m_program->setUniformValue("punchThrough", (m_materialFlags & 0x20));

        m_program->release();
    }

    m_program->bind();
    if (m_version != MetroidPrime3 && m_version != DKCR)
    {
        for (atUint32 i = 0; i < m_konstCount; i++)
        {
            const char* name = QString("konst[%1]").arg(i).toStdString().c_str();
            QColor konstColor = QColor::fromRgba(m_konstColor[i]);

            m_program->setUniformValue(name, konstColor);
        }
        m_program->setUniformValue("ambientLight", m_ambient);
        if (!(m_materialFlags & 0x80))
            glDepthMask(GL_FALSE);
        GXSetBlendMode(m_blendSrcFactor, m_blendDstFactor);
    }

    return true;
}

void CMaterial::release()
{
    if (!m_program)
        return;

    if (!(m_materialFlags & 0x80) && (m_version != MetroidPrime3 && m_version != DKCR))
        glDepthMask(GL_TRUE);

    m_program->release();
}

QOpenGLShaderProgram* CMaterial::program()
{
    return m_program;
}

bool CMaterial::operator==(const CMaterial& right)
{
    return (m_version == m_version && right.m_materialFlags == right.m_materialFlags &&
            m_textureIndices == right.textureIndices() && m_vertexAttributes == right.m_vertexAttributes &&
            m_unknown1_mp2 == right.m_unknown1_mp2 && m_unknown2_mp2 == right.m_unknown2_mp2 &&
            m_unknown1 == right.m_unknown1 && m_konstCount == right.m_konstCount &&
            !memcmp(m_konstColor, right.m_konstColor, sizeof(m_konstColor)) && m_blendDstFactor == right.m_blendDstFactor &&
            m_unknown2 == right.m_unknown2 && m_unkFlags == right.m_unkFlags &&
            m_tevStages == right.m_tevStages && m_texTEVIn == right.m_texTEVIn &&
            m_texGenFlags == right.m_texGenFlags && m_animationData == right.m_animationData);
}
