#ifndef GRAPH_H
#define GRAPH_H

#include "screenitem.h"
#include <QtCharts>
using namespace QtCharts;

class Graph : public ScreenItem
{
public:
    Graph();
    QString getProperty(QString property) override;
    bool setProperty(QString name, QVariant value) override;
    QStringList save() override;
    static std::pair<bool, Item *> load(QTextStream &in);
    ProjectTreeItem *connected2;
    QString property2;

    QChart *graph;
    QSplineSeries *series;

    double xMin;
    double xMax;
    double yMin;
    double yMax;

    QString tempLoad2;

private:
    void item2Deleted();
};

#endif // GRAPH_H
