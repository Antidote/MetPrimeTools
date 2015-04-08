#include "core/CResourceManager.hpp"
#include "core/IRenderableModel.hpp"
#include "generic/CWorldFile.hpp"
#include "ui_CMainWindow.h"
#include "ui/CMainWindow.hpp"
#include "ui/CGLViewer.hpp"
#include "ui/CPakTreeWidget.hpp"

#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>
#include <QThread>
#include <QShowEvent>
#include <iostream>

CMainWindow::CMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CMainWindow),
    m_currentTab(nullptr)
{
    ui->setupUi(this);
    ui->tabWidget->setUsesScrollButtons(true);
    ui->tabWidget->setElideMode(Qt::ElideNone);
    connect(ui->glView, SIGNAL(initialized()), this, SLOT(onViewerInitialized()));
    CResourceManager* resourceManager = CResourceManager::instance().get();

    connect(resourceManager, SIGNAL(newPak(CPakTreeWidget*)), this, SLOT(onNewPak(CPakTreeWidget*)));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(onTabChanged()));
    ui->tabWidget->setElideMode(Qt::ElideNone);
    ui->tabWidget->setUsesScrollButtons(true);

    installEventFilter(CKeyboardManager::instance());
    ui->actionMode0          ->setChecked(QSettings().value("mode0",           true ).toBool());
    ui->actionMode1          ->setChecked(QSettings().value("mode1",           true ).toBool());
    ui->actionMode2          ->setChecked(QSettings().value("mode2",           true ).toBool());
    ui->actionMode3          ->setChecked(QSettings().value("mode3",           true ).toBool());
    ui->actionMode4And5      ->setChecked(QSettings().value("mode4And5",       true ).toBool());
    ui->actionMode6          ->setChecked(QSettings().value("mode6",           true ).toBool());
    ui->actionMode7          ->setChecked(QSettings().value("mode7",           true ).toBool());
    ui->actionEnableTextures ->setChecked(QSettings().value("enableTextures",  true ).toBool());
    ui->actionDrawPoints     ->setChecked(QSettings().value("drawPoints",      false).toBool());
    ui->actionDrawJointNames ->setChecked(QSettings().value("drawJointNames",  false).toBool());
    ui->actionDrawBoundingBox->setChecked(QSettings().value("drawBoundingBox", false).toBool());
    ui->actionDrawCollision  ->setChecked(QSettings().value("drawCollision",   false).toBool());
    ui->actionWireframe      ->setChecked(QSettings().value("wireframe",       false).toBool());

    QString basePath = QFileDialog::getExistingDirectory(nullptr, "Specify Basepath");

    if (!basePath.isEmpty())
        resourceManager->initialize(basePath.toStdString());
    else
        QTimer::singleShot(10, qApp, SLOT(quit()));

    m_fpsUpdateTimer.setInterval(50);
}

CMainWindow::~CMainWindow()
{
    delete ui;
}

bool CMainWindow::canShow()
{
    return ui != nullptr;
}

bool CMainWindow::event(QEvent* e)
{
    switch(e->type())
    {

        case QEvent::WindowActivate:
            ui->glView->startUpdates();
            break;

        case QEvent::WindowDeactivate:
            ui->glView->stopUpdates();
            break;
        default:
            break;
    }
    return QMainWindow::event(e);
}

void CMainWindow::onToggled(bool checked)
{
    if (sender() == ui->actionMode0)
        QSettings().setValue("mode0", checked);
    else if (sender() == ui->actionMode1)
        QSettings().setValue("mode1", checked);
    else if (sender() == ui->actionMode2)
        QSettings().setValue("mode2", checked);
    else if (sender() == ui->actionMode3)
        QSettings().setValue("mode3", checked);
    else if (sender() == ui->actionMode4And5)
        QSettings().setValue("mode4And5", checked);
    else if (sender() == ui->actionMode6)
        QSettings().setValue("mode6", checked);
    else if (sender() == ui->actionMode7)
        QSettings().setValue("mode7", checked);
    else if (sender() == ui->actionEnableTextures)
        QSettings().setValue("enableTextures", checked);
    else if (sender() == ui->actionDrawPoints)
        QSettings().setValue("drawPoints", checked);
    else if (sender() == ui->actionDrawJointNames)
        QSettings().setValue("drawJointNames", checked);
    else if (sender() == ui->actionDrawBoundingBox)
        QSettings().setValue("drawBoundingBox", checked);
    else if (sender() == ui->actionDrawCollision)
        QSettings().setValue("drawCollision", checked);
    else if (sender() == ui->actionWireframe)
        QSettings().setValue("wireframe", checked);
    ui->glView->update();
}

