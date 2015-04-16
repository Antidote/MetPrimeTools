#include "core/CTemplateManager.hpp"
#include "core/CResourceManager.hpp"
#include "core/IRenderableModel.hpp"

#include "generic/CWorldFile.hpp"

#include "ui_CMainWindow.h"
#include "ui/CMainWindow.hpp"
#include "ui/CGLViewer.hpp"
#include "ui/CPakTreeWidget.hpp"

#include <QFileDialog>
#include <QDir>
#include <QDirIterator>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMessageBox>
#include <QMimeData>
#include <QTextStream>
#include <QThread>
#include <QSettings>
#include <QShowEvent>
#include <QStandardPaths>

#include <iostream>

QString getSource(QString Filename);

CMainWindow::CMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CMainWindow),
    m_currentTab(nullptr)
{
    ui->setupUi(this);
    ui->tabWidget->setUsesScrollButtons(true);
    ui->tabWidget->setElideMode(Qt::ElideNone);
    ui->tabWidget->setTabsClosable(true);
    ui->tabWidget->setMovable(true);
    connect(ui->glView, SIGNAL(initialized()), this, SLOT(onViewerInitialized()));
    CResourceManager* resourceManager = CResourceManager::instance().get();
    connect(resourceManager, SIGNAL(newPak(CPakTreeWidget*)), this, SLOT(onNewPak(CPakTreeWidget*)));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(onTabChanged()));
    connect(ui->actionLoad_Basepath, &QAction::triggered, this, &CMainWindow::_loadBasePath);
    connect(ui->glView, &CGLViewer::movementSpeedChanged, [=](float val){
        ui->horizontalSlider->setUpdatesEnabled(false);
        ui->horizontalSlider->setValue((int)((val * 100) / 1.0));
        ui->horizontalSlider->setUpdatesEnabled(true);
    });

    connect(ui->horizontalSlider, &QSlider::valueChanged, [=](int val){
        if (!ui->horizontalSlider->updatesEnabled())
            return;
        ui->horizontalSlider->setUpdatesEnabled(false);
        ui->glView->setMovementSpeed((float)(val / 100.f));
        ui->horizontalSlider->setValue((int)((ui->glView->movementSpeed() * 100) / 1.0));
        ui->horizontalSlider->setUpdatesEnabled(true);
    });

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

    m_fpsUpdateTimer.setInterval(50);
    QMenuBar* bar = this->menuBar();
    QHBoxLayout* previewLayout = new QHBoxLayout(bar);
    previewLayout->addStretch();
    QLabel* previewLabel  = new QLabel(bar);
    previewLabel->setObjectName("previewLabel");
    previewLabel->setText("<b>UNSTABLE BUILD</b>");
    previewLayout->setContentsMargins(150, 0, 6, 0);
    previewLayout->addWidget(previewLabel);
    bar->setLayout(previewLayout);
    statusBar()->addPermanentWidget(&m_cameraPosition);
}

CMainWindow::~CMainWindow()
{
    delete ui;
}

bool CMainWindow::canShow()
{
    return ui != nullptr;
}



void CMainWindow::initialize()
{
    QSettings().setValue("applicationRootPath", qApp->applicationDirPath());
    QSettings().setValue("applicationFilename", QFileInfo(qApp->applicationFilePath()).fileName());

    QFileInfo fi(qApp->applicationDirPath() + "/templates");
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

    QStringList arguments = qApp->arguments();
    if (!arguments.empty())
        arguments.removeFirst();

    foreach (QString arg, arguments)
    {
        QFileInfo info(arg);
        std::cout << arg.toStdString() << std::endl;
        if (info.isDir())
            CResourceManager::instance()->loadBasepath(arg.toStdString());
        else
            CResourceManager::instance()->loadPak(arg.toStdString());
    }

    if (!CResourceManager::instance()->hasPaks())
        _loadBasePath();
}

bool CMainWindow::event(QEvent* e)
{
    switch(e->type())
    {

        case QEvent::WindowActivate:
            //ui->glView->startUpdates();
            break;

        case QEvent::WindowDeactivate:
           // ui->glView->stopUpdates();
            break;
        default:
            break;
    }
    return QMainWindow::event(e);
}

void CMainWindow::dragEnterEvent(QDragEnterEvent* e)
{
    foreach (QUrl url, e->mimeData()->urls())
    {
        if (CResourceManager::canLoad(url.toLocalFile().toStdString()) || QFileInfo(url.toLocalFile()).isDir())
            e->acceptProposedAction();
    }
}

