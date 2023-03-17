#include "projecttreemodel.h"
#include <QStandardItemModel>
#include <QMimeData>
#include <QTextStream>
#include "model.h"
#include "world.h"
#include "graph.h"


ProjectTreeModel::ProjectTreeModel() : QAbstractItemModel()
{
    rootItem = new ProjectTreeItem(new Item());
}

ProjectTreeModel::~ProjectTreeModel()
{
    delete rootItem;
}

QModelIndex ProjectTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    ProjectTreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<ProjectTreeItem *>(parent.internalPointer());

    ProjectTreeItem *childItem = parentItem->child(row);
    return createIndex(row, column, childItem);
}

QModelIndex ProjectTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    ProjectTreeItem *childItem = static_cast<ProjectTreeItem *>(index.internalPointer());
    ProjectTreeItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int ProjectTreeModel::rowCount(const QModelIndex &parent) const
{
    ProjectTreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<ProjectTreeItem *>(parent.internalPointer());

    return parentItem->childCount();
}

int ProjectTreeModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

bool ProjectTreeModel::moveRow(const QModelIndex &sourceParent, int sourceRow, const QModelIndex &destinationParent, int destinationChild)
{
    if (sourceRow == -1)
        sourceRow = 0;

    if (destinationChild == -1)
        destinationChild = 0;

    //if trying to move an item to the same place, or just below itself.
    if ((sourceRow == destinationChild || sourceRow == destinationChild - 1) && sourceParent == destinationParent)
        return false;

    beginMoveRows(sourceParent, sourceRow, sourceRow, destinationParent, destinationChild);

    ProjectTreeItem *source;
    if (!sourceParent.isValid())
        source = rootItem;
    else
        source = static_cast<ProjectTreeItem *>(sourceParent.internalPointer());

    ProjectTreeItem *destination;
    if (!destinationParent.isValid())
        destination = rootItem;
    else
        destination = static_cast<ProjectTreeItem *>(destinationParent.internalPointer());
    ProjectTreeItem *item = source->child(sourceRow);

    source->removeChild(sourceRow); //this way around, otherwise data is duplicated when dragging items up
    destination->insertChild(item, destinationChild);

    endMoveRows();
    return true;
}

QMimeData *ProjectTreeModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;
    mimeData->setData("application/projecttreeitem", encodedData);

    //we don't allow copying outside of the model, so mimeData will only be used for dragging
    //thus this workaround is valid.

    QModelIndex index = indexes.at(0);
    const ProjectTreeModel *temp = static_cast<const ProjectTreeModel *>(index.model());
    //have to get rid of const to set non-const variable.
    ProjectTreeModel *model = const_cast<ProjectTreeModel *>(temp);
    model->dragging = index;
    return mimeData;
}

QStringList ProjectTreeModel::mimeTypes() const
{
    QStringList toReturn;
    toReturn.append("application/projecttreeitem");
    return toReturn;
}

bool ProjectTreeModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
    if (action & supportedDropActions())
    {//bitwise and. Effectively "if contains"
        if (data->hasFormat("application/projecttreeitem"))
        {
            ProjectTreeItem *dragged = static_cast<ProjectTreeItem *>(dragging.internalPointer());
            QString draggedType = dragged->getType();

            if (!parent.isValid())
            {
                if (rootTypes().contains(draggedType))
                    return true;
                else
                    return false;
            }

            ProjectTreeItem *recipient = static_cast<ProjectTreeItem *>(parent.internalPointer());
            if (recipient->getValidChildren().contains(draggedType))
                return true;
        }
    }
    return false;
}

QStringList ProjectTreeModel::rootTypes() const
{
    return QStringList({"World", "Display"});
}

bool ProjectTreeModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if (!canDropMimeData(data, action, row, column, parent))
        return false;

    if (action == Qt::IgnoreAction)
        return true;

    moveRow(dragging.parent(), dragging.row(), parent, row);
    return true;
}

QVariant ProjectTreeModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid())
    {
        ProjectTreeItem *item = static_cast<ProjectTreeItem *>(index.internalPointer());
        if (role == Qt::DisplayRole)
            return item->data();
        else if (role == Qt::DecorationRole)
            return item->icon();
        else if (role == Qt::UserRole + 1)
            return item->getType();
        //UserRole + 1 used for filtering in selectors
    }
    return QVariant();
}

Qt::ItemFlags ProjectTreeModel::flags(const QModelIndex &index) const
{
    //root item can only be dropped on
    if (!index.isValid())
        return Qt::ItemIsDropEnabled;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

bool ProjectTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    ProjectTreeItem *item = static_cast<ProjectTreeItem *>(index.internalPointer());

    dataChanged(index, index, QVector<int>(role));
    return item->setData(value);
}

