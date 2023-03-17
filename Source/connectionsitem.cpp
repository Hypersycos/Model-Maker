#include "connectionsitem.h"

ConnectionsItem::ConnectionsItem(const QString propertyName, ProjectTreeItem *item, QString type, ConnectionsItem *parentItem)
{
    this->type = type;
    property = propertyName;
    reference = item;
    this->parentItem = parentItem;
}

ConnectionsItem::~ConnectionsItem()
{
    qDeleteAll(children);
}

void ConnectionsItem::addChild(ConnectionsItem *item)
{
    children.append(item);
    item->parentItem = this;
}

void ConnectionsItem::removeChild(ConnectionsItem *item)
{
    children.removeOne(item);
}

void ConnectionsItem::removeChild(QString property)
{
    for (int i = 0; i<children.length(); i++)
    {
        if (child(i)->property == property)
        {
            removeChild(child(i));
            return;
        }
    }
}

ConnectionsItem *ConnectionsItem::child(int row)
{
    return children.value(row);
}

ConnectionsItem *ConnectionsItem::child(QString property)
{
    for (int i = 0; i<children.length(); i++)
    {
        if (children.at(i)->property == property)
            return children.at(i);
    }
    return nullptr;
}

int ConnectionsItem::childCount() const
{
    return children.length();
}

int ConnectionsItem::row() const
{
    if (parentItem) //if not root item
        return parentItem->children.indexOf(const_cast<ConnectionsItem *>(this));
    else
        return 0;
}


QVariant ConnectionsItem::data(int column, int role) const
{ //UserRole + 1 used for storing property type, e.g. float
    if (role == Qt::UserRole + 1)
        return type;

    if (property == "")
    { //if this item is a wrapper for an Item
        if (column == 0) //data is that Item's name
            return reference->reference->getProperty("Name");
        else
            return "";
    }
    else if (column == 0)
        return property;
    else
        return reference->reference->getProperty(property);
}

ConnectionsItem *ConnectionsItem::parent()
{
    return parentItem;
}