void CMainWindow::onMaterialSetChanged(int set)
{
    IRenderableModel* current = CGLViewer::instance()->currentModel();
    if (current)
        current->setCurrentMaterialSet(set);
}

void CMainWindow::onViewerInitialized()
{
    connect(ui->resetCameraButton, SIGNAL(clicked()), ui->glView, SLOT(resetCamera()));
    connect(&m_fpsUpdateTimer, SIGNAL(timeout()), this, SLOT(updateFPS()));
    m_fpsUpdateTimer.start();
}

void CMainWindow::onExport()
{
    ui->glView->stopUpdates();
    ui->glView->exportFile();
    ui->glView->startUpdates();
}

void CMainWindow::onNewPak(CPakTreeWidget* pak)
{
    connect(pak, SIGNAL(resourceChanged(IResource*)), this, SLOT(onResourceChanged(IResource*)));
    QString tabTitle = QFileInfo(pak->filepath()).fileName();
    ui->tabWidget->addTab(pak, tabTitle);
}

void CMainWindow::onLoadPak()
{
    CResourceManager* resourceManager = CResourceManager::instance().get();
    ui->glView->stopUpdates();

    QStringList files = QFileDialog::getOpenFileNames(this, "Load Pak(s)", QString(), "PAK Files (*.pak *.Pak)");

    ui->glView->startUpdates();

    if (files.count() == 0)
        return;

    foreach(QString file, files)
        resourceManager->loadPak(file.toStdString());
}

void CMainWindow::onTabChanged()
{
    if (m_currentTab != nullptr)
        m_currentTab->clearCurrent();

    CGLViewer::instance()->setCurrent(nullptr);
    CGLViewer::instance()->setSkybox(nullptr);
    CResourceManager::instance()->clear();


//    CPakTreeWidget* ptw = qobject_cast<CPakTreeWidget*>(ui->tabWidget->currentWidget());
//    if (ptw && ptw->pak()->isWorldPak())
//    {
//        m_currentTab = ptw;
//        std::vector<SPakResource> res = m_currentTab->pak()->resourcesByType("mlvl");
//        CWorldFile* world = nullptr;
//        if (res.size() > 0)
//            world = dynamic_cast<CWorldFile*>(CResourceManager::instance()->loadResourceFromPak(ptw->pak(), res.at(0).id, "mlvl"));
//        if (world)
//        {
//            CGLViewer::instance()->setSkybox(world->skyboxModel());
//            world->destroy();
//        }
//    }
}

void CMainWindow::onResourceChanged(IResource* res)
{
    static const QString materialLbl = "Material Sets (%1)";
    if (res == nullptr)
        return;

    IRenderableModel* renderable = dynamic_cast<IRenderableModel*>(res);
    if (renderable)
    {
        // Clear current, so we don't have a race condition
        CGLViewer::instance()->setCurrent(nullptr);
        atUint32 materialSetCount = renderable->materialSetCount();
        QString lblVal = materialLbl.arg(materialSetCount);
        ui->materialSetLbl->setText((materialSetCount > 0 ? lblVal : "Material Set:"));
        ui->materialSetComboBox->setEnabled(materialSetCount > 1);
        ui->materialSetComboBox->clear();
        for (atUint32 i = 0; i < materialSetCount; i++)
            ui->materialSetComboBox->addItem(QString("Set %1").arg(i + 1));
        ui->materialSetComboBox->setCurrentIndex(renderable->currentMaterialSetIndex());
        CGLViewer::instance()->setCurrent(renderable);
    }
}

void CMainWindow::updateFPS()
{
    ui->fpsLabel->setText(QString("FPS: %1").arg(CGLViewer::instance()->frameRate()));
}