QVariant ProjectTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{ //ProjectTree has no headers.
    return QVariant();
}

void ProjectTreeModel::addItem(ProjectTreeItem *item, ProjectTreeItem *parent)
{ //only for use when initally creating the model
    if (parent == nullptr)
        parent = rootItem;

    //update viewHolder
    if (item->getType() == "Display")
        emit displayAdded(static_cast<Display *>(item->reference));
    parent->appendChild(item);
}

void ProjectTreeModel::addItem(ProjectTreeItem *item, const QModelIndex &parent)
{
    ProjectTreeItem *parentItem;

    if (parent.isValid())
        parentItem = static_cast<ProjectTreeItem *>(parent.internalPointer());
    else
        parentItem = rootItem;

    if (item->getType() == "Display")
        emit displayAdded(static_cast<Display *>(item->reference));

    beginInsertRows(parent, parentItem->childCount(), parentItem->childCount());
    parentItem->appendChild(item);
    endInsertRows();
}

void ProjectTreeModel::removeItem(const QModelIndex &item, const QModelIndex &parent)
{
    if (!item.isValid())
        return;

    ProjectTreeItem *parentItem;

    if (parent.isValid())
        parentItem = static_cast<ProjectTreeItem *>(parent.internalPointer());
    else
        parentItem = rootItem;

    ProjectTreeItem *treeItem = static_cast<ProjectTreeItem *>(item.internalPointer());
    if (treeItem->getType() == "Display")
        emit displayRemoved(static_cast<Display *>(treeItem->reference));

    beginRemoveRows(parent, item.row(), item.row());
    parentItem->removeChild(item.row());
    endRemoveRows();
    delete treeItem;
}


Qt::DropActions ProjectTreeModel::supportedDropActions() const
{
    return Qt::MoveAction | Qt::CopyAction;
}

QString ProjectTreeModel::load(QString filePath)
{ //return error if we fail at any point
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return file.errorString();

    QTextStream in(&file);

    while(!in.atEnd())
    {
        QString temp = in.readLine();
        if (temp.contains("{"))
        {
            std::pair<bool, Item *> pair;
            if (temp == "Display{")
                pair = Display::load(in);
            else if (temp == "World{")
                pair = World::load(in);
            else
                pair = std::pair<bool, Item *>();

            if (!pair.first)
                return "Error parsing file. Make sure you opened a .proj";

            //creates TreeItems for all new Items
            ProjectTreeItem *treeItem = new ProjectTreeItem(pair.second, true);
            addItem(treeItem, nullptr);
        }
        else
        {
            return "Error parsing file. Make sure you opened a .proj";
        }
    }

    for (int i = 0; i < rowCount(); i++)
    {//gives Displays and ScreenItems pointers to their objects from their indexes
        ProjectTreeItem *item = rootItem->child(i);
        if (item->getType() == "Display")
        {
            Display *display = static_cast<Display *>(item->reference);
            int index = display->tempLoad;
            QVariant variant = QVariant::fromValue(this->index(index, 0));
            display->setProperty("World", variant);
            for (int j = 0; j < item->childCount(); j++)
            {
                ScreenItem *screenItem = static_cast<ScreenItem *>(item->child(j)->reference);
                std::pair<ProjectTreeItem *, QString> tempLoad = screenItemLoad(screenItem->tempLoad);
                screenItem->connected = tempLoad.first;
                screenItem->property = tempLoad.second;

                if (screenItem->getProperty("Type") == "Graph")
                {
                    Graph *graph = static_cast<Graph *>(screenItem);
                    std::pair<ProjectTreeItem *, QString> tempLoad2 = screenItemLoad(graph->tempLoad2);
                    graph->connected2 = tempLoad2.first;
                    graph->property2 = tempLoad2.second;
                }
            }
        }
    }
    return "";
}

std::pair<ProjectTreeItem *, QString> ProjectTreeModel::screenItemLoad(QString toLoad)
{
    ProjectTreeItem *currentItem = rootItem;
    while (toLoad.contains(";"))
    {
        int index = toLoad.left(toLoad.indexOf(";")).toInt();
        toLoad = toLoad.mid(toLoad.indexOf(";")+1);
        currentItem = currentItem->child(index);
    }
    return std::pair<ProjectTreeItem *, QString>(currentItem, toLoad);
}

void ProjectTreeModel::changeRoot(ProjectTreeItem *newRoot)
{
    beginResetModel();
    rootItem = newRoot;
    endResetModel();
}

void ProjectTreeModel::reset()
{
    beginResetModel();
    rootItem = new ProjectTreeItem(new Item());
    endResetModel();
}
