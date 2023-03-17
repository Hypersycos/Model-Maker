#ifndef DRIVINGFORCE_H
#define DRIVINGFORCE_H

#include "item.h"

class DrivingForce : public Item
{
public:
    DrivingForce();
    QString getProperty(QString name) override;
    bool setProperty(QString name, QVariant value) override;
    QStringList save() override;
    static std::pair<bool, Item *> load(QTextStream &in);

    double direction;
    double power;
};

#endif // DRIVINGFORCE_H
