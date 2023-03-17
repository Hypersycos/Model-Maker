#include "drivingforce.h"

DrivingForce::DrivingForce()
{
    validProperties << "Direction"
                    << "Power";

    doubles << "Direction"
            << "Power";

    name = "Driving Force";
    type = "Driving Force";

    direction = 0;
    power = 0;

    QFileIconProvider *temp = new QFileIconProvider();
    icon = temp->icon(temp->File);
}

QString DrivingForce::getProperty(QString property)
{
    if (name == "Direction")
    {
        return QString::number(direction);
    }
    else if (name == "Power")
    {
        return QString::number(power);
    }
    else if (name == "Name")
    {
        return name;
    }
    else if (name == "Type")
    {
        return type;
    }
    else
    {
        return "Invalid";
    }
}

bool DrivingForce::setProperty(QString name, QVariant value)
{
    bool validation;
    if (doubles.contains(name))
    {
        value.toDouble(&validation);
        if (!validation)
            return false;
    }

    if (name == "Direction")
    {
        direction = value.toDouble();
    }
    else if (name == "Power")
    {
        power = value.toDouble();
    }
    else if (name == "Name")
    {
        this->name = value.toString();
    }
    else
    {
        return false;
    }
    return true;
}

QStringList DrivingForce::save()
{
    QStringList toReturn = Item::save(this);
    toReturn << "}";
    return toReturn;
}

std::pair<bool, Item *> DrivingForce::load(QTextStream &in)
{
    DrivingForce *newScreenItem = new DrivingForce();
    if (!newScreenItem->setAllProperties(getPropertiesFromFile(in))
            || in.readLine() != "}")
        return std::pair<bool, Item *>(false, nullptr);
    return std::pair<bool, Item *>(true, newScreenItem);
}
