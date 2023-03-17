#include "projecttreeitem.h"
#include <iostream>

ProjectTreeItem::ProjectTreeItem(Item *item, ProjectTreeItem *parent) : QObject()
{
    parentItem = parent;
    reference = item;
}

ProjectTreeItem::ProjectTreeItem(Item *item, bool loading, ProjectTreeItem *parent)
{
    parentItem = parent;
    reference = item;

    if (loading)
    { //recursively create ProjectTreeItems for children when loading
        QList<Item *> itemChildren = reference->getChildren();
        for (int i = 0; i < itemChildren.length(); i++)
        {
            ProjectTreeItem *child = new ProjectTreeItem(itemChildren.at(i), true, this);
            children.append(child);
        }
    }
}

ProjectTreeItem::~ProjectTreeItem()
{
    delete reference;
    qDeleteAll(children);
}

void ProjectTreeItem::appendChild(ProjectTreeItem *child)
{
    reference->addChild(child->reference);
    children.append(child);
    child->parentItem = this;
}

void ProjectTreeItem::insertChild(ProjectTreeItem *child, int row)
{
    reference->addChild(child->reference, row);
    children.insert(row, child);
    child->parentItem = this;
}

void ProjectTreeItem::removeChild(int row)
{
    reference->removeChild(children.at(row)->reference);
    children.removeAt(row);
}

ProjectTreeItem * ProjectTreeItem::child(int row)
{
    return children.at(row);
}

int ProjectTreeItem::childCount()
{
    return children.count();
}

int ProjectTreeItem::row()
{
    if (parentItem)
    {
        return parentItem->children.indexOf(this);
    }

    return 0;
}

ProjectTreeItem * ProjectTreeItem::parent()
{
    return parentItem;
}

QStringList ProjectTreeItem::getValidChildren()
{
    return reference->getValidChildren();
}

QIcon ProjectTreeItem::icon()
{
    return reference->icon;
}

QVariant ProjectTreeItem::data()
{
    QString name = reference->getProperty("Name");
    QVariant toReturn;
    toReturn.setValue<QString>(name);
    return toReturn;
}

bool ProjectTreeItem::setData(QVariant data)
{
    reference->setProperty("Name", data);
    return true;
}

QString ProjectTreeItem::getType()
{
    return reference->getProperty("Type");
}

QList<int> ProjectTreeItem::getIndex()
{
    //get row number until at root item.
    if (parentItem == nullptr)
        return QList<int>();
    QList<int> rows = parentItem->getIndex();
    rows.append(row());
    return rows;
}
