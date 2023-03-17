#include "screenitem.h"
#include <QWidget>

ScreenItem::ScreenItem(Vector2 Position, Vector2 Size) : Item()
{
    validProperties << "Position"
                    << "Position.X"
                    << "Position.Y"
                    << "Size"
                    << "Size.X"
                    << "Size.Y"
                    << "Property";

    doubles << "Position.X"
            << "Position.Y"
            << "Size.X"
            << "Size.Y";

    QFileIconProvider *temp = new QFileIconProvider();
    icon = temp->icon(temp->Trashcan);

    propertyString = "IA";

    position = Position;
    size = Size;

    connected = nullptr;
    property = "";

    type = "ScreenItem";
    name = "ScreenItem";
}

QString ScreenItem::getProperty(QString name)
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
    else if (name == "Size")
    {
        return "(" + QString::number(size.x) + ", " + QString::number(size.y) + ")";
    }
    else if (name == "Size.X")
    {
        return QString::number(size.x);
    }
    else if (name == "Size.Y")
    {
        return QString::number(size.y);
    }
    else if (name == "Property")
    {
        if (connected == nullptr)
            return "";
        return connected->reference->getProperty("Name") + "." + property;
    }
    else if (name == "TypeString")
    {
        return propertyString;
    }
    else if (name == "Name")
    {
        return this->name;
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


bool ScreenItem::setProperty(QString name, QVariant value)
{
    emit widgetUpdated(this);
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
    else if (name == "Size")
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
            size.x = first;
            size.y = second;
        }
        else
            return false;
    }
    else if (name == "Size.X")
    {
        size.x = value.toDouble();
    }
    else if (name == "Size.Y")
    {
        size.y = value.toDouble();
    }
    else if (name == "Property")
    {
        tempLoad = value.toString();
    }
    else if (name == "Item")
    {
        disconnect(connected, &ProjectTreeItem::destroyed, this, &ScreenItem::itemDeleted);
        ProjectTreeItem *item = value.value<ProjectTreeItem *>();
        connected = item;
        connect(connected, &ProjectTreeItem::destroyed, this, &ScreenItem::itemDeleted);
    }
    else if (name == "PropertyString")
    {
        property = value.toString();
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

QStringList ScreenItem::save()
{
    QStringList toReturn = Item::save(this);
    QList<int> indexes = connected->getIndex();
    QString stringIndex;
    for (int i = 0; i < indexes.length(); i++)
    {
        stringIndex += QString::number(indexes.at(i)) + ";";
    }
    stringIndex += property;

    toReturn.replace(validProperties.indexOf("Property")-4,
                     stringIndex);

    toReturn << "}";
    return toReturn;
}

void ScreenItem::itemDeleted()
{
    //if item deleted, set connected to nullptr to avoid segfault
    connected = nullptr;
    property = "";
}

std::pair<bool, Item *> ScreenItem::load(QTextStream &in)
{
    ScreenItem *newScreenItem = new ScreenItem();
    if (!newScreenItem->setAllProperties(getPropertiesFromFile(in))
            || in.readLine() != "}")
        return std::pair<bool, Item *>(false, nullptr);
    return std::pair<bool, Item *>(true, newScreenItem);
}
