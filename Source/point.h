#ifndef POINT_H
#define POINT_H

#include "item.h"
#include "vector2.h"
class PolygonItem;

class Point : public Item
{
public:
    Point(QString name = "1", Vector2 value = Vector2(), PolygonItem *parent = nullptr);
    QString getProperty(QString name) override;
    bool setProperty(QString name, QVariant value) override;
    QStringList save() override;
    static std::pair<bool, Item *> load(QTextStream &in, PolygonItem *parent);

    void updateIndex(int index);
    void moveUpdate(PolygonItem *newParent, int index);
    PolygonItem *parent;
};

#endif // POINT_H
