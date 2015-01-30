#ifndef MODELLOADERTHEAD_HPP
#define MODELLOADERTHEAD_HPP

#include <QObject>
#include <QStringList>
#include <QMutex>
static QMutex globalMutex;
class IResource;
class CResourceLoaderThread : public QObject
{
    Q_OBJECT
public:
    CResourceLoaderThread(const QStringList& paths, QObject* parent=0);
    ~CResourceLoaderThread() {}

    void addFiles(const QStringList& paths);
public slots:
    void process();

signals:
    void error(QString);
    void newFile(IResource*,QString);
    void finished();
private:
    QStringList m_files;
};

#endif // MODELLOADERTHEAD_HPP
