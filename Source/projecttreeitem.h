#ifndef PROJECTTREEITEM_H
#define PROJECTTREEITEM_H

#include "item.h"
#include <QObject>

class ProjectTreeItem : public QObject
{
    Q_OBJECT
public:
    ProjectTreeItem(Item *item, ProjectTreeItem *parent = nullptr);
    ProjectTreeItem(Item *item, bool loading, ProjectTreeItem *parent = nullptr);
    ~ProjectTreeItem();

    void appendChild(ProjectTreeItem *child);
    void insertChild(ProjectTreeItem *child, int row);
    void removeChild(int row);

    QString getType();
    QStringList getValidChildren();

    QVariant data();
    ProjectTreeItem *child(int row);
    int childCount();
    int row();
    QIcon icon();
    ProjectTreeItem *parent();
    bool setData(QVariant data);

    QList<int> getIndex();

    Item *reference;

private:
    ProjectTreeItem *parentItem;
    QList<ProjectTreeItem *> children;
};

#endif // PROJECTTREEITEM_H
