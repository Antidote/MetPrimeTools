#include <GL/glew.h>
#include "models/CModelFile.hpp"
#include "core/CMaterialCache.hpp"
#include "core/GXCommon.hpp"
#include "ui/CGLViewer.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <dae.h>
#include <dae/daeUtils.h>
#include <dom/domCOLLADA.h>

CModelFile::CModelFile()
{
    setAmbient(.5f, .5f, .5f);
}

CModelFile::~CModelFile()
{

}

void CModelFile::draw()
{
    currentMaterialSet().setAmbient(m_ambient);

    glm::mat4 model = glm::mat4(1);
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
    return m_materialSets[0];
}

bool CModelFile::canExport() const
{
    return true;
}

template<typename T>
daeTArray<T> rawArrayToDaeArray(T rawArray[], size_t count)
{
    daeTArray<T> result;
    for (size_t i = 0; i < count; i++)
        result.append(rawArray[i]);

    return result;
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

