#include "model.h"
#include <iostream>
#include "polygon.h"
#include "circle.h"
#include "submodel.h"

Model::Model(Vector2 Position, double Rotation, Vector2 Scale, double Mass, Vector2 Velocity, double AngularVelocity, double Elasticity, double Friction, bool Anchored) : Item()
{
    validProperties << "Position"
                    << "Position.X"
                    << "Position.Y"
                    << "Rotation"
                    << "Scale"
                    << "Scale.X"
                    << "Scale.Y"
                    << "Mass"
                    << "Velocity"
                    << "Velocity.X"
                    << "Velocity.Y"
                    << "Acceleration"
                    << "Acceleration.X"
                    << "Acceleration.Y"
                    << "Gravitational Potential Energy"
                    << "Kinetic Energy"
                    << "Angular Velocity"
                    << "Elasticity"
                    << "Friction Coefficient"
                    << "Anchored"
                    << "Centre of Mass"
                    << "Centre of Mass.X"
                    << "Centre of Mass.Y";

    doubles << "Mass"
            << "Position.X"
            << "Position.Y"
            << "Rotation"
            << "Scale.X"
            << "Scale.Y"
            << "Mass"
            << "Velocity.X"
            << "Velocity.Y"
            << "Acceleration.X"
            << "Acceleration.Y"
            << "Gravitational Potential Energy"
            << "Kinetic Energy"
            << "Angular Velocity"
            << "Elasticity"
            << "Friction Coefficient"
            << "Centre of Mass.X"
            << "Centre of Mass.Y";

    QFileIconProvider *temp = new QFileIconProvider();
    icon = temp->icon(temp->Folder);

    type = "Model";
    name = "Model";

    GPE = 0;
    KE = 0;

    position = Position;
    rotation = Rotation;
    scale = Scale;
    mass = Mass;
    velocity = Velocity;
    angularVelocity = AngularVelocity;
    elasticity = Elasticity;
    frictionCoefficient = Friction;
    anchored = Anchored;
}

