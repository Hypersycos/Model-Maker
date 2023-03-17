#include "graph.h"

Graph::Graph() : ScreenItem()
{
    validProperties << "Property2"
                    << "Title"
                    << "X Label"
                    << "Y Label"
                    << "X Minimum"
                    << "X Maximum"
                    << "Y Minimum"
                    << "Y Maximum";

    doubles << "X Minimum"
            << "X Maximum"
            << "Y Minimum"
            << "Y Maximum";

    propertyString = "OF";

    connected2 = nullptr;
    property2 = nullptr;

    xMin = 0;
    xMax = 0;
    yMin = 0;
    yMax = 0;

    type = "Graph";
    name = "Graph";

    series = new QSplineSeries();
    graph = new QChart();
    graph->legend()->hide();
    graph->addSeries(series);
    graph->createDefaultAxes();
    QChartView *view = new QChartView(graph);
    widget = view;
    view->setRenderHint(QPainter::Antialiasing);
}

QString Graph::getProperty(QString property)
{
    QString toReturn = ScreenItem::getProperty(property);
    if (toReturn != "Invalid")
        return toReturn;
    else if (property == "Property2")
    {
        if (connected2 == nullptr)
            return "";
        return connected2->reference->getProperty("Name") + "." + property2;
    }
    else if (property == "Title")
        return graph->title();
    else if (property == "X Label")
        return graph->axisX()->titleText();
    else if (property == "Y Label")
        return graph->axisY()->titleText();
    else if (property == "X Maximum")
        return QString::number(xMax);
    else if (property == "X Minimum")
        return QString::number(xMin);
    else if (property == "Y Maximum")
        return QString::number(yMax);
    else if (property == "Y Minimum")
        return QString::number(yMin);
    else
        return "Invalid";
}

bool Graph::setProperty(QString property, QVariant value)
{
    bool toReturn = ScreenItem::setProperty(property, value);
    if (toReturn)
        return true;

    if (property == "Title")
    {
        graph->setTitle(value.toString());
    }
    else if (property == "X Label")
    {
        graph->axisX()->setTitleText(value.toString());
    }
    else if (property == "Y Label")
    {
        graph->axisY()->setTitleText(value.toString());
    }
    else if (property == "Property2")
    {
        tempLoad2 = value.toString();
    }
    else if (property == "Item2")
    {
        disconnect(connected2, &ProjectTreeItem::destroyed, this, &Graph::item2Deleted);
        ProjectTreeItem *item = value.value<ProjectTreeItem *>();
        connected2 = item;
        connect(connected2, &ProjectTreeItem::destroyed, this, &Graph::item2Deleted);
    }
    else if (property == "PropertyString2")
    {
        property2 = value.toString();
    }
    else if (property == "X Minimum")
    {
        //xMin required for if set to automatically expand
        xMin = value.toDouble();
        graph->axisX()->setMin(xMin);
    }
    else if (property == "X Maximum")
    {
        xMax = value.toDouble();
        graph->axisX()->setMax(xMax);
    }
    else if (property == "Y Minimum")
    {
        yMin = value.toDouble();
        graph->axisY()->setMin(yMin);
    }
    else if (property == "Y Maximum")
    {
        yMax = value.toDouble();
        graph->axisY()->setMax(yMax);
    }
    else
        return false;
    return true;
}

void Graph::item2Deleted()
{
    connected2 = nullptr;
    property2 = "";
}

QStringList Graph::save()
{
    QStringList toReturn = Item::save(this);
    QList<int> indexes = connected->getIndex();
    QString stringIndex;
    for (int i = 0; i < indexes.length(); i++)
    {//convert list of row numbers to string index
        stringIndex += QString::number(indexes.at(i)) + ";";
    }
    stringIndex += property;

    toReturn.replace(validProperties.indexOf("Property")-4,
                     stringIndex);

    indexes = connected2->getIndex();
    stringIndex = "";
    for (int i = 0; i < indexes.length(); i++)
    {
        stringIndex += QString::number(indexes.at(i)) + ";";
    }
    stringIndex += property2;

    toReturn.replace(validProperties.indexOf("Property2")-4,
                     stringIndex);

    toReturn << "}";
    return toReturn;
}

std::pair<bool, Item *> Graph::load(QTextStream &in)
{
    Graph *newGraph = new Graph();
    if (!newGraph->setAllProperties(getPropertiesFromFile(in))
            || in.readLine() != "}")
        return std::pair<bool, Item *>(false, nullptr);
    return std::pair<bool, Item *>(true, newGraph);
}
