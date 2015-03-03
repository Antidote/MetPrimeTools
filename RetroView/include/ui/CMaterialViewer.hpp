#ifndef CMATERIALVIEWER_HPP
#define CMATERIALVIEWER_HPP

#include <QWidget>

namespace Ui {
class CMaterialViewer;
}

class CModelFile;
class CAreaFile;
class IRenderableModel;
class CMaterialViewer : public QWidget
{
    Q_OBJECT

public:
    explicit CMaterialViewer(CModelFile* model, QWidget *parent = 0);
    explicit CMaterialViewer(CAreaFile* area, QWidget *parent = 0);
    virtual ~CMaterialViewer();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::CMaterialViewer *ui;
    IRenderableModel* m_renderable;
};

#endif // CMATERIALVIEWER_HPP
