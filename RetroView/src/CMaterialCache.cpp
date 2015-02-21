#include <GL/glew.h>
#include "CMaterialCache.hpp"
#include "CResourceLoaderThead.hpp"
#include <iostream>
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

void CMaterialCache::initialize()
{
    std::cout << "Material cache initialized" << std::endl;
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

        std::cout << "Building vtx shader for hash " << std::hex << hash << std::endl;
        QOpenGLShader* sh = new QOpenGLShader(type, this);
        if (sh->compileSourceCode(source))
        {
            m_vertexShaders[hash] = sh;
            return sh;
        }
        else
        {
            std::cout << sh->log().toStdString() << std::endl;
            std::cout << sh->sourceCode().toStdString() << std::endl;
        }
        delete sh;
    }
    else
    {
        if (m_fragmentShaders.find(hash) != m_fragmentShaders.end())
            return m_fragmentShaders[hash];

        std::cout << "Building px shader for hash " << std::hex << hash << std::endl;
        QOpenGLShader* sh = new QOpenGLShader(type, this);
        if (sh->compileSourceCode(source))
        {
            m_fragmentShaders[hash] = sh;
            return sh;
        }
        else
        {
            std::cout << sh->log().toStdString() << std::endl;
        }

        delete sh;
    }

    return nullptr;
}

