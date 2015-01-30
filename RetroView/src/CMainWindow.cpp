/**************************************************************************
**
**************************************************************************/

#include "GL/glew.h"
#include "CMainWindow.hpp"
#include "ui_MainWindow.h"
#include "CGLViewer.hpp"
#include "CResourceManager.hpp"
#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>
#include <QThread>
#include <QShowEvent>

CMainWindow::CMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->glView, SIGNAL(initialized()), this, SLOT(onViewerInitialized()));

    CResourceManager* resourceManager = CResourceManager::instance().get();
    QString basePath = QFileDialog::getExistingDirectory(this, "Specify Basepath");

    if (!basePath.isEmpty())
        resourceManager->initialize(basePath.toStdString());
    else
        QTimer::singleShot(10, qApp, SLOT(quit()));
}

CMainWindow::~CMainWindow()
{
    delete ui;
}

bool CMainWindow::canShow()
{
    return ui != nullptr;
}

void CMainWindow::onOpenFile()
{
    ui->glView->stopUpdates();

    QStringList files = QFileDialog::getOpenFileNames(this, "Open Model", QString(), "All Supported (*.CMDL *.MREA)");

    ui->glView->startUpdates();

    if (files.count() == 0)
        return;

    ui->glView->openModels(files);
}

void CMainWindow::onToggled(bool checked)
{
    if (sender() == ui->pointsCheckBox)
    {
        QSettings().setValue("drawPoints", checked);
    }
    else if (sender() == ui->jointNamesCheckBox)
    {
        QSettings().setValue("drawJointNames", checked);
    }
    else if (sender() == ui->boundingBoxChkBox)
    {
        QSettings().setValue("drawBoundingBox", checked);
    }
    else if (sender() == ui->enableLightingCheckBox)
    {
        QSettings().setValue("enableLighting", checked);
    }
    else if (sender() == ui->enableTexturesCheckBox)
    {
        QSettings().setValue("enableTextures", checked);
    }
    else if (sender() == ui->drawCollisionCheckBox)
    {
        QSettings().setValue("drawCollision", checked);
    }
    else if (sender() == ui->transActors)
    {
        QSettings().setValue("drawTranslucent", checked);
    }
    ui->glView->update();
}

void CMainWindow::onViewerInitialized()
{
    ui->pointsCheckBox->setChecked(QSettings().value("drawPoints").toBool());
    ui->jointNamesCheckBox->setChecked(QSettings().value("drawJointNames").toBool());
    ui->wireframeCheckBox->setChecked(QSettings().value("wireframe").toBool());
    ui->boundingBoxChkBox->setChecked(QSettings().value("drawBoundingBox").toBool());
    ui->enableLightingCheckBox->setChecked(QSettings().value("enableLighting").toBool());
    ui->enableTexturesCheckBox->setChecked(QSettings().value("enableTextures").toBool());
    ui->drawCollisionCheckBox->setChecked(QSettings().value("drawCollision").toBool());
    ui->transActors->setChecked(QSettings().value("drawTranslucent").toBool());
    ui->axisCheckBox->setChecked(QSettings().value("axisDrawn").toBool());
    ui->gridCheckBox->setChecked(QSettings().value("gridDrawn").toBool());


    connect(ui->pointsCheckBox, SIGNAL(toggled(bool)), this, SLOT(onToggled(bool)));
    connect(ui->jointNamesCheckBox, SIGNAL(toggled(bool)), this, SLOT(onToggled(bool)));
    connect(ui->enableTexturesCheckBox, SIGNAL(toggled(bool)), this, SLOT(onToggled(bool)));
    connect(ui->boundingBoxChkBox, SIGNAL(toggled(bool)), this, SLOT(onToggled(bool)));
    connect(ui->gridCheckBox, SIGNAL(toggled(bool)), ui->glView, SLOT(setGridIsDrawn(bool)));
    connect(ui->drawCollisionCheckBox, SIGNAL(toggled(bool)), this, SLOT(onToggled(bool)));
    connect(ui->axisCheckBox, SIGNAL(toggled(bool)), ui->glView, SLOT(setAxisIsDrawn(bool)));
    connect(ui->enableLightingCheckBox, SIGNAL(toggled(bool)), this, SLOT(onToggled(bool)));
    connect(ui->wireframeCheckBox, SIGNAL(toggled(bool)), this, SLOT(onToggled(bool)));
    connect(ui->transActors, SIGNAL(toggled(bool)), this, SLOT(onToggled(bool)));
    connect(ui->glView, SIGNAL(fileAdded(QString)), this, SLOT(onFileAdded(QString)));
    connect(ui->listWidget, SIGNAL(itemSelectionChanged()), ui->glView, SLOT(onItemSelectionChanged()));
    ui->listWidget->addAction(ui->actionClose);
    ui->listWidget->addAction(ui->actionExport);
    ui->listWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
    connect(ui->resetCameraPushButton, SIGNAL(clicked()), ui->glView, SLOT(resetCamera()));
    connect(ui->resetCameraPushButton, SIGNAL(clicked()), ui->glView, SLOT(update()));
    connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(onCloseFile()));
    connect(ui->actionCloseAll, SIGNAL(triggered()), this, SLOT(onCloseAll()));
}

void CMainWindow::onFileAdded(QString file)
{
    QListWidgetItem* item = new QListWidgetItem(QFileInfo(file).fileName());
    item->setData(Qt::UserRole, file);
    ui->listWidget->addItem(item);
    if (ui->listWidget->selectedItems().count() == 0 )
        ui->listWidget->setCurrentItem(item);
}

void CMainWindow::onCloseFile()
{
    foreach (QListWidgetItem* item, ui->listWidget->selectedItems())
    {
        ui->glView->closeFile(item->data(Qt::UserRole).toString());
        delete item;
        item = NULL;
    }
}

void CMainWindow::onCloseAll()
{
    QList<QListWidgetItem*> items = ui->listWidget->findItems("*", Qt::MatchWrap | Qt::MatchWildcard);
    foreach (QListWidgetItem* item, items)
    {
        ui->glView->closeFile(item->data(Qt::UserRole).toString());
        delete item;
        item = NULL;
    }
}

void CMainWindow::onExport()
{
    ui->glView->stopUpdates();
    foreach (QListWidgetItem* item, ui->listWidget->selectedItems())
    {
        ui->glView->exportFile(item->data(Qt::UserRole).toString());
    }
    ui->glView->startUpdates();
}
