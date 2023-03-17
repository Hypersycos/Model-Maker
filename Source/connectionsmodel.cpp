#include "connectionsmodel.h"

ConnectionsModel::ConnectionsModel(QString firstHeader)
{
    rootItem = new ConnectionsItem("", nullptr, "");
    header = firstHeader;
}

ConnectionsModel::~ConnectionsModel()
{
    delete rootItem;
}

QModelIndex ConnectionsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)) //if this index doesn't exist in the model
        return QModelIndex();

    ConnectionsItem *parentItem;

    if (parent.isValid()) //rootItem's index is invalid
        parentItem = static_cast<ConnectionsItem *>(parent.internalPointer());
    else
        parentItem = rootItem;

    ConnectionsItem *child = parentItem->child(row);
    return createIndex(row, column, child);
}

QModelIndex ConnectionsModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    ConnectionsItem *childItem = static_cast<ConnectionsItem *>(index.internalPointer());
    ConnectionsItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int ConnectionsModel::rowCount(const QModelIndex &parent) const
{
    ConnectionsItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<ConnectionsItem *>(parent.internalPointer());

    return parentItem->childCount();
}

int ConnectionsModel::columnCount(const QModelIndex &parent) const
{ //always 2 columns
    return 2;
}

QVariant ConnectionsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || (role != Qt::DisplayRole && role != Qt::UserRole + 1))
        return QVariant();

    ConnectionsItem *item = static_cast<ConnectionsItem *>(index.internalPointer());

    return item->data(index.column(), role);
}

Qt::ItemFlags ConnectionsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant ConnectionsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Orientation::Horizontal && role == Qt::DisplayRole)
    {
        if (section == 0)
            return header;
        else
            return "Current Values";
    }
    return QVariant();
}

ConnectionsItem *ConnectionsModel::findItem(ProjectTreeItem *item)
{ //basic linear search. Overhead from sorting for binary not worth it for such a small dataset
    for (int i = 0; i<rootItem->childCount(); i++)
    {
        if (item == rootItem->child(i)->reference)
        {
            return rootItem->child(i);
        }
    }
    return nullptr;
}

QModelIndex ConnectionsModel::findIndex(ProjectTreeItem *item)
{
    for (int i = 0; i<rootItem->childCount(); i++)
    {
        if (item == rootItem->child(i)->reference)
        {
            return index(i, 0);
        }
    }

    return QModelIndex();
}

QStringList ConnectionsModel::getItemProperties(ProjectTreeItem *item)
{
    QStringList toReturn;
    ConnectionsItem *holder = findItem(item);

    if (holder == nullptr)
        return toReturn;

    for (int j = 0; j<holder->childCount(); j++)
    { //property name is data(0)
        toReturn.append(holder->child(j)->data(0).toString());
    }

    return toReturn;
}

QModelIndex ConnectionsModel::addItem(ConnectionsItem *item)
{
    if (item->property != "")
    { //if we're not adding a holder
        QModelIndex parent = findIndex(item->reference);
        if (!parent.isValid())
        { //if this Item has no holder
            parent = addItem(new ConnectionsItem("", item->reference, "", rootItem));
        } //make a holder for it and set parent to its index

        ConnectionsItem *parentItem = static_cast<ConnectionsItem *>(parent.internalPointer());
        //inserting at the end of parentItem's children
        beginInsertRows(parent, parentItem->childCount(), parentItem->childCount());
        parentItem->addChild(item);
        endInsertRows();
        return QModelIndex();
    }
    else
    { //if we're adding a holder
        int row = rootItem->childCount();
        beginInsertRows(QModelIndex(), row, row);
        rootItem->addChild(item);
        endInsertRows();
        return index(row, 0);
    }
}

void ConnectionsModel::removeItem(ConnectionsItem *item)
{ //removeItem is never called on holders
    ConnectionsItem *parentItem = item->parent();
    QModelIndex parent = index(parentItem->row(), 0);
    beginRemoveRows(parent, item->row(), item->row());
    parentItem->removeChild(item);
    endRemoveRows();

    if (parentItem->childCount() == 0)
    { //if the last exposed property of an Item was deleted
        beginRemoveRows(QModelIndex(), parentItem->row(), parentItem->row());
        rootItem->removeChild(parentItem);
        endRemoveRows();
    } //delete the holder
}

void ConnectionsModel::reset()
{
    beginResetModel();
    delete rootItem;
    rootItem = new ConnectionsItem("", nullptr, "");
    endResetModel();
}
