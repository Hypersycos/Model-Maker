#ifndef CIRCLE_H
#define CIRCLE_H

#include "item.h"
#include <QPointF>
#include "vector2.h"

class Circle : public Item
{
public:
    Circle(double Radius = 0, Vector2 Position = Vector2(0, 0));
    QString getProperty(QString name) override;
    bool setProperty(QString name, QVariant value) override;
    QStringList save() override;
    static std::pair<bool, Item *> load(QTextStream &in);

    Vector2 position;
    double radius;
};

#endif // CIRCLE_H
