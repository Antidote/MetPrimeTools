#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "core/STEVStage.hpp"
#include "core/GXTypes.hpp"
#include "core/CMaterialSection.hpp"
#include "core/SAnimation.hpp"
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <vector>
#include <QColor>
#include <glm/glm.hpp>


enum MaterialFlags
{
    Unused1        = (1 <<  0), // ??? always set
    Unused2        = (1 <<  1), // ""
    Unused3        = (1 <<  2), // ??? always unset
    EnableKonst    = (1 <<  3),
    Transparent    = (1 <<  4),
    PunchThrough   = (1 <<  5),
    Reflective     = (1 <<  6),
    DepthWrite     = (1 <<  7),
    Unknown1       = (1 <<  8),
    Invisible      = (1 <<  9),
    EnableUnknown2 = (1 << 10),
    Unknown2       = (1 << 11),
    Unknown3       = (1 << 12),
    Lightmaps      = (1 << 13),
    Unknown4       = (1 << 14),
    Unknown5       = (1 << 15),
    Unknown6       = (1 << 16),
    Unknown7       = (1 << 17),
    Unknown8       = (1 << 18),
    Unknown9       = (1 << 19),
    // Next 13 bits are unused
};

bool operator==(const SAnimation& left, const SAnimation& right);

class CMaterial
{
public:
    glm::mat4 g_texMtx[11];
    glm::mat4 g_postMtx[21];
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
    void setViewMatrix(const glm::mat4& viewMatrix);
    void setProjectionMatrix(const glm::mat4& projectionMatrix);
    void setTexturesEnabled(const bool& enabled);

    atUint32 materialFlags()    const;
    void setMaterialFlags(atUint32 matFlags);
    void setVertexAttributes(atUint32 vertFlags);
    atUint32 vertexAttributes() const;

    void addTevStage(const STEVStage& tevStage, atUint32 texInFlags);

    void setBlendMode(EBlendMode dst, EBlendMode src);

    void setKonstColor(atUint32 id, const QColor& color);

    bool hasPosition() const;
    bool hasNormal() const;
    bool hasColor(atUint8 slot) const;
    bool hasUV(atUint8 slot) const;
    bool isTransparent() const;
    void setAmbient(const QColor& ambient);

    bool bind();
    void release();
    QOpenGLShaderProgram* program();
    bool operator==(const CMaterial& right);

    QString fragmentSource() const;
    QString vertexSource() const;
private:
    friend class CMaterialReader;

    void updateAnimations();
    atUint32 textureMatrixIdx(atUint32 id);
    atUint32 postTransformMatrixIdx(atUint32 id);
    void updateAnimation(const SAnimation& animation, glm::mat4& texMtx, glm::mat4& postMtx, float s);
    bool hasAttribute(atUint32 index) const;
    QString getSource(QString Filename);
    QOpenGLShader* buildVertex();
    QOpenGLShader* buildFragment();
    void assignKonstColor(atUint32 id);
    void assignModelMatrix();
    void assignViewMatrix();
    void assignProjectionMatrix();
    void assignTexturesEnabled();
    void assignTexTransformMatrices(atUint32 i, glm::mat4& texMtx, glm::mat4& postMtx);

    QOpenGLShaderProgram*          m_program;
    QColor                         m_ambient;
    Version                        m_version;
    atUint32                       m_materialFlags;
    atUint32                       m_vertexAttributes;
    atUint32                       m_unknown1_mp2;
    atUint32                       m_unknown2_mp2;
    atUint32                       m_unknown1;
    atUint32                       m_konstCount;
    QColor                         m_konstColor[4];
    EBlendMode                     m_blendDstFactor;
    EBlendMode                     m_blendSrcFactor;
    atUint32                       m_unknown2;
    std::vector<atUint32>          m_unkFlags;
    std::vector<STEVStage>         m_tevStages;
    std::vector<atUint32>          m_texTEVIn;
    std::vector<atUint32>          m_texGenFlags;
    std::vector<SAnimation>        m_animations;
    glm::mat4                      m_modelMatrix;
    glm::mat4                      m_viewMatrix;
    glm::mat4                      m_projectionMatrix;
    bool                           m_isBound;
    bool                           m_texturesEnabled;
    SPASSCommand*                  m_passes[12];
    std::vector<CMaterialSection*> m_materialSections;
    std::vector<CAssetID>          m_textures;
    QString                        m_fragmentSource;
    QString                        m_vertexSource;
};




#endif // MATERIAL_HPP
