#include "point.h"
#include "polygon.h"

Point::Point(QString name, Vector2 value, PolygonItem *parent) : Item()
{
    validProperties << "Position"
                    << "Position.X"
                    << "Position.Y";

    doubles << "Position.X"
            << "Position.Y";

    QFileIconProvider *temp = new QFileIconProvider();
    icon = temp->icon(temp->File);

    this->parent = parent;

    type = "Point";
    this->name = name;
    if (parent != nullptr)
        parent->vertices.replace(name.toInt(), value);
}

void Point::updateIndex(int index)
{//points can't be named, their name is their index
    name = QString::number(index+1);
}

void Point::moveUpdate(PolygonItem *newParent, int index)
{
    if (parent == newParent)
    { //if moving internally, move the vertex
        int oldIndex = this->name.toInt()-1;
        Vector2 vertex = parent->vertices.at(oldIndex);
        parent->vertices.removeAt(oldIndex);
        parent->vertices.insert(index, vertex);
    } //since a point stores its data in its parent, it must move the data from the parent before deleting it when moved.
    else if (parent != nullptr)
    {
        Vector2 value = parent->vertices.at(this->name.toInt()-1);
        newParent->vertices.replace(index, value);
        if (parent != newParent)
            parent->vertices.removeAt(this->name.toInt()-1);
    }
    parent = newParent;
    name = QString::number(index+1);
}

QString Point::getProperty(QString name)
{
    bool success;
    int index = this->name.toInt(&success)-1;

    if (!success)
        return "Invalid";

    Vector2 position;
    if (parent != nullptr)
        position = parent->vertices.at(index);
    else if (name.contains("Position"))
        return "Invalid";

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
    else if (name == "Name")
    {
        return "Vertex"+this->name;
    }
    else if (name == "Type")
    {
        return "Point";
    }
    else
    {
        return "Invalid";
    }
}


bool Point::setProperty(QString name, QVariant value)
{
    bool success;
    int index = this->name.toInt(&success)-1;

    if (!success)
        return false;

    Vector2 position = parent->vertices.at(index);

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
            if (parent->forceClockwiseOrder)
                parent->sortClockwise();
        }
        else
            return false;
    }
    else if (name == "Position.X")
    {
        position.x = value.toDouble();
        if (parent->forceClockwiseOrder)
            parent->sortClockwise();
    }
    else if (name == "Position.Y")
    {
        position.y = value.toDouble();
        if (parent->forceClockwiseOrder)
            parent->sortClockwise();
    }
    else
    {
        return false;
    }
    parent->vertices.replace(index, position);
    return true;
}

QStringList Point::save()
{
    QStringList toReturn = Item::save(this);
    toReturn << "}";
    return toReturn;
}

std::pair<bool, Item *> Point::load(QTextStream &in, PolygonItem *parent)
{
    Point *newPoint = new Point();
    newPoint->name = QString::number(parent->vertices.length());
    newPoint->parent = parent;
    if (!newPoint->setAllProperties(getPropertiesFromFile(in))
            || in.readLine() != "}")
        return std::pair<bool, Item *>(false, nullptr);
    return std::pair<bool, Item *>(true, newPoint);
}
