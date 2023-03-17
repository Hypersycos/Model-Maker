#ifndef DISPLAY_H
#define DISPLAY_H

#include <QGraphicsScene>
#include <QGraphicsItem>
#include "world.h"
#include "screenitem.h"
#include "projecttreeitem.h"

class Display : public Item
{
    Q_OBJECT
public:
    Display(Vector2 CameraPos = Vector2(0, 0), Vector2 Scale = Vector2(1, 1));
    void worldDeleted();
    void update(int state);
    void renderObject(Item *object, Vector2 transform, double rotation, Vector2 scale, Vector2 subTransform=Vector2(0, 0));
    void drawForces(Model *model);
    void drawArrow(Vector2 position, Vector2 direction, QString label);
    void widgetUpdated(ScreenItem *item);
    void start();

    QString getProperty(QString name) override;
    bool setProperty(QString name, QVariant value) override;
    void addChild(Item *newChild, int position = -1) override;
    void removeChild(Item *child) override;
    QList<Item *> getChildren() override;
    QStringList getValidChildren() override;
    QStringList save() override;
    static std::pair<bool, Item *> load(QTextStream &in);

    QGraphicsScene *scene;
    int tempLoad;
    Vector2 cameraPos;
    World *world;

signals:
    void nameChanged(Display *display);
    //for ViewHolder

private:
    std::map<ScreenItem *, QGraphicsProxyWidget *> widgets;
    Vector2 scale;
    QList<ScreenItem *> screenItems;
    ProjectTreeItem *worldItem;
    bool renderForces;
};

#endif // DISPLAY_H
