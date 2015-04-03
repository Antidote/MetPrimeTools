/**************************************************************************
**
**************************************************************************/

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include "ui/CGLViewer.hpp"
#include "core/CPakFileModel.hpp"

namespace Ui {
class CMainWindow;
}

class CPakTreeWidget;
class IResource;
class CMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CMainWindow(QWidget *parent = 0);
    virtual ~CMainWindow();

    bool canShow();
signals:
    void closing();
protected:
    bool event(QEvent *event);

private slots:
    void onModeToggled(bool value);
    void onToggled(bool);
    void onMaterialSetChanged(int set);
    void onViewerInitialized();
    void onExport();
    void onNewPak(CPakTreeWidget* pak);
    void onLoadPak();
    void onTabChanged();
    void onResourceChanged(IResource* res);
    void updateFPS();
private:
    Ui::CMainWindow *ui;
    CPakTreeWidget* m_currentTab;
    QStringList    m_filters;
    QString        m_allSupportedFilter;
    QTimer         m_fpsUpdateTimer;
};

#endif // MAINWINDOW_HPP
