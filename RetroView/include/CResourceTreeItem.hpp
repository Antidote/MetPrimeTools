#ifndef CRESOURCETREEITEM_HPP
#define CRESOURCETREEITEM_HPP

#include <QList>
#include <QVariant>

class CResourceTreeItem
{
public:
    CResourceTreeItem(const QList<QVariant>& data, CResourceTreeItem* parent = 0);
    ~CResourceTreeItem();

    void appendChild(CResourceTreeItem* child);
    CResourceTreeItem* child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    CResourceTreeItem* parent();

private:
    QList<CResourceTreeItem*> m_childItems;
    QList<QVariant>           m_itemData;
    CResourceTreeItem*        m_parentItem;
};

#endif // CPAKTREEITEM_HPP
