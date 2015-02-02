#include <GL/glew.h>
#include "CMaterialCache.hpp"
#include <QCryptographicHash>
#include <QDataStream>
#include <GL/gl.h>

atUint64 hashString(const QString & str)
{
    QByteArray hash = QCryptographicHash::hash(QByteArray::fromRawData((const char*)str.utf16(), str.length()*2), QCryptographicHash::Md5);
    Q_ASSERT(hash.size() == 16);
    QDataStream stream(hash);
    atUint64 a, b;
    stream >> a >> b;
    return a ^ b;
}

CMaterialCache::CMaterialCache()
{
}

CMaterialCache::~CMaterialCache()
{
}

void CMaterialCache::updateViewProjectionUniforms(glm::mat4 view, glm::mat4 proj)
{
    for (CMaterial& mat : m_cachedMaterials)
    {
        mat.bind();
        atUint32 projectionLoc = mat.program()->uniformLocation("projection");
        atUint32 viewLoc = mat.program()->uniformLocation("view");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &proj[0][0]);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        mat.release();
    }
}

std::shared_ptr<CMaterialCache> CMaterialCache::instance()
{
    static std::shared_ptr<CMaterialCache> m_instance = std::make_shared<CMaterialCache>();

    return m_instance;
}

atUint32 CMaterialCache::addMaterial(CMaterial mat)
{
    ConstMaterialIterator iter = std::find_if(m_cachedMaterials.begin(), m_cachedMaterials.end(),
                                              [&mat](CMaterial m)->bool{return m == mat; });
    if (iter != m_cachedMaterials.end())
        return iter - m_cachedMaterials.begin();

    m_cachedMaterials.push_back(mat);

    return m_cachedMaterials.size() - 1;
}

CMaterial& CMaterialCache::material(atUint32 index)
{
    return m_cachedMaterials.at(index);
}

void CMaterialCache::setAmbientOnMaterials(std::vector<atUint32> materials, const QColor& ambient)
{
    for (atUint32 mat : materials)
    {
        m_cachedMaterials[mat].setAmbient(ambient);
    }
}

QOpenGLShader* CMaterialCache::shaderFromSource(const QString& source, QOpenGLShader::ShaderType type)
{
    atUint64 hash = hashString(source);

    if (type == QOpenGLShader::Vertex)
    {
        if (m_vertexShaders.find(hash) != m_vertexShaders.end())
            return m_vertexShaders[hash];

        qDebug() << "Building vtx shader for hash " << hex << hash;
        QOpenGLShader* sh = new QOpenGLShader(type, this);
        if (sh->compileSourceCode(source))
        {
            m_vertexShaders[hash] = sh;
            return sh;
        }
        delete sh;
    }
    else
    {
        if (m_fragmentShaders.find(hash) != m_fragmentShaders.end())
            return m_fragmentShaders[hash];

        qDebug() << "Building px shader for hash " << hex << hash;
        QOpenGLShader* sh = new QOpenGLShader(type, this);
        if (sh->compileSourceCode(source))
        {
            m_fragmentShaders[hash] = sh;
            return sh;
        }
        delete sh;
    }

    return nullptr;
}

