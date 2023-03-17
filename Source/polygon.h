#ifndef POLYGON_H
#define POLYGON_H

#include "item.h"
#include <QList>
#include "face.h"
class Point;
#include "vector2.h"

class PolygonItem : public Item
{
public:
    PolygonItem(QList<Vector2> Vertices = QList<Vector2>(), Vector2 Position = Vector2(0, 0), int Rotation = 0);
    QString getProperty(QString name) override;
    bool setProperty(QString name, QVariant value) override;
    void addChild(Item *newChild, int position = -1) override;
    void removeChild(Item *child) override;
    QList<Item *> getChildren() override;
    QStringList getValidChildren() override;
    QStringList save() override;
    static std::pair<bool, Item *> load(QTextStream &in);

    void sortClockwise();

    QList<Vector2> vertices;
    double rotation;
    Vector2 position;
    bool forceClockwiseOrder;


private:
    QList<Vector2> mergeSort(QList<Vector2> listToSort);
    QList<Vector2> mergeLists(QList<Vector2> listOne, QList<Vector2> listTwo);
    QList<Point *> points;
};

#endif // POLYGON_H
