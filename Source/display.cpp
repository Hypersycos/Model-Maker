#include "display.h"
#include "polygon.h"
#include "circle.h"
#include "vector2.h"
#include <QWidget>
#include <math.h>
#include "graph.h"
#include "slider.h"
#include "submodel.h"
#include <QGraphicsProxyWidget>

Display::Display(Vector2 CameraPos, Vector2 Scale) : Item()
{
    validProperties << "Scale"
                    << "Scale.X"
                    << "Scale.Y"
                    << "World"
                    << "Camera Position"
                    << "Camera Position.X"
                    << "Camera Position.Y"
                    << "Render Forces";

    doubles << "Scale.X"
            << "Scale.Y"
            << "Centre Position.X"
            << "Centre Position.Y";

    cameraPos = CameraPos;
    scale = Scale;

    QFileIconProvider *temp = new QFileIconProvider();
    icon = temp->icon(temp->Desktop);

    scene = new QGraphicsScene();
    world = nullptr;

    type = "Display";
    name = "Display";
}

void Display::update(int state)
{ /*state is from ViewHolder.
    0 = editing
    1 = running
    2 = paused*/

    if (world == nullptr)
        return;
    QList<QGraphicsItem *> items = scene->items();
    for (int i = 0; i < items.length(); i++)
    { //if not a QGraphicsProxyWidget
        if (items.at(i)->type() != 12)
        { //if QGraphicsProxyWidget removed, then the widget is also deleted, so they can't be removed
            scene->removeItem(items.at(i));
            delete items.at(i); //delete required to avoid memory leak
        }
    }
    QList<Model *> models = world->models;
    for (int i = 0; i < models.length(); i++)
    {
        Model *model = models.at(i);
        if (state != 0 && renderForces)
            drawForces(model);
        //don't render force arrows while editing
        Vector2 transform = model->position;
        double rotation = model->rotation;
        for (int j = 0; j < model->objects.length(); j++)
        {
            renderObject(model->objects.at(j), transform, rotation, model->scale);
        }
        for (int j = 0; j < model->subModels.length(); j++)
        {
            SubModel *subModel = model->subModels.at(j);
            for (int k = 0; k < subModel->objects.length(); k++)
            {
                renderObject(subModel->objects.at(k), transform, rotation, model->scale*subModel->scale, subModel->position);
            }
        }
    }

    double w = world->hSize * scale.x;
    double h = world->vSize * scale.y;
    if (w == 0.0 || h == 0.0)
    { //if a size is set to 0, the scene should expand and retract as required
        QRectF bounding = scene->itemsBoundingRect();
        if (bounding.bottom() < 0) //keeps 0, 0 visible
            bounding.setBottom(0);
        if (bounding.top() < 0)
            bounding.setTop(0);
        if (bounding.left() < 0)
            bounding.setLeft(0);
        if (bounding.right() < 0)
            bounding.setRight(0);
        //maintains camera position
        bounding.setX(scene->sceneRect().x());
        bounding.setY(scene->sceneRect().y());
        scene->setSceneRect(bounding);
    }
    else
    {
        scene->setSceneRect(QRectF(-w/2, -h/2, w, h));
        scene->addRect(QRectF(-w/2, -h/2, w, h));
    }
    if (state == 1)
    { //graphs should only be updated if running
        for (int i = 0; i < screenItems.length(); i++)
        {
            ScreenItem *item = screenItems.at(i);
            if (item->getProperty("Type") == "Graph")
            {
                Graph *graph = static_cast<Graph *>(item);
                if (graph->connected != nullptr && graph->connected2 != nullptr)
                { //if graph has two properties
                    double x = graph->connected->reference->getProperty(graph->property).toDouble();
                    double y = graph->connected2->reference->getProperty(graph->property2).toDouble();
                    QValueAxis *xAxis = static_cast<QValueAxis *>(graph->graph->axisX());
                    QValueAxis *yAxis = static_cast<QValueAxis *>(graph->graph->axisY());

                    if (graph->xMin == 0.0 && graph->xMax == 0.0)
                    { //if graph set to expand automatically, then check to see whether new value exceeds current bounds
                        if (x < 0)
                        {
                            if (x * 0.95 > xAxis->max())
                                xAxis->setMax(x*0.95);
                            else if (x * 1.05 < xAxis->min())
                                xAxis->setMin(x*1.05);
                        }
                        else
                        {
                            if (x * 1.05 > xAxis->max())
                                xAxis->setMax(x*1.05);
                            else if (x * 0.95 < xAxis->min())
                                xAxis->setMin(x*0.95);
                        }
                    }

                    if (graph->yMin == 0.0 && graph->yMax == 0.0)
                    {
                        if (y < 0)
                        {
                            if (y * 0.95 > yAxis->max())
                                yAxis->setMax(y*0.95);
                            else if (y * 1.05 < yAxis->min())
                                yAxis->setMin(y*1.05);
                        }
                        else
                        {
                            if (y * 1.05 > yAxis->max())
                                yAxis->setMax(y*1.05);
                            else if (y * 0.95 < yAxis->min())
                                yAxis->setMin(y*0.95);
                        }
                    }
                    graph->series->append(x, y);
                }
            }
        }
    }
}

