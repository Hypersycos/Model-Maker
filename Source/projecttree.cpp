#include "projecttree.h"
#include "mainwindow.h"
#include <QTreeView>
#include "subwindow.h"
#include <QFileIconProvider>
#include <QHBoxLayout>
#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QClipboard>
#include <QApplication>
#include <QMimeData>

#include "world.h"
#include "model.h"
#include "polygon.h"
#include "point.h"
#include "display.h"
#include "slider.h"
#include "circle.h"
#include "force.h"
#include "graph.h"
#include "submodel.h"

ProjectTree::ProjectTree(MainWindow *parent, ProjectTreeModel *itemmodel, QItemSelectionModel *selectionmodel, QAction *toggle) : SubWindow(parent, toggle, tr("Project Tree"))
{
    itemModel = itemmodel;

    treeView = new QTreeView();
    treeView->setExpandsOnDoubleClick(true);
    treeView->setDragEnabled(true);
    treeView->setDragDropMode(QTreeView::DragDropMode::InternalMove);
    treeView->setHeaderHidden(true);
    treeView->setModel(itemModel);
    treeView->setSelectionModel(selectionmodel);
    treeView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(treeView, &QTreeView::customContextMenuRequested, this, &ProjectTree::contextMenu);

    layout->addWidget(treeView);
}

void ProjectTree::contextMenu(QPoint pos)
{
    QModelIndex index = treeView->indexAt(pos);
    QMenu *menu = new QMenu(this);
    QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *currentItem = clipboard->mimeData();

    QStringList validTypes;

    QAction *pasteAct = new QAction(tr("&Paste Into"));
    pasteAct->setData(index);
    if (!currentItem->hasFormat("ProjectTreeItem"))
        pasteAct->setEnabled(false);
    else
        connect(pasteAct, &QAction::triggered, this, &ProjectTree::paste);


    if (index.isValid())
    { //if an item is right-clicked
        ProjectTreeItem *item = static_cast<ProjectTreeItem *>(index.internalPointer());

        QAction *copyAct = new QAction(tr("&Copy"));
        copyAct->setData(index);
        connect(copyAct, &QAction::triggered, this, &ProjectTree::copy);
        menu->addAction(copyAct);

        menu->addAction(pasteAct);

        QAction *deleteAct = new QAction(tr("&Delete"));
        deleteAct->setData(index);
        connect(deleteAct, &QAction::triggered, this, &ProjectTree::remove);
        menu->addAction(deleteAct);

        validTypes = item->getValidChildren();
    }
    else
    {
        menu->addAction(pasteAct);
        validTypes = itemModel->rootTypes();
    }

    if (validTypes.length() > 0)
    {
        QMenu *insertMenu = new QMenu(tr("&Insert Into"));

        for (int i = 0; i < validTypes.length(); i++)
        {
            QAction *tempAct = new QAction("&"+validTypes.at(i));
            tempAct->setData(index);
            connect(tempAct, &QAction::triggered, this, &ProjectTree::insert);
            insertMenu->addAction(tempAct);
        }

        menu->addMenu(insertMenu);
    }

    menu->popup(treeView->viewport()->mapToGlobal(pos));
}

Item *ProjectTree::loadCorrectItem(QByteArray data, Item *parent)
{
    QStringList text;
    QTextStream stream(&data, QIODevice::ReadOnly | QIODevice::Text);

    QString type = stream.readLine();

    std::pair<bool, Item *> toReturn;

    if (type == "World{")
        toReturn = World::load(stream);
    else if (type == "Model{")
        toReturn = Model::load(stream);
    else if (type == "Polygon{")
        toReturn = PolygonItem::load(stream);
    else if (type == "Point{")
        toReturn = Point::load(stream, static_cast<PolygonItem *>(parent));
    else if (type == "Circle{")
        toReturn = Circle::load(stream);
    else if (type == "Force{")
        toReturn = Force::load(stream);
    else if (type == "Display{")
        toReturn = Display::load(stream);
    else if (type == "Slider{")
        toReturn = Slider::load(stream);
    else if (type == "Graph{")
        toReturn = Graph::load(stream);
    else if (type == "SubModel{")
        toReturn = SubModel::load(stream);

    if (!toReturn.first)
        return nullptr;
    else
        return toReturn.second;
}

void ProjectTree::copy()
{
    QObject *sender = QObject::sender();
    QAction *act = static_cast<QAction *>(sender);
    QModelIndex index = act->data().value<QModelIndex>();
    ProjectTreeItem *item = static_cast<ProjectTreeItem *>(index.internalPointer());

    QStringList text = item->reference->save();

    //mimedata required a QByteArray
    QByteArray data;
    QTextStream stream(&data, QIODevice::WriteOnly | QIODevice::Text);
    for (int i = 0; i < text.length(); i++)
    {
        stream << text.at(i) << "\n";
    }
    stream.flush();

    QMimeData *mime = new QMimeData();
    mime->setData("ProjectTreeItem", data);

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setMimeData(mime);
}

void ProjectTree::paste()
{
    QObject *sender = QObject::sender();
    QAction *act = static_cast<QAction *>(sender);
    QModelIndex index = act->data().value<QModelIndex>();
    ProjectTreeItem *parent = static_cast<ProjectTreeItem *>(index.internalPointer());

    QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mime = clipboard->mimeData();

    if (!mime->hasFormat("ProjectTreeItem"))
        return;

    QByteArray data = mime->data("ProjectTreeItem");

    Item *newItem = loadCorrectItem(data, parent->reference);
    if (newItem == nullptr)
        return;
    if (!parent->getValidChildren().contains(newItem->getProperty("Type")))
        return;
    ProjectTreeItem *newTreeItem = new ProjectTreeItem(newItem, true);
    itemModel->addItem(newTreeItem, index);
}

void ProjectTree::remove()
{
    QObject *sender = QObject::sender();
    QAction *act = static_cast<QAction *>(sender);
    QModelIndex index = act->data().value<QModelIndex>();

    if (index.isValid())
        itemModel->removeItem(index, index.parent());
}

void ProjectTree::insert()
{
    QObject *sender = QObject::sender();
    QAction *act = static_cast<QAction *>(sender);
    QModelIndex index = act->data().value<QModelIndex>();
    QString type = act->text();

    Item *item;
    if (type == "&World")
        item = new World();
    else if (type == "&Model")
        item = new Model();
    else if (type == "&Polygon")
        item = new PolygonItem();
    else if (type == "&Point")
        item = new Point();
    else if (type == "&Display")
        item = new Display();
    else if (type == "&Slider")
        item = new Slider();
    else if (type == "&Circle")
        item = new Circle();
    else if (type == "&Force")
        item = new Force();
    else if (type == "&Graph")
        item = new Graph();
    else if (type == "&SubModel")
        item = new SubModel();
    else
        return;

    ProjectTreeItem *newItem = new ProjectTreeItem(item);
    itemModel->addItem(newItem, index);
}
