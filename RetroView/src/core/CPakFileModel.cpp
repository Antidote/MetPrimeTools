#include "core/CPakFileModel.hpp"
#include "core/CResourceManager.hpp"
#include "generic/CWorldFile.hpp"

#include <CUniqueID.hpp>

CPakFileModel::CPakFileModel(CPakFile* pak, QObject* parent)
    : QAbstractItemModel(parent),
      m_pakFile(pak)
{
    QList<QVariant> rootData;
    rootData << "Contents";

    m_rootItem = new CResourceTreeItem(rootData, CUniqueID(nullptr, CUniqueID::E_Invalid));

    setupModelData();
}

CPakFileModel::~CPakFileModel()
{
    delete m_rootItem;
}

QVariant CPakFileModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    CResourceTreeItem* item = static_cast<CResourceTreeItem*>(index.internalPointer());

    return item->data(index.column());
}

Qt::ItemFlags CPakFileModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant CPakFileModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return m_rootItem->data(section);

    return QVariant();
}

QModelIndex CPakFileModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    CResourceTreeItem* parentItem;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<CResourceTreeItem*>(parent.internalPointer());

    CResourceTreeItem* childItem = parentItem->child(row);

    if (childItem)
        return createIndex(row, column, childItem);

    return QModelIndex();
}

QModelIndex CPakFileModel::parent(const QModelIndex& child) const
{
    if (!child.isValid())
        return QModelIndex();

    CResourceTreeItem* childItem = static_cast<CResourceTreeItem*>(child.internalPointer());
    CResourceTreeItem* parentItem = childItem->parent();

    if (parentItem == m_rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int CPakFileModel::rowCount(const QModelIndex& parent) const
{
    CResourceTreeItem* parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<CResourceTreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int CPakFileModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return static_cast<CResourceTreeItem*>(parent.internalPointer())->columnCount();

    return m_rootItem->columnCount();
}

QString CPakFileModel::filepath() const
{
    return QString::fromStdString(m_pakFile->filename());
}

CPakFile* CPakFileModel::pak()
{
    return m_pakFile;
}

void CPakFileModel::setupModelData()
{
    QMap<QString, CResourceTreeItem*> parents;

    std::vector<CWorldFile*> worlds;
    if (m_pakFile->isWorldPak())
    {
        std::vector<SPakResource> mlvls = m_pakFile->resourcesByType("mlvl");
        for (SPakResource res : mlvls)
        {
            CWorldFile* world = dynamic_cast<CWorldFile*>(CResourceManager::instance()->loadResource(res.id, "mlvl"));
            if (world)
                worlds.push_back(world);
        }
    }

    for (const SPakResource& res : m_pakFile->resources())
    {
        QList<QVariant> tmpData;
        QString areaName;
        if (res.tag == "MREA")
        {
            for (CWorldFile* world : worlds)
            {
                areaName = QString::fromStdString(world->areaName(res.id, m_pakFile));
                if (!areaName.isEmpty())
                    break;
            }
        }

        tmpData << (areaName.isEmpty() ? "" : areaName + " (" ) + QString::fromStdString(res.id.toString()) + (areaName.isEmpty() ? "" : ")");

        QString parentNodeTag = QString::fromStdString(res.tag.toString());
        if (parents[parentNodeTag] == nullptr)
        {
            QList<QVariant> nodeTag;
            nodeTag << parentNodeTag;
            parents[parentNodeTag] = new CResourceTreeItem(nodeTag, CUniqueID(), m_rootItem);
            m_rootItem->appendChild(parents[parentNodeTag]);
        }

        parents[parentNodeTag]->appendChild(new CResourceTreeItem(tmpData, res.id, parents[parentNodeTag]));
    }

    for (CWorldFile* world : worlds)
        world->destroy();
    worlds.clear();
}
