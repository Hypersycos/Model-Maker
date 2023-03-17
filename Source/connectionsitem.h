#ifndef CONNECTIONSITEM_H
#define CONNECTIONSITEM_H

#include <QString>
#include "projecttreeitem.h"

class ConnectionsItem
{
public:
    ConnectionsItem(const QString propertyName, ProjectTreeItem *item, QString type, ConnectionsItem *parentItem = nullptr);
    ~ConnectionsItem();

    void addChild(ConnectionsItem *child);
    void removeChild(ConnectionsItem *child); //used when resetting
    void removeChild(QString property); //used by Properties to remove inputs/outputs

    ConnectionsItem *child(int row);
    ConnectionsItem * child(QString property);
    int childCount() const;
    QVariant data(int column, int role = Qt::DisplayRole) const;
    int row() const;
    ConnectionsItem *parent();

    //ProjectTreeItem stored rather than Item, so the Item using the connection can get the index.
    ProjectTreeItem *reference;
    QString property;

private:
    QString type;
    ConnectionsItem *parentItem;
    QList<ConnectionsItem *> children;
};

#endif // CONNECTIONSITEM_H
