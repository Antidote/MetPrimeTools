#include "ui/CPakTreeWidget.hpp"
#include "ui_CPakTreeWidget.h"
#include "core/CPakFileModel.hpp"
#include "core/CResourceManager.hpp"
#include "core/IRenderableModel.hpp"
#include "ui/CGLViewer.hpp"

#include <CPakFile.hpp>
#include <iostream>

CPakTreeWidget::CPakTreeWidget(CPakFile* pak, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CPakTreeWidget),
    m_model(new CPakFileModel(pak)),
    m_currentResource(nullptr)
{
    ui->setupUi(this);
    ui->treeView->setModel(m_model);
    QItemSelectionModel* selectionModel = ui->treeView->selectionModel();
    connect(selectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onSelectionChanged(QItemSelection,QItemSelection)));
}

CPakTreeWidget::~CPakTreeWidget()
{
    delete ui;
}

QString CPakTreeWidget::filepath() const
{
    return m_model->filepath();
}

CPakFile* CPakTreeWidget::pak() const
{
    return m_model->pak();
}

void CPakTreeWidget::clearCurrent()
{
//    if (m_currentResource)
//        m_currentResource->destroy();
//    m_currentResource = nullptr;
}

void CPakTreeWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

void CPakTreeWidget::onItemClicked(QModelIndex idx)
{
    CResourceTreeItem* item = static_cast<CResourceTreeItem*>(idx.internalPointer());
    if (item)
    {
        //CResourceManager::instance()->clear();
        m_currentResource = CResourceManager::instance()->loadResourceFromPak(m_model->pak(), item->assetID());
        emit resourceChanged(m_currentResource);
    }
}

void CPakTreeWidget::onSelectionChanged(QItemSelection, QItemSelection)
{
    QModelIndex idx = ui->treeView->selectionModel()->selectedIndexes().first();

    onItemClicked(idx);
}
