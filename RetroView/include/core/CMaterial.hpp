#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "STEVStage.hpp"
#include "GXTypes.hpp"
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <vector>
#include <QColor>
#include <glm/glm.hpp>

static glm::mat4 g_texMtx[11];
static glm::mat4 g_postMtx[21];

struct SAnimation
{
    atUint32 mode;
    float parms[4];
};

bool operator==(const SAnimation& left, const SAnimation& right);

class CMaterial
{
public:
    enum Version
    {
        MetroidPrime1,
        MetroidPrime2,
        MetroidPrime3,
        DKCR
    };
    CMaterial();
    virtual ~CMaterial();


    void setModelMatrix(const glm::mat4& modelMatrix);
    atUint32 materialFlags() const;
    atUint32 vertexAttributes() const;

    std::vector<atUint32> textureIndices() const;

    bool hasPosition();
    bool hasNormal();
    bool hasColor(atUint8 slot);
    bool hasUV(atUint8 slot);
    bool isTransparent();
    void setAmbient(const QColor& ambient);

    bool bind();
    void release();
    QOpenGLShaderProgram* program();
    bool operator==(const CMaterial& right);
private:
    friend class CMaterialReader;

    void updateAnimations();
    atUint32 textureMatrixIdx(atUint32 id);
    atUint32 postTransformMatrixIdx(atUint32 id);
    bool hasAttribute(atUint32 index);
    QString getSource(QString Filename);
    QOpenGLShader* buildVertex();
    QOpenGLShader* buildFragment();
    QOpenGLShaderProgram*    m_program;
    QColor                   m_ambient;
    Version                  m_version;
    atUint32                 m_materialFlags;
    std::vector<atUint32>    m_textureIndices;
    atUint32                 m_vertexAttributes;
    atUint32                 m_unknown1_mp2;
    atUint32                 m_unknown2_mp2;
    atUint32                 m_unknown1;
    atUint32                 m_konstCount;
    atUint32                 m_konstColor[4];
    EBlendMode               m_blendDstFactor;
    EBlendMode               m_blendSrcFactor;
    atUint32                 m_unknown2;
    std::vector<atUint32>    m_unkFlags;
    std::vector<STEVStage>   m_tevStages;
    std::vector<atUint32>    m_texTEVIn;
    std::vector<atUint32>    m_texGenFlags;
    std::vector<SAnimation>  m_animations;
    glm::mat4                m_modelMatrix;
};


#endif // MATERIAL_HPP
