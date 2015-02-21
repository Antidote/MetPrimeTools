#include "CResourceLoaderThead.hpp"
#include "CResourceManager.hpp"
#include "IRenderableModel.hpp"
#include <Athena/Exception.hpp>
#include "CGLViewer.hpp"
#include <QFileInfo>
#include <QMutexLocker>

CResourceLoaderThread::CResourceLoaderThread(const QStringList& paths, QObject* parent)
    : QObject(parent),
      m_files(paths)
{
}

void CResourceLoaderThread::addFiles(const QStringList& paths)
{
    QMutexLocker locker(&globalMutex);
    m_files << paths;
    locker.unlock();
}

void CResourceLoaderThread::process()
{
    QMutexLocker locker(&globalMutex);
    while (!m_files.isEmpty())
    {
        QString file = m_files.takeFirst();

        if (QFileInfo(file).isDir())
            continue;
        if (file.isEmpty())
        {
            emit error("Invalid path Specified");
            return;
        }

        QString tmpFilename = file;
#ifdef Q_OS_WIN32
        tmpFilename = tmpFilename.toLower();
#endif
        if (CGLViewer::instance()->hasFile(tmpFilename))
            continue;

        IResource* ret = CResourceManager::instance().get()->loadResource(tmpFilename.toStdString());
        if (ret)
            emit newFile(ret, tmpFilename);
    }
    locker.unlock();
    emit finished();
}
