/**************************************************************************
**
**************************************************************************/

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include "CGLViewer.hpp"
#include <CPakFileModel.hpp>

namespace Ui {
class CMainWindow;
}

class CPakTreeWidget;
class CMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CMainWindow(QWidget *parent = 0);
    ~CMainWindow();

    bool canShow();
signals:
    void closing();
protected:
    bool event(QEvent *event);

private slots:
    void onModeToggled(bool value);
    void onOpenFile();
    void onToggled(bool);
    void onViewerInitialized();
    void onFileAdded(QString file);
    void onCloseFile();
    void onCloseAll();
    void onExport();
    void onNewPak(CPakTreeWidget* pak);
    void onLoadPak();
private:
    Ui::CMainWindow *ui;
    QStringList    m_filters;
    QString        m_allSupportedFilter;
};

#endif // MAINWINDOW_HPP
