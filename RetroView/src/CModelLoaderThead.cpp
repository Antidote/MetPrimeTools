#include "CModelLoaderThead.hpp"
#include "CModelLoader.hpp"
#include "IRenderableModel.hpp"
#include <Athena/Exception.hpp>
#include "CGLViewer.hpp"
#include <QFileInfo>
#include <QMutexLocker>

ModelLoaderThread::ModelLoaderThread(const QStringList& paths, QObject* parent)
    : QObject(parent),
      m_files(paths)
{
}

void ModelLoaderThread::addFiles(const QStringList& paths)
{
    QMutexLocker locker(&globalMutex);
    m_files << paths;
    locker.unlock();
}

void ModelLoaderThread::process()
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

        IRenderableModel* ret = nullptr;
//        try
//        {
            ret = SModelLoader::instance().get()->loadFile(tmpFilename.toStdString());
            emit newFile(ret, tmpFilename);
//        }
//        catch(const Athena::error::Exception& e)
//        {
//            delete ret;
//            emit error(QString::fromStdString(e.message()));
//        }

    }

    locker.unlock();
    emit finished();
}
