#ifndef PROJECTTREEVIEW_H
#define PROJECTTREEVIEW_H

#include <QTreeView>
#include <QStandardItem>

class ProjectTreeView : public QTreeView
{
public:
    ProjectTreeView();

private:
    //bool dropResultsInValidIndex(const QPoint& pos, const QMimeData *data);
    //virtual void dragMoveEvent(QDragMoveEvent *event);
    //virtual bool dropMimeData(QStandardItem *parent, int index, const QMimeData* data, Qt::DropAction action);
};

#endif // PROJECTTREEVIEW_H
