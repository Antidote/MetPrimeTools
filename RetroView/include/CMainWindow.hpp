/**************************************************************************
**
**************************************************************************/

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include "CGLViewer.hpp"

namespace Ui {
class MainWindow;
}

class CMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CMainWindow(QWidget *parent = 0);
    ~CMainWindow();

signals:
    void closing();
private slots:
    void onOpenFile();
    void onToggled(bool);
    void onViewerInitialized();
    void onFileAdded(QString file);
    void onCloseFile();
    void onCloseAll();
    void onExport();
protected:
private:
    Ui::MainWindow *ui;
    QStringList    m_filters;
    QString        m_allSupportedFilter;
};

#endif // MAINWINDOW_HPP
