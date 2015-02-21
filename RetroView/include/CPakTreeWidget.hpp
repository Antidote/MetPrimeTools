#ifndef CPAKTREEWIDGET_HPP
#define CPAKTREEWIDGET_HPP

#include <QWidget>

namespace Ui {
class CPakTreeWidget;
}

class CPakFile;
class CPakFileModel;
class CPakTreeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CPakTreeWidget(CPakFile* pak, QWidget *parent = 0);
    ~CPakTreeWidget();

    QString filepath() const;
protected:
    void changeEvent(QEvent *e);

private:
    Ui::CPakTreeWidget *ui;
    CPakFileModel* m_model;
};

#endif // CPAKTREEWIDGET_HPP
