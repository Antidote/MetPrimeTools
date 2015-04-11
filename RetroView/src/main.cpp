#include "ui/CMainWindow.hpp"
#include "core/CTemplateManager.hpp"
#include <QApplication>
#include <QSurfaceFormat>
#include <QMessageBox>
#include <glm/glm.hpp>
#include <QDir>
#include <QDirIterator>
#include <QStandardPaths>
#include <QSettings>
#include <QDebug>

QString getSource(QString Filename)
{
    QFile file(Filename);

    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        std::cout << "could not open file for read: " << Filename.toStdString() << std::endl;
        return QString();
    }

    QTextStream in(&file);
    QString source = in.readAll();

    file.close();
    return source;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setAttribute(Qt::AA_DontUseNativeMenuBar);
    // initialize settings info so that default QSettings() access works
    a.setWindowIcon(QIcon(":/icons/64x64/apps/retroview.png"));
    a.setOrganizationName("MetPrimeTools");
    a.setApplicationName("RetroView");

    QSettings().setValue("applicationRootPath", a.applicationDirPath());

    QFileInfo fi(a.applicationDirPath() + "/templates");
    if (fi.exists() && fi.isWritable())
        CTemplateManager::instance()->initialize(fi.absolutePath().toStdString());
    else
    {
        QString homeLocation = QStandardPaths::locate(QStandardPaths::HomeLocation, QString(), QStandardPaths::LocateDirectory);
        QDir homeDir = QDir(homeLocation + "/.retroview");

        if (!homeDir.exists("templates"))
        {
            QDirIterator iter(":templates", QDirIterator::Subdirectories);
            while (iter.hasNext())
            {
                QString file = iter.next();
                QFileInfo fileInfo(file);
                file = file.remove(0, 1);
                if (fileInfo.isDir())
                {
                    homeDir.mkpath(file);
                }
                else
                {
                    QString outPath = homeDir.absolutePath() + "/" + file;
                    QFile out(outPath);
                    QString data = getSource(":" + file);
                    if (out.open(QFile::WriteOnly))
                    {
                        out.write(data.toLocal8Bit());
                        out.close();
                    }
                }
            }
        }

        homeDir.cd("templates");

        std::cout << homeDir.absolutePath().toStdString() << std::endl;
        CTemplateManager::instance()->initialize(homeDir.absolutePath().toStdString());
    }
    QSurfaceFormat fmt = QSurfaceFormat::defaultFormat();
    fmt.setDepthBufferSize(24);
    fmt.setMajorVersion(3);
    fmt.setMinorVersion(3);
#ifdef __APPLE__
    fmt.setProfile(QSurfaceFormat::CoreProfile);
#endif

    fmt.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    QSurfaceFormat::setDefaultFormat(fmt);

    CMainWindow w;
    w.show();

    return a.exec();
}