void Display::renderObject(Item *object, Vector2 transform, double rotation, Vector2 modelScale, Vector2 subTransform)
{
    if (object->getProperty("Type") == "Polygon")
    {
        PolygonItem *polygon = static_cast<PolygonItem *>(object);
        QPolygonF renderPolygon = QPolygonF();
        QList<Vector2> points = polygon->vertices;
        for (int i = 0; i < points.length(); i++)
        {
            //using Vector2 as QPointF doesn't support multiplying with other QPointFs
            //movement and rotation inside the polygon and/or submodel
            Vector2 point = points.at(i);
            if (polygon->rotation != 0.0)
                point = point.rotate(polygon->rotation);
            point = point + polygon->position + subTransform;
            //movement and rotation inside the model
            if (rotation != 0.0)
            {
                point = point.rotate(rotation);
            }
            point = point * modelScale;
            //movement relative to world
            point = point + transform;
            point = point * scale;
            renderPolygon.append(point.toQPointF());
        }
        scene->addPolygon(renderPolygon);
    }
    else
    {
        Circle *circle = static_cast<Circle *>(object);
        Vector2 circleCorrection = Vector2(-circle->radius/2, circle->radius/2);
        //circleCorrection ensures circle's centre is its position
        Vector2 pos = (((circle->position + subTransform).rotate(rotation) + circleCorrection * modelScale) + transform) * scale;
        Vector2 sizeTransform = scale*modelScale;
        //QGraphicsScene has y positive-negative as bottom-top. Users will expect it to be the other way round, so -pos.y is used.
        scene->addEllipse(pos.x, -pos.y, circle->radius*sizeTransform.x, circle->radius*sizeTransform.y);
    }
}

void Display::drawForces(Model *model)
{
    if (model->anchored == true)
    { //if anchored then the forces are relevant to the subModels
        for (int i = 0; i < model->subModels.length(); i++)
        {
            SubModel *subModel = model->subModels.at(i);
            for (int j = 0; j < subModel->forces.length(); j++)
            {
                Force *force = subModel->forces.at(j); //from most relative to absolute
                Vector2 position = (((force->position + subModel->position) * subModel->scale).rotate(model->rotation) + model->position) * model->scale;
                Vector2 direction = force->force.unitVector().rotate(model->rotation);
                drawArrow(position, direction, QString::number(force->force.magnitude()));
            }

            Vector2 force = subModel->reaction;
            if (force.x != 0.0 || force.y != 0.0)
            { //reaction is always from centre of mass
                Vector2 position = (((subModel->CoM + subModel->position) * subModel->scale).rotate(model->rotation) + model->position) * model->scale;
                Vector2 direction = force.unitVector().rotate(model->rotation);
                drawArrow(position, direction, QString::number(force.magnitude()));
            }

            force = subModel->friction;
            if (force.x != 0.0 || force.y != 0.0)
            { //these lines are pretty horrific, but there's no real repeated data to use in them.
                Vector2 position = (((subModel->CoM + subModel->position) * subModel->scale).rotate(model->rotation) + model->position) * model->scale;
                Vector2 direction = force.unitVector().rotate(model->rotation);
                drawArrow(position, direction, QString::number(force.magnitude()));
            }

            //gravity is always straight down
            Vector2 position = (((subModel->CoM + subModel->position) * subModel->scale).rotate(model->rotation) + model->position) * model->scale;
            drawArrow(position, Vector2(0, -1), QString::number(subModel->mass * world->g));
        }
    }
    else
    {
        QList<Force *> forces = model->forces;
        QList<SubModel *> weights = model->subModels;
        for (int i = 0; i < forces.length(); i++)
        {
            Force *force = forces.at(i);
            Vector2 position = ((force->position+model->position)*model->scale).rotate(model->rotation);
            Vector2 direction = force->force.unitVector().rotate(model->rotation);
            drawArrow(position, direction, QString::number(force->force.magnitude()));
        }
        if (world->doGravity)
        {
            for (int i = 0; i < weights.length(); i++)
            {
                SubModel *subModel = weights.at(i);
                Vector2 position = (((subModel->position + subModel->CoM) * subModel->scale).rotate(model->rotation) + model->position) * model->scale;
                drawArrow(position, Vector2(0, -1), QString::number(subModel->mass * world->g));
            }

            Vector2 position = (model->position + model->CoM) * model->scale;
            drawArrow(position, Vector2(0, -1), QString::number(model->mass * world->g));
        }
    }
}

