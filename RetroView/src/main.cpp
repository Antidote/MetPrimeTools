#include "ui/CMainWindow.hpp"
#include "core/CTemplateManager.hpp"
#include <QApplication>
#include <QSurfaceFormat>
#include <QMessageBox>
#include <glm/glm.hpp>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setAttribute(Qt::AA_DontUseNativeMenuBar);
    // initialize settings info so that default QSettings() access works
    a.setWindowIcon(QIcon(":/icons/mainicon.png"));
    a.setOrganizationName("MetPrimeTools");
    a.setApplicationName("RetroView");

    QFileInfo fi(a.applicationDirPath());
    if (!fi.isWritable())
        CTemplateManager::instance()->initialize("../templates");
    else
    {
        QString homeLocation = QStandardPaths::locate(QStandardPaths::HomeLocation, QString(), QStandardPaths::LocateDirectory);
        QDir homeDir = QDir(homeLocation + "/.retroview");
        if (!homeDir.exists())
        {
            homeDir.mkpath("templates");
            homeDir.cd("templates");
        }

        CTemplateManager::instance()->initialize(QString(homeLocation + "/retroview/templates").toStdString());
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
