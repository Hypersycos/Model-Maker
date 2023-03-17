#include "item.h"
#include <tuple>

Item::Item() : QObject()
{
   validProperties.append("Type");
   validProperties.append("Name");
}

Item::~Item()
{
    qDeleteAll(getChildren());
}

QList<std::pair<QString, QString>> Item::getAllProperties()
{
    QList<std::pair<QString, QString>> toReturn;
    for (int i = 0; i < validProperties.length(); i++)
    {
        std::pair<QString, QString> newPair;
        newPair.first = validProperties.at(i);
        newPair.second = getProperty(validProperties.at(i));
        toReturn.append(newPair);
    }
    return toReturn;
}

bool Item::setAllProperties(QStringList values)
{
    int j = 1;
    for (int i = 0; i < values.length(); i++)
    {
        QVariant value;
        if (values.at(i) == "true" || values.at(i) == "false")
            value.setValue(values.at(i) == "true");
        else
            value.setValue(values.at(i));
        while (j < validProperties.length() && validProperties.at(j).contains("."))
            j++;
        if (j >= validProperties.length() || validProperties.at(j).contains("."))
            return false;
        setProperty(validProperties.at(j), value);
        j++;
    }
    return true;
}

QString Item::getProperty(QString name)
{
    if (name == "Name")
    {
        return this->name;
    }
    else if (name == "Type")
    {
        return this->type;
    }
    return "Invalid";
}

bool Item::setProperty(QString name, QVariant value)
{
    if (name == "Name")
    {
        this->name = value.toString();
        return true;
    }
    return false;
}

void Item::addChild(Item *, int position)
{

}

void Item::removeChild(Item *)
{

}

QList<Item *> Item::getChildren()
{
    return QList<Item *>();
}

QStringList Item::getValidChildren()
{
    return QStringList();
}

QStringList Item::save(Item *item)
{
    QStringList toReturn;
    toReturn << (item->type + "{");
    QList<std::pair<QString, QString>> properties = item->getAllProperties();
    for (int i = 1; i < properties.length(); i++) //the first property is always the type
    {
        if (!properties.at(i).first.contains("."))
        {
            toReturn << properties.at(i).second;
        }
    }
    return toReturn;
}

QStringList Item::getPropertiesFromFile(QTextStream &in)
{
    QStringList toReturn;
    QString temp;
    qint64 lastPos = in.pos();

    while (!temp.contains("{") && temp != "}" && !in.atEnd())
    {
        lastPos = in.pos();
        temp = in.readLine();
        toReturn << temp;
    }

    in.seek(lastPos);
    toReturn.removeLast();
    return toReturn;
}

QStringList Item::save()
{
    return QStringList();
}