void Display::drawArrow(Vector2 position, Vector2 direction, QString label)
{
    position = position * scale;
    //the scale can stretch the drawn direction. direction should always be a unit vector, so we use the unit vector of scale and multiply by root 2 so at scale 1,1 the direction is unchanged
    direction = direction * scale.unitVector() * sqrt(2);
    if (label == "0")
        return;
    QPainterPath path = QPainterPath();
    // using magnitudeSquared as sqrt is costly, and we can just divide by 2 instead.
    Vector2 lineSize = (direction * (50 * scale.magnitudeSquared() / 2));
    Vector2 end = lineSize + position;
    path.moveTo(position.toQPointF());
    path.lineTo(end.toQPointF());
    path.addText(end.toQPointF(), QFont(), label);
    scene->addPath(path);
}

void Display::start()
{
    for (int i = 0; i < screenItems.length(); i++)
    {
        if (screenItems.at(i)->getProperty("Type") == "Graph")
        {
            Graph *graph = static_cast<Graph *>(screenItems.at(i));
            graph->series->clear();
            graph->graph->axisX()->setRange(graph->xMin, graph->xMax);
            graph->graph->axisY()->setRange(graph->yMin, graph->yMax);
        }
    }
}

QString Display::getProperty(QString name)
{
    if (name == "Scale")
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
    else if (name == "Name")
    {
        return this->name;
    }
    else if (name == "World")
    {
        if (world == nullptr)
            return "";
        return world->getProperty("Name");
    }
    else if (name == "Camera Position")
    {
        return "(" + QString::number(cameraPos.x) + ", " + QString::number(cameraPos.y) + ")";
    }
    else if (name == "Camera Position.X")
    {
        return QString::number(cameraPos.x);
    }
    else if (name == "Camera Position.Y")
    {
        return QString::number(cameraPos.y);
    }
    else if (name == "Render Forces")
    {
        if (renderForces)
            return "true";
        else
            return "false";
    }
    else if (name == "Type")
    {
        return "Display";
    }
    else
    {
        return "Invalid";
    }
}

void Display::worldDeleted()
{
    worldItem = nullptr;
    world = nullptr;
}

bool Display::setProperty(QString name, QVariant value)
{
    bool validation;
    if (doubles.contains(name))
    {
        value.toDouble(&validation);
        if (!validation)
            return false;
    } //neater to do float validation here, rather than for every float

    if (name == "Scale")
    {
        QString temp = value.toString();

        //we strip out redundant formatting
        if (!temp.contains(","))
            return false;
        temp = temp.remove("(");
        temp = temp.remove(")");
        temp = temp.remove(" ");
        QStringList tempList = temp.split(",");
        //fails if there's no comma

        bool valid1;
        bool valid2;

        double first = tempList.first().toDouble(&valid1);
        double second = tempList.last().toDouble(&valid2);

        if (valid1 && valid2)
        {
            scale.x = first;
            scale.y = second;
            for (int i = 0; i < screenItems.length(); i++)
            {//redraw with updated scale
                widgetUpdated(screenItems.at(i));
            }
        }
        else
            return false;
    }
    else if (name == "Scale.X")
    {
        scale.x = value.toDouble();
        for (int i = 0; i < screenItems.length(); i++)
        {
            widgetUpdated(screenItems.at(i));
        }
    }
    else if (name == "Scale.Y")
    {
        scale.y = value.toDouble();
        for (int i = 0; i < screenItems.length(); i++)
        {
            widgetUpdated(screenItems.at(i));
        }
    }
    else if (name == "Name")
    {
        this->name = value.toString();
        emit nameChanged(this);
    }
    else if (name == "World")
    {
        QModelIndex index = value.toModelIndex();
        //when loading from a file, the World is stored as a numerical index. We keep this index for later.
        if (!index.isValid())
        {
            bool temp;
            int tempInt = value.toInt(&temp);
            if (temp)
                tempLoad = tempInt;
            return false;
        }
        ProjectTreeItem *item = static_cast<ProjectTreeItem *>(index.internalPointer());
        if (item->getType() != "World")
            return false;
        disconnect(world, &World::destroyed, this, &Display::worldDeleted);
        worldItem = item;
        world = static_cast<World *>(item->reference);
        //prevents crash if world is deleted while display points to it.
        connect(world, &World::destroyed, this, &Display::worldDeleted);
    }
    else if (name == "Camera Position")
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
            cameraPos.x = first;
            cameraPos.y = second;
        }
        else
            return false;
    }
    else if (name == "Camera Position.X")
    {
        cameraPos.x = value.toDouble();
    }
    else if (name == "Camera Position.Y")
    {
        cameraPos.y = value.toDouble();
    }
    else if (name == "Render Forces")
    {
        renderForces = value.toBool();
    }
    else
    {
        return false;
    }
    return true;
}

