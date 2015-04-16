#include "ui/CMainWindow.hpp"
#include <QApplication>
#include <QTimer>
#include <QSurfaceFormat>


#if __APPLE__
extern "C" {
void osx_init();
}
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setAttribute(Qt::AA_DontUseNativeMenuBar);
    // initialize settings info so that default QSettings() access works
    a.setWindowIcon(QIcon(":/icons/64x64/apps/retroview.png"));
    a.setOrganizationName("MetPrimeTools");
    a.setApplicationName("RetroView");
    
#if __APPLE__
    osx_init();
#endif

    QSurfaceFormat fmt = QSurfaceFormat::defaultFormat();
    fmt.setDepthBufferSize(32);
    fmt.setMajorVersion(3);
    fmt.setMinorVersion(3);
#if __linux__
    fmt.setProfile(QSurfaceFormat::NoProfile);
#else
    fmt.setProfile(QSurfaceFormat::CoreProfile);
#endif

    fmt.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    QSurfaceFormat::setDefaultFormat(fmt);

    CMainWindow w;
    QTimer::singleShot(0, &w, SLOT(initialize()));

    w.show();

    return a.exec();
}