void CMainWindow::dropEvent(QDropEvent* e)
{
    foreach (QUrl url, e->mimeData()->urls())
    {
        QFileInfo info(url.toLocalFile());
        if (info.isDir())
            CResourceManager::instance()->loadBasepath(info.absoluteFilePath().toStdString());
        else if (info.isFile())
            CResourceManager::instance()->loadPak(info.absoluteFilePath().toStdString());
    }
}

void CMainWindow::moveEvent(QMoveEvent* e)
{
    this->setFocus();
    QMainWindow::moveEvent(e);
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
    QFileInfo basePath(pak->filepath());
    QTabWidget* widget = nullptr;
    QString tmp = basePath.absolutePath();
#ifdef Q_OS_WIN
    tmp = tmp.toLower();
#endif

    if (m_basePaths.contains(tmp))
        widget = m_basePaths[tmp];
    else
    {
        widget = new QTabWidget(ui->tabWidget);
        m_basePaths[tmp] = widget;
        widget->setObjectName(tmp.toLower());
        widget->setUsesScrollButtons(true);
        widget->setElideMode(Qt::ElideNone);
        widget->setTabsClosable(true);
        widget->setMovable(true);
        connect(widget, SIGNAL(currentChanged(int)), this, SLOT(onTabChanged()));
        connect(widget, SIGNAL(tabCloseRequested(int)), this, SLOT(onTabClosed(int)));
        int tabIdx = ui->tabWidget->addTab(widget, QFileInfo(basePath.absolutePath()).fileName());
        ui->tabWidget->setTabToolTip(tabIdx, basePath.absolutePath());
    }

    connect(pak, SIGNAL(resourceChanged(IResource*)), this, SLOT(onResourceChanged(IResource*)));
    QString tabTitle = QFileInfo(pak->filepath()).fileName();
    int tabIdx = widget->addTab(pak, tabTitle);
    widget->setTabToolTip(tabIdx, pak->filepath());
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


    CPakTreeWidget* ptw = qobject_cast<CPakTreeWidget*>(ui->tabWidget->currentWidget());
    if (ptw && ptw->pak()->isWorldPak())
    {
        m_currentTab = ptw;
        std::vector<SPakResource> res = m_currentTab->pak()->resourcesByType("mlvl");
        CWorldFile* world = nullptr;
        if (res.size() > 0)
            world = dynamic_cast<CWorldFile*>(CResourceManager::instance()->loadResourceFromPak(ptw->pak(), res.at(0).id, "MLVL"));
        if (world)
        {
            CGLViewer::instance()->setSkybox(world->skyboxModel());
            world->destroy();
        }
    }
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
    CVector3f pos = CGLViewer::instance()->cameraPosition();
    ui->fpsLabel->setText(QString("FPS: %1").arg(CGLViewer::instance()->frameRate()));
    m_cameraPosition.setText(QString("X: %1 Y: %2 Z: %3").arg(pos.x).arg(pos.y).arg(pos.z));
}

void CMainWindow::onTabClosed(int tabIdx)
{
    QTabWidget* tabWidget = qobject_cast<QTabWidget*>(sender());

    QWidget* widget = tabWidget->widget(tabIdx);
    QTabWidget* tw = qobject_cast<QTabWidget*>(widget);
    if (tw)
    {
        while (tw->count() > 0)
        {
            CPakTreeWidget* ptw = qobject_cast<CPakTreeWidget*>(tw->widget(tw->count() - 1));
            if (ptw)
            {
                CResourceManager::instance()->removePak(ptw->pak());
                delete ptw;
            }
        }
        if (m_basePaths.values().contains(tw))
            m_basePaths.remove(m_basePaths.key(tw));

        delete tw;
    }
    else
    {
        CPakTreeWidget* ptw = qobject_cast<CPakTreeWidget*>(widget);
        if (ptw)
        {
            CResourceManager::instance()->removePak(ptw->pak());
            delete ptw;
        }

        if (tabWidget->count() == 0)
        {
            if (m_basePaths.values().contains(tabWidget))
                m_basePaths.remove(m_basePaths.key(tabWidget));
            delete tabWidget;
        }
    }
}

void CMainWindow::_loadBasePath()
{
    CGLViewer::instance()->stopUpdates();
    QString basePath = QFileDialog::getExistingDirectory(this, "Specify Basepath");
    CGLViewer::instance()->startUpdates();

    if (!basePath.isEmpty())
        CResourceManager::instance()->loadBasepath(basePath.toStdString());
}

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