void Display::addChild(Item *newChild, int position)
{
    if (position == -1)
        position = screenItems.length();

    ScreenItem *screenItem = static_cast<ScreenItem *>(newChild);
    screenItems.insert(position, screenItem);
    QGraphicsProxyWidget *proxy = scene->addWidget(screenItem->widget);

    widgets.insert(std::pair<ScreenItem *, QGraphicsProxyWidget *>
                   (screenItem, proxy));

    //updates proportions if widget's properties are changed
    connect(screenItem, &ScreenItem::widgetUpdated, this, &Display::widgetUpdated);
    widgetUpdated(screenItem);
}

void Display::widgetUpdated(ScreenItem *item)
{
    QGraphicsProxyWidget *proxy = widgets.at(item);
    proxy->setPos((item->position*scale).toQPointF());
    //forces widget to set size
    proxy->setMaximumSize(item->size.x*scale.x, item->size.y*scale.y);
    proxy->setMinimumSize(item->size.x*scale.x, item->size.y*scale.y);
}

void Display::removeChild(Item *child)
{
    ScreenItem *screenItem = static_cast<ScreenItem *>(child);
    screenItems.removeOne(screenItem);

    QGraphicsProxyWidget *proxy = widgets.at(screenItem);
    //disconnected proxy from widget first, otherwise deleting proxy also deletes the widget. It could be the ScreenItem is being dragged from one display to another.
    proxy->setWidget(nullptr);
    scene->removeItem(proxy);
    delete proxy;
}

QList<Item *> Display::getChildren()
{
    QList<Item *> toReturn;
    for (int i = 0; i < screenItems.length(); i++)
    {
        toReturn.append(screenItems.at(i));
    }
    return toReturn;
}

QStringList Display::getValidChildren()
{
    return QStringList({"Slider", "Graph"});
}

std::pair<bool, Item *> Display::load(QTextStream &in)
{
    Display *newDisplay = new Display();
    //set properties. If there is an error, return false to stop loading.
    if (!newDisplay->setAllProperties(getPropertiesFromFile(in)))
        return std::pair<bool, Item *>(false, nullptr);
    QString temp = in.readLine();
    while (temp.contains("{"))
    { //if there is a child to add, it will have {
        std::pair<bool, Item *> child;
        if (temp == "Slider{")
            child = Slider::load(in);
        else if (temp == "Graph{")
            child = Graph::load(in);
        if (!child.first)
            return std::pair<bool, Item *>(false, nullptr);
        newDisplay->addChild(child.second);
        temp = in.readLine();
    }
    //it should end with a { to show the object is closed
    if (temp != "}")
        return std::pair<bool, Item *>(false, nullptr);
    return std::pair<bool, Item *>(true, newDisplay);
}

QStringList Display::save()
{
    QStringList toReturn = Item::save(this);
    toReturn.replace(validProperties.indexOf("World")-2,
                     QString::number(worldItem->row()));
    //replace world's name with its index, for use when loading
    for (int i = 0; i < screenItems.length(); i++)
    {
        QStringList child = screenItems.at(i)->save();
        toReturn += child;
    }
    toReturn << "}";
    return toReturn;
}
