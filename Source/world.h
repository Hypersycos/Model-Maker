#ifndef WORLD_H
#define WORLD_H

#include <tuple>
#include "item.h"
#include "model.h"
#include <chrono>

using namespace std::chrono;

class World : public Item
{
public:
    World();
    void render(Model camera, int x, int y);
    void physicsUpdate();
    void startRun();
    void stopRun();
    void unpause();

    QString getProperty(QString name) override;
    bool setProperty(QString name, QVariant value) override;
    void addChild(Item *newChild, int position = -1) override;
    void removeChild(Item *child) override;
    QList<Item *> getChildren() override;
    QStringList getValidChildren() override;
    QStringList save() override;
    static std::pair<bool, Item *> load(QTextStream &in);
    QList<Model *> models;
    double hSize;
    double vSize;
    double time;
    double g;
    bool doGravity;

private:
    double speed;
    QStringList storedValues;
    time_point<steady_clock> lastUpdate;
};

#endif // WORLD_H
