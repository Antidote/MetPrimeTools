#ifndef CMATERIALCACHE_HPP
#define CMATERIALCACHE_HPP


#include <vector>
#include <memory>
#include <unordered_map>
#include <Athena/Types.hpp>
#include <glm/glm.hpp>
#include <QOpenGLShader>
#include "core/CMaterial.hpp"

class CMaterialCache : QObject
{
    Q_OBJECT
public:
    typedef std::vector<CMaterial>::iterator       MaterialIterator;
    typedef std::vector<CMaterial>::const_iterator ConstMaterialIterator;
    CMaterialCache();
    virtual ~CMaterialCache();


    void initialize();
    static std::shared_ptr<CMaterialCache> instance();
    atUint32 addMaterial(CMaterial mat);
    CMaterial& material(atUint32 index);

    void setAmbientOnMaterials(std::vector<atUint32> materials, const QColor& ambient);

    QOpenGLShader* shaderFromSource(const QString& source, QOpenGLShader::ShaderType type);

private:
    std::vector<CMaterial> m_cachedMaterials;
    std::unordered_map<atUint64, QOpenGLShader*> m_vertexShaders;
    std::unordered_map<atUint64, QOpenGLShader*> m_fragmentShaders;
};

#endif // CMATERIALCACHE_HPP
