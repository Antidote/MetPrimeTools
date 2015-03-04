#include <GL/glew.h>
#include "ui/CGLViewer.hpp"
#include "core/CMaterial.hpp"
#include "core/CMaterialCache.hpp"
#include "core/GXCommon.hpp"

#include <QStringList>
#include <QFile>
#include <QDebug>
#include <QColor>
#include <chrono>
#include <ctime>
#include <QSettings>

CMaterial::CMaterial()
    : m_program(nullptr),
      m_version(MetroidPrime1),
      m_materialFlags(0),
      m_unknown1_mp2(0),
      m_unknown2_mp2(0),
      m_konstCount(0),
      m_isBound(false)
{
    memset(m_konstColor, 0, sizeof(m_konstColor));
}

CMaterial::~CMaterial()
{

}

void CMaterial::setModelMatrix(const glm::mat4& modelMatrix)
{
    m_modelMatrix = modelMatrix;
    assignModelMatrix();
}

void CMaterial::setViewMatrix(const glm::mat4& viewMatrix)
{
    m_viewMatrix = viewMatrix;
    assignViewMatrix();
}

void CMaterial::setProjectionMatrix(const glm::mat4& projectionMatrix)
{
    m_projectionMatrix = projectionMatrix;
    assignProjectionMatrix();
}

atUint32 CMaterial::materialFlags() const
{
    return m_materialFlags;
}

void CMaterial::setMaterialFlags(atUint32 matFlags)
{
    m_materialFlags = matFlags;
}

void CMaterial::setVertexAttributes(atUint32 vertFlags)
{
    m_vertexAttributes = vertFlags;
}

atUint32 CMaterial::vertexAttributes() const
{
    return m_vertexAttributes;
}

void CMaterial::addTevStage(const STEVStage& tevStage, atUint32 texInFlags)
{
    m_tevStages.push_back(tevStage);
    m_texTEVIn.push_back(texInFlags);
}

std::vector<atUint32> CMaterial::textureIndices() const
{
    return m_textureIndices;
}

void CMaterial::setBlendMode(EBlendMode dst, EBlendMode src)
{
    m_blendDstFactor = dst;
    m_blendSrcFactor = src;
}

void CMaterial::setKonstColor(atUint32 id, const QColor& color)
{
    atUint32 realIndex = id % 4;
    m_konstColor[realIndex] = color;

    if (m_konstCount < (realIndex + 1))
        m_konstCount = realIndex + 1;

    assignKonstColor(realIndex);
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

QOpenGLShader* CMaterial::buildFragment()
{
    QStringList fragmentSource;
    QString source = getSource(":/shaders/default_fragment.glsl");
    if  (m_version != MetroidPrime3 && m_version != DKCR)
    {
        source = source.replace("//{GXSHADERINFO}", QString("// %1 TEV Stages %2 TexGens\n").arg(m_tevStages.size()).arg(m_texGenFlags.size()));

        for (atUint32 i = 0; i < m_tevStages.size(); i++)
            fragmentSource << m_tevStages[i].fragmentSource(m_texTEVIn[i], m_textureIndices.size(), i);

        source = source.replace("//{TEVSTAGES}", fragmentSource.join("\n"));
    }
    else
        source = source.replace("//{TEVSTAGES}", "prev = texture(tex0, texCoord0.xy);//color0;");

    return CMaterialCache::instance()->shaderFromSource(source, QOpenGLShader::Fragment);
}

void CMaterial::assignKonstColor(atUint32 id)
{
    if (!m_isBound)
        return;

    id = id % 4;
    const char* name = QString("konst[%1]").arg(id).toStdString().c_str();
    m_program->setUniformValue(name, m_konstColor[id]);
}

void CMaterial::assignModelMatrix()
{
    if (!m_isBound)
        return;

    atUint32 modelLoc = m_program->uniformLocation("model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &m_modelMatrix[0][0]);
}

void CMaterial::assignViewMatrix()
{
    if (!m_isBound)
        return;

    atUint32 viewLoc = m_program->uniformLocation("view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
}

void CMaterial::assignProjectionMatrix()
{
    if (!m_isBound)
        return;
    atUint32 projLoc = m_program->uniformLocation("projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &m_projectionMatrix[0][0]);
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
    m_isBound = true;

    assignModelMatrix();
    assignViewMatrix();
    assignProjectionMatrix();

    if (m_version != MetroidPrime3 && m_version != DKCR)
    {
        updateAnimations();

        for (atUint32 i = 0; i < m_konstCount; i++)
            assignKonstColor(i);

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
    m_isBound = false;
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



