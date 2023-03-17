#ifndef PROJECTTREE_H
#define PROJECTTREE_H

#include <QWidget>
#include <QFileIconProvider>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include "subwindow.h"
#include "projecttreemodel.h"
#include <QTreeView>

class MainWindow;
class TreeItem;

class ProjectTree : public SubWindow
{
    Q_OBJECT
public:
    ProjectTree(MainWindow *parent, ProjectTreeModel *itemmodel, QItemSelectionModel *selectionmodel, QAction *toggle);
    void copy();
    void paste();
    void remove();
    void insert();

private:
    ProjectTreeModel *itemModel;
    QTreeView *treeView;
    Item *loadCorrectItem(QByteArray data, Item *parent);
    void contextMenu(QPoint pos);

};

#endif // PROJECTTREE_H
