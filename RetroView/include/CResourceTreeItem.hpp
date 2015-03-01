#ifndef CRESOURCETREEITEM_HPP
#define CRESOURCETREEITEM_HPP

#include <QList>
#include <QVariant>
#include "CAssetID.hpp"

class CResourceTreeItem
{
public:
    CResourceTreeItem(const QList<QVariant>& data, const CAssetID& assetID, CResourceTreeItem* parent = 0);
    ~CResourceTreeItem();

    void appendChild(CResourceTreeItem* child);
    CResourceTreeItem* child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    CResourceTreeItem* parent();

    CAssetID assetID() const;

private:
    QList<CResourceTreeItem*> m_childItems;
    QList<QVariant>           m_itemData;
    CResourceTreeItem*        m_parentItem;
    CAssetID                  m_assetID;
};

#endif // CPAKTREEITEM_HPP
