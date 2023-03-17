#ifndef PROPERTIES_H
#define PROPERTIES_H

#include "subwindow.h"
#include <QItemSelectionModel>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QTreeView>
#include <QStandardItemModel>
#include "projecttreeitem.h"
#include "connections.h"

class Properties : public SubWindow
{

public:
    Properties(MainWindow *parent, QItemSelectionModel *selectionmodel, QAction *toggle, Connections *connector);

private:
    QItemSelectionModel *selectionModel;
    QVBoxLayout *info;
    QWidget *container;
    QScrollArea *scrollArea;
    QTreeView *treeView;
    QStandardItemModel *model;
    ProjectTreeItem *item;
    Connections *connector;

    QStringList propertyOrder;
    QStringList bools;
    QStringList dropdowns;
    QStringList selectors;
    QStringList strings;
    QStringList vectors;
    QStringList noOutput;
    QStringList noInput;
    QStringList IOLessItems;
    QList<QStringList> dropdownValues;

    void updateProperties(QItemSelection selected, QItemSelection deselected);
    void updateData(QStandardItem *changed);
    void dropdownChanged(const QString &text);
    void selectorClicked();
    void reset() override;

    void contextMenu(QPoint pos);
    void copy();
    void paste();
};

#endif // PROPERTIES_H
