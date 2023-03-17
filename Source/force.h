#ifndef FORCE_H
#define FORCE_H

#include <item.h>
#include "vector2.h"

class Force : public Item
{
public:
    Force();
    QString getProperty(QString name) override;
    bool setProperty(QString name, QVariant value) override;
    QStringList save() override;
    static std::pair<bool, Item *> load(QTextStream &in);

    Vector2 force;
    Vector2 position;
};

#endif // FORCE_H
