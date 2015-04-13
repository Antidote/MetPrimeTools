/**************************************************************************
**
**************************************************************************/

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QLabel>
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

public slots:
    void initialize();

signals:
    void closing();
protected:
    bool event(QEvent *event);
    void dragEnterEvent(QDragEnterEvent* e);
    void dropEvent(QDropEvent* e);
    void moveEvent(QMoveEvent* e);

private slots:
    void onToggled(bool checked);
    void onMaterialSetChanged(int set);
    void onViewerInitialized();
    void onExport();
    void onNewPak(CPakTreeWidget* pak);
    void onLoadPak();
    void onTabChanged();
    void onResourceChanged(IResource* res);
    void updateFPS();
    void onTabClosed(int tabIdx);
private:
    Ui::CMainWindow *ui;
    CPakTreeWidget*    m_currentTab;
    QStringList        m_filters;
    QString            m_allSupportedFilter;
    QTimer             m_fpsUpdateTimer;
    QLabel             m_cameraPosition;
    QMap<QString, QTabWidget*> m_basePaths;
};

#endif // MAINWINDOW_HPP
