#include "ui/CMaterialViewer.hpp"
#include "ui_CMaterialViewer.h"
#include "core/IRenderableModel.hpp"
#include "models/CModelFile.hpp"
#include "models/CAreaFile.hpp"

CMaterialViewer::CMaterialViewer(CModelFile* model, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CMaterialViewer),
    m_renderable(model)
{
    ui->setupUi(this);
}

CMaterialViewer::CMaterialViewer(CAreaFile* model, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CMaterialViewer),
    m_renderable(model)
{
    ui->setupUi(this);
}

CMaterialViewer::~CMaterialViewer()
{
    delete ui;
}

void CMaterialViewer::changeEvent(QEvent *e)
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
