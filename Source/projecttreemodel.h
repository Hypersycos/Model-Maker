#ifndef PROJECTTREEMODEL_H
#define PROJECTTREEMODEL_H

#include <QStandardItemModel>
#include "projecttreeitem.h"
#include "display.h"

class ProjectTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    ProjectTreeModel();
    ~ProjectTreeModel() override;

    //the below are all required by views
    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    Qt::DropActions supportedDropActions() const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    QStringList mimeTypes() const override;
    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    bool moveRow(const QModelIndex &sourceParent, int sourceRow, const QModelIndex &destinationParent, int destinationChild);
    void addItem(ProjectTreeItem *item, const QModelIndex &parent = QModelIndex());
    void addItem(ProjectTreeItem *item, ProjectTreeItem *parent = nullptr);
    void removeItem(ProjectTreeItem *item);
    void removeItem(const QModelIndex &item, const QModelIndex &parent = QModelIndex());
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QString load(QString filePath);
    std::pair<ProjectTreeItem *, QString> screenItemLoad(QString toLoad);
    void changeRoot(ProjectTreeItem *newRoot);
    void reset();
    QStringList rootTypes() const;

    QModelIndex dragging;
    ProjectTreeItem *rootItem;

signals:
    void displayAdded(Display *item);
    void displayRemoved(Display *item);
};

#endif // PROJECTTREEMODEL_H
