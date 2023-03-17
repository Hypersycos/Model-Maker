#ifndef SCREENITEM_H
#define SCREENITEM_H

#include "projecttreeitem.h"
#include "vector2.h"

class ScreenItem : public Item
{
    Q_OBJECT
public:
    ScreenItem(Vector2 Position = Vector2(0, 0), Vector2 Size = Vector2(0, 0));
    QString getProperty(QString name) override;
    bool setProperty(QString name, QVariant value) override;
    QStringList save() override;
    static std::pair<bool, Item *> load(QTextStream &in);
    ProjectTreeItem *connected;
    QString property;
    QWidget *widget;
    Vector2 size;
    Vector2 position;
    QString tempLoad;

signals:
    void widgetUpdated(ScreenItem *item);

protected:
    QString propertyString;
    void itemDeleted();
};

#endif // SCREENITEM_H
