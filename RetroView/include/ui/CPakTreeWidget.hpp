#ifndef CPAKTREEWIDGET_HPP
#define CPAKTREEWIDGET_HPP

#include <QWidget>
#include <QModelIndex>
#include <QItemSelection>

namespace Ui {
class CPakTreeWidget;
}

class CPakFile;
class CPakFileModel;
class IResource;
class CPakTreeWidget final : public QWidget
{
    Q_OBJECT

public:
    explicit CPakTreeWidget(CPakFile* pak, QWidget *parent = 0);
    virtual ~CPakTreeWidget();

    QString filepath() const;
    CPakFile* pak() const;
    void clearCurrent();
signals:
    void resourceChanged(IResource*);
protected:
    void changeEvent(QEvent *e);

private slots:
    void onItemClicked(QModelIndex idx);
    void onSelectionChanged(QItemSelection, QItemSelection);
private:
    Ui::CPakTreeWidget *ui;
    CPakFileModel* m_model;
    IResource* m_currentResource;
};

#endif // CPAKTREEWIDGET_HPP
