#include "CPakTreeWidget.hpp"
#include "ui_CPakTreeWidget.h"
#include "CPakFile.hpp"
#include "CPakFileModel.hpp"

CPakTreeWidget::CPakTreeWidget(CPakFile* pak, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CPakTreeWidget),
    m_model(new CPakFileModel(pak))
{
    ui->setupUi(this);
    ui->treeView->setModel(m_model);
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
