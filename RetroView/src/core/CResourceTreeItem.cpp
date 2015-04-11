#include "core/CResourceTreeItem.hpp"

CResourceTreeItem::CResourceTreeItem(const QList<QVariant> &data, const CUniqueID& assetID, CResourceTreeItem *parent)
    : m_itemData(data),
      m_parentItem(parent),
      m_assetID(assetID)
{
}

CResourceTreeItem::~CResourceTreeItem()
{
    qDeleteAll(m_childItems);
}

void CResourceTreeItem::appendChild(CResourceTreeItem* child)
{
    m_childItems.append(child);
}

CResourceTreeItem* CResourceTreeItem::child(int row)
{
    return m_childItems.value(row);
}

int CResourceTreeItem::childCount() const
{
    return m_childItems.count();
}

int CResourceTreeItem::columnCount() const
{
    return m_itemData.count();
}

QVariant CResourceTreeItem::data(int column) const
{
    return m_itemData.value(column);
}

int CResourceTreeItem::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<CResourceTreeItem*>(this));

    return 0;
}

CResourceTreeItem* CResourceTreeItem::parent()
{
    return m_parentItem;
}

CUniqueID CResourceTreeItem::assetID() const
{
    return m_assetID;
}
