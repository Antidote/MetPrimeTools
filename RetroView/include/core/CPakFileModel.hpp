#ifndef CPAKFILEMODEL_HPP
#define CPAKFILEMODEL_HPP


#include <QAbstractItemModel>
#include "CResourceTreeItem.hpp"
class CPakFile;
class CWorldFile;
class CPakFileModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    CPakFileModel(CPakFile* pak, QObject* parent = 0);
    virtual ~CPakFileModel();

    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;

    QModelIndex parent(const QModelIndex &child) const;

    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;

    QString filepath() const;

    CPakFile* pak();
private:
    void setupModelData();
    CPakFile* m_pakFile;
    CResourceTreeItem* m_rootItem;
};

#endif // CPAKLISTMODEL_HPP
