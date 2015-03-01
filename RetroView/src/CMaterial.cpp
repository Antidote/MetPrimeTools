#include <GL/glew.h>
#include "CGLViewer.hpp"
#include "CMaterial.hpp"
#include "CMaterialCache.hpp"
#include <QStringList>
#include <QFile>
#include <QDebug>
#include <QColor>
#include <chrono>
#include <ctime>
#include <QSettings>
#include "GXCommon.hpp"

typedef std::chrono::high_resolution_clock hrClock_t;
typedef std::chrono::duration<float> fsec;

static auto start = hrClock_t::now();

float secondsMod900()
{
    auto now = hrClock_t::now();

    float result = fsec(now - start).count();
    return fmod(result, 900.f);
}

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

void CMaterial::setModelMatrix(const glm::mat4& modelMatrix)
{
    m_modelMatrix = modelMatrix;
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
    "in_Normal",
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
            atUint32 texcoordSource = ((m_texGenFlags[i] & 0xF0) >> 4);
            atUint32 texMtxSource = textureMatrixIdx(((m_texGenFlags[i] >> 9) & 0x1F) + 30);
            atUint32 postMtxSource = postTransformMatrixIdx(((m_texGenFlags[i] >> 15) & 0x1F) + 64);
            QString source = QString("texCoord%1").arg(i);
            vertSource << QString("    %1 = vec3(vec4(%2, 1.0f) * texMtx[%3]);")
                          .arg(source)
                          .arg(QString::fromLatin1(texcoordTransform[texcoordSource]))
                          .arg(texMtxSource);

            if ((m_texGenFlags[i] >> 14) & 1)
                vertSource << QString("%1 = normalize(%1);").arg(source);

            vertSource << QString("%1 = vec3(vec4(%1, 1.0) * postMtx[%2]);").arg(source).arg(postMtxSource);

        }
        source = source.replace("//{GXSHADERINFO}", QString("// %1 TEV Stages %2 TexGens").arg(m_tevStages.size()).arg(m_texGenFlags.size()));
        source = source.replace("//{TEXGEN}", vertSource.join("\n"));
    }
    else
    {
        source = source.replace("//{TEXGEN}", "texCoord0 = vec3(in_TexCoord0, 1);");
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
        source = source.replace("//{TEVSTAGES}", "prev = texture(tex0, texCoord0.xy);//color0;");

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

        for (atUint32 i = 0; i < 11; i++)
        {
            std::string texMtxName = QString("texMtx[%1]").arg(i).toStdString();
            atUint32 texMtxLoc = m_program->uniformLocation(texMtxName.c_str());
            glUniformMatrix4fv(texMtxLoc, 1, GL_FALSE, &g_texMtx[i][0][0]);
        }

        for (atUint32 i = 0; i < 21; i++)
        {
            std::string postMtxName = QString("postMtx[%1]").arg(i).toStdString();
            atUint32 postMtxLoc = m_program->uniformLocation(postMtxName.c_str());
            glUniformMatrix4fv(postMtxLoc, 1, GL_FALSE, &g_postMtx[i][0][0]);
        }

        m_program->release();
    }

    for (atUint32 i = 0; i < 11; i++)
        g_texMtx[i] = glm::mat4(1);
    for (atUint32 i = 0; i < 21; i++)
        g_postMtx[i] = glm::mat4(1);

    m_program->bind();
    if (m_version != MetroidPrime3 && m_version != DKCR)
    {
        updateAnimations();

        for (atUint32 i = 0; i < m_konstCount; i++)
        {
            const char* name = QString("konst[%1]").arg(i).toStdString().c_str();
            QColor konstColor = QColor::fromRgba(m_konstColor[i]);
            int b = konstColor.red();
            konstColor.setRed(konstColor.blue());
            konstColor.setBlue(b);

            m_program->setUniformValue(name, konstColor);
        }
        m_program->setUniformValue("ambientLight", m_ambient);
        if (!(m_materialFlags & 0x80))
            glDepthMask(GL_FALSE);


        GXSetBlendMode(m_blendSrcFactor, m_blendDstFactor);
    }
    else
        glBlendFunc(GL_ONE, GL_ZERO);

    return true;
}