QString Model::getProperty(QString name)
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
    else if (name == "Scale")
    {
        return "(" + QString::number(scale.x) + ", " + QString::number(scale.y) + ")";
    }
    else if (name == "Scale.X")
    {
        return QString::number(scale.x);
    }
    else if (name == "Scale.Y")
    {
        return QString::number(scale.y);
    }
    else if (name == "Mass")
    {
        return QString::number(mass);
    }
    else if (name == "Velocity")
    {
        return "(" + QString::number(velocity.x) + ", " + QString::number(velocity.y) + ")";
    }
    else if (name == "Velocity.X")
    {
        return QString::number(velocity.x);
    }
    else if (name == "Velocity.Y")
    {
        return QString::number(velocity.y);
    }
    else if (name == "Acceleration")
    {
        return "(" + QString::number(acceleration.x) + ", " + QString::number(acceleration.y) + ")";
    }
    else if (name == "Acceleration.X")
    {
        return QString::number(acceleration.x);
    }
    else if (name == "Acceleration.Y")
    {
        return QString::number(acceleration.y);
    }
    else if (name == "Gravitational Potential Energy")
    {
        return QString::number(GPE);
    }
    else if (name == "Kinetic Energy")
    {
        return QString::number(KE);
    }
    else if (name == "Angular Velocity")
    {
        return QString::number(angularVelocity);
    }
    else if (name == "Elasticity")
    {
        return QString::number(elasticity);
    }
    else if (name == "Friction Coefficient")
    {
        return QString::number(frictionCoefficient);
    }
    else if (name == "Anchored")
    {
        if (anchored)
            return "true";
        else
            return "false";
    }
    else if (name == "Centre of Mass")
    {
        return "(" + QString::number(CoM.x) + ", " + QString::number(CoM.y) + ")";
    }
    else if (name == "Centre of Mass.X")
    {
        return QString::number(CoM.x);
    }
    else if (name == "Centre of Mass.Y")
    {
        return QString::number(CoM.y);
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

bool Model::setProperty(QString name, QVariant value)
{
    bool validation;
    double doubleValue;
    if (doubles.contains(name))
    {
        doubleValue = value.toDouble(&validation);
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
        position.x = doubleValue;
    }
    else if (name == "Position.Y")
    {
        position.y = doubleValue;
    }
    else if (name == "Rotation")
    {
        rotation = doubleValue;
    }
    else if (name == "Scale")
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
            scale.x = first;
            scale.y = second;
        }
        else
            return false;
    }
    else if (name == "Scale.X")
    {
        scale.x = doubleValue;
    }
    else if (name == "Scale.Y")
    {
        scale.y = doubleValue;
    }
    else if (name == "Mass")
    {
        mass = doubleValue;
    }
    else if (name == "Velocity")
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
            velocity.x = first;
            velocity.y = second;
        }
        else
            return false;
    }
    else if (name == "Velocity.X")
    {
        velocity.x = doubleValue;
    }
    else if (name == "Velocity.Y")
    {
        velocity.y = doubleValue;
    }
    else if (name == "Angular Velocity")
    {
        angularVelocity = doubleValue;
    }
    else if (name == "Elasticity")
    {
        if (doubleValue < 0 || doubleValue > 1)
            return false;
        elasticity = doubleValue;
    }
    else if (name == "Friction Coefficient")
    {
        if (doubleValue < 0)
            return false;
        frictionCoefficient = doubleValue;
    }
    else if (name == "Anchored")
    {
        anchored = value.toBool();
    }
    else if (name == "Centre of Mass")
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
            CoM.x = first;
            CoM.y = second;
        }
        else
            return false;
    }
    else if (name == "Centre of Mass.X")
    {
        CoM.x = doubleValue;
    }
    else if (name == "Centre of Mass.Y")
    {
        CoM.y = doubleValue;
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

void Model::addChild(Item *newChild, int position)
{
    QString type = newChild->getProperty("Type");
    if (type == "Force")
        forces.append(static_cast<Force *>(newChild));
    else if (type == "SubModel")
        subModels.append(static_cast<SubModel *>(newChild));
    else
    {
        objects.append(newChild);
    }
}

void Model::removeChild(Item *child)
{
    QString type = child->getProperty("Type");
    if (type == "Force")
        forces.removeOne(static_cast<Force *>(child));
    else if (type == "SubModel")
        subModels.removeOne(static_cast<SubModel *>(child));
    else
        objects.removeOne(child);
}

QList<Item *> Model::getChildren()
{
    QList<Item *> toReturn = objects;
    for (int i = 0; i < forces.length(); i++)
    {
        toReturn.append(static_cast<Item *>(forces.at(i)));
    }
    for (int i = 0; i < subModels.length(); i++)
    {
        toReturn.append(static_cast<Item *>(subModels.at(i)));
    }
    return toReturn;
}

QStringList Model::getValidChildren()
{
    return QStringList({"Polygon", "Circle", "Force", "SubModel"});
}

std::pair<bool, Item *> Model::load(QTextStream &in)
{
    Model *newModel = new Model();
    if (!newModel->setAllProperties(getPropertiesFromFile(in)))
        return std::pair<bool, Item *>(false, nullptr);
    QString temp = in.readLine();
    while (temp.contains("{"))
    {
        std::pair<bool, Item*> child;
        if (temp == "Polygon{")
            child = PolygonItem::load(in);
        else if (temp == "Circle{")
            child = Circle::load(in);
        else if (temp == "Force{")
            child = Force::load(in);
        else if (temp == "SubModel{")
            child = SubModel::load(in);
        if (!child.first)
            return std::pair<bool, Item *>(false, nullptr);
        newModel->addChild(child.second);
        temp = in.readLine();
    }
    if (temp != "}")
        return std::pair<bool, Item *>(false, nullptr);
    return std::pair<bool, Item *>(true, newModel);
}

QStringList Model::save()
{
    QStringList toReturn = Item::save(this);
    for (int i = 0; i < objects.length(); i++)
    {
        QStringList child = objects.at(i)->save();
        toReturn += child;
    }
    for (int i = 0; i < forces.length(); i++)
    {
        QStringList child = forces.at(i)->save();
        toReturn += child;
    }
    for (int i = 0; i < subModels.length(); i++)
    {
        QStringList child = subModels.at(i)->save();
        toReturn += child;
    }
    toReturn << "}";
    return toReturn;
}
