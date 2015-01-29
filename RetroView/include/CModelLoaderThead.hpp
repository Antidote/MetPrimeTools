#ifndef MODELLOADERTHEAD_HPP
#define MODELLOADERTHEAD_HPP

#include <QObject>
#include <QStringList>
#include <QMutex>
static QMutex globalMutex;
class IRenderableModel;
class ModelLoaderThread : public QObject
{
    Q_OBJECT
public:
    ModelLoaderThread(const QStringList& paths, QObject* parent=0);
    ~ModelLoaderThread() {}

    void addFiles(const QStringList& paths);
public slots:
    void process();

signals:
    void error(QString);
    void newFile(IRenderableModel*,QString);
    void finished();
private:
    QStringList m_files;
};

#endif // MODELLOADERTHEAD_HPP
