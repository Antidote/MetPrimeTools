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
    connect(ui->treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(onItemClicked(QModelIndex)));
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
    bool ok = false;
    QString text = m_model->data(idx, Qt::DisplayRole).toString();
    std::cout << text.toStdString() << std::endl;
    text = "0x" + text.toUpper();
    quint64 id = text.toULongLong(&ok, 16);

    if (ok)
    {
        std::cout << std::hex << id << std::dec << std::endl;
        IRenderableModel* renderable = dynamic_cast<IRenderableModel*>(CResourceManager::instance()->loadResource(id));
        if (renderable)
            CGLViewer::instance()->setCurrent(renderable);
    }
}
