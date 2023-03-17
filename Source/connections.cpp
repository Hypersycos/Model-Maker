#include "connections.h"

Connections::Connections(MainWindow *creator, QAction *toggle) : SubWindow (creator, toggle, tr("Connections"))
{
    inputs = new ConnectionsModel("Inputs");
    outputs = new ConnectionsModel("Outputs");

    inputSelection = new QItemSelectionModel(inputs);
    outputSelection = new QItemSelectionModel(outputs);

    inputsView = new QTreeView();
    inputsView->setExpandsOnDoubleClick(true);
    inputsView->setHeaderHidden(false);
    inputsView->setModel(inputs);
    inputsView->setSelectionModel(inputSelection);

    outputsView = new QTreeView();
    outputsView->setExpandsOnDoubleClick(true);
    outputsView->setHeaderHidden(false);
    outputsView->setModel(outputs);
    outputsView->setSelectionModel(outputSelection);

    layout->addWidget(inputsView);
    layout->addWidget(outputsView);
}

void Connections::addInput(ProjectTreeItem *item, QString property, QString type)
{
    ConnectionsItem *newItem = new ConnectionsItem(property, item, type);
    inputs->addItem(newItem);
}

void Connections::addOutput(ProjectTreeItem *item, QString property, QString type)
{
    ConnectionsItem *newItem = new ConnectionsItem(property, item, type);
    outputs->addItem(newItem);
}

void Connections::removeInput(ProjectTreeItem *item, QString property)
{
    ConnectionsItem *parent = inputs->findItem(item);
    inputs->removeItem(parent->child(property));
}

void Connections::removeOutput(ProjectTreeItem *item, QString property)
{
    ConnectionsItem *parent = outputs->findItem(item);
    outputs->removeItem(parent->child(property));
}

void Connections::reset()
{
    outputs->reset();
    inputs->reset();
}

QStringList Connections::getInputsForItem(ProjectTreeItem *item)
{
    return inputs->getItemProperties(item);
}

QStringList Connections::getOutputsForItem(ProjectTreeItem *item)
{
    return outputs->getItemProperties(item);
}
