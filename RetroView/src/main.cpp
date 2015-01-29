#ifndef NO_MAIN
/**************************************************************************
**
**************************************************************************/

//#define GLEW_STATIC
#include <GL/glew.h>
#include <QApplication>
#include "CMainWindow.hpp"
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // initialize settings info so that default QSettings() access works
    a.setWindowIcon(QIcon(":/MPxViewer.png"));
    a.setOrganizationName("MetPrimeTools");
    a.setApplicationName("cmdlreader");

    CMainWindow w;
    w.show();

    return a.exec();
}
#endif
