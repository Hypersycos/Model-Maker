#ifndef CONNECTIONSMODEL_H
#define CONNECTIONSMODEL_H

#include <QAbstractItemModel>
#include "connectionsitem.h"

class ConnectionsModel : public QAbstractItemModel
{
public:
    ConnectionsModel(QString firstHeader);
    ~ConnectionsModel() override;

    //the below are all required by views
    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    ConnectionsItem *findItem(ProjectTreeItem *item);
    QModelIndex findIndex(ProjectTreeItem *item);
    QStringList getItemProperties(ProjectTreeItem *item);

    bool moveRow(const QModelIndex &sourceParent, int sourceRow, const QModelIndex &destinationParent, int destinationChild);

    QModelIndex addItem(ConnectionsItem *item);
    void removeItem(ConnectionsItem *item);
    void reset();

private:
    ConnectionsItem *rootItem;
    QString header;
};

#endif // CONNECTIONSMODEL_H
