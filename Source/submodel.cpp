#include "submodel.h"
#include "polygon.h"
#include "circle.h"

SubModel::SubModel() : Model()
{
    validProperties.removeAll("Rotation");
    validProperties.removeAll("Angular Velocity");

    type = "SubModel";
    name = "SubModel";

    reaction = Vector2();
    friction = Vector2();
}

QStringList SubModel::getValidChildren()
{
    return QStringList({"Polygon", "Circle", "Force"});
}

std::pair<bool, Item *> SubModel::load(QTextStream &in)
{
    SubModel *newModel = new SubModel();
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
        if (!child.first)
            return std::pair<bool, Item *>(false, nullptr);
        newModel->addChild(child.second);
        temp = in.readLine();
    }
    if (temp != "}")
        return std::pair<bool, Item *>(false, nullptr);
    return std::pair<bool, Item *>(true, newModel);
}
