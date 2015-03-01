#include "CPakTreeWidget.hpp"
#include "ui_CPakTreeWidget.h"
#include "CPakFile.hpp"
#include "CPakFileModel.hpp"
#include "CResourceManager.hpp"
#include "IRenderableModel.hpp"
#include "CGLViewer.hpp"
#include <iostream>

CPakTreeWidget::CPakTreeWidget(CPakFile* pak, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CPakTreeWidget),
    m_model(new CPakFileModel(pak))
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
        IRenderableModel* renderable = dynamic_cast<IRenderableModel*>(CResourceManager::instance()->loadResourceFromPak(m_model->pak(), item->assetID()));
        if (renderable)
            CGLViewer::instance()->setCurrent(renderable);
    }
}

void CPakTreeWidget::onSelectionChanged(QItemSelection, QItemSelection)
{
    QModelIndex idx = ui->treeView->selectionModel()->selectedIndexes().first();

    onItemClicked(idx);
}