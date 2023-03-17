#ifndef CONNECTION_H
#define CONNECTION_H

#include "subwindow.h"
#include "projecttreeitem.h"
#include "connectionsmodel.h"
#include <QItemSelectionModel>
#include <QTreeView>

class Connections : public SubWindow
{
public:
    Connections(MainWindow *creator, QAction *toggle);
    void addInput(ProjectTreeItem* item, QString property, QString type);
    void addOutput(ProjectTreeItem *item, QString property, QString type);
    void removeInput(ProjectTreeItem* item, QString property);
    void removeOutput(ProjectTreeItem *item, QString property);
    void reset() override;
    QStringList getInputsForItem(ProjectTreeItem *item);
    QStringList getOutputsForItem(ProjectTreeItem *item);

    QItemSelectionModel *inputSelection;
    QItemSelectionModel *outputSelection;

    ConnectionsModel *inputs;
    ConnectionsModel *outputs;

private:

    QTreeView *inputsView;
    QTreeView *outputsView;

};

#endif // CONNECTION_H