void CMaterial::release()
{
    if (!m_program)
        return;

    if (!(m_materialFlags & 0x80) && (m_version != MetroidPrime3 && m_version != DKCR))
        glDepthMask(GL_TRUE);

    glBlendFunc(GL_ONE, GL_ZERO);

    m_program->release();
}

QOpenGLShaderProgram* CMaterial::program()
{
    return m_program;
}

bool CMaterial::operator==(const CMaterial& right)
{
    return (m_version == right.m_version && right.m_materialFlags == right.m_materialFlags &&
            m_textureIndices == right.textureIndices() && m_vertexAttributes == right.m_vertexAttributes &&
            m_unknown1_mp2 == right.m_unknown1_mp2 && m_unknown2_mp2 == right.m_unknown2_mp2 &&
            m_unknown1 == right.m_unknown1 && m_konstCount == right.m_konstCount &&
            !memcmp(m_konstColor, right.m_konstColor, sizeof(m_konstColor)) && m_blendDstFactor == right.m_blendDstFactor &&
            m_unknown2 == right.m_unknown2 && m_unkFlags == right.m_unkFlags &&
            m_tevStages == right.m_tevStages && m_texTEVIn == right.m_texTEVIn &&
            m_texGenFlags == right.m_texGenFlags && m_animations == right.m_animations);
}

void CMaterial::updateAnimations()
{
    float s = secondsMod900();
    for (atUint32 i = 0; i < m_animations.size(); i++)
    {
        glm::mat4& texMtx = g_texMtx[i];
        glm::mat4& postMtx = g_postMtx[i];

        switch (m_animations[i].mode)
        {
            case 0:
            case 1:
            case 7:
            {
                if ((m_animations[i].mode == 0 && !QSettings().value("mode0").toBool()) ||
                        (m_animations[i].mode == 1 && !QSettings().value("mode1").toBool()) ||
                        (m_animations[i].mode == 7 && !QSettings().value("mode7").toBool()))
                    break;

                texMtx = glm::inverse(CGLViewer::instance()->viewMatrix()) * m_modelMatrix;
                if (m_animations[i].mode != 7)
                {
                    postMtx = glm::mat4(glm::mat3x4(
                                            0.5, 0.0, 0.0, 0.5,
                                            0.0, 0.0, 0.5, 0.5,
                                            0.0, 0.0, 0.0, 1.0));
                }
                else
                {
                    glm::mat4 view = CGLViewer::instance()->viewMatrix();
                    float xy = (view[0][3] + view[1][3]) * 0.025f * m_animations[i].parms[1];
                    xy = (xy - (int)xy);
                    float z = (view[2][3]) * 0.05f * m_animations[i].parms[1];
                    z = (z - (int)z);

                    float halfA = m_animations[i].parms[0] * 0.5f;

                    postMtx = glm::mat4(glm::mat2x4(halfA, 0.0, 0.0, xy,
                                                    0.0, 0.0, halfA, z));
                }

                if (m_animations[i].mode == 0 || m_animations[i].mode == 7)
                    texMtx[0][3] = texMtx[1][3] = texMtx[2][3] = 0;
            }
                break;
            case 2:
            {
                if (!QSettings().value("mode2").toBool())
                    break;
                glm::vec2 texCoordBias;
                texCoordBias.s = (s * m_animations[i].parms[2]) + m_animations[i].parms[0];
                texCoordBias.t = (s * m_animations[i].parms[3]) + m_animations[i].parms[1];

                texMtx = glm::mat4(1);
                texMtx[0][3] = texCoordBias.s;
                texMtx[1][3] = texCoordBias.t;
            }
                break;
            case 3:
            {
                if (!QSettings().value("mode3").toBool())
                    break;

                float angle = (s * m_animations[i].parms[1]) + m_animations[i].parms[0];
                float acos = cos(angle);
                float asin = sin(angle);
                float translateX = (1.0 - (acos - asin)) * 0.5;
                float translateY = (1.0 - (asin + acos)) * 0.5;

                texMtx = glm::mat4(glm::mat3x4(acos, -asin, 0.0, translateX,
                                               asin, acos, 0.0, translateY,
                                               0.0, 0.0, 1.0, 0.0));
            }
                break;
            case 4:
            case 5:
            {
                if (!QSettings().value("mode4And5").toBool())
                    break;

                float scale = m_animations[i].parms[0];
                float numFrames = m_animations[i].parms[1];
                float step = m_animations[i].parms[2];
                float offset  = m_animations[i].parms[3];

                texMtx = glm::mat4(1);
                float value = scale * step * (offset + s);

                float uvOffset = (float)(short)(float)(numFrames * fmod(value, 1.0f)) * step;
                if (m_animations[i].mode == 4)
                    texMtx[0][3] = uvOffset;
                else
                    texMtx[1][3] = uvOffset;
            }
                break;
            case 6:
            {
                if (!QSettings().value("mode6").toBool())
                    break;

                texMtx = m_modelMatrix;
                texMtx[0][3] = texMtx[1][3] = texMtx[2][3] = 0;
                postMtx = glm::mat4(glm::mat3x4(
                                        0.5, 0.0, 0.0, m_modelMatrix[0][3] * 0.50000001,
                        0.0, 0.0, 0.5, m_modelMatrix[1][3] * 0.50000001,
                        0.0, 0.0, 0.0, 1.0));
            }
                break;
        }
        std::string texMtxName = QString("texMtx[%1]").arg(i).toStdString();
        atUint32 texMtxLoc = m_program->uniformLocation(texMtxName.c_str());
        glUniformMatrix4fv(texMtxLoc, 1, GL_FALSE, &g_texMtx[i][0][0]);
        std::string postMtxName = QString("postMtx[%1]").arg(i).toStdString();
        atUint32 postMtxLoc = m_program->uniformLocation(postMtxName.c_str());
        glUniformMatrix4fv(postMtxLoc, 1, GL_FALSE, &g_postMtx[i][0][0]);
    }
}

