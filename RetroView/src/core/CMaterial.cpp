#include "ui/CGLViewer.hpp"
#include "core/CMaterial.hpp"
#include "core/CMaterialCache.hpp"
#include "core/GXCommon.hpp"
#include "core/CResourceManager.hpp"
#include "generic/CTexture.hpp"

#include <QStringList>
#include <QFile>
#include <QDebug>
#include <QColor>
#include <chrono>
#include <ctime>
#include <QSettings>

static const GLint TEX_UNIS[] =
{
    0,1,2,3,4,5,6,7
};

CMaterial::CMaterial()
    : m_program(nullptr),
      m_version(MetroidPrime1),
      m_materialFlags(0),
      m_unknown1_mp2(0),
      m_unknown2_mp2(0),
      m_konstCount(0),
      m_isBound(false),
      m_texturesEnabled(true)
{
    memset(m_konstColor, 0, sizeof(m_konstColor));
    for (atUint32 i= 0; i < 12; i++)
        m_passes[i] = nullptr;
}

CMaterial::~CMaterial()
{
    m_materialSections.clear();
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

void CMaterial::setTexturesEnabled(const bool& enabled)
{
    m_texturesEnabled = enabled;
    assignTexturesEnabled();
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

bool CMaterial::hasAttribute(atUint32 index) const
{
    atUint32 tmp = ((m_vertexAttributes >> (index << 1)) & 3);
    return (tmp == 2 || tmp == 3);
}

QString CMaterial::getSource(QString Filename)
{
    QFile file(Filename);

    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
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

static const char* texcoordTransformMP3[] =
{
    "in_Position",
    "in_Normal",
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
        atUint32 pass = 0;
        for (atUint32 i = 0; i < 12; i++)
        {
            if (m_passes[i] != nullptr)
            {
                if (m_passes[i]->hasAnim)
                {
                    if (m_passes[i]->subCommand == EMaterialCommand::RFLD)
                    {
                        std::cout << "Anim mode: " << std::dec << m_passes[i]->anim.mode << std::endl;
                        std::cout << m_passes[i]->textureId.toString() << std::endl;
                        std::cout << std::dec << texcoordTransformMP3[m_passes[i]->animUvSource] << std::endl;
                    }

                    if (m_passes[i]->anim.mode < 2 || m_passes[i]->anim.mode > 5)
                        vertSource << QString("texCoord%1 = vec3(vec4(%2, 1.0) * texMtx[%1]);").arg(pass)
                                      .arg(texcoordTransformMP3[m_passes[i]->animUvSource]);
                    else
                        vertSource << QString("texCoord%1 = vec3(vec4(vec3(in_TexCoord%2, 1.0), 1.0) * texMtx[%1]);").arg(pass)
                                      .arg(m_passes[i]->uvSource);

                    if (m_passes[i]->anim.mode < 2 || m_passes[i]->anim.mode > 5)
                        vertSource << QString("texCoord%1 = normalize(texCoord%1);").arg(pass);

                    vertSource << QString("texCoord%1 = vec3(vec4(texCoord%1, 1.0) * postMtx[%1]);").arg(pass);
                }
                else
                    vertSource << QString("texCoord%1 = vec3(in_TexCoord%2, 1);").arg(pass).arg(m_passes[i]->uvSource);

                pass++;
            }
        }
        source = source.replace("//{TEXGEN}", vertSource.join("\n"));
    }

    m_vertexSource = source;
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
            fragmentSource << m_tevStages[i].fragmentSource(m_texTEVIn[i], m_textures.size(), i);

        source = source.replace("//{TEVSTAGES}", fragmentSource.join("\n"));
    }
    else
    {
        atUint32 passIdx = 0;
        for (atUint32 i = 0; i < 12; i++)
        {
            SPASSCommand* pass = m_passes[i];
            if (pass)
            {
                fragmentSource << pass->fragmentSource(passIdx);
                passIdx++;
            }
        }

        for (CMaterialSection* section : m_materialSections)
        {
            SINTCommand* intC = dynamic_cast<SINTCommand*>(section->m_commandImpl);
            if (intC && intC->subCommand == EMaterialCommand::OPAC)
                fragmentSource << QString("        prev = vec4(prev.rgb, %1);").arg((atUint8)intC->val * (1.f/255.f));
        }

        source = source.replace("//{TEVSTAGES}", fragmentSource.join("\n"));
    }

    m_fragmentSource = source;
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

void CMaterial::assignTexturesEnabled()
{
    if (!m_isBound)
        return;
    m_program->setUniformValue("texturesEnabled", m_texturesEnabled);
}

bool CMaterial::hasPosition() const
{
    return hasAttribute(0);
}

bool CMaterial::hasNormal() const
{
    return hasAttribute(1);
}

bool CMaterial::hasColor(atUint8 slot) const
{
    if (slot >= 2)
        return false;

    return hasAttribute(2 + slot);
}

bool CMaterial::hasUV(atUint8 slot) const
{
    if (slot >= 7)
        return false;

    return hasAttribute(4 + slot);
}

bool CMaterial::isTransparent() const
{
    if (m_version != MetroidPrime3 && m_version != DKCR)
        return (m_materialFlags & Transparent);
    else
    {
        if (m_passes[11] != nullptr)
            return true;

        return (m_unknown1 & 0x20);
    }

    return false;
}

void CMaterial::setAmbient(const QColor& ambient)
{
    m_ambient = ambient;
}

bool CMaterial::bind()
{
    if ((m_version == MetroidPrime3 || m_version == DKCR))
    {
        if (m_unknown1 & 0x100)
            return false;
    }

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
        
        std::string texsName = QString("texs").toStdString();
        int texsLoc = m_program->uniformLocation(texsName.c_str());
        if (texsLoc >= 0)
            glUniform1iv(texsLoc, 8, TEX_UNIS);

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
    assignTexturesEnabled();
    updateAnimations();

    atUint32 pass = 0;
    if (m_version != MetroidPrime3 && m_version != DKCR)
    {
        for (CAssetID& texID : m_textures)
        {
            CTexture* texture = dynamic_cast<CTexture*>(CResourceManager::instance()->loadResource(texID, "txtr"));

            if (texture)
            {
                glActiveTexture(GL_TEXTURE0 + pass++);
                texture->bind();
            }
        }

        for (atUint32 i = 0; i < m_konstCount; i++)
            assignKonstColor(i);

        m_program->setUniformValue("ambientLight", m_ambient);
        if (!(m_materialFlags & 0x80))
            glDepthMask(GL_FALSE);


        GXSetBlendMode(m_blendSrcFactor, m_blendDstFactor);
    }
    else
    {
        if (m_passes[11] != nullptr)
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        else
            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

        for (atUint32 i = 0; i < 12; i++)
        {
            if (m_passes[i] == nullptr)
                continue;

            if (pass == 0 && m_passes[i]->subCommand == EMaterialCommand::INCA)
                glBlendFunc(GL_ONE, GL_ONE);

            CTexture* texture = dynamic_cast<CTexture*>(CResourceManager::instance()->loadResource(m_passes[i]->textureId, "txtr"));

            if (texture)
            {
                glActiveTexture(GL_TEXTURE0 + pass);
                texture->bind();
                pass++;
            }
        }
    }

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
    return  m_program;
}

bool CMaterial::operator==(const CMaterial& right)
{
    return (m_version == right.m_version && right.m_materialFlags == right.m_materialFlags &&
            m_textures == right.m_textures && m_vertexAttributes == right.m_vertexAttributes &&
            m_unknown1_mp2 == right.m_unknown1_mp2 && m_unknown2_mp2 == right.m_unknown2_mp2 &&
            m_unknown1 == right.m_unknown1 && m_konstCount == right.m_konstCount &&
            !memcmp(m_konstColor, right.m_konstColor, sizeof(m_konstColor)) && m_blendDstFactor == right.m_blendDstFactor &&
            m_unknown2 == right.m_unknown2 && m_unkFlags == right.m_unkFlags &&
            m_tevStages == right.m_tevStages && m_texTEVIn == right.m_texTEVIn &&
            m_texGenFlags == right.m_texGenFlags && m_animations == right.m_animations &&
            m_materialSections == right.m_materialSections);
}

QString CMaterial::fragmentSource() const
{
    return m_fragmentSource;
}

QString CMaterial::vertexSource() const
{
    return m_vertexSource;
}


void CMaterial::assignTexTransformMatrices(atUint32 pass, glm::mat4& texMtx, glm::mat4& postMtx)
{
    std::string texMtxName = QString("texMtx[%1]").arg(pass).toStdString();
    atUint32 texMtxLoc = m_program->uniformLocation(texMtxName.c_str());
    glUniformMatrix4fv(texMtxLoc, 1, GL_FALSE, &texMtx[0][0]);
    std::string postMtxName = QString("postMtx[%1]").arg(pass).toStdString();
    atUint32 postMtxLoc = m_program->uniformLocation(postMtxName.c_str());
    glUniformMatrix4fv(postMtxLoc, 1, GL_FALSE, &postMtx[0][0]);
}

void CMaterial::updateAnimations()
{
    float s = secondsMod900();
    if (m_version == MetroidPrime1 || m_version == MetroidPrime2)
    {
        for (atUint32 i = 0; i < m_animations.size(); i++)
        {
            glm::mat4& texMtx = g_texMtx[i];
            glm::mat4& postMtx = g_postMtx[i];
            SAnimation& animation = m_animations[i];

            updateAnimation(animation, texMtx, postMtx, s);
            assignTexTransformMatrices(i, texMtx, postMtx);
        }
    }
    else
    {
        atUint32 pass = 0;
        for (atUint32 i = 0; i < 12; i++)
        {
            if (m_passes[i] == nullptr)
                continue;
            if (!m_passes[i]->hasAnim)
            {
                pass++;
                continue;
            }

            glm::mat4& texMtx = g_texMtx[pass];
            glm::mat4& postMtx = g_postMtx[pass];
            updateAnimation(m_passes[i]->anim, texMtx, postMtx, s);
            assignTexTransformMatrices(pass, texMtx, postMtx);
            pass++;
        }
    }
}

void CMaterial::updateAnimation(const SAnimation& animation, glm::mat4& texMtx, glm::mat4& postMtx, float s)
{
    switch (animation.mode)
    {
        case 0:
        case 1:
        case 7:
        {
            if ((animation.mode == 0 && !QSettings().value("mode0").toBool()) ||
                    (animation.mode == 1 && !QSettings().value("mode1").toBool()) ||
                    (animation.mode == 7 && !QSettings().value("mode7").toBool()))
                break;

            texMtx = glm::inverse(CGLViewer::instance()->viewMatrix() * m_modelMatrix);
            if (animation.mode != 7)
            {
                postMtx = glm::mat4(0.5, 0.0, 0.0, 0.5,
                                    0.0, 0.5, 0.0, 0.5,
                                    0.0, 0.0, 0.0, 1.0,
                                    0.0, 0.0, 0.0, 1.0);
            }
            else
            {
                glm::mat4 view = CGLViewer::instance()->viewMatrix();
                float xy = (view[0][3] + view[1][3]) * 0.025f * animation.parms[1];
                xy = (xy - (int)xy);
                float z = (view[2][3]) * 0.05f * animation.parms[1];
                z = (z - (int)z);

                float halfA = animation.parms[0] * 0.5f;

                postMtx = glm::mat4(halfA, 0.0, 0.0, xy,
                                    0.0, 0.0, halfA, z,
                                    0.0, 0.0, 0.0, 1.0,
                                    0.0, 0.0, 0.0, 1.0);
            }

            if (animation.mode == 0 || animation.mode == 7)
                texMtx[0][3] = texMtx[1][3] = texMtx[2][3] = 0;
        }
            break;
        case 2:
        {
            if (!QSettings().value("mode2").toBool())
                break;
            glm::vec2 texCoordBias;
            texCoordBias.s = (s * animation.parms[2]) + animation.parms[0];
            texCoordBias.t = (s * animation.parms[3]) + animation.parms[1];

            texMtx = glm::mat4(1);
            texMtx[0][3] = texCoordBias.s;
            texMtx[1][3] = texCoordBias.t;
        }
            break;
        case 3:
        {
            if (!QSettings().value("mode3").toBool())
                break;

            float angle = (s * animation.parms[1]) + animation.parms[0];
            float acos  = cos(angle);
            float asin  = sin(angle);
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

            float scale     = animation.parms[0];
            float numFrames = animation.parms[1];
            float step      = animation.parms[2];
            float offset    = animation.parms[3];

            texMtx = glm::mat4(1);
            float value = scale * step * (offset + s);

            float uvOffset = (float)(short)(float)(numFrames * fmod(value, 1.0f)) * step;
            if (animation.mode == 4)
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



