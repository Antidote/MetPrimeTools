#include "models/CModelFile.hpp"
#include "core/CMaterialCache.hpp"
#include "core/GXCommon.hpp"
#include "ui/CGLViewer.hpp"

#include <glm/gtc/matrix_transform.hpp>

CModelFile::CModelFile()
    : m_currentSet(0)
{
    restoreDefaults();
    setAmbient(.5f, .5f, .5f);
}

CModelFile::~CModelFile()
{

}

void CModelFile::draw()
{
    currentMaterialSet().setAmbient(m_ambient);

    glm::mat4 model = glm::mat4(m_rotation) * glm::scale(glm::mat4(1), m_scale) * glm::translate(glm::mat4(1), m_position);
    CModelData::drawIbos(false, currentMaterialSet(), model);
    CModelData::drawIbos(true,  currentMaterialSet(), model);
}

void CModelFile::drawBoundingBox()
{
    CModelData::drawBoundingBoxes();
}

void CModelFile::updateViewProjectionUniforms(const glm::mat4& view, const glm::mat4& proj)
{
    for (atUint32 matId : currentMaterialSet().materials())
    {
        CMaterial& mat = CMaterialCache::instance()->material(matId);
        mat.setViewMatrix(view);
        mat.setProjectionMatrix(proj);
    }
}

void CModelFile::updateTexturesEnabled(const bool& enabled)
{
    for (atUint32 matId : currentMaterialSet().materials())
    {
        CMaterial& mat = CMaterialCache::instance()->material(matId);
        mat.setTexturesEnabled(enabled);
    }
}

CMaterialSet& CModelFile::currentMaterialSet()
{
    return m_materialSets[m_currentSet];
}

atUint32 CModelFile::materialSetCount() const
{
    return m_materialSets.size();
}

void CModelFile::setCurrentMaterialSet(atUint32 set)
{
    m_currentSet = set % m_materialSets.size();;
}

atUint32 CModelFile::currentMaterialSetIndex() const
{
    return m_currentSet;
}

void CModelFile::setPosition(const glm::vec3& pos)
{
    m_position = pos;
}

void CModelFile::setRotation(const glm::vec3& rotation)
{
    m_rotation = glm::quat(glm::radians(rotation));
}

void CModelFile::setScale(const glm::vec3& scale)
{
    m_scale = scale;
}

void CModelFile::restoreDefaults()
{
    m_position = glm::vec3(0.0);
    m_rotation = glm::quat(glm::vec3(0.0));
    m_scale    = glm::vec3(1.0);
}

bool CModelFile::canExport() const
{
    return true;
}

void CModelFile::exportToObj(const std::string& filepath)
{
    atUint32 vertexOff = 1;
    atUint32 normalOff = 1;
    atUint32 texCoordOff = 1;
    std::ofstream out(filepath);
    CModelData::exportModel(out, vertexOff, normalOff, texCoordOff, currentMaterialSet());
    out.flush();
    out.close();
}

SBoundingBox& CModelFile::boundingBox()
{
    static SBoundingBox bbox{m_boundingBox};

    if (bbox != SBoundingBox{m_boundingBox})
        bbox =  SBoundingBox{m_boundingBox};

    return bbox;
}

