#include "projecttreeview.h"
#include <QDragMoveEvent>
#include <QMimeData>
#include <QMessageBox>
#include <iostream>

ProjectTreeView::ProjectTreeView() : QTreeView()
{

}

/*bool ProjectTreeView::dropResultsInValidIndex(const QPoint &pos, const QMimeData *data)
{
    return true;
    QStandardItemModel *model = reinterpret_cast<QStandardItemModel *>(this->model());
    QModelIndex index = this->indexAt(pos);
    QStandardItem *reciever = model->itemFromIndex(index);
    QStandardItemModel *recieverModel = reciever->data(Qt::UserRole + 1).value<QStandardItemModel *>();
    int typeRow = recieverModel->findItems("Type").first()->row();
    QStandardItem *type = recieverModel->item(typeRow, 1);
    QString value = type->text();
    //std::cout << value.toStdString() << std::endl;*/
    /*for (int i = 0; i < data->formats().length(); i++)
    {
        std::cout << data->formats().at(i).toStdString() << std::endl;
    }*//*
    QByteArray test = data->data("customMimeTest");
    QStandardItem *dragged = model->item(test.at(0), test.at(1));
    QDataStream stream;
    stream << test;
    dragged->read(stream);
    //std::cout << dragged->text().toStdString() << std::endl;
    //std::cout << dragged->data(Qt::UserRole + 1).value<QStandardItemModel *>();
    return false;
}

void ProjectTreeView::dragMoveEvent(QDragMoveEvent* event)
{
    event->accept();
    QTreeView::dragMoveEvent(event);
    if (!event->isAccepted())
        return;

    if (dropResultsInValidIndex(event->pos(), event->mimeData()))
        event->accept();
    else
        event->ignore(); //Show 'forbidden' cursor.
}

/*bool ProjectTreeView::dropMimeData(QStandardItem *parent, int index, const QMimeData *data, Qt::DropAction action)
{
    Q_UNUSED(index);
    //Modify the following action and data format checks to suit your needs:
    if (parent == NULL || action != Qt::CopyAction || !data->hasFormat("my/preferred-type"))
        return false;

    QModelIndex modelIndex = indexFromItem(parent);
    //modelIndex is where the data is dropped onto. Implement your custom drop action here...

    return true;
}
*/
