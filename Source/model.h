#ifndef MODEL_H
#define MODEL_H

#include "item.h"
#include <vector2.h>
#include "force.h"
#include "drivingforce.h"

class SubModel;

class Model : public Item
{
public:
    Model(Vector2 Position = Vector2(0, 0), double Rotation = 0, Vector2 Scale = Vector2(1, 1), double Mass = 1, Vector2 Velocity = Vector2(0, 0),
          double AngularVelocity = 0, double Elasticity = 0, double Friction = 0, bool Anchored = false);
    QString getProperty(QString name) override;
    bool setProperty(QString name, QVariant value) override;
    void addChild(Item *newChild, int position = -1) override;
    void removeChild(Item *child) override;
    QList<Item *> getChildren() override;
    QStringList getValidChildren() override;
    QStringList save() override;
    static std::pair<bool, Item *> load(QTextStream &in);

    Vector2 position;
    Vector2 scale;
    double rotation;    
    Vector2 velocity;
    Vector2 acceleration;
    Vector2 CoM;
    double GPE;
    double KE;
    bool anchored;
    double angularVelocity;
    QList<Force *> forces;
    QList<SubModel *> subModels;
    double frictionCoefficient;
    double mass;
    QList<Item *> objects;

protected:
    double elasticity;
};

#endif // MODEL_H
