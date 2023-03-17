#include "circle.h"

Circle::Circle(double Radius, Vector2 Position) : Item()
{
    validProperties << "Radius"
                    << "Position"
                    << "Position.X"
                    << "Position.Y";

    doubles << "Position.X"
            << "Position.Y"
            << "Radius";

    QFileIconProvider *temp = new QFileIconProvider();
    icon = temp->icon(temp->File);

    type = "Circle";
    name = "Circle";

    position = Position;
    radius = Radius;
}

QString Circle::getProperty(QString name)
{
    if (name == "Position")
    {
        return "(" + QString::number(position.x) + ", " + QString::number(position.y) + ")";
    }
    else if (name == "Position.X")
    {
        return QString::number(position.x);
    }
    else if (name == "Position.Y")
    {
        return QString::number(position.y);
    }
    else if (name == "Radius")
    {
        return QString::number(radius);
    }
    else if (name == "Name")
    {
        return this->name;
    }
    else if (name == "Type")
    {
        return "Circle";
    }
    else
    {
        return "Invalid";
    }
}


bool Circle::setProperty(QString name, QVariant value)
{
    bool validation;
    if (doubles.contains(name))
    {
        value.toDouble(&validation);
        if (!validation)
            return false;
    }

    if (name == "Position")
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
    else if (name == "Radius")
    {
        radius = value.toDouble();
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

QStringList Circle::save()
{
    QStringList toReturn = Item::save(this);
    toReturn << "}";
    return toReturn;
}

std::pair<bool, Item *> Circle::load(QTextStream &in)
{
    Circle *newScreenItem = new Circle();
    if (!newScreenItem->setAllProperties(getPropertiesFromFile(in))
            || in.readLine() != "}")
        return std::pair<bool, Item *>(false, nullptr);
    return std::pair<bool, Item *>(true, newScreenItem);
}
