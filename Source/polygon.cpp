#include "polygon.h"
#include "point.h"
#include "vector2.h"

PolygonItem::PolygonItem(QList<Vector2> Vertices, Vector2 Position, int Rotation) : Item()
{
    validProperties << "Position"
                    << "Position.X"
                    << "Position.Y"
                    << "Rotation"
                    << "Force Clockwise Order";

    doubles << "Position.X"
            << "Position.Y"
            << "Rotation";

    QFileIconProvider *temp = new QFileIconProvider();
    icon = temp->icon(temp->File);

    type = "Polygon";
    name = "Polygon";

    forceClockwiseOrder = false;
    position = Position;
    rotation = Rotation;
    vertices = Vertices;
}

QString PolygonItem::getProperty(QString name)
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
    else if (name == "Rotation")
    {
        return QString::number(rotation);
    }
    else if (name == "Force Clockwise Order")
    {
        if (forceClockwiseOrder)
            return "true";
        else
            return "false";
    }
    else if (name == "Name")
    {
        return this->name;
    }
    else if (name == "Type")
    {
        return "Polygon";
    }
    else
    {
        return "Invalid";
    }
}


bool PolygonItem::setProperty(QString name, QVariant value)
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
    else if (name == "Rotation")
    {
        rotation = value.toDouble();
    }
    else if (name == "Force Clockwise Order")
    {
        forceClockwiseOrder = value.toBool();
        if (forceClockwiseOrder)
            sortClockwise();
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

void PolygonItem::sortClockwise()
{
    vertices = mergeSort(vertices);
}

QList<Vector2> PolygonItem::mergeSort(QList<Vector2> listToSort)
{
    if (listToSort.length() < 2)
        return listToSort;
    int mid = int(ceil(listToSort.length() / 2));
    QList<Vector2> list1 = mergeSort(listToSort.mid(0, mid));
    QList<Vector2> list2 = mergeSort(listToSort.mid(mid));
    return mergeLists(list1, list2);
}

QList<Vector2> PolygonItem::mergeLists(QList<Vector2> list1, QList<Vector2> list2)
{
    QList<Vector2> toReturn;
    while (list1.length() > 0 && list2.length() > 0)
    {
        if (list1.at(0).angle() < list2.at(0).angle())
        {
            toReturn.append(list1.at(0));
            list1.removeFirst();
        }
        else
        {
            toReturn.append(list2.at(0));
            list2.removeFirst();
        }
    }
    if (list1.length() > 0)
        toReturn += list1;
    else
        toReturn += list2;
    return toReturn;
}

void PolygonItem::addChild(Item *newChild, int position)
{
    if (position == -1)
        position = points.length();
    Point *point = static_cast<Point *>(newChild);

    //if point being moved is from another polygon then add a new vertex
    if (points.length() == vertices.length())
        vertices.insert(position, Vector2());
    point->moveUpdate(this, position);

    if (forceClockwiseOrder)
        sortClockwise();

    points.insert(position, point);
    for (int i = 0; i < points.length(); i++)
        points.at(i)->updateIndex(i);
}

void PolygonItem::removeChild(Item *child)
{
    Point *point = static_cast<Point *>(child);
    int index = points.indexOf(point);
    points.removeAt(index);
    for (int i = index; i < points.length(); i++)
        points.at(i)->updateIndex(i);
}

QList<Item *> PolygonItem::getChildren()
{
    QList<Item *> toReturn;
    for (int i = 0; i < points.length(); i++)
    {
        toReturn.append(points.at(i));
    }
    return toReturn;
}

QStringList PolygonItem::getValidChildren()
{
    return QStringList("Point");
}


QStringList PolygonItem::save()
{
    QStringList toReturn = Item::save(this);
    for (int i = 0; i < points.length(); i++)
    {
        QStringList child = points.at(i)->save();
        toReturn += child;
    }
    toReturn << "}";
    return toReturn;
}

std::pair<bool, Item *> PolygonItem::load(QTextStream &in)
{
    PolygonItem *newPolygon = new PolygonItem();
    if (!newPolygon->setAllProperties(getPropertiesFromFile(in)))
        return std::pair<bool, Item *>(false, nullptr);
    QString temp = in.readLine();
    while (temp.contains("{"))
    {
        newPolygon->vertices.append(Vector2());
        std::pair<bool, Item *> child = Point::load(in, newPolygon);
        if (!child.first)
            return std::pair<bool, Item *>(false, nullptr);
        newPolygon->addChild(child.second);
        temp = in.readLine();
    }
    if (temp != "}")
        return std::pair<bool, Item *>(false, nullptr);
    return std::pair<bool, Item *>(true, newPolygon);
}