atUint32 CMaterial::textureMatrixIdx(atUint32 id)
{
    switch((ETextureMatrix)id)
    {
        case ETextureMatrix::TextureMatrix0:  return 0;
        case ETextureMatrix::TextureMatrix1:  return 1;
        case ETextureMatrix::TextureMatrix2:  return 2;
        case ETextureMatrix::TextureMatrix3:  return 3;
        case ETextureMatrix::TextureMatrix4:  return 4;
        case ETextureMatrix::TextureMatrix5:  return 5;
        case ETextureMatrix::TextureMatrix6:  return 6;
        case ETextureMatrix::TextureMatrix7:  return 7;
        case ETextureMatrix::TextureMatrix8:  return 8;
        case ETextureMatrix::TextureMatrix9:  return 9;
        case ETextureMatrix::TextureIdentity:
        default: return 10;
    }
}

atUint32 CMaterial::postTransformMatrixIdx(atUint32 id)
{
    switch((EPostTransformMatrix)id)
    {
        case EPostTransformMatrix::PostTransformMatrix0:  return 0;
        case EPostTransformMatrix::PostTransformMatrix1:  return 1;
        case EPostTransformMatrix::PostTransformMatrix2:  return 2;
        case EPostTransformMatrix::PostTransformMatrix3:  return 3;
        case EPostTransformMatrix::PostTransformMatrix4:  return 4;
        case EPostTransformMatrix::PostTransformMatrix5:  return 5;
        case EPostTransformMatrix::PostTransformMatrix6:  return 6;
        case EPostTransformMatrix::PostTransformMatrix7:  return 7;
        case EPostTransformMatrix::PostTransformMatrix8:  return 8;
        case EPostTransformMatrix::PostTransformMatrix9:  return 9;
        case EPostTransformMatrix::PostTransformMatrix10: return 10;
        case EPostTransformMatrix::PostTransformMatrix11: return 11;
        case EPostTransformMatrix::PostTransformMatrix12: return 12;
        case EPostTransformMatrix::PostTransformMatrix13: return 13;
        case EPostTransformMatrix::PostTransformMatrix14: return 14;
        case EPostTransformMatrix::PostTransformMatrix15: return 15;
        case EPostTransformMatrix::PostTransformMatrix16: return 16;
        case EPostTransformMatrix::PostTransformMatrix17: return 17;
        case EPostTransformMatrix::PostTransformMatrix18: return 18;
        case EPostTransformMatrix::PostTransformMatrix19: return 19;
        case EPostTransformMatrix::PostTransformIdentity:
        default: return 20;
    }
}


bool operator==(const SAnimation& left, const SAnimation& right)
{
    return !memcmp(&left, &right, sizeof(SAnimation));
}
