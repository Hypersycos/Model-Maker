#include "force.h"

Force::Force()
{
    validProperties << "Force"
                    << "Force.X"
                    << "Force.Y"
                    << "Position"
                    << "Position.X"
                    << "Position.Y";

    doubles << "Force.X"
            << "Force.Y"
            << "Position.X"
            << "Position.Y";

    name = "Force";
    type = "Force";

    force = Vector2();

    QFileIconProvider *temp = new QFileIconProvider();
    icon = temp->icon(temp->File);
}

QString Force::getProperty(QString property)
{
    if (property == "Force")
    {
        return "(" + QString::number(force.x) + ", " + QString::number(force.y) + ")";
    }
    else if (property == "Force.X")
    {
        return QString::number(force.x);
    }
    else if (property == "Force.Y")
    {
        return QString::number(force.y);
    }
    else if (property == "Position")
    {
        return "(" + QString::number(position.x) + ", " + QString::number(position.y) + ")";
    }
    else if (property == "Position.X")
    {
        return QString::number(position.x);
    }
    else if (property == "Position.Y")
    {
        return QString::number(position.y);
    }
    else if (property == "Name")
    {
        return name;
    }
    else if (property == "Type")
    {
        return type;
    }
    else
    {
        return "Invalid";
    }
}

bool Force::setProperty(QString name, QVariant value)
{
    bool validation;
    if (doubles.contains(name))
    {
        value.toDouble(&validation);
        if (!validation)
            return false;
    }

    if (name == "Force")
    {
        QString temp = value.toString();

        if (!temp.contains(","))
            return false;
        temp = temp.remove("(");
        temp = temp.remove(")");
        temp = temp.remove(" ");
        QStringList tempList = temp.split(",");

        bool valid1;
        bool valid2;

        double first = tempList.first().toDouble(&valid1);
        double second = tempList.last().toDouble(&valid2);

        if (valid1 && valid2)
        {
            force.x = first;
            force.y = second;
        }
        else
            return false;
    }
    else if (name == "Force.X")
    {
        force.x = value.toDouble();
    }
    else if (name == "Force.Y")
    {
        force.y = value.toDouble();
    }
    else if (name == "Position")
    {
        QString temp = value.toString();

        if (!temp.contains(","))
            return false;
        temp = temp.remove("(");
        temp = temp.remove(")");
        temp = temp.remove(" ");
        QStringList tempList = temp.split(",");

        bool valid1;
        bool valid2;

        double first = tempList.first().toDouble(&valid1);
        double second = tempList.last().toDouble(&valid2);

        if (valid1 && valid2)
        {
            position.x = first;
            position.y = second;
        }
        else
            return false;
    }
    else if (name == "Position.X")
    {
        position.x = value.toDouble();
    }
    else if (name == "Position.Y")
    {
        position.y = value.toDouble();
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

QStringList Force::save()
{
    QStringList toReturn = Item::save(this);
    toReturn << "}";
    return toReturn;
}

std::pair<bool, Item *> Force::load(QTextStream &in)
{
    Force *newScreenItem = new Force();
    if (!newScreenItem->setAllProperties(getPropertiesFromFile(in))
            || in.readLine() != "}")
        return std::pair<bool, Item *>(false, nullptr);
    return std::pair<bool, Item *>(true, newScreenItem);
}
